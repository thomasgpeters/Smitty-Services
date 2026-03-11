#include "ApiClient.h"
#include "AppSettings.h"
#include <curl/curl.h>
#include <stdexcept>
#include <sstream>
#include <cstdlib>

static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

ApiClient& ApiClient::instance() {
    static ApiClient client;
    return client;
}

ApiClient::ApiClient() {
    const char* envUrl = std::getenv("SMITTY_API_ENDPOINT");
    baseUrl_ = envUrl ? std::string(envUrl) : "http://localhost:5656/api";
}

void ApiClient::setBaseUrl(const std::string& url) {
    baseUrl_ = url;
}

std::string ApiClient::getBaseUrl() const {
    return baseUrl_;
}

std::string ApiClient::httpGet(const std::string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }

    std::string response;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/vnd.api+json");
    headers = curl_slist_append(headers, "Accept: application/vnd.api+json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

    CURLcode res = curl_easy_perform(curl);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        throw std::runtime_error(std::string("HTTP request failed: ") + curl_easy_strerror(res));
    }

    return response;
}

json ApiClient::fetchAll(const std::string& resource, const std::string& filter) {
    std::string url = baseUrl_ + "/" + resource;
    if (!filter.empty()) {
        url += "?filter[" + resource + "]=" + filter;
    }

    std::string response = httpGet(url);
    return json::parse(response);
}

json ApiClient::fetchOne(const std::string& resource, const std::string& id) {
    std::string url = baseUrl_ + "/" + resource + "/" + id;
    std::string response = httpGet(url);
    return json::parse(response);
}
