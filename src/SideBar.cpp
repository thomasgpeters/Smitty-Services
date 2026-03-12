#include "SideBar.h"

SideBar::SideBar()
    : collapsed_(false) {
    setStyleClass("sidebar");

    addButton("\xF0\x9F\x93\x8A", "Dashboard", "dashboard");  // chart icon
    addSeparator();
    addButton("\xF0\x9F\x91\xA5", "Customers", "customers");   // people icon
    addButton("\xF0\x9F\x93\x8B", "Orders", "orders");          // clipboard icon
    addButton("\xF0\x9F\x93\xA6", "Products", "products");      // package icon
    addSeparator();
    addButton("\xF0\x9F\x94\xA7", "Jobs", "jobs");              // wrench icon
    addButton("\xF0\x9F\x9A\x9B", "Vehicles", "vehicles");      // truck icon
    addButton("\xF0\x9F\x9B\x92", "Purchases", "purchases");    // shopping cart icon
    addSeparator();
    addButton("\xE2\x9A\x99", "Settings", "settings");          // gear icon
}

void SideBar::setNavigationCallback(std::function<void(const std::string&)> callback) {
    navigationCallback_ = callback;
}

void SideBar::setActivePage(const std::string& page) {
    for (auto& pair : buttons_) {
        if (pair.first == page) {
            pair.second->setStyleClass("sidebar-btn active");
        } else {
            pair.second->setStyleClass("sidebar-btn");
        }
    }
}

void SideBar::toggleCollapsed() {
    collapsed_ = !collapsed_;
    if (collapsed_) {
        setStyleClass("sidebar collapsed");
    } else {
        setStyleClass("sidebar");
    }
}

void SideBar::addButton(const std::string& icon, const std::string& label, const std::string& page) {
    auto btn = addWidget(std::make_unique<Wt::WPushButton>());
    btn->setStyleClass("sidebar-btn");
    btn->setTextFormat(Wt::TextFormat::XHTML);
    btn->setText("<span class='icon'>" + icon + "</span> " + label);
    btn->clicked().connect([this, page] {
        setActivePage(page);
        if (navigationCallback_) navigationCallback_(page);
    });
    buttons_[page] = btn;
}

void SideBar::addSeparator() {
    auto sep = addWidget(std::make_unique<Wt::WContainerWidget>());
    sep->setStyleClass("sidebar-separator");
}
