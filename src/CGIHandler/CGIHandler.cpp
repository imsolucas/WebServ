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


	const char * path;
	char **args;
	char * const *envp = &_env[0];

	if (execve(path, args, envp) < 0)
		throw ExecveException();
}

void CGIHandler::_cgiParentProcess()
{
	// closing unused pipe ends
	close(_stdinPipe[0]);
	close(_stdoutPipe[1]);

	waitpid(_childPid, NULL, 0);
}

CGIHandler::PipeException::PipeException(string pipe)
: runtime_error("CGI: Failed to create pipe: " + pipe + ".") {}

CGIHandler::ForkException::ForkException()
: runtime_error("CGI: Failed to fork child process.") {}

CGIHandler::ExecveException::ExecveException()
: runtime_error("CGI: Failed to execute script.") {}
