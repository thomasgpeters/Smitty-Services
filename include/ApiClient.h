#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include "json.hpp"

using json = nlohmann::json;

struct ApiResponse {
    long httpStatus = 0;
    json body;
    bool ok() const { return httpStatus >= 200 && httpStatus < 300; }
    bool hasData() const { return body.contains("data"); }
    bool hasErrors() const { return body.contains("errors"); }

    // Convenience: extract "data" array or object
    json data() const { return hasData() ? body["data"] : json(); }

    // Extract JSONAPI error messages into a single string
    std::string errorMessage() const;
};

class ApiClient {
public:
    static ApiClient& instance();

    // High-level JSONAPI endpoints
    ApiResponse fetchAll(const std::string& resource, const std::string& filter = "");
    ApiResponse fetchOne(const std::string& resource, const std::string& id);

    void setBaseUrl(const std::string& url);
    std::string getBaseUrl() const;

private:
    ApiClient();
    ~ApiClient() = default;
    ApiClient(const ApiClient&) = delete;
    ApiClient& operator=(const ApiClient&) = delete;

    // Raw HTTP with status code capture
    ApiResponse httpGet(const std::string& url);

    // Parse and validate JSONAPI response body
    ApiResponse parseResponse(const std::string& rawBody, long httpStatus, const std::string& url);

    // Logging helpers
    void logRequest(const std::string& method, const std::string& url);
    void logResponse(const std::string& method, const std::string& url,
                     long status, double elapsedMs, const ApiResponse& resp);
    void logError(const std::string& method, const std::string& url, const std::string& error);

    std::string baseUrl_;
};

#endif // API_CLIENT_H
