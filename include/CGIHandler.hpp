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
		int execute();
		const std::string &getCGIOutput() const;

	private:
		HttpRequest &_req;
		int _stdinPipe[2];
		int _stdoutPipe[2];
		// environment variables for execve
		std::vector<std::string> _envStrings;
		std::vector<char *> _env;
		std::string _virtualPath;
		pid_t _childPid;
		std::string _cgiOutput;

		void _unchunkBody();
		void _setupPipes();
		void _setupEnv();
		void _parseRequestTarget();
		void _addHeaderToEnv(std::string key, std::string headerField);
		void _cgiChildProcess();
		void _cgiParentProcess();

	public:
		class UnchunkingException : public std::runtime_error
		{
			public:
				UnchunkingException();
		};

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

		class ScriptNotFoundException : public std::runtime_error
		{
			public:
				ScriptNotFoundException();
		};

		class ScriptPermissionDeniedException : public std::runtime_error
		{
			public:
				ScriptPermissionDeniedException();
		};

		class ScriptExecutionFailureException : public std::runtime_error
		{
			public:
				ScriptExecutionFailureException();
		};

		static void testCGIHandler(const std::string &stream);
};
