# pragma once

# include <string>
# include <vector>
# include <iostream>

class Location {
public:
    // Constructors & Destructor
    Location();
    Location(const std::string &path);
    ~Location();

    // Setters
    void setPath(const std::string &path);
    void setRoot(const std::string &root);
    void setIndex(const std::string &index);
    void setRedirect(const std::string &redirect);
    void setAutoindex(bool autoindex);
    void setClientMaxBodySize(size_t size, const std::string &unit);
    void addAllowedMethod(const std::string &method);
    void removeAllowedMethod(const std::string &method);
    void clearAllowedMethods();

    // Getters
    const std::string &getPath() const;
    const std::string &getRoot() const;
    const std::string &getIndex() const;
    const std::string &getRedirect() const;
    bool getAutoindex() const;
    size_t getClientMaxBodySizeInBytes() const;
    const std::vector<std::string> &getAllowedMethods() const;
    std::string getAllowedMethodsAsString() const;

    // Utils
    bool isMethodAllowed(const std::string &method) const;
    void printConfig() const;

private:
    std::string path;                // e.g., "/upload"
    std::string root;                // e.g., "/var/www/html"
    std::string index;               // e.g., "index.html"
    std::string redirect;            // e.g., "301 http://example.com"
    bool autoindex;                  // Directory listing toggle
    std::pair<size_t, std::string> client_max_body_size; // Max body size for client requests
    std::vector<std::string> allowed_methods; // Allowed HTTP methods
};
