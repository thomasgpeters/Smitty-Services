#include "SettingsView.h"
#include "AppSettings.h"
#include <Wt/WPushButton.h>
#include <Wt/WText.h>

SettingsView::SettingsView() {
    buildUI();
}

void SettingsView::buildUI() {
    // Page header
    auto header = addWidget(std::make_unique<Wt::WContainerWidget>());
    header->setStyleClass("page-header");
    header->addWidget(std::make_unique<Wt::WText>("Settings"))->setStyleClass("page-title");

    auto& settings = AppSettings::instance();

    // API section
    auto section = addWidget(std::make_unique<Wt::WContainerWidget>());
    section->setStyleClass("settings-section");
    section->addWidget(std::make_unique<Wt::WText>("API Configuration"))->setStyleClass("settings-title");

    // API Endpoint
    auto row1 = section->addWidget(std::make_unique<Wt::WContainerWidget>());
    row1->setStyleClass("settings-row");
    row1->addWidget(std::make_unique<Wt::WText>("API Endpoint"))->setStyleClass("settings-label");
    apiEndpointInput_ = row1->addWidget(std::make_unique<Wt::WLineEdit>(settings.apiEndpoint()));
    apiEndpointInput_->setStyleClass("filter-input");

    // Date Format
    auto row2 = section->addWidget(std::make_unique<Wt::WContainerWidget>());
    row2->setStyleClass("settings-row");
    row2->addWidget(std::make_unique<Wt::WText>("Date Format"))->setStyleClass("settings-label");
    dateFormatInput_ = row2->addWidget(std::make_unique<Wt::WLineEdit>(settings.dateFormat()));
    dateFormatInput_->setStyleClass("filter-input");

    // Currency Symbol
    auto row3 = section->addWidget(std::make_unique<Wt::WContainerWidget>());
    row3->setStyleClass("settings-row");
    row3->addWidget(std::make_unique<Wt::WText>("Currency Symbol"))->setStyleClass("settings-label");
    currencyInput_ = row3->addWidget(std::make_unique<Wt::WLineEdit>(settings.currencySymbol()));
    currencyInput_->setStyleClass("filter-input");

    // Display section
    auto displaySection = addWidget(std::make_unique<Wt::WContainerWidget>());
    displaySection->setStyleClass("settings-section");
    displaySection->addWidget(std::make_unique<Wt::WText>("Display"))->setStyleClass("settings-title");

    auto themeRow = displaySection->addWidget(std::make_unique<Wt::WContainerWidget>());
    themeRow->setStyleClass("settings-row");
    themeRow->addWidget(std::make_unique<Wt::WText>("Theme"))->setStyleClass("settings-label");
    themeRow->addWidget(std::make_unique<Wt::WText>(settings.theme()))->setStyleClass("settings-value");

    auto localeRow = displaySection->addWidget(std::make_unique<Wt::WContainerWidget>());
    localeRow->setStyleClass("settings-row");
    localeRow->addWidget(std::make_unique<Wt::WText>("Locale"))->setStyleClass("settings-label");
    localeRow->addWidget(std::make_unique<Wt::WText>(settings.locale()))->setStyleClass("settings-value");

    // Save button
    auto saveBtn = addWidget(std::make_unique<Wt::WPushButton>("Save Settings"));
    saveBtn->setStyleClass("filter-btn");
    saveBtn->setMargin(20, Wt::Side::Top);
    saveBtn->clicked().connect(this, &SettingsView::saveSettings);
}

void SettingsView::saveSettings() {
    auto& settings = AppSettings::instance();
    settings.setApiEndpoint(apiEndpointInput_->text().toUTF8());
    settings.setDateFormat(dateFormatInput_->text().toUTF8());
    settings.setCurrencySymbol(currencyInput_->text().toUTF8());
}
