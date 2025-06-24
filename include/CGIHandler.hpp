# pragma once

# include "Http.h"

# include <stdexcept>
# include <string>
# include <vector>

// 2-second timeout window for CGI scripts to execute
# define TIMEOUT_MS 2000

class CGIHandler
{
	public:
		// http request will be modified when unchunking the body
		CGIHandler(HttpRequest &req, std::string root);
		StatusCode execute();
		const std::string &getCGIOutput() const;
		const std::map<std::string, std::string> &getCGIHeaders() const;
		const std::string &getCGIBody() const;

	private:
		HttpRequest &_req;
		std::string _root;
		int _stdinPipe[2];
		int _stdoutPipe[2];
		// environment variables for execve
		std::vector<std::string> _envStrings;
		std::vector<char *> _env;
		std::string _virtualPath;
		pid_t _childPid;
		size_t _headersEnd;
		std::string _cgiOutput;
		std::map<std::string, std::string> _cgiHeaders;
		std::string _cgiBody;

		void _unchunkBody();
		void _setupPipes();
		void _setupEnv();
		void _parseRequestTarget();
		void _addHeaderToEnv(std::string key, std::string headerField);
		void _cgiChildProcess();
		void _cgiParentProcess();
		void _resolveChildStatus();
		void _validateCGIOutput();
		void _normalizeHeaderSeparator();
		bool _hasHeaderSeparator();
		bool _parseCGIOutput();

	public:
		class UnchunkingException : public std::runtime_error
		{
			public:
				UnchunkingException();
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

		class AbnormalTerminationException : public std::runtime_error
		{
			public:
				AbnormalTerminationException();
		};

		class TimeoutException : public std::runtime_error
		{
			public:
				TimeoutException();
		};

		class MalformedOutputException : public std::runtime_error
		{
			public:
				MalformedOutputException(std::string msg);
		};
};
