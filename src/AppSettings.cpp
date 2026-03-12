#include "AppSettings.h"
#include <cstdlib>

AppSettings& AppSettings::instance() {
    static AppSettings settings;
    return settings;
}

AppSettings::AppSettings()
    : apiEndpoint_(std::getenv("ALS_API_URL")
                   ? std::getenv("ALS_API_URL")
                   : (std::getenv("SMITTY_API_ENDPOINT")
                      ? std::getenv("SMITTY_API_ENDPOINT")
                      : "http://localhost:5656/api"))
    , dateFormat_("YYYY-MM-DD")
    , currencySymbol_("$")
    , locale_("en")
    , theme_("light")
    , maxListColumns_(8) {
}

std::string AppSettings::apiEndpoint() const { return apiEndpoint_; }
void AppSettings::setApiEndpoint(const std::string& url) { apiEndpoint_ = url; }

std::string AppSettings::dateFormat() const { return dateFormat_; }
void AppSettings::setDateFormat(const std::string& fmt) { dateFormat_ = fmt; }

std::string AppSettings::currencySymbol() const { return currencySymbol_; }
void AppSettings::setCurrencySymbol(const std::string& sym) { currencySymbol_ = sym; }

std::string AppSettings::locale() const { return locale_; }
void AppSettings::setLocale(const std::string& loc) { locale_ = loc; }

std::string AppSettings::theme() const { return theme_; }
void AppSettings::setTheme(const std::string& theme) { theme_ = theme; }

int AppSettings::maxListColumns() const { return maxListColumns_; }
void AppSettings::setMaxListColumns(int n) { maxListColumns_ = n; }
