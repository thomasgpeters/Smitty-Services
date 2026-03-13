#include "EntityDetailView.h"
#include "EntityRegistry.h"
#include "ApiClient.h"
#include <Wt/WText.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <map>

class ProductDetail : public EntityDetailView {
public:
    ProductDetail()
        : EntityDetailView(EntityRegistry::instance().getEntity("Product")) {
    }

protected:
    void populateFields(const json& record) override {
        fieldEdits_.clear();

        for (const auto& col : entity_->columns()) {
            auto group = fieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
            group->setStyleClass("form-group");

            auto label = group->addWidget(std::make_unique<Wt::WText>(col.label));
            label->setStyleClass("form-label");

            std::string val = entity_->getFieldValue(record, col.name);

            if (col.name == entity_->primaryKey()) {
                auto value = group->addWidget(std::make_unique<Wt::WText>(
                    val.empty() ? "-" : val, Wt::TextFormat::Plain));
                value->setStyleClass("form-value");
            } else {
                auto input = group->addWidget(std::make_unique<Wt::WLineEdit>());
                input->setStyleClass("form-input");
                if (!val.empty() && val != "-") {
                    input->setText(val);
                }
                fieldEdits_[col.name] = input;
            }
        }

        auto saveBar = fieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
        saveBar->setStyleClass("detail-save-bar");

        saveStatus_ = saveBar->addWidget(std::make_unique<Wt::WText>());
        saveStatus_->setStyleClass("detail-save-status");

        auto saveBtn = saveBar->addWidget(std::make_unique<Wt::WPushButton>("Save"));
        saveBtn->setStyleClass("action-btn");
        saveBtn->clicked().connect(this, &ProductDetail::saveRecord);
    }

private:
    void saveRecord() {
        json attrs;
        for (const auto& col : entity_->columns()) {
            if (col.name == entity_->primaryKey()) continue;
            auto it = fieldEdits_.find(col.name);
            if (it == fieldEdits_.end()) continue;

            std::string val = it->second->text().toUTF8();
            if (val.empty()) continue;

            if (col.type == "FLOAT") {
                try { attrs[col.name] = std::stod(val); }
                catch (...) { attrs[col.name] = val; }
            } else if (col.type == "SMALLINT" || col.type == "INTEGER") {
                try { attrs[col.name] = std::stoi(val); }
                catch (...) { attrs[col.name] = val; }
            } else {
                attrs[col.name] = val;
            }
        }

        try {
            auto resp = ApiClient::instance().updateRecord(
                entity_->resourceName(), currentRecordId_, attrs);
            if (resp.ok()) {
                saveStatus_->setText("Saved successfully.");
                saveStatus_->setStyleClass("detail-save-status detail-save-success");
            } else {
                saveStatus_->setText("Error: " + resp.errorMessage());
                saveStatus_->setStyleClass("detail-save-status detail-save-error");
            }
        } catch (const std::exception& e) {
            saveStatus_->setText(std::string("Error: ") + e.what());
            saveStatus_->setStyleClass("detail-save-status detail-save-error");
        }
    }

    std::map<std::string, Wt::WLineEdit*> fieldEdits_;
    Wt::WText* saveStatus_ = nullptr;
};

std::unique_ptr<EntityDetailView> createProductDetail() {
    return std::make_unique<ProductDetail>();
}
