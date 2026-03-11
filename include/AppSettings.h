#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#include <string>
#include <map>

class AppSettings {
public:
    static AppSettings& instance();

    std::string apiEndpoint() const;
    void setApiEndpoint(const std::string& url);

    std::string dateFormat() const;
    void setDateFormat(const std::string& fmt);

    std::string currencySymbol() const;
    void setCurrencySymbol(const std::string& sym);

    std::string locale() const;
    void setLocale(const std::string& loc);

    std::string theme() const;
    void setTheme(const std::string& theme);

    int maxListColumns() const;
    void setMaxListColumns(int n);

private:
    AppSettings();
    ~AppSettings() = default;
    AppSettings(const AppSettings&) = delete;
    AppSettings& operator=(const AppSettings&) = delete;

    std::string apiEndpoint_;
    std::string dateFormat_;
    std::string currencySymbol_;
    std::string locale_;
    std::string theme_;
    int maxListColumns_;
};

#endif // APP_SETTINGS_H
