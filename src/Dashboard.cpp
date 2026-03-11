#include "Dashboard.h"
#include "ApiClient.h"
#include "AppSettings.h"
#include <Wt/WPushButton.h>
#include <ctime>
#include <cstring>

Dashboard::Dashboard()
    : customerCount_(nullptr)
    , orderCount_(nullptr)
    , productCount_(nullptr)
    , unpaidRevenueValue_(nullptr)
    , outstandingPOValue_(nullptr)
    , agingWeekValue_(nullptr)
    , agingMonthValue_(nullptr)
    , agingQuarterValue_(nullptr) {
    buildUI();
}

void Dashboard::buildUI() {
    // Page header
    auto header = addWidget(std::make_unique<Wt::WContainerWidget>());
    header->setStyleClass("page-header");
    header->addWidget(std::make_unique<Wt::WText>("Dashboard"))->setStyleClass("page-title");

    // Cards container
    auto cards = addWidget(std::make_unique<Wt::WContainerWidget>());
    cards->setStyleClass("dashboard-cards");

    // Customer card
    auto custCard = cards->addWidget(std::make_unique<Wt::WContainerWidget>());
    custCard->setStyleClass("dash-card customers");
    customerCount_ = custCard->addWidget(std::make_unique<Wt::WText>("--"));
    customerCount_->setStyleClass("dash-card-value");
    custCard->addWidget(std::make_unique<Wt::WText>("Customers"))->setStyleClass("dash-card-title");

    // Order card
    auto orderCard = cards->addWidget(std::make_unique<Wt::WContainerWidget>());
    orderCard->setStyleClass("dash-card orders");
    orderCount_ = orderCard->addWidget(std::make_unique<Wt::WText>("--"));
    orderCount_->setStyleClass("dash-card-value");
    orderCard->addWidget(std::make_unique<Wt::WText>("Orders"))->setStyleClass("dash-card-title");

    // Product card
    auto prodCard = cards->addWidget(std::make_unique<Wt::WContainerWidget>());
    prodCard->setStyleClass("dash-card products");
    productCount_ = prodCard->addWidget(std::make_unique<Wt::WText>("--"));
    productCount_->setStyleClass("dash-card-value");
    prodCard->addWidget(std::make_unique<Wt::WText>("Products"))->setStyleClass("dash-card-title");

    // Unpaid Revenue card
    auto unpaidCard = cards->addWidget(std::make_unique<Wt::WContainerWidget>());
    unpaidCard->setStyleClass("dash-card unpaid-revenue");
    unpaidRevenueValue_ = unpaidCard->addWidget(std::make_unique<Wt::WText>("--"));
    unpaidRevenueValue_->setStyleClass("dash-card-value");
    unpaidCard->addWidget(std::make_unique<Wt::WText>("Unpaid Revenue"))->setStyleClass("dash-card-title");

    // Outstanding Purchase Orders card
    auto poCard = cards->addWidget(std::make_unique<Wt::WContainerWidget>());
    poCard->setStyleClass("dash-card outstanding-po");
    outstandingPOValue_ = poCard->addWidget(std::make_unique<Wt::WText>("--"));
    outstandingPOValue_->setStyleClass("dash-card-value");
    poCard->addWidget(std::make_unique<Wt::WText>("Outstanding POs"))->setStyleClass("dash-card-title");

    // Aging section header
    auto agingHeader = addWidget(std::make_unique<Wt::WContainerWidget>());
    agingHeader->setStyleClass("page-header");
    agingHeader->addWidget(std::make_unique<Wt::WText>("Order Aging"))->setStyleClass("page-title");

    auto agingCards = addWidget(std::make_unique<Wt::WContainerWidget>());
    agingCards->setStyleClass("dashboard-cards");

    // Aging: 1 quarter
    auto quarterCard = agingCards->addWidget(std::make_unique<Wt::WContainerWidget>());
    quarterCard->setStyleClass("dash-card aging-quarter");
    agingQuarterValue_ = quarterCard->addWidget(std::make_unique<Wt::WText>("--"));
    agingQuarterValue_->setStyleClass("dash-card-value");
    quarterCard->addWidget(std::make_unique<Wt::WText>("Aging > 1 Quarter"))->setStyleClass("dash-card-title");

    // Aging: 1 month
    auto monthCard = agingCards->addWidget(std::make_unique<Wt::WContainerWidget>());
    monthCard->setStyleClass("dash-card aging-month");
    agingMonthValue_ = monthCard->addWidget(std::make_unique<Wt::WText>("--"));
    agingMonthValue_->setStyleClass("dash-card-value");
    monthCard->addWidget(std::make_unique<Wt::WText>("Aging > 1 Month"))->setStyleClass("dash-card-title");

    // Aging: 1 week
    auto weekCard = agingCards->addWidget(std::make_unique<Wt::WContainerWidget>());
    weekCard->setStyleClass("dash-card aging-week");
    agingWeekValue_ = weekCard->addWidget(std::make_unique<Wt::WText>("--"));
    agingWeekValue_->setStyleClass("dash-card-value");
    weekCard->addWidget(std::make_unique<Wt::WText>("Aging > 1 Week"))->setStyleClass("dash-card-title");

    // Refresh button
    auto refreshBtn = addWidget(std::make_unique<Wt::WPushButton>("Refresh"));
    refreshBtn->setStyleClass("filter-btn");
    refreshBtn->clicked().connect(this, &Dashboard::refresh);

    refresh();
}

// Helper: parse a date string "YYYY-MM-DD" into a time_t
static time_t parseDate(const std::string& dateStr) {
    struct tm tm;
    memset(&tm, 0, sizeof(tm));
    if (dateStr.size() >= 10) {
        tm.tm_year = std::stoi(dateStr.substr(0, 4)) - 1900;
        tm.tm_mon  = std::stoi(dateStr.substr(5, 2)) - 1;
        tm.tm_mday = std::stoi(dateStr.substr(8, 2));
    }
    return mktime(&tm);
}

void Dashboard::refresh() {
    const auto& currency = AppSettings::instance().currencySymbol();

    // --- Customers ---
    try {
        auto custResp = ApiClient::instance().fetchAll("Customer");
        if (custResp.ok() && custResp.hasData() && custResp.data().is_array()) {
            customerCount_->setText(std::to_string(custResp.data().size()));
        } else {
            customerCount_->setText("N/A");
        }
    } catch (...) {
        customerCount_->setText("N/A");
    }

    // --- Orders (total count + unpaid revenue + aging) ---
    try {
        auto orderResp = ApiClient::instance().fetchAll("Order");
        if (orderResp.ok() && orderResp.hasData() && orderResp.data().is_array()) {
            const auto& orders = orderResp.data();
            orderCount_->setText(std::to_string(orders.size()));

            // Compute unpaid revenue and aging from orders with no shipped_date
            double unpaidTotal = 0.0;
            int agingWeek = 0, agingMonth = 0, agingQuarter = 0;

            time_t now = time(nullptr);
            const double secsPerDay = 86400.0;

            for (const auto& order : orders) {
                // Unpaid = not shipped
                bool isUnpaid = false;
                if (!order.contains("attributes")) continue;
                const auto& attrs = order["attributes"];

                bool shipped = attrs.contains("shipped_date")
                    && !attrs["shipped_date"].is_null()
                    && attrs["shipped_date"].get<std::string>() != "";
                if (!shipped) {
                    isUnpaid = true;
                }

                if (isUnpaid) {
                    // Sum freight as approximate order value
                    if (attrs.contains("freight") && !attrs["freight"].is_null()) {
                        try {
                            unpaidTotal += attrs["freight"].get<double>();
                        } catch (...) {}
                    }

                    // Aging calculation based on order_date
                    if (attrs.contains("order_date") && !attrs["order_date"].is_null()) {
                        std::string dateStr = attrs["order_date"].get<std::string>();
                        if (!dateStr.empty()) {
                            time_t orderTime = parseDate(dateStr);
                            double daysOld = difftime(now, orderTime) / secsPerDay;
                            if (daysOld > 90) agingQuarter++;
                            else if (daysOld > 30) agingMonth++;
                            else if (daysOld > 7) agingWeek++;
                        }
                    }
                }
            }

            char buf[64];
            snprintf(buf, sizeof(buf), "%s%.2f", currency.c_str(), unpaidTotal);
            unpaidRevenueValue_->setText(buf);

            agingWeekValue_->setText(std::to_string(agingWeek));
            agingMonthValue_->setText(std::to_string(agingMonth));
            agingQuarterValue_->setText(std::to_string(agingQuarter));
        } else {
            orderCount_->setText("N/A");
            unpaidRevenueValue_->setText("N/A");
            agingWeekValue_->setText("N/A");
            agingMonthValue_->setText("N/A");
            agingQuarterValue_->setText("N/A");
        }
    } catch (...) {
        orderCount_->setText("N/A");
        unpaidRevenueValue_->setText("N/A");
        agingWeekValue_->setText("N/A");
        agingMonthValue_->setText("N/A");
        agingQuarterValue_->setText("N/A");
    }

    // --- Products (total count + outstanding POs) ---
    try {
        auto prodResp = ApiClient::instance().fetchAll("Product");
        if (prodResp.ok() && prodResp.hasData() && prodResp.data().is_array()) {
            const auto& products = prodResp.data();
            productCount_->setText(std::to_string(products.size()));

            int outstandingCount = 0;
            for (const auto& product : products) {
                if (!product.contains("attributes")) continue;
                const auto& attrs = product["attributes"];
                if (attrs.contains("units_on_order") && !attrs["units_on_order"].is_null()) {
                    try {
                        int onOrder = attrs["units_on_order"].get<int>();
                        if (onOrder > 0) outstandingCount++;
                    } catch (...) {}
                }
            }
            outstandingPOValue_->setText(std::to_string(outstandingCount));
        } else {
            productCount_->setText("N/A");
            outstandingPOValue_->setText("N/A");
        }
    } catch (...) {
        productCount_->setText("N/A");
        outstandingPOValue_->setText("N/A");
    }
}
