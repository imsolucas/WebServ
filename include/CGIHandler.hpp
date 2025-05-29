# pragma once

# include "Http.h"

# include <stdexcept>
# include <string>
# include <vector>

class CGIHandler {
public:
	CGIHandler(struct HttpRequest);
	// CGIHandler(const Request& req, const ServerConfig& config);
	std::string execute();
	static void testCGIHandler();

private:
	HttpRequest _req;
	int _stdinPipe[2];
	int _stdoutPipe[2];
	pid_t _childPid;
	std::string _cgiOutput;
	std::vector<std::string> _envString;
	std::vector<char *> _envp;

	void _setupPipes();
	void _setupEnv();
	void _cgiChildProcess();
	void _cgiParentProcess();

	public:
		class PipeException : public std::runtime_error
		{
			public:
				PipeException(std::string pipe);
		};

		class ForkException : public std::runtime_error
		{
			public:
				ForkException();
		};

		class ExecveException : public std::runtime_error
		{
			public:
				ExecveException();
		};
};
