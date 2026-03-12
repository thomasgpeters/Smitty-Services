#include "EntityListView.h"
#include "EntityRegistry.h"
#include "ApiClient.h"
#include <Wt/WCheckBox.h>
#include <Wt/WComboBox.h>
#include <Wt/WDialog.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WTable.h>
#include <Wt/WContainerWidget.h>
#include <map>
#include <vector>

static const std::vector<std::string> JOB_STATUSES = {
    "New", "In Progress", "Waiting Parts", "Road Test Pending", "Complete"
};

class JobList : public EntityListView {
public:
    JobList()
        : EntityListView(EntityRegistry::instance().getEntity("Job")) {
        loadLookups();
    }

protected:
    std::string includeParam() const override {
        return "customer,vehicle";
    }

    std::string resolveFieldValue(const json& record, const ColumnDef& col,
                                   const std::string& rawValue) const override {
        if (col.name == "customer_id") {
            return lookupRelationship(record, "customer", "company_name", "");
        }
        if (col.name == "vehicle_id") {
            return lookupRelationship(record, "vehicle", "description", "");
        }
        return rawValue;
    }

    void addCustomFilters(Wt::WContainerWidget* filterBar) override {
        // Status filter
        statusFilterCombo_ = filterBar->addWidget(std::make_unique<Wt::WComboBox>());
        statusFilterCombo_->setStyleClass("filter-combo");
        statusFilterCombo_->addItem("All Statuses");
        for (const auto& s : JOB_STATUSES) {
            statusFilterCombo_->addItem(s);
        }
        statusFilterCombo_->changed().connect(this, &EntityListView::refresh);

        // Customer filter
        customerFilterCombo_ = filterBar->addWidget(std::make_unique<Wt::WComboBox>());
        customerFilterCombo_->setStyleClass("filter-combo");
        customerFilterCombo_->addItem("All Customers");
        for (const auto& pair : customerNames_) {
            customerFilterCombo_->addItem(pair.second);
        }
        customerFilterCombo_->changed().connect(this, &EntityListView::refresh);

        // Active jobs only checkbox
        activeOnlyCheck_ = filterBar->addWidget(
            std::make_unique<Wt::WCheckBox>("Active Only"));
        activeOnlyCheck_->setStyleClass("filter-checkbox");
        activeOnlyCheck_->setChecked(true);
        activeOnlyCheck_->changed().connect(this, &EntityListView::refresh);
    }

    void addActionButtons(Wt::WContainerWidget* actionBar) override {
        auto newBtn = actionBar->addWidget(
            std::make_unique<Wt::WPushButton>("New Job"));
        newBtn->setStyleClass("action-btn");
        newBtn->clicked().connect(this, &JobList::showAddJobDialog);
    }

    bool filterRecord(const json& record) const override {
        if (!record.contains("attributes")) return false;
        const auto& attrs = record["attributes"];

        // Status filter
        if (statusFilterCombo_ && statusFilterCombo_->currentIndex() > 0) {
            std::string selectedStatus = statusFilterCombo_->currentText().toUTF8();
            std::string recordStatus;
            if (attrs.contains("status") && !attrs["status"].is_null())
                recordStatus = attrs["status"].get<std::string>();
            if (recordStatus != selectedStatus) return false;
        }

        // Customer filter
        if (customerFilterCombo_ && customerFilterCombo_->currentIndex() > 0) {
            std::string selectedName = customerFilterCombo_->currentText().toUTF8();
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

        // Active only filter (exclude Complete)
        if (activeOnlyCheck_ && activeOnlyCheck_->isChecked()) {
            std::string status;
            if (attrs.contains("status") && !attrs["status"].is_null())
                status = attrs["status"].get<std::string>();
            if (status == "Complete") return false;
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

        try {
            auto resp = ApiClient::instance().fetchAll("Vehicle");
            if (resp.ok() && resp.hasData() && resp.data().is_array()) {
                for (const auto& v : resp.data()) {
                    std::string id = v.contains("id")
                        ? (v["id"].is_string() ? v["id"].get<std::string>() : v["id"].dump())
                        : "";
                    std::string desc;
                    std::string custId;
                    if (v.contains("attributes")) {
                        const auto& attrs = v["attributes"];
                        if (attrs.contains("description") && !attrs["description"].is_null())
                            desc = attrs["description"].get<std::string>();
                        if (attrs.contains("customer_id") && !attrs["customer_id"].is_null())
                            custId = attrs["customer_id"].is_string()
                                ? attrs["customer_id"].get<std::string>()
                                : attrs["customer_id"].dump();
                    }
                    if (!id.empty() && !desc.empty()) {
                        vehicleNames_[id] = desc;
                        vehicleOwners_[id] = custId;
                    }
                }
            }
        } catch (...) {}
    }

    std::string lookupRelationship(const json& record, const std::string& relName,
                                    const std::string& attr1, const std::string& attr2) const {
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
                            std::string val1 = attrs.contains(attr1) && !attrs[attr1].is_null()
                                ? attrs[attr1].get<std::string>() : "";
                            if (attr2.empty()) return val1.empty() ? "-" : val1;
                            std::string val2 = attrs.contains(attr2) && !attrs[attr2].is_null()
                                ? attrs[attr2].get<std::string>() : "";
                            std::string result = val1;
                            if (!result.empty() && !val2.empty()) result += " ";
                            result += val2;
                            return result.empty() ? "-" : result;
                        }
                    }
                }
            }
        }

        // Fallback to pre-fetched lookups
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
                if (relName == "vehicle") {
                    auto it = vehicleNames_.find(id);
                    if (it != vehicleNames_.end()) return it->second;
                }
            }
        }
        return "-";
    }

    void showAddJobDialog() {
        auto dialog = addChild(std::make_unique<Wt::WDialog>("Add Job"));
        dialog->setStyleClass("smitty-dialog smitty-dialog-wide");
        dialog->setModal(true);
        dialog->setClosable(true);
        dialog->rejectWhenEscapePressed(true);

        auto content = dialog->contents();
        content->setStyleClass("dialog-content");

        // Customer dropdown
        content->addWidget(std::make_unique<Wt::WText>("Customer"))
               ->setStyleClass("dialog-label");
        auto custCombo = content->addWidget(std::make_unique<Wt::WComboBox>());
        custCombo->setStyleClass("dialog-input");
        custCombo->addItem("");
        for (const auto& pair : customerNames_) {
            custCombo->addItem(pair.second);
        }

        // Vehicle dropdown (filtered by customer)
        content->addWidget(std::make_unique<Wt::WText>("Vehicle"))
               ->setStyleClass("dialog-label");
        auto vehicleCombo = content->addWidget(std::make_unique<Wt::WComboBox>());
        vehicleCombo->setStyleClass("dialog-input");
        vehicleCombo->addItem("");

        // When customer changes, filter vehicles to that customer
        custCombo->changed().connect([this, custCombo, vehicleCombo] {
            vehicleCombo->clear();
            vehicleCombo->addItem("");

            std::string selectedCustName = custCombo->currentText().toUTF8();
            std::string selectedCustId;
            for (const auto& pair : customerNames_) {
                if (pair.second == selectedCustName) { selectedCustId = pair.first; break; }
            }

            if (!selectedCustId.empty()) {
                for (const auto& pair : vehicleNames_) {
                    auto ownerIt = vehicleOwners_.find(pair.first);
                    if (ownerIt != vehicleOwners_.end() && ownerIt->second == selectedCustId) {
                        vehicleCombo->addItem(pair.second);
                    }
                }
            }
        });

        // Service Description
        content->addWidget(std::make_unique<Wt::WText>("Service Description"))
               ->setStyleClass("dialog-label");
        auto descInput = content->addWidget(std::make_unique<Wt::WLineEdit>());
        descInput->setStyleClass("dialog-input");

        // Status dropdown
        content->addWidget(std::make_unique<Wt::WText>("Status"))
               ->setStyleClass("dialog-label");
        auto statusCombo = content->addWidget(std::make_unique<Wt::WComboBox>());
        statusCombo->setStyleClass("dialog-input");
        for (const auto& s : JOB_STATUSES) {
            statusCombo->addItem(s);
        }

        // Estimated Cost
        content->addWidget(std::make_unique<Wt::WText>("Estimated Cost"))
               ->setStyleClass("dialog-label");
        auto estCostInput = content->addWidget(std::make_unique<Wt::WLineEdit>());
        estCostInput->setStyleClass("dialog-input");
        estCostInput->setPlaceholderText("0.00");

        // Notes
        content->addWidget(std::make_unique<Wt::WText>("Notes"))
               ->setStyleClass("dialog-label");
        auto notesInput = content->addWidget(std::make_unique<Wt::WLineEdit>());
        notesInput->setStyleClass("dialog-input");

        // ===== LINKED PURCHASES SECTION =====
        auto purchaseSection = content->addWidget(std::make_unique<Wt::WContainerWidget>());
        purchaseSection->setStyleClass("order-lines-section");

        auto purchaseHeader = purchaseSection->addWidget(std::make_unique<Wt::WContainerWidget>());
        purchaseHeader->setStyleClass("order-lines-header");
        purchaseHeader->addWidget(std::make_unique<Wt::WText>("Linked POs"))
                      ->setStyleClass("order-lines-title");
        auto addPurchBtn = purchaseHeader->addWidget(
            std::make_unique<Wt::WPushButton>("+ Link PO"));
        addPurchBtn->setStyleClass("order-lines-add-btn");

        auto purchaseTable = purchaseSection->addWidget(std::make_unique<Wt::WTable>());
        purchaseTable->setStyleClass("order-lines-table");
        purchaseTable->setHeaderCount(1);
        purchaseTable->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("PO"));
        purchaseTable->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>(""));

        // Load purchases for linking
        std::map<std::string, std::string> purchaseDisplayNames;
        try {
            auto resp = ApiClient::instance().fetchAll("Purchase", "", "supplier");
            if (resp.ok() && resp.hasData() && resp.data().is_array()) {
                for (const auto& purch : resp.data()) {
                    std::string id = purch.contains("id")
                        ? (purch["id"].is_string() ? purch["id"].get<std::string>() : purch["id"].dump())
                        : "";
                    std::string status;
                    if (purch.contains("attributes") && purch["attributes"].contains("status")
                        && !purch["attributes"]["status"].is_null())
                        status = purch["attributes"]["status"].get<std::string>();

                    // Try to get supplier name from included
                    std::string supName;
                    if (purch.contains("relationships") && purch["relationships"].contains("supplier")
                        && purch["relationships"]["supplier"].contains("data")
                        && !purch["relationships"]["supplier"]["data"].is_null()) {
                        const auto& relData = purch["relationships"]["supplier"]["data"];
                        std::string relType = relData.contains("type") ? relData["type"].get<std::string>() : "";
                        std::string relId = relData.contains("id")
                            ? (relData["id"].is_string() ? relData["id"].get<std::string>() : relData["id"].dump()) : "";
                        if (resp.body.contains("included")) {
                            for (const auto& inc : resp.body["included"]) {
                                if (inc.contains("type") && inc["type"].get<std::string>() == relType
                                    && inc.contains("id") && (inc["id"].is_string() ? inc["id"].get<std::string>() : inc["id"].dump()) == relId
                                    && inc.contains("attributes") && inc["attributes"].contains("company_name"))
                                    supName = inc["attributes"]["company_name"].get<std::string>();
                            }
                        }
                    }

                    if (!id.empty()) {
                        std::string display = supName.empty() ? ("PO #" + id) : (supName + " (#" + id + ")");
                        if (!status.empty()) display += " [" + status + "]";
                        purchaseDisplayNames[id] = display;
                    }
                }
            }
        } catch (...) {}

        struct PurchaseLinkRow {
            Wt::WComboBox* purchaseCombo;
            int tableRow;
        };
        auto purchaseRows = std::make_shared<std::vector<PurchaseLinkRow>>();

        auto addPurchaseRow = [purchaseTable, purchaseRows, &purchaseDisplayNames]() {
            int row = purchaseTable->rowCount();

            auto purchaseCombo = purchaseTable->elementAt(row, 0)->addWidget(
                std::make_unique<Wt::WComboBox>());
            purchaseCombo->setStyleClass("line-combo");
            purchaseCombo->addItem("");
            for (const auto& pair : purchaseDisplayNames) {
                purchaseCombo->addItem(pair.second);
            }

            auto removeBtn = purchaseTable->elementAt(row, 1)->addWidget(
                std::make_unique<Wt::WPushButton>("×"));
            removeBtn->setStyleClass("line-remove-btn");

            PurchaseLinkRow pr{purchaseCombo, row};
            purchaseRows->push_back(pr);

            removeBtn->clicked().connect([purchaseTable, purchaseRows, row] {
                for (int c = 0; c < 2; c++)
                    purchaseTable->elementAt(row, c)->hide();
                for (auto& pr : *purchaseRows) {
                    if (pr.tableRow == row) { pr.purchaseCombo = nullptr; break; }
                }
            });
        };

        addPurchBtn->clicked().connect(addPurchaseRow);

        // Status message
        auto statusMsg = content->addWidget(std::make_unique<Wt::WText>());
        statusMsg->setStyleClass("dialog-status");

        // Buttons
        auto btnBar = content->addWidget(std::make_unique<Wt::WContainerWidget>());
        btnBar->setStyleClass("dialog-buttons");
        auto saveBtn = btnBar->addWidget(std::make_unique<Wt::WPushButton>("Save"));
        saveBtn->setStyleClass("action-btn");

        saveBtn->clicked().connect([=] {
            if (custCombo->currentText().empty()) {
                statusMsg->setText("Customer is required.");
                return;
            }
            if (vehicleCombo->currentText().empty()) {
                statusMsg->setText("Vehicle is required.");
                return;
            }
            if (descInput->text().empty()) {
                statusMsg->setText("Service Description is required.");
                return;
            }

            json attrs;

            // Resolve customer ID
            std::string custName = custCombo->currentText().toUTF8();
            for (const auto& pair : customerNames_) {
                if (pair.second == custName) {
                    attrs["customer_id"] = pair.first;
                    break;
                }
            }

            // Resolve vehicle ID
            std::string vehDesc = vehicleCombo->currentText().toUTF8();
            for (const auto& pair : vehicleNames_) {
                if (pair.second == vehDesc) {
                    try { attrs["vehicle_id"] = std::stoi(pair.first); }
                    catch (...) { attrs["vehicle_id"] = pair.first; }
                    break;
                }
            }

            attrs["service_description"] = descInput->text().toUTF8();
            attrs["status"] = statusCombo->currentText().toUTF8();

            if (!estCostInput->text().empty()) {
                try { attrs["estimated_cost"] = std::stod(estCostInput->text().toUTF8()); }
                catch (...) {}
            }
            if (!notesInput->text().empty())
                attrs["notes"] = notesInput->text().toUTF8();

            try {
                auto resp = ApiClient::instance().createRecord("Job", attrs);
                if (resp.ok()) {
                    // Get new job ID and link purchases
                    std::string newJobId;
                    if (resp.hasData()) {
                        const auto& data = resp.data();
                        if (data.contains("id"))
                            newJobId = data["id"].is_string()
                                ? data["id"].get<std::string>() : data["id"].dump();
                    }

                    if (!newJobId.empty()) {
                        for (const auto& pr : *purchaseRows) {
                            if (!pr.purchaseCombo) continue;
                            std::string purchDisplay = pr.purchaseCombo->currentText().toUTF8();
                            if (purchDisplay.empty()) continue;

                            // Find purchase ID from display name
                            std::string purchaseId;
                            for (const auto& pair : purchaseDisplayNames) {
                                if (pair.second == purchDisplay) {
                                    purchaseId = pair.first;
                                    break;
                                }
                            }
                            if (purchaseId.empty()) continue;

                            json jpAttrs;
                            try { jpAttrs["job_id"] = std::stoi(newJobId); }
                            catch (...) { jpAttrs["job_id"] = newJobId; }
                            try { jpAttrs["purchase_id"] = std::stoi(purchaseId); }
                            catch (...) { jpAttrs["purchase_id"] = purchaseId; }

                            ApiClient::instance().createRecord("JobPurchase", jpAttrs);
                        }
                    }

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

    Wt::WComboBox* statusFilterCombo_ = nullptr;
    Wt::WComboBox* customerFilterCombo_ = nullptr;
    Wt::WCheckBox* activeOnlyCheck_ = nullptr;
    std::map<std::string, std::string> customerNames_;
    std::map<std::string, std::string> vehicleNames_;
    std::map<std::string, std::string> vehicleOwners_;  // vehicle_id -> customer_id
};

std::unique_ptr<EntityListView> createJobList() {
    return std::make_unique<JobList>();
}
