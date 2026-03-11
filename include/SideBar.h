#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <Wt/WContainerWidget.h>
#include <Wt/WPushButton.h>
#include <functional>
#include <string>
#include <map>

class SideBar : public Wt::WContainerWidget {
public:
    SideBar();

    void setNavigationCallback(std::function<void(const std::string&)> callback);
    void setActivePage(const std::string& page);
    void toggleCollapsed();

private:
    void addButton(const std::string& icon, const std::string& label, const std::string& page);
    void addSeparator();

    std::function<void(const std::string&)> navigationCallback_;
    std::map<std::string, Wt::WPushButton*> buttons_;
    bool collapsed_;
};

#endif // SIDEBAR_H
