#ifndef AUTH_H
#define AUTH_H

#include <string>
#include <functional>
#include "json.hpp"

using json = nlohmann::json;

struct UserSession {
    std::string username;
    std::string displayName;
    std::string token;
    bool authenticated = false;
};

class Auth {
public:
    static Auth& instance();

    bool login(const std::string& username, const std::string& password);
    void logout();
    bool isAuthenticated() const;
    const UserSession& currentUser() const;

    void setLoginEndpoint(const std::string& endpoint);

    using AuthCallback = std::function<void(bool success)>;
    void onAuthChanged(AuthCallback callback);

private:
    Auth();
    ~Auth() = default;
    Auth(const Auth&) = delete;
    Auth& operator=(const Auth&) = delete;

    UserSession session_;
    std::string loginEndpoint_;
    std::vector<AuthCallback> callbacks_;
};

#endif // AUTH_H
