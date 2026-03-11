#include "NavBar.h"
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WImage.h>

NavBar::NavBar() {
    setStyleClass("navbar");

    // Left section: toggle + logo + title
    auto left = addWidget(std::make_unique<Wt::WContainerWidget>());
    left->setStyleClass("navbar-left");

    auto toggleBtn = left->addWidget(std::make_unique<Wt::WPushButton>());
    toggleBtn->setStyleClass("sidebar-toggle");
    toggleBtn->setText("\u2630"); // hamburger icon
    toggleBtn->clicked().connect([this] {
        if (sidebarToggleCallback_) sidebarToggleCallback_();
    });

    auto logo = left->addWidget(std::make_unique<Wt::WText>());
    logo->setStyleClass("navbar-logo");
    logo->setText("\xF0\x9F\x94\xA7"); // wrench emoji as placeholder logo

    auto title = left->addWidget(std::make_unique<Wt::WText>("Smitty Services"));
    title->setStyleClass("navbar-title");

    // Right section: settings + account
    auto right = addWidget(std::make_unique<Wt::WContainerWidget>());
    right->setStyleClass("navbar-right");

    auto settingsBtn = right->addWidget(std::make_unique<Wt::WPushButton>("Settings"));
    settingsBtn->setStyleClass("navbar-btn");

    auto accountBtn = right->addWidget(std::make_unique<Wt::WPushButton>("Account"));
    accountBtn->setStyleClass("navbar-btn");
}

void NavBar::setSidebarToggleCallback(std::function<void()> callback) {
    sidebarToggleCallback_ = callback;
}
