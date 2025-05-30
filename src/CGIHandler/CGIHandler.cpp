# include "CGIHandler.hpp"
# include "utils.hpp"

# include <sys/wait.h> // waitpid
# include <unistd.h> // close, dup2, execve, fork, pipe
# include <iostream> // TODO: DELETE

using std::string;

// TODO: DELETE
void printRequest(const HttpRequest& req) {
	std::cout << "Method: " << req.method << std::endl;
	std::cout << "Request Target: " << req.requestTarget << std::endl;
	std::cout << "Protocol: " << req.protocol << std::endl;
	std::cout << "Headers:" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = req.headers.begin(); it != req.headers.end(); ++it) {
		std::cout << "  " << it->first << ": " << it->second << std::endl;
	}
	std::cout << "Body:" << std::endl;
	std::cout << req.body << std::endl;
}

// TODO: DELETE
void CGIHandler::testCGIHandler()
{
	const char *stream =
		"GET /index.html HTTP/1.1\r\n"
		"Host: example.com\r\n"
		"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)\r\n"
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9\r\n"
		"Connection: keep-alive\r\n"
		"\r\n"
		"name=John+Doe&age=30&email=john.doe%40example.com";

	HttpRequest request = deserialize(stream);
	printRequest(request);

	CGIHandler cgi(request);
	cgi.execute();
}

CGIHandler::CGIHandler(const HttpRequest &req) : _req(req) {}

string CGIHandler::execute()
{
	_setupPipes();
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

// environment variables may remain unused, but our job is to emulate the
// standard environment expected by CGI scripts based on the RFC.
void CGIHandler::_setupEnv()
{
	std::vector<string> envStrings;

	// required CGI variables
	envStrings.push_back("REQUEST_METHOD=" + _req.method);
	envStrings.push_back("SCRIPT_NAME=" + _req.requestTarget);
	envStrings.push_back("SERVER_PROTOCOL=" + _req.protocol);
	envStrings.push_back("CONTENT_LENGTH=" + utils::toString(_req.body.length()));

	// optional but common CGI variables
	// some headers are optional so count() will check that the header key exists
	// before trying to access its value with at().
	// count value will either be 0 or 1 in a map.
	if (_req.headers.count("Content-Type"))
		envStrings.push_back("CONTENT_TYPE=" + _req.headers.at("Content-Type"));
	if (_req.headers.count("Host"))
		envStrings.push_back("HTTP_HOST=" + _req.headers.at("Host"));
	if (_req.headers.count("User-Agent"))
		envStrings.push_back("HTTP_USER_AGENT=" + _req.headers.at("User-Agent"));

	// convert envStrings to C-style char* array for execve.
	for (std::vector<string>::iterator it = envStrings.begin(); it != envStrings.end(); ++it)
	{
		// c_str() returns const char *.
		// use const_cast to drop the const so that push_back works on the char * vector.
		_env.push_back(const_cast<char *>(it->c_str()));
	}
	_env.push_back(NULL);
}

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
	std::string cgiBinPath = "public/cgi-bin/";
	std::string scriptPath = cgiBinPath + _req.requestTarget;
	const char *path = scriptPath.c_str();
	// CGI scripts usually don’t expect command-line arguments
	char *arg[] = { (char *)path, NULL };
	char * const *envp = &_env[0];

	execve(path, arg, envp);
	// exit with status code 1 if execve fails
	exit(1);
}

void CGIHandler::_cgiParentProcess()
{
	// closing unused pipe ends
	close(_stdinPipe[0]);
	close(_stdoutPipe[1]);

	// if the request method is POST, write the request body to the pipe
	// to pass it as STDIN to execve.
	if (_req.method == "POST")
		write(_stdinPipe[1], _req.body.c_str(), _req.body.length());
	// signal EOF for child process to start reading.
	close(_stdinPipe[1]);

	char buffer[1024];
	// actively drain the pipe otherwise child will block on write() when pipe is full.
	while (ssize_t bytesRead = read(_stdoutPipe[0], buffer, sizeof(buffer)) > 0)
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

CGIHandler::ExecveException::ExecveException()
: runtime_error("CGI: Failed to execute script.") {}
