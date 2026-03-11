#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <string>
#include <vector>
#include <functional>
#include "json.hpp"

using json = nlohmann::json;

class ApiClient {
public:
    static ApiClient& instance();

    json fetchAll(const std::string& resource, const std::string& filter = "");
    json fetchOne(const std::string& resource, const std::string& id);

    void setBaseUrl(const std::string& url);
    std::string getBaseUrl() const;

private:
    ApiClient();
    ~ApiClient() = default;
    ApiClient(const ApiClient&) = delete;
    ApiClient& operator=(const ApiClient&) = delete;

    std::string httpGet(const std::string& url);

    std::string baseUrl_;
};

#endif // API_CLIENT_H
