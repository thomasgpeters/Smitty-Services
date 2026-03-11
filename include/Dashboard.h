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

    // Existing count cards
    Wt::WText* customerCount_;
    Wt::WText* orderCount_;
    Wt::WText* productCount_;

    // New stat cards
    Wt::WText* unpaidRevenueValue_;
    Wt::WText* outstandingPOValue_;
    Wt::WText* agingWeekValue_;
    Wt::WText* agingMonthValue_;
    Wt::WText* agingQuarterValue_;
};

#endif // DASHBOARD_H
