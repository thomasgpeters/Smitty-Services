#include "SmittyApplication.h"
#include "NavBar.h"
#include "SideBar.h"
#include "Footer.h"
#include "Dashboard.h"
#include "EntityListView.h"
#include "EntityDetailView.h"
#include "EntityRegistry.h"
#include "SettingsView.h"
#include "AppSettings.h"
#include "ApiClient.h"
#include <Wt/WStackedWidget.h>
#include <Wt/WBootstrapTheme.h>

// Factory functions from entity-specific files
extern std::unique_ptr<EntityListView> createCustomerList();
extern std::unique_ptr<EntityDetailView> createCustomerDetail();
extern std::unique_ptr<EntityListView> createOrderList();
extern std::unique_ptr<EntityDetailView> createOrderDetail();
extern std::unique_ptr<EntityListView> createProductList();
extern std::unique_ptr<EntityDetailView> createProductDetail();
extern std::unique_ptr<EntityListView> createJobList();
extern std::unique_ptr<EntityDetailView> createJobDetail();
extern std::unique_ptr<EntityListView> createVehicleList();
extern std::unique_ptr<EntityDetailView> createVehicleDetail();

SmittyApplication::SmittyApplication(const Wt::WEnvironment& env)
    : Wt::WApplication(env)
    , mainWrapper_(nullptr)
    , sideBar_(nullptr)
    , contentStack_(nullptr)
    , currentPage_("dashboard") {
    setTitle("Smitty Services");
    useStyleSheet("css/smitty.css");

    // Sync API client with settings
    ApiClient::instance().setBaseUrl(AppSettings::instance().apiEndpoint());

    createLayout();
}

void SmittyApplication::createLayout() {
    auto appContainer = root()->addWidget(std::make_unique<Wt::WContainerWidget>());
    appContainer->setStyleClass("app-container");

    // Navbar
    auto navBar = appContainer->addWidget(std::make_unique<NavBar>());
    navBar->setSidebarToggleCallback([this] {
        sideBar_->toggleCollapsed();
    });

    // Main wrapper (sidebar + content)
    mainWrapper_ = appContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    mainWrapper_->setStyleClass("main-wrapper");

    // Sidebar
    sideBar_ = mainWrapper_->addWidget(std::make_unique<SideBar>());
    sideBar_->setNavigationCallback([this](const std::string& page) {
        navigateTo(page);
    });
    sideBar_->setActivePage("dashboard");

    // Content area
    auto contentArea = mainWrapper_->addWidget(std::make_unique<Wt::WContainerWidget>());
    contentArea->setStyleClass("content-area");
    contentStack_ = contentArea->addWidget(std::make_unique<Wt::WStackedWidget>());

    // === Add pages to stack ===

    // Dashboard (index 0)
    auto dashboard = std::make_unique<Dashboard>();
    dashboardIdx_ = contentStack_->count();
    contentStack_->addWidget(std::move(dashboard));

    // Customer List (index 1)
    auto custList = createCustomerList();
    customerListIdx_ = contentStack_->count();
    auto custListPtr = custList.get();
    contentStack_->addWidget(std::move(custList));

    // Customer Detail (index 2)
    auto custDetail = createCustomerDetail();
    customerDetailIdx_ = contentStack_->count();
    auto custDetailPtr = custDetail.get();
    contentStack_->addWidget(std::move(custDetail));

    // Wire customer list -> detail navigation
    custListPtr->setRowClickCallback([this, custDetailPtr](const std::string& id) {
        custDetailPtr->loadRecord(id);
        contentStack_->setCurrentIndex(customerDetailIdx_);
    });
    custDetailPtr->setBackCallback([this] {
        contentStack_->setCurrentIndex(customerListIdx_);
    });

    // Order List (index 3)
    auto ordList = createOrderList();
    orderListIdx_ = contentStack_->count();
    auto ordListPtr = ordList.get();
    contentStack_->addWidget(std::move(ordList));

    // Order Detail (index 4)
    auto ordDetail = createOrderDetail();
    orderDetailIdx_ = contentStack_->count();
    auto ordDetailPtr = ordDetail.get();
    contentStack_->addWidget(std::move(ordDetail));

    ordListPtr->setRowClickCallback([this, ordDetailPtr](const std::string& id) {
        ordDetailPtr->loadRecord(id);
        contentStack_->setCurrentIndex(orderDetailIdx_);
    });
    ordDetailPtr->setBackCallback([this] {
        contentStack_->setCurrentIndex(orderListIdx_);
    });

    // Product List (index 5)
    auto prodList = createProductList();
    productListIdx_ = contentStack_->count();
    auto prodListPtr = prodList.get();
    contentStack_->addWidget(std::move(prodList));

    // Product Detail (index 6)
    auto prodDetail = createProductDetail();
    productDetailIdx_ = contentStack_->count();
    auto prodDetailPtr = prodDetail.get();
    contentStack_->addWidget(std::move(prodDetail));

    prodListPtr->setRowClickCallback([this, prodDetailPtr](const std::string& id) {
        prodDetailPtr->loadRecord(id);
        contentStack_->setCurrentIndex(productDetailIdx_);
    });
    prodDetailPtr->setBackCallback([this] {
        contentStack_->setCurrentIndex(productListIdx_);
    });

    // Job List
    auto jobList = createJobList();
    jobListIdx_ = contentStack_->count();
    auto jobListPtr = jobList.get();
    contentStack_->addWidget(std::move(jobList));

    // Job Detail
    auto jobDetail = createJobDetail();
    jobDetailIdx_ = contentStack_->count();
    auto jobDetailPtr = jobDetail.get();
    contentStack_->addWidget(std::move(jobDetail));

    jobListPtr->setRowClickCallback([this, jobDetailPtr](const std::string& id) {
        jobDetailPtr->loadRecord(id);
        contentStack_->setCurrentIndex(jobDetailIdx_);
    });
    jobDetailPtr->setBackCallback([this] {
        contentStack_->setCurrentIndex(jobListIdx_);
    });

    // Vehicle List
    auto vehList = createVehicleList();
    vehicleListIdx_ = contentStack_->count();
    auto vehListPtr = vehList.get();
    contentStack_->addWidget(std::move(vehList));

    // Vehicle Detail
    auto vehDetail = createVehicleDetail();
    vehicleDetailIdx_ = contentStack_->count();
    auto vehDetailPtr = vehDetail.get();
    contentStack_->addWidget(std::move(vehDetail));

    vehListPtr->setRowClickCallback([this, vehDetailPtr](const std::string& id) {
        vehDetailPtr->loadRecord(id);
        contentStack_->setCurrentIndex(vehicleDetailIdx_);
    });
    vehDetailPtr->setBackCallback([this] {
        contentStack_->setCurrentIndex(vehicleListIdx_);
    });

    // Settings
    settingsIdx_ = contentStack_->count();
    contentStack_->addWidget(std::make_unique<SettingsView>());

    // Start on dashboard
    contentStack_->setCurrentIndex(dashboardIdx_);
}

void SmittyApplication::navigateTo(const std::string& page) {
    currentPage_ = page;

    if (page == "dashboard") {
        contentStack_->setCurrentIndex(dashboardIdx_);
    } else if (page == "customers") {
        contentStack_->setCurrentIndex(customerListIdx_);
    } else if (page == "orders") {
        contentStack_->setCurrentIndex(orderListIdx_);
    } else if (page == "products") {
        contentStack_->setCurrentIndex(productListIdx_);
    } else if (page == "jobs") {
        contentStack_->setCurrentIndex(jobListIdx_);
    } else if (page == "vehicles") {
        contentStack_->setCurrentIndex(vehicleListIdx_);
    } else if (page == "settings") {
        contentStack_->setCurrentIndex(settingsIdx_);
    }
}
