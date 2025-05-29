# include "CGIHandler.hpp"

# include <sys/wait.h> // waitpid
# include <unistd.h> // close, dup2, execve, fork, pipe

using std::string;

CGIHandler::CGIHandler(HttpRequest req) : _req(req) {}

void CGIHandler::testCGIHandler()
{
	const char *stream =
		"GET /index.html HTTP/1.1\r\n"
		"Host: example.com\r\n"
		"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)\r\n"
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9\r\n"
		"Connection: keep-alive\r\n"
		"\r\n";

	HttpRequest request = deserialize(stream);

	CGIHandler cgi(request);

}

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

void CGIHandler::_setupEnv()
{

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
	char **envp;

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
