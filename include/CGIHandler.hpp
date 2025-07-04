# pragma once

# include "Http.hpp"

# include <stdexcept>
# include <string>
# include <vector>

// CGI-specific exit codes
# define CGI_EXIT_FAILURE 1
# define CGI_EXIT_ENOEXEC 125
# define CGI_EXIT_EACCES  126
# define CGI_EXIT_ENOENT  127

// 2-second timeout window for CGI scripts to execute
# define TIMEOUT_MS 2000

class CGIHandler
{
	public:
		// http request will be modified for unchunked bodies
		CGIHandler(HttpRequest &req, std::string root);
		StatusCode execute();
		const std::string &getCGIOutput() const;
		const std::map<std::string, std::string> &getCGIHeaders() const;
		const std::string &getCGIBody() const;
		int getCGIStatusCode() const;

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
		int _cgiStatusCode;

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
		void _parseCGIOutput();
		void _extractCGIStatusCode();

	public:
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
