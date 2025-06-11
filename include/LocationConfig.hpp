# pragma once

# include <string>
# include <vector>
# include <iostream>

class LocationConfig {
public:
    // Constructors & Destructor
    LocationConfig();
    LocationConfig(const std::string &path);
    ~LocationConfig();

    // Setters
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

    // Getters
    std::string getPath() const;
    std::string getRoot() const;
    std::string getIndex() const;
    std::string getCgiPath() const;
    std::string getRedirect() const;
    std::string getUploadStore() const;
    bool getAutoindex() const;
    size_t getClientMaxBodySize() const;
    std::vector<std::string> getAllowedMethods() const;
    std::string getAllowedMethodsAsString() const;

    // Utils
    bool isMethodAllowed(const std::string &method) const;
    void printConfig() const;

private:
    std::string path;                // e.g., "/upload"
    std::string root;                // e.g., "/var/www/html"
    std::string index;               // e.g., "index.html"
    std::string cgi_path;            // e.g., "/usr/bin/php-cgi"
    std::string redirect;            // e.g., "301 http://example.com"
    std::string upload_store;        // Upload destination directory
    bool autoindex;                  // Directory listing toggle
    size_t client_max_body_size;     // Limit for POST uploads
    std::vector<std::string> allowed_methods; // Allowed HTTP methods
};
