#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include "Includes.h"

class LocationConfig
{
	public:
		// Constructors
		LocationConfig();
		LocationConfig(const std::string &path);
		~LocationConfig();

		// Methods
		bool isMethodAllowed(const std::string &method) const; // Check if a method is allowed example GET, POST, DELETE
		void setPath(const std::string &path);
		void setRoot(const std::string &root);
		void setIndex(const std::string &index);
		void setCgiPath(const std::string &cgi_path);
		void setRedirect(const std::string &redirect);
		void setUploadStore(const std::string &upload_store);
		void setAutoindex(bool autoindex);
		void setClientMaxBodySize(size_t size);
		void addAllowedMethod(const std::string &method);
		void removeAllowedMethod(const std::string &method);
		void clearAllowedMethods();
		void printConfig() const;
		std::string getPath() const;
		std::string getRoot() const;
		std::string getIndex() const;
		std::string getCgiPath() const;
		std::string getRedirect() const;
		std::string getUploadStore() const;
		bool getAutoindex() const;
		size_t getClientMaxBodySize() const;
		std::vector<std::string> getAllowedMethods() const;
		std::string getAllowedMethodsAsString() const; // Returns allowed methods as a comma-separated string

	private:
		// Core attributes from nginx location blocks
		std::string path;		  // e.g., "/upload"
		std::string root;		  // e.g., "/var/www/html"
		std::string index;		  // e.g., "index.html"
		std::string cgi_path;	  // e.g., "/usr/bin/php-cgi"
		std::string redirect;	  // e.g., "301 http://example.com"
		std::string upload_store; // directory to store uploads

		bool autoindex;				 // true if directory listing is on
		size_t client_max_body_size; // upload limit in bytes

		std::vector<std::string> allowed_methods; // e.g., GET, POST, DELETE
};

#endif // LOCATIONCONFIG_HPP
