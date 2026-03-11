#include "Dashboard.h"
#include "ApiClient.h"
#include <Wt/WPushButton.h>

Dashboard::Dashboard()
    : customerCount_(nullptr)
    , orderCount_(nullptr)
    , productCount_(nullptr) {
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
    custCard->addWidget(std::make_unique<Wt::WText>("Customers"))->setStyleClass("dash-card-title");
    customerCount_ = custCard->addWidget(std::make_unique<Wt::WText>("--"));
    customerCount_->setStyleClass("dash-card-value");

    // Order card
    auto orderCard = cards->addWidget(std::make_unique<Wt::WContainerWidget>());
    orderCard->setStyleClass("dash-card orders");
    orderCard->addWidget(std::make_unique<Wt::WText>("Orders"))->setStyleClass("dash-card-title");
    orderCount_ = orderCard->addWidget(std::make_unique<Wt::WText>("--"));
    orderCount_->setStyleClass("dash-card-value");

    // Product card
    auto prodCard = cards->addWidget(std::make_unique<Wt::WContainerWidget>());
    prodCard->setStyleClass("dash-card products");
    prodCard->addWidget(std::make_unique<Wt::WText>("Products"))->setStyleClass("dash-card-title");
    productCount_ = prodCard->addWidget(std::make_unique<Wt::WText>("--"));
    productCount_->setStyleClass("dash-card-value");

    // Refresh button
    auto refreshBtn = addWidget(std::make_unique<Wt::WPushButton>("Refresh"));
    refreshBtn->setStyleClass("filter-btn");
    refreshBtn->clicked().connect(this, &Dashboard::refresh);

    refresh();
}

void Dashboard::refresh() {
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

    try {
        auto orderResp = ApiClient::instance().fetchAll("Order");
        if (orderResp.ok() && orderResp.hasData() && orderResp.data().is_array()) {
            orderCount_->setText(std::to_string(orderResp.data().size()));
        } else {
            orderCount_->setText("N/A");
        }
    } catch (...) {
        orderCount_->setText("N/A");
    }

    try {
        auto prodResp = ApiClient::instance().fetchAll("Product");
        if (prodResp.ok() && prodResp.hasData() && prodResp.data().is_array()) {
            productCount_->setText(std::to_string(prodResp.data().size()));
        } else {
            productCount_->setText("N/A");
        }
    } catch (...) {
        productCount_->setText("N/A");
    }
}
