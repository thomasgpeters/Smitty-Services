#ifndef SMITTY_APPLICATION_H
#define SMITTY_APPLICATION_H

#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WStackedWidget.h>
#include <string>

class SideBar;

class SmittyApplication : public Wt::WApplication {
public:
    SmittyApplication(const Wt::WEnvironment& env);

    void navigateTo(const std::string& page);

private:
    void createLayout();

    Wt::WContainerWidget* mainWrapper_;
    SideBar* sideBar_;
    Wt::WStackedWidget* contentStack_;
    std::string currentPage_;

    // Page indices in the stacked widget
    int dashboardIdx_;
    int customerListIdx_;
    int customerDetailIdx_;
    int orderListIdx_;
    int orderDetailIdx_;
    int productListIdx_;
    int productDetailIdx_;
    int settingsIdx_;
};

#endif // SMITTY_APPLICATION_H
