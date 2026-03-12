#include "ApiClient.h"
#include "AppSettings.h"
#include <Wt/WLogger.h>
#include <curl/curl.h>
#include <stdexcept>
#include <sstream>
#include <cstdlib>

static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

// ---------------------------------------------------------------------------
// ApiResponse
// ---------------------------------------------------------------------------

std::string ApiResponse::errorMessage() const {
    if (!hasErrors()) return "";

    std::string msg;
    for (const auto& err : body["errors"]) {
        if (!msg.empty()) msg += "; ";
        if (err.contains("title") && !err["title"].is_null())
            msg += err["title"].get<std::string>();
        if (err.contains("detail") && !err["detail"].is_null()) {
            if (!msg.empty() && msg.back() != ' ') msg += ": ";
            msg += err["detail"].get<std::string>();
        }
        if (err.contains("status") && !err["status"].is_null()) {
            msg += " [" + err["status"].dump() + "]";
        }
    }
    return msg.empty() ? "Unknown API error" : msg;
}

// ---------------------------------------------------------------------------
// ApiClient singleton
// ---------------------------------------------------------------------------

ApiClient& ApiClient::instance() {
    static ApiClient client;
    return client;
}

ApiClient::ApiClient() {
    const char* envUrl = std::getenv("SMITTY_API_ENDPOINT");
    baseUrl_ = envUrl ? std::string(envUrl) : "http://localhost:5659/api";
}

void ApiClient::setBaseUrl(const std::string& url) {
    baseUrl_ = url;
}

std::string ApiClient::getBaseUrl() const {
    return baseUrl_;
}

// ---------------------------------------------------------------------------
// Logging helpers  (uses Wt's log, visible in server console)
// ---------------------------------------------------------------------------

void ApiClient::logRequest(const std::string& method, const std::string& url) {
    Wt::log("info") << "[ApiClient] " << method << " " << url;
}

void ApiClient::logResponse(const std::string& method, const std::string& url,
                            long status, double elapsedMs, const ApiResponse& resp) {
    std::ostringstream oss;
    oss << "[ApiClient] " << method << " " << url
        << " -> " << status
        << " (" << std::fixed << std::setprecision(1) << elapsedMs << " ms)";

    if (resp.hasData()) {
        const auto& d = resp.body["data"];
        if (d.is_array())
            oss << " [" << d.size() << " records]";
        else
            oss << " [1 record]";
    }

    if (resp.ok()) {
        Wt::log("info") << oss.str();
    } else {
        oss << " error: " << resp.errorMessage();
        Wt::log("error") << oss.str();
    }
}

void ApiClient::logError(const std::string& method, const std::string& url,
                         const std::string& error) {
    Wt::log("error") << "[ApiClient] " << method << " " << url << " FAILED: " << error;
}

// ---------------------------------------------------------------------------
// HTTP layer
// ---------------------------------------------------------------------------

ApiResponse ApiClient::httpGet(const std::string& url) {
    logRequest("GET", url);
    auto startTime = std::chrono::steady_clock::now();

    CURL* curl = curl_easy_init();
    if (!curl) {
        logError("GET", url, "Failed to initialize CURL");
        throw std::runtime_error("Failed to initialize CURL");
    }

    std::string rawBody;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/vnd.api+json");
    headers = curl_slist_append(headers, "Accept: application/vnd.api+json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &rawBody);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

    CURLcode res = curl_easy_perform(curl);

    long httpStatus = 0;
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpStatus);
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    auto endTime = std::chrono::steady_clock::now();
    double elapsedMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();

    if (res != CURLE_OK) {
        std::string errMsg = std::string("HTTP request failed: ") + curl_easy_strerror(res);
        logError("GET", url, errMsg);
        throw std::runtime_error(errMsg);
    }

    ApiResponse resp = parseResponse(rawBody, httpStatus, url);
    logResponse("GET", url, httpStatus, elapsedMs, resp);
    return resp;
}

ApiResponse ApiClient::httpPost(const std::string& url, const std::string& postBody) {
    logRequest("POST", url);
    auto startTime = std::chrono::steady_clock::now();

    CURL* curl = curl_easy_init();
    if (!curl) {
        logError("POST", url, "Failed to initialize CURL");
        throw std::runtime_error("Failed to initialize CURL");
    }

    std::string rawBody;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/vnd.api+json");
    headers = curl_slist_append(headers, "Accept: application/vnd.api+json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &rawBody);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postBody.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(postBody.size()));

    CURLcode res = curl_easy_perform(curl);

    long httpStatus = 0;
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpStatus);
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    auto endTime = std::chrono::steady_clock::now();
    double elapsedMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();

    if (res != CURLE_OK) {
        std::string errMsg = std::string("HTTP request failed: ") + curl_easy_strerror(res);
        logError("POST", url, errMsg);
        throw std::runtime_error(errMsg);
    }

    ApiResponse resp = parseResponse(rawBody, httpStatus, url);
    logResponse("POST", url, httpStatus, elapsedMs, resp);
    return resp;
}

ApiResponse ApiClient::httpPatch(const std::string& url, const std::string& patchBody) {
    logRequest("PATCH", url);
    auto startTime = std::chrono::steady_clock::now();

    CURL* curl = curl_easy_init();
    if (!curl) {
        logError("PATCH", url, "Failed to initialize CURL");
        throw std::runtime_error("Failed to initialize CURL");
    }

    std::string rawBody;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/vnd.api+json");
    headers = curl_slist_append(headers, "Accept: application/vnd.api+json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &rawBody);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, patchBody.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(patchBody.size()));

    CURLcode res = curl_easy_perform(curl);

    long httpStatus = 0;
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpStatus);
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    auto endTime = std::chrono::steady_clock::now();
    double elapsedMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();

    if (res != CURLE_OK) {
        std::string errMsg = std::string("HTTP request failed: ") + curl_easy_strerror(res);
        logError("PATCH", url, errMsg);
        throw std::runtime_error(errMsg);
    }

    ApiResponse resp = parseResponse(rawBody, httpStatus, url);
    logResponse("PATCH", url, httpStatus, elapsedMs, resp);
    return resp;
}

ApiResponse ApiClient::httpDelete(const std::string& url) {
    logRequest("DELETE", url);
    auto startTime = std::chrono::steady_clock::now();

    CURL* curl = curl_easy_init();
    if (!curl) {
        logError("DELETE", url, "Failed to initialize CURL");
        throw std::runtime_error("Failed to initialize CURL");
    }

    std::string rawBody;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/vnd.api+json");
    headers = curl_slist_append(headers, "Accept: application/vnd.api+json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &rawBody);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

    CURLcode res = curl_easy_perform(curl);

    long httpStatus = 0;
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpStatus);
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    auto endTime = std::chrono::steady_clock::now();
    double elapsedMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();

    if (res != CURLE_OK) {
        std::string errMsg = std::string("HTTP request failed: ") + curl_easy_strerror(res);
        logError("DELETE", url, errMsg);
        throw std::runtime_error(errMsg);
    }

    ApiResponse resp = parseResponse(rawBody, httpStatus, url);
    logResponse("DELETE", url, httpStatus, elapsedMs, resp);
    return resp;
}

// ---------------------------------------------------------------------------
// JSONAPI response parsing
// ---------------------------------------------------------------------------

ApiResponse ApiClient::parseResponse(const std::string& rawBody, long httpStatus,
                                     const std::string& url) {
    ApiResponse resp;
    resp.httpStatus = httpStatus;

    if (rawBody.empty()) {
        if (httpStatus >= 200 && httpStatus < 300) {
            resp.body = json::object();
            return resp;
        }
        resp.body = {{"errors", {{{"title", "Empty response"}, {"status", std::to_string(httpStatus)}}}}};
        return resp;
    }

    try {
        resp.body = json::parse(rawBody);
    } catch (const json::parse_error& e) {
        Wt::log("error") << "[ApiClient] JSON parse error for " << url << ": " << e.what();
        resp.body = {{"errors", {{{"title", "Invalid JSON response"}, {"detail", e.what()}}}}};
        return resp;
    }

    // If HTTP error but body has no "errors" key, synthesize one
    if (httpStatus >= 400 && !resp.hasErrors()) {
        std::string detail;
        if (resp.body.contains("message") && resp.body["message"].is_string())
            detail = resp.body["message"].get<std::string>();
        else
            detail = rawBody.substr(0, 200);

        resp.body["errors"] = {{{"title", "HTTP " + std::to_string(httpStatus)}, {"detail", detail}}};
    }

    return resp;
}

// ---------------------------------------------------------------------------
// High-level JSONAPI endpoints
// ---------------------------------------------------------------------------

ApiResponse ApiClient::fetchAll(const std::string& resource, const std::string& filter,
                                const std::string& include) {
    std::string url = baseUrl_ + "/" + resource;
    std::string sep = "?";
    if (!filter.empty()) {
        url += sep + "filter[" + resource + "]=" + filter;
        sep = "&";
    }
    if (!include.empty()) {
        url += sep + "include=" + include;
    }
    return httpGet(url);
}

ApiResponse ApiClient::fetchOne(const std::string& resource, const std::string& id) {
    std::string url = baseUrl_ + "/" + resource + "/" + id;
    return httpGet(url);
}

ApiResponse ApiClient::createRecord(const std::string& resource, const json& attributes) {
    std::string url = baseUrl_ + "/" + resource;
    json payload = {
        {"data", {
            {"type", resource},
            {"attributes", attributes}
        }}
    };
    return httpPost(url, payload.dump());
}

ApiResponse ApiClient::updateRecord(const std::string& resource, const std::string& id,
                                     const json& attributes) {
    std::string url = baseUrl_ + "/" + resource + "/" + id;
    json payload = {
        {"data", {
            {"type", resource},
            {"id", id},
            {"attributes", attributes}
        }}
    };
    return httpPatch(url, payload.dump());
}

ApiResponse ApiClient::deleteRecord(const std::string& resource, const std::string& id) {
    std::string url = baseUrl_ + "/" + resource + "/" + id;
    return httpDelete(url);
}
