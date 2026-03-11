#include "EntityDetailView.h"
#include "ApiClient.h"
#include "AppSettings.h"
#include <Wt/WPushButton.h>

EntityDetailView::EntityDetailView(std::shared_ptr<Entity> entity)
    : entity_(entity)
    , fieldsContainer_(nullptr)
    , titleText_(nullptr) {
    buildUI();
}

void EntityDetailView::setBackCallback(std::function<void()> callback) {
    backCallback_ = callback;
}

void EntityDetailView::buildUI() {
    // Back button
    auto backBtn = addWidget(std::make_unique<Wt::WPushButton>("\xE2\x86\x90 Back to List"));
    backBtn->setStyleClass("back-btn");
    backBtn->clicked().connect([this] {
        if (backCallback_) backCallback_();
    });

    // Page header
    auto header = addWidget(std::make_unique<Wt::WContainerWidget>());
    header->setStyleClass("page-header");
    titleText_ = header->addWidget(std::make_unique<Wt::WText>(entity_->displayName() + " Detail"));
    titleText_->setStyleClass("page-title");

    // Fields container
    fieldsContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    fieldsContainer_->setStyleClass("detail-form");
}

void EntityDetailView::loadRecord(const std::string& id) {
    fieldsContainer_->clear();

    try {
        auto resp = ApiClient::instance().fetchOne(entity_->resourceName(), id);
        if (!resp.ok()) {
            fieldsContainer_->addWidget(
                std::make_unique<Wt::WText>("API error: " + resp.errorMessage(), Wt::TextFormat::Plain));
            return;
        }
        if (resp.hasData()) {
            populateFields(resp.data());
        }
    } catch (const std::exception& e) {
        fieldsContainer_->addWidget(
            std::make_unique<Wt::WText>(std::string("Error loading record: ") + e.what(), Wt::TextFormat::Plain));
    }
}

void EntityDetailView::populateFields(const json& record) {
    for (const auto& col : entity_->columns()) {
        auto group = fieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
        group->setStyleClass("form-group");

        auto label = group->addWidget(std::make_unique<Wt::WText>(col.label));
        label->setStyleClass("form-label");

        std::string val = entity_->getFieldValue(record, col.name);
        std::string formatted = formatFieldValue(col, val);

        auto value = group->addWidget(std::make_unique<Wt::WText>(formatted, Wt::TextFormat::Plain));
        value->setStyleClass("form-value");
    }
}

std::string EntityDetailView::formatFieldValue(const ColumnDef& col, const std::string& value) {
    if (value.empty()) return "-";
    if (col.type == "FLOAT") {
        try {
            double d = std::stod(value);
            char buf[64];
            snprintf(buf, sizeof(buf), "%.2f", d);
            return AppSettings::instance().currencySymbol() + std::string(buf);
        } catch (...) {
            return value;
        }
    }
    return value;
}
