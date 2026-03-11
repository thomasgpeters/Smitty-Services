#include "Auth.h"
#include "ApiClient.h"
#include <stdexcept>

Auth& Auth::instance() {
    static Auth auth;
    return auth;
}

Auth::Auth()
    : loginEndpoint_("/auth/login") {
    session_.authenticated = false;
}

bool Auth::login(const std::string& username, const std::string& password) {
    // For now, accept any non-empty credentials
    // Will integrate with the actual auth endpoint later
    if (username.empty() || password.empty()) {
        return false;
    }

    session_.username = username;
    session_.displayName = username;
    session_.authenticated = true;

    for (auto& cb : callbacks_) {
        cb(true);
    }

    return true;
}

void Auth::logout() {
    session_ = UserSession{};
    session_.authenticated = false;

    for (auto& cb : callbacks_) {
        cb(false);
    }
}

bool Auth::isAuthenticated() const {
    return session_.authenticated;
}

const UserSession& Auth::currentUser() const {
    return session_;
}

void Auth::setLoginEndpoint(const std::string& endpoint) {
    loginEndpoint_ = endpoint;
}

void Auth::onAuthChanged(AuthCallback callback) {
    callbacks_.push_back(callback);
}
