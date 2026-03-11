#ifndef NAVBAR_H
#define NAVBAR_H

#include <Wt/WContainerWidget.h>
#include <functional>

class NavBar : public Wt::WContainerWidget {
public:
    NavBar();

    void setSidebarToggleCallback(std::function<void()> callback);

private:
    std::function<void()> sidebarToggleCallback_;
};

#endif // NAVBAR_H
