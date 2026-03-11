#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>

class Dashboard : public Wt::WContainerWidget {
public:
    Dashboard();

    void refresh();

private:
    void buildUI();
    Wt::WText* customerCount_;
    Wt::WText* orderCount_;
    Wt::WText* productCount_;
};

#endif // DASHBOARD_H
