#include "response.hpp"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <sys/stat.h>
#include <fstream>
#include <sstream>

response::response() : _status_code(OK) {}

response::~response() {}

std::string response::generateResponse(const request_parsing& request, const server_block& config) {
    // Check if request is complete
    if (request.getStatus() != COMPLETED) {
        setStatus(BAD_REQUEST);
        setBody(_generateErrorPage(BAD_REQUEST));
        return buildResponse();
    }
    
    // Parse HTTP method
    http_method method = _parseMethod(request.getRequestLine().getMethod());
    
    // Check if method is allowed
    if (!_isMethodAllowed(method, config)) {
        setStatus(METHOD_NOT_ALLOWED);
        setBody(_generateErrorPage(METHOD_NOT_ALLOWED));
        return buildResponse();
    }
    
    // Handle different HTTP methods
    switch (method) {
        case GET:
            return handleGET(request, config);
        case POST:
            return handlePOST(request, config);
        case DELETE:
            return handleDELETE(request, config);
        default:
            setStatus(NOT_IMPLEMENTED);
            setBody(_generateErrorPage(NOT_IMPLEMENTED));
            return buildResponse();
    }
}

std::string response::handleGET(const request_parsing& request, const server_block& config) {
    std::string uri = request.getRequestLine().getUri();
    std::string root = config.get_root_safe();
    
    if (root.empty()) {
        setStatus(INTERNAL_SERVER_ERROR);
        setBody(_generateErrorPage(INTERNAL_SERVER_ERROR));
        return buildResponse();
    }
    
    // Construct full file path
    std::string file_path = root + uri;
    
    // Check if path exists
    if (!_isFile(file_path) && !_isDirectory(file_path)) {
        setStatus(NOT_FOUND);
        setBody(_generateErrorPage(NOT_FOUND));
        return buildResponse();
    }
    
    // If it's a directory, look for index file
    if (_isDirectory(file_path)) {
        std::string index_file = config.get_index_safe();
        if (!index_file.empty()) {
            std::string index_path = _findFileInDirectory(file_path, index_file);
            if (!index_path.empty()) {
                file_path = index_path;
            } else {
                // Directory listing (if enabled)
                setStatus(OK);
                setBody(_getDirectoryListing(file_path, uri));
                addHeader("Content-Type", "text/html");
                return buildResponse();
            }
        } else {
            setStatus(FORBIDDEN);
            setBody(_generateErrorPage(FORBIDDEN));
            return buildResponse();
        }
    }
    
    // Check if it's a CGI script
    if (file_path.find(".php") != std::string::npos || 
        file_path.find(".py") != std::string::npos ||
        file_path.find(".cgi") != std::string::npos) {
        std::string cgi_output = _handleCGI(file_path, request, config);
        if (!cgi_output.empty()) {
            setStatus(OK);
            setBody(cgi_output);
            addHeader("Content-Type", "text/html");
            return buildResponse();
        }
    }
    
    // Serve static file
    std::string file_content = _readFile(file_path);
    if (file_content.empty()) {
        setStatus(INTERNAL_SERVER_ERROR);
        setBody(_generateErrorPage(INTERNAL_SERVER_ERROR));
        return buildResponse();
    }
    
    setStatus(OK);
    setBody(file_content);
    addHeader("Content-Type", _getContentType(file_path));
    return buildResponse();
}

std::string response::handlePOST(const request_parsing& request, const server_block& config) {
    std::string uri = request.getRequestLine().getUri();
    std::string root = config.get_root_safe();
    std::string body = request.getBodyContent();
    
    // Check client body size
    std::string max_size_str = config.get_client_max_body_size_safe();
    if (!max_size_str.empty()) {
        int max_size = atoi(max_size_str.c_str());
        if (static_cast<int>(body.length()) > max_size) {
            setStatus(REQUEST_ENTITY_TOO_LARGE);
            setBody(_generateErrorPage(REQUEST_ENTITY_TOO_LARGE));
            return buildResponse();
        }
    }
    
    // Handle file upload
    if (uri.find("/upload") != std::string::npos) {
        std::string upload_path = root + "/uploads/";
        std::string result = _uploadFile(upload_path, body);
        setStatus(CREATED);
        setBody(result);
        addHeader("Content-Type", "text/html");
        return buildResponse();
    }
    
    // Handle CGI POST
    std::string file_path = root + uri;
    if (_isFile(file_path) && (file_path.find(".php") != std::string::npos || 
                               file_path.find(".py") != std::string::npos ||
                               file_path.find(".cgi") != std::string::npos)) {
        std::string cgi_output = _handleCGI(file_path, request, config);
        if (!cgi_output.empty()) {
            setStatus(OK);
            setBody(cgi_output);
            addHeader("Content-Type", "text/html");
            return buildResponse();
        }
    }
    
    // Default POST response
    setStatus(OK);
    setBody("POST request processed successfully");
    addHeader("Content-Type", "text/plain");
    return buildResponse();
}

std::string response::handleDELETE(const request_parsing& request, const server_block& config) {
    std::string uri = request.getRequestLine().getUri();
    std::string root = config.get_root_safe();
    std::string file_path = root + uri;
    
    if (!_isFile(file_path)) {
        setStatus(NOT_FOUND);
        setBody(_generateErrorPage(NOT_FOUND));
        return buildResponse();
    }
    
    if (_deleteFile(file_path)) {
        setStatus(NO_CONTENT);
        setBody("");
        return buildResponse();
    } else {
        setStatus(INTERNAL_SERVER_ERROR);
        setBody(_generateErrorPage(INTERNAL_SERVER_ERROR));
        return buildResponse();
    }
}

void response::setStatus(http_status_code code) {
    _status_code = code;
    _status_line = "HTTP/1.1 " + ft_to_string(code) + " " + _getStatusMessage(code);
}

void response::addHeader(const std::string& key, const std::string& value) {
    _headers[key] = value;
}

void response::setBody(const std::string& body) {
    _body = body;
}

std::string response::buildResponse() {
    std::string response = _status_line + "\r\n";
    
    // Add headers
    for (std::map<std::string, std::string>::iterator it = _headers.begin(); 
         it != _headers.end(); ++it) {
        response += it->first + ": " + it->second + "\r\n";
    }
    
    // Add content length
    response += "Content-Length: " + ft_to_string(_body.length()) + "\r\n";
    
    // End of headers
    response += "\r\n";
    
    // Add body
    response += _body;
    
    return response;
}

std::string response::_getStatusMessage(http_status_code code) {
    switch (code) {
        case CONTINUE: return "Continue";
        case OK: return "OK";
        case CREATED: return "Created";
        case ACCEPTED: return "Accepted";
        case NO_CONTENT: return "No Content";
        case MOVED_PERMANENTLY: return "Moved Permanently";
        case FOUND: return "Found";
        case NOT_MODIFIED: return "Not Modified";
        case BAD_REQUEST: return "Bad Request";
        case UNAUTHORIZED: return "Unauthorized";
        case FORBIDDEN: return "Forbidden";
        case NOT_FOUND: return "Not Found";
        case METHOD_NOT_ALLOWED: return "Method Not Allowed";
        case REQUEST_ENTITY_TOO_LARGE: return "Request Entity Too Large";
        case UNSUPPORTED_MEDIA_TYPE: return "Unsupported Media Type";
        case INTERNAL_SERVER_ERROR: return "Internal Server Error";
        case NOT_IMPLEMENTED: return "Not Implemented";
        case BAD_GATEWAY: return "Bad Gateway";
        case SERVICE_UNAVAILABLE: return "Service Unavailable";
        default: return "Unknown";
    }
}

std::string response::_getContentType(const std::string& file_path) {
    size_t dot_pos = file_path.find_last_of('.');
    if (dot_pos == std::string::npos) return "text/plain";
    
    std::string extension = file_path.substr(dot_pos + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == "html" || extension == "htm") return "text/html";
    if (extension == "css") return "text/css";
    if (extension == "js") return "application/javascript";
    if (extension == "json") return "application/json";
    if (extension == "xml") return "application/xml";
    if (extension == "txt") return "text/plain";
    if (extension == "jpg" || extension == "jpeg") return "image/jpeg";
    if (extension == "png") return "image/png";
    if (extension == "gif") return "image/gif";
    if (extension == "pdf") return "application/pdf";
    if (extension == "zip") return "application/zip";
    
    return "application/octet-stream";
}

std::string response::_generateErrorPage(http_status_code code) {
    std::string title = ft_to_string(code) + " " + _getStatusMessage(code);
    std::string html = "<!DOCTYPE html>\n<html>\n<head>\n";
    html += "<title>" + title + "</title>\n";
    html += "<style>\n";
    html += "body { font-family: Arial, sans-serif; text-align: center; padding: 50px; }\n";
    html += "h1 { color: #333; }\n";
    html += "p { color: #666; }\n";
    html += "</style>\n</head>\n<body>\n";
    html += "<h1>" + title + "</h1>\n";
    html += "<p>The requested resource could not be found or processed.</p>\n";
    html += "</body>\n</html>";
    return html;
}

bool response::_isDirectory(const std::string& path) {
    struct stat path_stat;
    if (stat(path.c_str(), &path_stat) != 0) return false;
    return S_ISDIR(path_stat.st_mode);
}

bool response::_isFile(const std::string& path) {
    struct stat path_stat;
    if (stat(path.c_str(), &path_stat) != 0) return false;
    return S_ISREG(path_stat.st_mode);
}

std::string response::_getDirectoryListing(const std::string& path, const std::string& uri) {
    DIR* dir = opendir(path.c_str());
    if (!dir) return "";
    
    std::string html = "<!DOCTYPE html>\n<html>\n<head>\n";
    html += "<title>Directory Listing</title>\n";
    html += "<style>\n";
    html += "body { font-family: Arial, sans-serif; margin: 20px; }\n";
    html += "h1 { color: #333; }\n";
    html += "a { color: #0066cc; text-decoration: none; }\n";
    html += "a:hover { text-decoration: underline; }\n";
    html += ".file { margin: 5px 0; }\n";
    html += "</style>\n</head>\n<body>\n";
    html += "<h1>Directory Listing for " + uri + "</h1>\n";
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") continue;
        
        std::string full_path = path + "/" + name;
        std::string link = uri + (uri[uri.length()-1] == '/' ? "" : "/") + name;
        
        html += "<div class='file'>";
        if (_isDirectory(full_path)) {
            html += "📁 <a href='" + link + "/'>" + name + "/</a>";
        } else {
            html += "📄 <a href='" + link + "'>" + name + "</a>";
        }
        html += "</div>\n";
    }
    
    closedir(dir);
    html += "</body>\n</html>";
    return html;
}

std::string response::_readFile(const std::string& file_path) {
    std::ifstream file(file_path.c_str(), std::ios::binary);
    if (!file.is_open()) return "";
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool response::_deleteFile(const std::string& file_path) {
    return unlink(file_path.c_str()) == 0;
}

std::string response::_handleCGI(const std::string& file_path, const request_parsing& request, const server_block& config) {
    (void)config; // Suppress unused parameter warning
    int pipe_in[2], pipe_out[2];
    
    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) return "";
    
    pid_t pid = fork();
    if (pid == -1) {
        close(pipe_in[0]); close(pipe_in[1]);
        close(pipe_out[0]); close(pipe_out[1]);
        return "";
    }
    
    if (pid == 0) {
        // Child process
        close(pipe_in[1]);
        close(pipe_out[0]);
        
        dup2(pipe_in[0], STDIN_FILENO);
        dup2(pipe_out[1], STDOUT_FILENO);
        
        // Set environment variables
        setenv("REQUEST_METHOD", request.getRequestLine().getMethod().c_str(), 1);
        setenv("REQUEST_URI", request.getRequestLine().getUri().c_str(), 1);
        setenv("QUERY_STRING", request.getRequestLine().getUri().c_str(), 1);
        setenv("CONTENT_LENGTH", ft_to_string(request.getBodyContent().length()).c_str(), 1);
        setenv("CONTENT_TYPE", "application/x-www-form-urlencoded", 1);
        setenv("PATH_INFO", file_path.c_str(), 1);
        
        // Change to script directory
        std::string script_dir = file_path.substr(0, file_path.find_last_of('/'));
        chdir(script_dir.c_str());
        
        // Execute CGI script
        if (file_path.find(".php") != std::string::npos) {
            execlp("php-cgi", "php-cgi", file_path.c_str(), NULL);
        } else if (file_path.find(".py") != std::string::npos) {
            execlp("python", "python", file_path.c_str(), NULL);
        } else {
            execlp(file_path.c_str(), file_path.c_str(), NULL);
        }
        
        exit(1);
    } else {
        // Parent process
        close(pipe_in[0]);
        close(pipe_out[1]);
        
        // Write request body to CGI
        if (request.getRequestLine().getMethod() == "POST") {
            write(pipe_in[1], request.getBodyContent().c_str(), request.getBodyContent().length());
        }
        close(pipe_in[1]);
        
        // Read CGI output
        char buffer[4096];
        std::string output;
        ssize_t bytes_read;
        
        while ((bytes_read = read(pipe_out[0], buffer, sizeof(buffer))) > 0) {
            output.append(buffer, bytes_read);
        }
        
        close(pipe_out[0]);
        waitpid(pid, NULL, 0);
        
        return output;
    }
}

std::string response::_uploadFile(const std::string& upload_path, const std::string& body) {
    // Create uploads directory if it doesn't exist
    mkdir(upload_path.c_str(), 0755);
    
    // Parse multipart form data (simplified)
    std::map<std::string, std::string> form_data = parseFormData(body);
    
    std::string result = "<!DOCTYPE html>\n<html>\n<head>\n";
    result += "<title>Upload Result</title>\n</head>\n<body>\n";
    result += "<h1>Upload Result</h1>\n";
    
    for (std::map<std::string, std::string>::iterator it = form_data.begin(); 
         it != form_data.end(); ++it) {
        std::string filename = upload_path + it->first;
        std::ofstream file(filename.c_str());
        if (file.is_open()) {
            file << it->second;
            file.close();
            result += "<p>File '" + it->first + "' uploaded successfully.</p>\n";
        } else {
            result += "<p>Failed to upload file '" + it->first + "'.</p>\n";
        }
    }
    
    result += "</body>\n</html>";
    return result;
}

http_method response::_parseMethod(const std::string& method) {
    if (method == "GET") return GET;
    if (method == "POST") return POST;
    if (method == "DELETE") return DELETE;
    return UNKNOWN_METHOD;
}

bool response::_isMethodAllowed(http_method method, const server_block& config) {
    // For now, allow all methods. This can be extended with location-based restrictions
    (void)config;
    return method != UNKNOWN_METHOD;
}

std::string response::_findFileInDirectory(const std::string& dir_path, const std::string& index_file) {
    std::string index_path = dir_path + "/" + index_file;
    if (_isFile(index_path)) {
        return index_path;
    }
    return "";
}

std::string response::urlDecode(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '%' && i + 2 < str.length()) {
            int value;
            std::istringstream iss(str.substr(i + 1, 2));
            iss >> std::hex >> value;
            result += static_cast<char>(value);
            i += 2;
        } else if (str[i] == '+') {
            result += ' ';
        } else {
            result += str[i];
        }
    }
    return result;
}

std::map<std::string, std::string> response::parseQueryString(const std::string& query) {
    std::map<std::string, std::string> result;
    std::istringstream iss(query);
    std::string pair;
    
    while (std::getline(iss, pair, '&')) {
        size_t pos = pair.find('=');
        if (pos != std::string::npos) {
            std::string key = pair.substr(0, pos);
            std::string value = pair.substr(pos + 1);
            result[urlDecode(key)] = urlDecode(value);
        }
    }
    
    return result;
}

std::map<std::string, std::string> response::parseFormData(const std::string& body) {
    std::map<std::string, std::string> result;
    
    // Simple multipart parsing (for basic file uploads)
    size_t pos = body.find("filename=\"");
    if (pos != std::string::npos) {
        size_t filename_start = pos + 10;
        size_t filename_end = body.find("\"", filename_start);
        if (filename_end != std::string::npos) {
            std::string filename = body.substr(filename_start, filename_end - filename_start);
            
            // Find content after double newline
            size_t content_start = body.find("\r\n\r\n");
            if (content_start != std::string::npos) {
                std::string content = body.substr(content_start + 4);
                result[filename] = content;
            }
        }
    }
    
    return result;
}
