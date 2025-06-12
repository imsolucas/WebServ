# pragma once

# include "Http.h"

# include <stdexcept>
# include <string>
# include <vector>

class CGIHandler
{
	public:
		// http request will be modified when unchunking the body
		CGIHandler(HttpRequest &req);
		std::string execute();

	private:
		HttpRequest &_req;
		int _stdinPipe[2];
		int _stdoutPipe[2];
		// environment variables for execve
		std::vector<std::string> _envStrings;
		std::vector<char *> _env;
		std::string _scriptName;
		pid_t _childPid;
		std::string _cgiOutput;

		void _unchunkBody();
		void _setupPipes();
		void _setupEnv();
		void _parseRequestTarget();
		void _addToEnv(std::string key, std::string headerField);
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

		class UnchunkingException : public std::runtime_error
		{
			public:
				UnchunkingException();
		};

		class ExecveException : public std::runtime_error
		{
			public:
				ExecveException();
		};

		static void testCGIHandler(const std::string &stream);
};
