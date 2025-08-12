#pragma once

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include "request_parsing.hpp"
#include "server_block.hpp"
#include "utils.hpp"

// HTTP Status Codes according to RFC 9112
enum http_status_code {
    // 1xx Informational
    CONTINUE = 100,
    
    // 2xx Successful
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    NO_CONTENT = 204,
    
    // 3xx Redirection
    MOVED_PERMANENTLY = 301,
    FOUND = 302,
    NOT_MODIFIED = 304,
    
    // 4xx Client Errors
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    REQUEST_ENTITY_TOO_LARGE = 413,
    UNSUPPORTED_MEDIA_TYPE = 415,
    
    // 5xx Server Errors
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    BAD_GATEWAY = 502,
    SERVICE_UNAVAILABLE = 503
};

// HTTP Methods
enum http_method {
    GET,
    POST,
    DELETE,
    UNKNOWN_METHOD
};

class response {
private:
    std::string _status_line;
    std::map<std::string, std::string> _headers;
    std::string _body;
    http_status_code _status_code;
    
    // Helper methods
    std::string _getStatusMessage(http_status_code code);
    std::string _getContentType(const std::string& file_path);
    std::string _generateErrorPage(http_status_code code);
    bool _isDirectory(const std::string& path);
    bool _isFile(const std::string& path);
    std::string _getDirectoryListing(const std::string& path, const std::string& uri);
    std::string _readFile(const std::string& file_path);
    bool _deleteFile(const std::string& file_path);
    std::string _handleCGI(const std::string& file_path, const request_parsing& request, const server_block& config);
    std::string _uploadFile(const std::string& upload_path, const std::string& body);
    http_method _parseMethod(const std::string& method);
    bool _isMethodAllowed(http_method method, const server_block& config);
    std::string _findFileInDirectory(const std::string& dir_path, const std::string& index_file);
    
public:
    response();
    ~response();
    
    // Main response generation method
    std::string generateResponse(const request_parsing& request, const server_block& config);
    
    // Method handlers
    std::string handleGET(const request_parsing& request, const server_block& config);
    std::string handlePOST(const request_parsing& request, const server_block& config);
    std::string handleDELETE(const request_parsing& request, const server_block& config);
    
    // Response building
    void setStatus(http_status_code code);
    void addHeader(const std::string& key, const std::string& value);
    void setBody(const std::string& body);
    std::string buildResponse();
    
    // Utility methods
    static std::string urlDecode(const std::string& str);
    static std::map<std::string, std::string> parseQueryString(const std::string& query);
    static std::map<std::string, std::string> parseFormData(const std::string& body);
};
