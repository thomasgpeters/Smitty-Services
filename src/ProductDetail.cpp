#include "EntityDetailView.h"
#include "EntityRegistry.h"
#include "ApiClient.h"
#include <Wt/WText.h>
#include <Wt/WLineEdit.h>
#include <Wt/WCheckBox.h>
#include <Wt/WComboBox.h>
#include <Wt/WPushButton.h>
#include <map>

class ProductDetail : public EntityDetailView {
public:
    ProductDetail()
        : EntityDetailView(EntityRegistry::instance().getEntity("Product")) {
        loadLookups();
    }

protected:
    void populateFields(const json& record) override {
        fieldEdits_.clear();
        discontinuedCheck_ = nullptr;
        categoryCombo_ = nullptr;
        supplierCombo_ = nullptr;

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

            } else if (col.name == "discontinued") {
                discontinuedCheck_ = group->addWidget(std::make_unique<Wt::WCheckBox>("Yes"));
                discontinuedCheck_->setStyleClass("filter-checkbox");
                if (val == "1") {
                    discontinuedCheck_->setChecked(true);
                }

            } else if (col.name == "category_id") {
                categoryCombo_ = group->addWidget(std::make_unique<Wt::WComboBox>());
                categoryCombo_->setStyleClass("form-input");
                categoryCombo_->addItem("");
                int selectedIdx = 0;
                int idx = 1;
                for (const auto& pair : categoryNames_) {
                    categoryCombo_->addItem(pair.second);
                    if (pair.first == val) selectedIdx = idx;
                    idx++;
                }
                categoryCombo_->setCurrentIndex(selectedIdx);

            } else if (col.name == "supplier_id") {
                supplierCombo_ = group->addWidget(std::make_unique<Wt::WComboBox>());
                supplierCombo_->setStyleClass("form-input");
                supplierCombo_->addItem("");
                int selectedIdx = 0;
                int idx = 1;
                for (const auto& pair : supplierNames_) {
                    supplierCombo_->addItem(pair.second);
                    if (pair.first == val) selectedIdx = idx;
                    idx++;
                }
                supplierCombo_->setCurrentIndex(selectedIdx);

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
    void loadLookups() {
        try {
            auto resp = ApiClient::instance().fetchAll("Category");
            if (resp.ok() && resp.hasData() && resp.data().is_array()) {
                for (const auto& cat : resp.data()) {
                    std::string id = cat.contains("id")
                        ? (cat["id"].is_string() ? cat["id"].get<std::string>() : cat["id"].dump())
                        : "";
                    std::string name;
                    if (cat.contains("attributes") && cat["attributes"].contains("category_name"))
                        name = cat["attributes"]["category_name"].get<std::string>();
                    if (!id.empty() && !name.empty())
                        categoryNames_[id] = name;
                }
            }
        } catch (...) {}

        try {
            auto resp = ApiClient::instance().fetchAll("Supplier");
            if (resp.ok() && resp.hasData() && resp.data().is_array()) {
                for (const auto& sup : resp.data()) {
                    std::string id = sup.contains("id")
                        ? (sup["id"].is_string() ? sup["id"].get<std::string>() : sup["id"].dump())
                        : "";
                    std::string name;
                    if (sup.contains("attributes") && sup["attributes"].contains("company_name"))
                        name = sup["attributes"]["company_name"].get<std::string>();
                    if (!id.empty() && !name.empty())
                        supplierNames_[id] = name;
                }
            }
        } catch (...) {}
    }

    void saveRecord() {
        json attrs;

        // Discontinued checkbox
        if (discontinuedCheck_) {
            attrs["discontinued"] = discontinuedCheck_->isChecked() ? 1 : 0;
        }

        // Category dropdown
        if (categoryCombo_ && categoryCombo_->currentIndex() > 0) {
            std::string catName = categoryCombo_->currentText().toUTF8();
            for (const auto& pair : categoryNames_) {
                if (pair.second == catName) {
                    try { attrs["category_id"] = std::stoi(pair.first); }
                    catch (...) { attrs["category_id"] = pair.first; }
                    break;
                }
            }
        }

        // Supplier dropdown
        if (supplierCombo_ && supplierCombo_->currentIndex() > 0) {
            std::string supName = supplierCombo_->currentText().toUTF8();
            for (const auto& pair : supplierNames_) {
                if (pair.second == supName) {
                    try { attrs["supplier_id"] = std::stoi(pair.first); }
                    catch (...) { attrs["supplier_id"] = pair.first; }
                    break;
                }
            }
        }

        // Regular text fields
        for (const auto& col : entity_->columns()) {
            if (col.name == entity_->primaryKey()) continue;
            if (col.name == "discontinued" || col.name == "category_id" || col.name == "supplier_id") continue;

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
    Wt::WCheckBox* discontinuedCheck_ = nullptr;
    Wt::WComboBox* categoryCombo_ = nullptr;
    Wt::WComboBox* supplierCombo_ = nullptr;
    Wt::WText* saveStatus_ = nullptr;
    std::map<std::string, std::string> categoryNames_;
    std::map<std::string, std::string> supplierNames_;
};

std::unique_ptr<EntityDetailView> createProductDetail() {
    return std::make_unique<ProductDetail>();
}
