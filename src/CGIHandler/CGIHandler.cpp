# include "CGIHandler.hpp"
# include "colors.h"
# include "utils.hpp"

# include <iostream>
# include <sys/wait.h> // waitpid
# include <unistd.h> // close, dup2, execve, _exit, fork, pipe

using std::cerr;
using std::istringstream;
using std::runtime_error;
using std::string;

CGIHandler::CGIHandler(const HttpRequest &req) : _req(req) {}

string CGIHandler::execute()
{
	_setupPipes();
	_unchunkBody();
	_setupEnv();
	_childPid = fork();
	if (_childPid < 0)
		throw ForkException();
	else if (_childPid == 0)
		_cgiChildProcess();
	else
		_cgiParentProcess();
	return _cgiOutput;
}

void CGIHandler::_setupPipes()
{
	if (pipe(_stdinPipe) < 0)
		throw PipeException("_stdinPipe");
	if (pipe(_stdoutPipe) < 0)
		throw PipeException("_stdoutPipe");
}

// Format for chunked transfer encoding as per RFC 7230
// <chunk-size in hex>\r\n
// <chunk-data>\r\n
// ...
// 0\r\n
// \r\n
void CGIHandler::_unchunkBody()
{
	if (!_req.headers.count("transfer-encoding")
		|| _req.headers.at("transfer-encoding") != "chunked")
		return;

	istringstream stream(_req.body);
	string line;
	string unchunkedBody;

	// getline breaks lines on "\n" by default
	while (std::getline(stream, line))
	{
		if (!line.empty() && line.back() == '\r')
			// erase last character of the string ('\r') so that hexStrToInt won't trigger !eof()
			line.pop_back();
		size_t chunkSize = 0;
		try
		{
			chunkSize = utils::hexStrToInt(line);
		}
		catch (const runtime_error &e)
		{
			utils::printError(e.what());
			throw UnchunkingException();
		}


	}
	// once the body is unchunked, update the request body, content length
	// and strip the transfer encoding header to prevent confusion.
	_req.body = unchunkedBody;
	// content length header was previously absent in chunked transfer, so add it in.
	_req.headers["Content-Length"] = utils::toString(_req.body.size());
	_req.headers.erase("Transfer-Encoding");
}

// Environment variables may remain unused, but our job is to emulate the
// standard environment expected by CGI scripts based on the RFC.
void CGIHandler::_setupEnv()
{
	// required CGI variables
	_envStrings.push_back("REQUEST_METHOD=" + _req.method);
	_envStrings.push_back("SERVER_PROTOCOL=" + _req.protocol);

	_parseRequestTarget();

	// CONTENT_LENGTH AND CONTENT_TYPE are only relevant for POST requests.
	if (_req.method == "POST")
	{
		if (_req.headers.count("content-length"))
			_envStrings.push_back("CONTENT_LENGTH=" + _req.headers.at("content-length"));
		if (_req.headers.count("content-type"))
			_envStrings.push_back("CONTENT_TYPE=" + _req.headers.at("content-type"));
	}

	// optional but common CGI variables
	// some headers are optional so count() will check that the header key exists
	// before trying to access its value with at().
	// count value will either be 0 or 1 in a map.
	if (_req.headers.count("host"))
		_envStrings.push_back("HTTP_HOST=" + _req.headers.at("host"));
	if (_req.headers.count("user-agent"))
		_envStrings.push_back("HTTP_USER_AGENT=" + _req.headers.at("user-agent"));

	// convert _envStrings to C-style char* array for execve.
	for (std::vector<string>::iterator it = _envStrings.begin(); it != _envStrings.end(); ++it)
		// c_str() returns const char *.
		// use const_cast to drop the const so that push_back works on the char * vector.
		_env.push_back(const_cast<char *>(it->c_str()));
	_env.push_back(NULL);
}

// Parses requestTarget and sets the SCRIPT_NAME and
// QUERY_STRING environment variables.
void CGIHandler::_parseRequestTarget()
{
	string requestTarget = _req.requestTarget;
	string queryString = "";

	// check whether requestTarget has a '?' that will demarcate
	// the beginning of the query string
	std::size_t pos = requestTarget.find("?");
	if (pos != std::string::npos)
	{
		_scriptName = requestTarget.substr(0, pos);
		queryString = requestTarget.substr(pos + 1);
	}
	else
		_scriptName = requestTarget;

	_envStrings.push_back("SCRIPT_NAME=" + _scriptName);
	if (queryString != "")
		_envStrings.push_back("QUERY_STRING=" + queryString);
}

// Child will call execve to execute the CGI script.
void CGIHandler::_cgiChildProcess()
{
	// closing unused pipe ends
	close(_stdinPipe[1]);
	close(_stdoutPipe[0]);
	// reading input from stdinPipe
	dup2(_stdinPipe[0], STDIN_FILENO);
	// writing output to stdoutPipe
	dup2(_stdoutPipe[1], STDOUT_FILENO);
	// closing pipe ends after dup2
	close(_stdinPipe[0]);
	close(_stdoutPipe[1]);

	// TODO: MODIFY - DO NOT HARD CODE
	// TODO: SANITIZE PATH TO PREVENT PATH TRAVERSAL AND VALIDATE THAT PATH EXISTS.
	chdir("public/cgi-bin");

	// ensure scriptName is not an absolute path (strip '/').
	if (!_scriptName.empty() && _scriptName[0] == '/')
		_scriptName = _scriptName.substr(1);

	// CGI scripts usually don’t expect command-line arguments
	char *arg[] = { (char *)_scriptName.c_str(), NULL };
	char * const *envp = &_env[0];
	execve(_scriptName.c_str(), arg, envp);
	// exit with status code 1 if execve fails
	// exit and _exit are different in C++.
	// exit will call destructors for global/static objects and may cause double-closing of fds.
	// _exit will terminate the child process without touching the parent's runtime state.
	_exit(1);
}

// Parent will pipe POST request body to stdin of child then assemble CGI output from stdout of child.
void CGIHandler::_cgiParentProcess()
{
	// closing unused pipe ends
	close(_stdinPipe[0]);
	close(_stdoutPipe[1]);

	// if the request method is POST, write the request body to the pipe
	// to pass it as STDIN to execve.
	if (_req.method == "POST")
		write(_stdinPipe[1], _req.body.c_str(), _req.body.size());
	// signal EOF for child process to start reading.
	close(_stdinPipe[1]);

	char buffer[1024];
	ssize_t bytesRead;
	// actively drain the pipe otherwise child will block on write() when pipe is full.
	while ((bytesRead = read(_stdoutPipe[0], buffer, sizeof(buffer))) > 0)
		// string& append (const char* s, size_t n);
		_cgiOutput.append(buffer, bytesRead);
	// close pipe after reading complete.
	close(_stdoutPipe[0]);

	int status;
	// wait for child's exit status to check that CGI script was successfully executed.
	waitpid(_childPid, &status, 0);

	// if child returns 1, throw exception.
	// WIFEXITED(status) returns true if the child terminated normally via exit().
	// WEXITSTATUS(status) only gives the actual exit code if WIFEXITED(status) is true.
	if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
		throw ExecveException();
}

CGIHandler::PipeException::PipeException(string pipe)
: runtime_error("CGI: Failed to create pipe: " + pipe + ".") {}

CGIHandler::ForkException::ForkException()
: runtime_error("CGI: Failed to fork child process.") {}

CGIHandler::UnchunkingException::UnchunkingException()
: runtime_error("CGI: Failed to unchunk request body.") {}

CGIHandler::ExecveException::ExecveException()
: runtime_error("CGI: Failed to execute script.") {}

void CGIHandler::testCGIHandler(const string &stream)
{
	// // GET request with query string
	// string stream =
	// 	"GET /test_cgi.py?name=Alice&age=25 HTTP/1.1\r\n"
	// 	"Host: example.com\r\n"
	// 	"User-Agent: TestClient/1.0\r\n"
	// 	"Accept: */*\r\n"
	// 	"Connection: close\r\n"
	// 	"\r\n";

	// // POST request with form data in body
	// string stream =
	// 	"POST /test_cgi.py HTTP/1.1\r\n"
	// 	"Host: example.com\r\n"
	// 	"User-Agent: TestClient/1.0\r\n"
	// 	"Content-Type: application/x-www-form-urlencoded\r\n"
	// 	"Content-Length: 42\r\n"
	// 	"Connection: close\r\n"
	// 	"\r\n"
	// 	"name=Bob+Smith&email=bob.smith%40mail.com";

	HttpRequest request = deserialize(stream);

	CGIHandler cgi(request);
	string cgiOutput = cgi.execute();
	std::cout << "\nCGI Output: \n" << cgiOutput << std::endl;
}
