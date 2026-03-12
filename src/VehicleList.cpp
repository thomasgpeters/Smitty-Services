#include "EntityListView.h"
#include "EntityRegistry.h"
#include "ApiClient.h"
#include <Wt/WCheckBox.h>
#include <Wt/WComboBox.h>
#include <Wt/WDialog.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <map>

class VehicleList : public EntityListView {
public:
    VehicleList()
        : EntityListView(EntityRegistry::instance().getEntity("Vehicle")) {
        loadLookups();
    }

protected:
    std::string includeParam() const override {
        return "customer";
    }

    std::string resolveFieldValue(const json& record, const ColumnDef& col,
                                   const std::string& rawValue) const override {
        if (col.name == "customer_id") {
            return lookupRelationship(record, "customer", "company_name");
        }
        return rawValue;
    }

    void addCustomFilters(Wt::WContainerWidget* filterBar) override {
        // Owner (Customer) filter
        ownerFilterCombo_ = filterBar->addWidget(std::make_unique<Wt::WComboBox>());
        ownerFilterCombo_->setStyleClass("filter-combo");
        ownerFilterCombo_->addItem("All Owners");
        for (const auto& pair : customerNames_) {
            ownerFilterCombo_->addItem(pair.second);
        }
        ownerFilterCombo_->changed().connect(this, &EntityListView::refresh);
    }

    void addActionButtons(Wt::WContainerWidget* actionBar) override {
        auto newBtn = actionBar->addWidget(
            std::make_unique<Wt::WPushButton>("New Vehicle"));
        newBtn->setStyleClass("action-btn");
        newBtn->clicked().connect(this, &VehicleList::showAddVehicleDialog);
    }

    bool filterRecord(const json& record) const override {
        if (!record.contains("attributes")) return false;
        const auto& attrs = record["attributes"];

        if (ownerFilterCombo_ && ownerFilterCombo_->currentIndex() > 0) {
            std::string selectedName = ownerFilterCombo_->currentText().toUTF8();
            std::string selectedId;
            for (const auto& pair : customerNames_) {
                if (pair.second == selectedName) { selectedId = pair.first; break; }
            }
            if (!selectedId.empty()) {
                std::string recordId;
                if (attrs.contains("customer_id") && !attrs["customer_id"].is_null())
                    recordId = attrs["customer_id"].is_string()
                        ? attrs["customer_id"].get<std::string>()
                        : attrs["customer_id"].dump();
                if (recordId != selectedId) return false;
            }
        }

        return true;
    }

private:
    void loadLookups() {
        try {
            auto resp = ApiClient::instance().fetchAll("Customer");
            if (resp.ok() && resp.hasData() && resp.data().is_array()) {
                for (const auto& cust : resp.data()) {
                    std::string id = cust.contains("id")
                        ? (cust["id"].is_string() ? cust["id"].get<std::string>() : cust["id"].dump())
                        : "";
                    std::string name;
                    if (cust.contains("attributes") && cust["attributes"].contains("company_name"))
                        name = cust["attributes"]["company_name"].get<std::string>();
                    if (!id.empty() && !name.empty())
                        customerNames_[id] = name;
                }
            }
        } catch (...) {}
    }

    std::string lookupRelationship(const json& record, const std::string& relName,
                                    const std::string& attr1) const {
        if (record.contains("relationships")) {
            const auto& rels = record["relationships"];
            if (rels.contains(relName) && rels[relName].contains("data")
                && !rels[relName]["data"].is_null()) {
                const auto& relData = rels[relName]["data"];
                std::string relType = relData.contains("type")
                    ? relData["type"].get<std::string>() : "";
                std::string relId = relData.contains("id")
                    ? (relData["id"].is_string() ? relData["id"].get<std::string>()
                                                  : relData["id"].dump())
                    : "";

                if (lastResponseBody_.contains("included")) {
                    for (const auto& inc : lastResponseBody_["included"]) {
                        std::string incType = inc.contains("type")
                            ? inc["type"].get<std::string>() : "";
                        std::string incId = inc.contains("id")
                            ? (inc["id"].is_string() ? inc["id"].get<std::string>()
                                                      : inc["id"].dump())
                            : "";
                        if (incType == relType && incId == relId && inc.contains("attributes")) {
                            const auto& attrs = inc["attributes"];
                            if (attrs.contains(attr1) && !attrs[attr1].is_null())
                                return attrs[attr1].get<std::string>();
                        }
                    }
                }
            }
        }

        // Fallback to pre-fetched lookup
        if (record.contains("attributes")) {
            const auto& attrs = record["attributes"];
            std::string fieldName = relName + "_id";
            if (attrs.contains(fieldName) && !attrs[fieldName].is_null()) {
                std::string id = attrs[fieldName].is_string()
                    ? attrs[fieldName].get<std::string>()
                    : attrs[fieldName].dump();
                if (relName == "customer") {
                    auto it = customerNames_.find(id);
                    if (it != customerNames_.end()) return it->second;
                }
            }
        }
        return "-";
    }

    void showAddVehicleDialog() {
        auto dialog = addChild(std::make_unique<Wt::WDialog>("Add Vehicle"));
        dialog->setStyleClass("smitty-dialog");
        dialog->setModal(true);
        dialog->setClosable(true);
        dialog->rejectWhenEscapePressed(true);

        auto content = dialog->contents();
        content->setStyleClass("dialog-content");

        // Owner (Customer) dropdown
        content->addWidget(std::make_unique<Wt::WText>("Owner"))
               ->setStyleClass("dialog-label");
        auto ownerCombo = content->addWidget(std::make_unique<Wt::WComboBox>());
        ownerCombo->setStyleClass("dialog-input");
        ownerCombo->addItem("");
        for (const auto& pair : customerNames_) {
            ownerCombo->addItem(pair.second);
        }

        // VIN
        content->addWidget(std::make_unique<Wt::WText>("VIN"))
               ->setStyleClass("dialog-label");
        auto vinInput = content->addWidget(std::make_unique<Wt::WLineEdit>());
        vinInput->setStyleClass("dialog-input");

        // Description
        content->addWidget(std::make_unique<Wt::WText>("Description"))
               ->setStyleClass("dialog-label");
        auto descInput = content->addWidget(std::make_unique<Wt::WLineEdit>());
        descInput->setStyleClass("dialog-input");
        descInput->setPlaceholderText("e.g. 2019 Ford F-350 White Flatbed");

        // Year
        content->addWidget(std::make_unique<Wt::WText>("Year"))
               ->setStyleClass("dialog-label");
        auto yearInput = content->addWidget(std::make_unique<Wt::WLineEdit>());
        yearInput->setStyleClass("dialog-input");

        // Make
        content->addWidget(std::make_unique<Wt::WText>("Make"))
               ->setStyleClass("dialog-label");
        auto makeInput = content->addWidget(std::make_unique<Wt::WLineEdit>());
        makeInput->setStyleClass("dialog-input");

        // Model
        content->addWidget(std::make_unique<Wt::WText>("Model"))
               ->setStyleClass("dialog-label");
        auto modelInput = content->addWidget(std::make_unique<Wt::WLineEdit>());
        modelInput->setStyleClass("dialog-input");

        // License Plate
        content->addWidget(std::make_unique<Wt::WText>("License Plate"))
               ->setStyleClass("dialog-label");
        auto plateInput = content->addWidget(std::make_unique<Wt::WLineEdit>());
        plateInput->setStyleClass("dialog-input");

        // Notes
        content->addWidget(std::make_unique<Wt::WText>("Notes"))
               ->setStyleClass("dialog-label");
        auto notesInput = content->addWidget(std::make_unique<Wt::WLineEdit>());
        notesInput->setStyleClass("dialog-input");

        // Status
        auto statusMsg = content->addWidget(std::make_unique<Wt::WText>());
        statusMsg->setStyleClass("dialog-status");

        // Buttons
        auto btnBar = content->addWidget(std::make_unique<Wt::WContainerWidget>());
        btnBar->setStyleClass("dialog-buttons");
        auto saveBtn = btnBar->addWidget(std::make_unique<Wt::WPushButton>("Save"));
        saveBtn->setStyleClass("action-btn");

        saveBtn->clicked().connect([=] {
            if (ownerCombo->currentText().empty()) {
                statusMsg->setText("Owner is required.");
                return;
            }
            if (vinInput->text().empty()) {
                statusMsg->setText("VIN is required.");
                return;
            }
            if (descInput->text().empty()) {
                statusMsg->setText("Description is required.");
                return;
            }

            json attrs;

            std::string ownerName = ownerCombo->currentText().toUTF8();
            for (const auto& pair : customerNames_) {
                if (pair.second == ownerName) {
                    attrs["customer_id"] = pair.first;
                    break;
                }
            }

            attrs["vin"] = vinInput->text().toUTF8();
            attrs["description"] = descInput->text().toUTF8();

            if (!yearInput->text().empty()) {
                try { attrs["year"] = std::stoi(yearInput->text().toUTF8()); }
                catch (...) {}
            }
            if (!makeInput->text().empty())
                attrs["make"] = makeInput->text().toUTF8();
            if (!modelInput->text().empty())
                attrs["model"] = modelInput->text().toUTF8();
            if (!plateInput->text().empty())
                attrs["license_plate"] = plateInput->text().toUTF8();
            if (!notesInput->text().empty())
                attrs["notes"] = notesInput->text().toUTF8();

            try {
                auto resp = ApiClient::instance().createRecord("Vehicle", attrs);
                if (resp.ok()) {
                    dialog->accept();
                    refresh();
                } else {
                    statusMsg->setText("Error: " + resp.errorMessage());
                }
            } catch (const std::exception& e) {
                statusMsg->setText(std::string("Error: ") + e.what());
            }
        });

        dialog->finished().connect([this, dialog](Wt::DialogCode) {
            removeChild(dialog);
        });

        dialog->show();
    }

    Wt::WComboBox* ownerFilterCombo_ = nullptr;
    std::map<std::string, std::string> customerNames_;
};

std::unique_ptr<EntityListView> createVehicleList() {
    return std::make_unique<VehicleList>();
}
