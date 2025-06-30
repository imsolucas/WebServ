# include "CGIHandler.hpp"
# include "colors.h"
# include "utils.hpp"

# include <cerrno> // errno
# include <iostream>
# include <sys/wait.h> // waitpid
# include <unistd.h> // close, dup2, execve, _exit, fork, pipe

using std::cerr;
using std::exception;
using std::getline;
using std::istringstream;
using std::map;
using std::runtime_error;
using std::string;
using std::vector;

CGIHandler::CGIHandler(HttpRequest &req, string root)
: _req(req), _root(root), _headersEnd(string::npos), _cgiOutput(), _cgiBody(), _cgiStatusCode(200) {}

// If CGI executed successfully, function will return OK.
// Otherwise, it will return the HTTP status code for any errors.
StatusCode CGIHandler::execute()
{
	try
	{
		_unchunkBody();
		_setupPipes();
		_setupEnv();
		_childPid = fork();
		if (_childPid < 0)
			throw runtime_error("CGI: Failed to fork child process.");
		else if (_childPid == 0)
			_cgiChildProcess();
		else
			_cgiParentProcess();
		_validateCGIOutput();
		return OK;
	}
	catch (const UnchunkingException& e)
	{
		utils::printError(e.what());
		return BAD_REQUEST;
	}
	catch (const ScriptNotFoundException& e)
	{
		utils::printError(e.what());
		return NOT_FOUND;
	}
	catch (const ScriptPermissionDeniedException& e)
	{
		utils::printError(e.what());
		return FORBIDDEN;
	}
	catch (const AbnormalTerminationException& e)
	{
		utils::printError(e.what());
		return BAD_GATEWAY;
	}
	catch (const MalformedOutputException& e)
	{
		utils::printError(e.what());
		return BAD_GATEWAY;
	}
	catch (const TimeoutException& e)
	{
		utils::printError(e.what());
		return GATEWAY_TIMEOUT;
	}
	// catches for pipe, fork, waitpid and ENOEXEC exceptions
	catch (const exception& e)
	{
		utils::printError(e.what());
		return INTERNAL_SERVER_ERROR;
	}
}

const string &CGIHandler::getCGIOutput() const
{
	return this->_cgiOutput;
}

const map<string, string> &CGIHandler::getCGIHeaders() const
{
	return this->_cgiHeaders;
}

const string &CGIHandler::getCGIBody() const
{
	return this->_cgiBody;
}

int CGIHandler::getCGIStatusCode() const
{
	return this->_cgiStatusCode;
}

// Format for chunked transfer encoding as per RFC 7230
// <chunk-size in hex>\r\n
// <chunk-data>\r\n
// ...
// 0\r\n
// \r\n
// For each chunk-size and chunk-data line pair:
// get chunk-size -> read that many bytes of chunk-data -> append those bytes to the unchunked body.
void CGIHandler::_unchunkBody()
{
	if (!_req.headers.count("transfer-encoding")
		|| _req.headers.at("transfer-encoding") != "chunked")
		return;

	istringstream iss(_req.body);
	string line, unchunkedBody;

	// getline breaks lines on "\n" by default
	while (getline(iss, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
			// erase last character of the string ('\r') to sanitize input for hexStrToInt
			line.erase(line.length() - 1);
		size_t chunkSize = 0;
		try
		{
			chunkSize = utils::hexStrToInt(line);
		}
		catch (const exception& e)
		{
			utils::printError(e.what());
			throw UnchunkingException();
		}
		// unchunking is completed when there are no more chunks to process
		// i.e. the "0\r\n\r\n" line is encountered.
		if (chunkSize == 0)
			break;

		// character array can be accurately sized because chunk-size is known.
		vector<char> buffer(chunkSize);
		// read() (an istream member function) extracts n characters from the stream and
		// stores them in the array pointed to by s. It will also advance the stream.
		iss.read(&buffer[0], chunkSize);
		// additional check to ensure chunk-size tallies with chunk-data length.
		// gcount() returns the number of characters extracted by the most recent unformatted
		// input operation (e.g. getline, read, etc.) performed on the object.
		if (iss.gcount() != static_cast<std::streamsize>(chunkSize))
		{
			utils::printError("Chunk size does not match number of bytes of chunk data read.");
			throw UnchunkingException();
		}
		// append to the unchunked body with the range of the character array.
		unchunkedBody.append(buffer.begin(), buffer.end());
		// after appending the buffer, "\r\n" of the chunk-data still remains in the stream,
		// so cleanly skip over it with getline
		getline(iss, line);
	}
	// once the body is unchunked, update the request body, content length
	// and strip the transfer encoding header to prevent confusion.
	_req.body = unchunkedBody;
	// content length header was previously absent in chunked transfer, so add it in.
	_req.headers["content-length"] = utils::toString(_req.body.size());
	_req.headers.erase("transfer-encoding");
}

void CGIHandler::_setupPipes()
{
	if (pipe(_stdinPipe) < 0)
		throw runtime_error("CGI: Failed to create pipe: _stdinPipe.");
	if (pipe(_stdoutPipe) < 0)
		throw runtime_error("CGI: Failed to create pipe: _stdoutPipe.");
}

// Environment variables may remain unused, but our job is to emulate the
// standard environment expected by CGI scripts based on the RFC.
void CGIHandler::_setupEnv()
{
	_envStrings.push_back("REQUEST_METHOD=" + _req.method);
	_envStrings.push_back("SERVER_PROTOCOL=" + _req.protocol);

	// sets PATH_INFO, SCRIPT_NAME and QUERY_STRING
	_parseRequestTarget();

	// only set CONTENT_LENGTH and CONTENT_TYPE if there is a body
	if (_req.body.size() > 0)
	{
		_addHeaderToEnv("CONTENT_LENGTH", "content-length");
		_addHeaderToEnv("CONTENT_TYPE", "content-type");
	}

	// iterate through headers and set all of them as environment variables
	// in the format (HTTP_ + uppercase of header name with all '-' converted to '_')
	// e.g. "HTTP_HOST" for "host"
	for (map<string, string>::iterator it = _req.headers.begin();
		it != _req.headers.end(); ++it)
	{
		string key = it->first;
		if (key == "content-length" || key == "content-type")
			continue;

		string envKey = "HTTP_" + utils::toUpper(key);
		for (size_t i = 0; i < envKey.size(); ++i)
		{
			if (envKey[i] == '-')
				envKey[i] = '_';
		}
		_addHeaderToEnv(envKey, key);
	}

	// convert _envStrings to C-style char* array for execve.
	for (vector<string>::iterator it = _envStrings.begin(); it != _envStrings.end(); ++it)
		// c_str() returns const char *.
		// use const_cast to drop the const so that push_back works on the char * vector.
		_env.push_back(const_cast<char *>(it->c_str()));
	_env.push_back(NULL);
}

void CGIHandler::_parseRequestTarget()
{
	string requestTarget = _req.requestTarget;
	string queryString = "";

	// check whether requestTarget has a '?' that will demarcate
	// the beginning of the query string
	size_t pos = requestTarget.find("?");
	if (pos != string::npos)
	{
		_virtualPath = requestTarget.substr(0, pos);
		queryString = requestTarget.substr(pos + 1);
	}
	else
		_virtualPath = requestTarget;

	// subject expects us to use the full path as PATH_INFO.
	_envStrings.push_back("PATH_INFO=" + _virtualPath);
	_envStrings.push_back("SCRIPT_NAME=" + _virtualPath);
	if (queryString != "")
		_envStrings.push_back("QUERY_STRING=" + queryString);
}

// some headers are optional so count() will check that the header key exists
// before trying to access its value with at().
// count value will either be 0 or 1 in a map.
void CGIHandler::_addHeaderToEnv(string key, string headerField)
{
	if (_req.headers.count(headerField))
		_envStrings.push_back(key + "=" + _req.headers.at(headerField));
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

	string execPath = _root + _virtualPath;
	size_t lastSlash = execPath.find_last_of('/');
	string scriptDir, scriptFile;
	// no slashes mean relative path was given e.g. hello.py
	if (lastSlash == string::npos)
	{
		scriptDir = ".";
		scriptFile = execPath;
	}
	// handles for /dir/hello.py, dir/hello.py and /hello.py
	else
	{
		scriptDir = execPath.substr(0, lastSlash);
		if (scriptDir.empty())
			scriptDir = "/";
		scriptFile = execPath.substr(lastSlash + 1);
	}
	chdir(scriptDir.c_str());

	// CGI scripts usually don’t expect command-line arguments
	char *arg[] = { (char *)scriptFile.c_str(), NULL };
	char * const *envp = &_env[0];
	execve(scriptFile.c_str(), arg, envp);
	// exit and _exit are different in C++.
	// exit will call destructors for global/static objects and may cause double-closing of fds.
	// _exit will terminate the child process without touching the parent's runtime state.
	switch (errno)
	{
		// script not found
		case ENOENT:
			_exit(CGI_EXIT_ENOENT);
		// permission denied
		case EACCES:
			_exit(CGI_EXIT_EACCES);
		// invalid executable format
		case ENOEXEC:
			_exit(CGI_EXIT_ENOEXEC);
		// generic failure
		default:
			_exit(CGI_EXIT_FAILURE);
	}
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

	// timeout should happen before reading from stdout because read()
	// will block if the script loops forever.
	_resolveChildStatus();

	char buffer[1024];
	ssize_t bytesRead;
	while ((bytesRead = read(_stdoutPipe[0], buffer, sizeof(buffer))) > 0)
		// string& append (const char* s, size_t n);
		_cgiOutput.append(buffer, bytesRead);
	// close pipe after reading complete.
	close(_stdoutPipe[0]);
}

// This function will wait on the child process and throw exceptions if the child exited
// with non-zero exit status or timed out.
void CGIHandler::_resolveChildStatus()
{
	int status;
	int msElapsed = 0;

	while (msElapsed < TIMEOUT_MS)
	{
		// wait for child's exit status to check that CGI script was successfully executed.
		// WNOHANG means waitpid is non-blocking and will immediately return if no child has exited.
		pid_t exited = waitpid(_childPid, &status, WNOHANG);
		if (exited < 0)
			throw runtime_error("CGI: Failed to wait for child process.");
		// > 0 means the child exited
		else if (exited > 0)
			break;
		// sleep for 1ms
		usleep(1000);
		++msElapsed;
	}

	// if CGI has timed out, kill the child process.
	if (msElapsed >= TIMEOUT_MS)
	{
		kill(_childPid, SIGKILL);
		throw TimeoutException();
	}

	// WIFEXITED(status) returns true if the child terminated normally via exit().
	// WEXITSTATUS(status) only gives the actual exit code if WIFEXITED(status) is true.
	if (WIFEXITED(status))
	{
		int exitStatus = WEXITSTATUS(status);
		if (exitStatus != 0)
		{
			switch (exitStatus)
			{
				case CGI_EXIT_ENOENT:
					throw ScriptNotFoundException();
				case CGI_EXIT_EACCES:
					throw ScriptPermissionDeniedException();
				case CGI_EXIT_ENOEXEC:
					throw runtime_error("CGI: Invalid executable format (ENOEXEC).");
				default:
					throw AbnormalTerminationException();
			}
		}
	}
	// child was terminated by signal e.g.SIGSEGV, SIGKILL
	else if (WIFSIGNALED(status))
		throw AbnormalTerminationException();
}

// This function performs basic checks on the CGI output after normalizing the header separator:
// 1) has a header-body separator
// 2) attempt to parse CGI headers and body
// 3) has a "Content-Type" header at minimum
// 4) attempt to extract the CGI status code from the "Status" header
// If any check fails, a MalformedOutputException is thrown (error code 502).
// CGI headers are not governed by the HTTP RFC in the same way as HTTP request/response headers.
// We have opted for case-sensitive matching of the headers for simplicity.
void CGIHandler::_validateCGIOutput()
{
	_normalizeHeaderSeparator();
	if (!_hasHeaderSeparator())
		throw MalformedOutputException("missing header-body separator");
	_parseCGIOutput();
	if (_cgiHeaders.count("Content-Type") == 0)
		throw MalformedOutputException("missing Content-Type header");
	if (_cgiHeaders.count("Status"))
		_extractCGIStatusCode();
}

// CGI scripts are not strictly bound to HTTP formatting rules.
// Although the HTTP standard (RFC) specifies "\r\n\r\n" to separate headers from the body,
// many CGI scripts (especially in languages like Python) use "\n\n" instead, e.g. print("Content-Type: text/plain\n").
// Therefore, it is the server's responsibility to accept "\n\n" as a valid separator and normalize the output.
// Normalization is done by replacing "\n\n" with "\r\n\r\n".
void CGIHandler::_normalizeHeaderSeparator()
{
	size_t pos = _cgiOutput.find("\n\n");
	if (pos != string::npos)
		_cgiOutput.replace(pos, 2, "\r\n\r\n");
}

// Checks that the output has a header-body separator.
// If it does, sets the position of _headersEnd.
bool CGIHandler::_hasHeaderSeparator()
{
	size_t pos = _cgiOutput.find("\r\n\r\n");
	if (pos != string::npos)
	{
		_headersEnd = pos + 4;
		return true;
	}
	return false;
}

// This function will parse the headers and body of the CGI output.
// If parsing fails at any point, it will throw a MalformedOutputException.
// Formatting checked:
// - key: leading, internal and trailing whitespace is forbidden
// - value: leading and trailing whitespace will be trimmed. internal whitespace is allowed.
// - value must not be empty
// - body is everything after headersEnd
void CGIHandler::_parseCGIOutput()
{
	string headerSection = _cgiOutput.substr(0, _headersEnd - 4);
	istringstream iss(headerSection);
	string line;

	while (getline(iss, line))
	{
		size_t colonPos = line.find(':');
		if (colonPos == string::npos || colonPos == 0)
			throw MalformedOutputException("malformed header");

		string key = line.substr(0, colonPos);
		if (key.find_first_of(" \t\r") != string::npos)
			throw MalformedOutputException("malformed header");

		string value = utils::trim(line.substr(colonPos + 1), " \t\r");
		if (value.empty())
			throw MalformedOutputException("malformed header");

		_cgiHeaders[key] = value;
	}
	_cgiBody = _cgiOutput.substr(_headersEnd);
}

// CGI scripts may optionally set a "Status" header to overwrite the HTTP response status code.
// If the header is present, the status code will be extracted and removed from the headers map.
// 100-599 is the valid range of HTTP status codes as defined by the RFC.
void CGIHandler::_extractCGIStatusCode()
{
	string value = _cgiHeaders.at("Status");
	istringstream iss(value);
	int statusCode;
 	if (!(iss >> statusCode) || statusCode < 100 || statusCode > 599)
	 	throw MalformedOutputException("invalid status code in headers");
	_cgiStatusCode = statusCode;
	_cgiHeaders.erase("Status");
}

CGIHandler::UnchunkingException::UnchunkingException()
: runtime_error("CGI: Failed to unchunk request body.") {}

CGIHandler::ScriptNotFoundException::ScriptNotFoundException()
: runtime_error("CGI: Script not found (ENOENT).") {}

CGIHandler::ScriptPermissionDeniedException::ScriptPermissionDeniedException()
: runtime_error("CGI: Script permission denied (EACCES).") {}

// for non-zero exits and signal crash
CGIHandler::AbnormalTerminationException::AbnormalTerminationException()
: runtime_error("CGI: Child process terminated abnormally.") {}

CGIHandler::TimeoutException::TimeoutException()
: runtime_error("CGI: Child process has timed out.") {}

CGIHandler::MalformedOutputException::MalformedOutputException(string msg)
: runtime_error("CGI: Malformed output - " + msg + ".") {}
