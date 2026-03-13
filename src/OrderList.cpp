#include "EntityListView.h"
#include "EntityDetailView.h"
#include "EntityRegistry.h"
#include "ApiClient.h"
#include "AppSettings.h"
#include <Wt/WCheckBox.h>
#include <Wt/WDialog.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WComboBox.h>
#include <Wt/WText.h>
#include <Wt/WContainerWidget.h>
#include <map>

class OrderList : public EntityListView {
public:
    OrderList()
        : EntityListView(EntityRegistry::instance().getEntity("Order"))
        , unpaidCheck_(nullptr) {
        loadLookups();
    }

protected:
    bool showRowActions() const override { return true; }

    std::string includeParam() const override {
        return "employee,customer";
    }

    std::string resolveFieldValue(const json& record, const ColumnDef& col,
                                   const std::string& rawValue) const override {
        if (col.name == "employee_id") {
            return lookupRelationship(record, "employee", "first_name", "last_name");
        }
        if (col.name == "customer_id") {
            return lookupRelationship(record, "customer", "company_name", "");
        }
        return rawValue;
    }

    bool customRenderCell(Wt::WTableCell* cell, const json& record,
                           const ColumnDef& col, const std::string& value) override {
        if (col.name == "customer_id") {
            std::string custId = getCustomerIdFromRecord(record);
            auto link = cell->addWidget(std::make_unique<Wt::WText>(value));
            link->setStyleClass("cell-link");
            if (!custId.empty()) {
                link->clicked().connect([this, custId] {
                    showCustomerDetailDialog(custId);
                });
            }
            return true;
        }
        return false;
    }

    void addCustomFilters(Wt::WContainerWidget* filterBar) override {
        // Customer filter dropdown
        customerFilterCombo_ = filterBar->addWidget(std::make_unique<Wt::WComboBox>());
        customerFilterCombo_->setStyleClass("filter-combo");
        customerFilterCombo_->addItem("All Customers");
        for (const auto& pair : customerNames_) {
            customerFilterCombo_->addItem(pair.second);
        }
        customerFilterCombo_->changed().connect(this, &EntityListView::refresh);

        // Employee filter dropdown
        employeeFilterCombo_ = filterBar->addWidget(std::make_unique<Wt::WComboBox>());
        employeeFilterCombo_->setStyleClass("filter-combo");
        employeeFilterCombo_->addItem("All Employees");
        for (const auto& pair : employeeNames_) {
            employeeFilterCombo_->addItem(pair.second);
        }
        employeeFilterCombo_->changed().connect(this, &EntityListView::refresh);

        // Unpaid Invoices checkbox
        unpaidCheck_ = filterBar->addWidget(
            std::make_unique<Wt::WCheckBox>("Unpaid Invoices"));
        unpaidCheck_->setStyleClass("filter-checkbox");
        unpaidCheck_->changed().connect(this, &EntityListView::refresh);
    }

    void addActionButtons(Wt::WContainerWidget* actionBar) override {
        auto newOrderBtn = actionBar->addWidget(
            std::make_unique<Wt::WPushButton>("New Invoice"));
        newOrderBtn->setStyleClass("action-btn");
        newOrderBtn->clicked().connect(this, &OrderList::showAddOrderDialog);
    }

    bool filterRecord(const json& record) const override {
        if (!record.contains("attributes")) return false;
        const auto& attrs = record["attributes"];

        // Customer filter
        if (customerFilterCombo_ && customerFilterCombo_->currentIndex() > 0) {
            std::string selectedCustName = customerFilterCombo_->currentText().toUTF8();
            std::string selectedCustId;
            for (const auto& pair : customerNames_) {
                if (pair.second == selectedCustName) { selectedCustId = pair.first; break; }
            }
            if (!selectedCustId.empty()) {
                std::string recordCustId;
                if (attrs.contains("customer_id") && !attrs["customer_id"].is_null())
                    recordCustId = attrs["customer_id"].is_string()
                        ? attrs["customer_id"].get<std::string>()
                        : attrs["customer_id"].dump();
                if (recordCustId != selectedCustId) return false;
            }
        }

        // Employee filter
        if (employeeFilterCombo_ && employeeFilterCombo_->currentIndex() > 0) {
            std::string selectedEmpName = employeeFilterCombo_->currentText().toUTF8();
            std::string selectedEmpId;
            for (const auto& pair : employeeNames_) {
                if (pair.second == selectedEmpName) { selectedEmpId = pair.first; break; }
            }
            if (!selectedEmpId.empty()) {
                std::string recordEmpId;
                if (attrs.contains("employee_id") && !attrs["employee_id"].is_null())
                    recordEmpId = attrs["employee_id"].is_string()
                        ? attrs["employee_id"].get<std::string>()
                        : attrs["employee_id"].dump();
                if (recordEmpId != selectedEmpId) return false;
            }
        }

        // Unpaid filter
        if (unpaidCheck_ && unpaidCheck_->isChecked()) {
            bool shipped = attrs.contains("shipped_date")
                && !attrs["shipped_date"].is_null()
                && attrs["shipped_date"].get<std::string>() != "";
            if (shipped) return false;
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
                    if (!id.empty() && !name.empty()) {
                        customerNames_[id] = name;
                    }
                }
            }
        } catch (...) {}

        try {
            auto resp = ApiClient::instance().fetchAll("Employee");
            if (resp.ok() && resp.hasData() && resp.data().is_array()) {
                for (const auto& emp : resp.data()) {
                    std::string id = emp.contains("id")
                        ? (emp["id"].is_string() ? emp["id"].get<std::string>() : emp["id"].dump())
                        : "";
                    if (emp.contains("attributes")) {
                        const auto& attrs = emp["attributes"];
                        std::string first = attrs.contains("first_name") && !attrs["first_name"].is_null()
                            ? attrs["first_name"].get<std::string>() : "";
                        std::string last = attrs.contains("last_name") && !attrs["last_name"].is_null()
                            ? attrs["last_name"].get<std::string>() : "";
                        std::string fullName = first;
                        if (!fullName.empty() && !last.empty()) fullName += " ";
                        fullName += last;
                        if (!id.empty() && !fullName.empty()) {
                            employeeNames_[id] = fullName;
                        }
                    }
                }
            }
        } catch (...) {}

    }

    std::string getCustomerIdFromRecord(const json& record) const {
        if (record.contains("relationships")) {
            const auto& rels = record["relationships"];
            if (rels.contains("customer") && rels["customer"].contains("data")
                && !rels["customer"]["data"].is_null()) {
                const auto& relData = rels["customer"]["data"];
                if (relData.contains("id")) {
                    return relData["id"].is_string()
                        ? relData["id"].get<std::string>()
                        : relData["id"].dump();
                }
            }
        }
        if (record.contains("attributes")) {
            const auto& attrs = record["attributes"];
            if (attrs.contains("customer_id") && !attrs["customer_id"].is_null()) {
                return attrs["customer_id"].is_string()
                    ? attrs["customer_id"].get<std::string>()
                    : attrs["customer_id"].dump();
            }
        }
        return "";
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

        if (record.contains("attributes")) {
            const auto& attrs = record["attributes"];
            std::string fieldName = relName + "_id";
            if (attrs.contains(fieldName) && !attrs[fieldName].is_null()) {
                std::string id = attrs[fieldName].is_string()
                    ? attrs[fieldName].get<std::string>()
                    : attrs[fieldName].dump();
                if (relName == "employee") {
                    auto it = employeeNames_.find(id);
                    if (it != employeeNames_.end()) return it->second;
                }
                if (relName == "customer") {
                    auto it = customerNames_.find(id);
                    if (it != customerNames_.end()) return it->second;
                }
            }
        }

        return "-";
    }

    void showCustomerDetailDialog(const std::string& customerId) {
        auto dialog = addChild(std::make_unique<Wt::WDialog>("Customer Detail"));
        dialog->setStyleClass("smitty-dialog smitty-dialog-wide");
        dialog->setModal(true);
        dialog->setClosable(true);
        dialog->rejectWhenEscapePressed(true);

        auto content = dialog->contents();
        content->setStyleClass("dialog-content");

        auto customerEntity = EntityRegistry::instance().getEntity("Customer");

        try {
            auto resp = ApiClient::instance().fetchOne("Customer", customerId);
            if (resp.ok() && resp.hasData()) {
                const auto& record = resp.data();
                for (const auto& col : customerEntity->columns()) {
                    auto group = content->addWidget(std::make_unique<Wt::WContainerWidget>());
                    group->setStyleClass("form-group");

                    group->addWidget(std::make_unique<Wt::WText>(col.label))
                         ->setStyleClass("form-label");

                    std::string val = customerEntity->getFieldValue(record, col.name);
                    if (val.empty()) val = "-";

                    group->addWidget(std::make_unique<Wt::WText>(val, Wt::TextFormat::Plain))
                         ->setStyleClass("form-value");
                }
            } else {
                content->addWidget(std::make_unique<Wt::WText>(
                    "Error loading customer: " + resp.errorMessage(), Wt::TextFormat::Plain));
            }
        } catch (const std::exception& e) {
            content->addWidget(std::make_unique<Wt::WText>(
                std::string("Error: ") + e.what(), Wt::TextFormat::Plain));
        }

        dialog->finished().connect([this, dialog](Wt::DialogCode) {
            removeChild(dialog);
        });

        dialog->show();
    }

    void showAddOrderDialog() {
        auto dialog = addChild(std::make_unique<Wt::WDialog>("Add Order"));
        dialog->setStyleClass("smitty-dialog");
        dialog->setModal(true);
        dialog->setClosable(true);
        dialog->rejectWhenEscapePressed(true);

        auto content = dialog->contents();
        content->setStyleClass("dialog-content");

        // 3-column grid for fields
        auto grid = content->addWidget(std::make_unique<Wt::WContainerWidget>());
        grid->setStyleClass("dialog-content-grid");

        // Customer (required)
        auto custGroup = grid->addWidget(std::make_unique<Wt::WContainerWidget>());
        custGroup->setStyleClass("dialog-field-group");
        custGroup->addWidget(std::make_unique<Wt::WText>("Customer"))
               ->setStyleClass("dialog-label");
        auto custCombo = custGroup->addWidget(std::make_unique<Wt::WComboBox>());
        custCombo->setStyleClass("dialog-input");
        custCombo->addItem("");
        for (const auto& pair : customerNames_) {
            custCombo->addItem(pair.second);
        }

        // Invoice Date
        auto dateGroup = grid->addWidget(std::make_unique<Wt::WContainerWidget>());
        dateGroup->setStyleClass("dialog-field-group");
        dateGroup->addWidget(std::make_unique<Wt::WText>("Invoice Date"))
               ->setStyleClass("dialog-label");
        auto orderDateInput = dateGroup->addWidget(std::make_unique<Wt::WLineEdit>());
        orderDateInput->setStyleClass("dialog-input");
        orderDateInput->setPlaceholderText("YYYY-MM-DD");

        // Ship Name
        auto shipGroup = grid->addWidget(std::make_unique<Wt::WContainerWidget>());
        shipGroup->setStyleClass("dialog-field-group");
        shipGroup->addWidget(std::make_unique<Wt::WText>("Ship Name"))
               ->setStyleClass("dialog-label");
        auto shipNameInput = shipGroup->addWidget(std::make_unique<Wt::WLineEdit>());
        shipNameInput->setStyleClass("dialog-input");

        // Ship City
        auto cityGroup = grid->addWidget(std::make_unique<Wt::WContainerWidget>());
        cityGroup->setStyleClass("dialog-field-group");
        cityGroup->addWidget(std::make_unique<Wt::WText>("Ship City"))
               ->setStyleClass("dialog-label");
        auto shipCityInput = cityGroup->addWidget(std::make_unique<Wt::WLineEdit>());
        shipCityInput->setStyleClass("dialog-input");

        // Ship Country
        auto countryGroup = grid->addWidget(std::make_unique<Wt::WContainerWidget>());
        countryGroup->setStyleClass("dialog-field-group");
        countryGroup->addWidget(std::make_unique<Wt::WText>("Ship Country"))
               ->setStyleClass("dialog-label");
        auto shipCountryInput = countryGroup->addWidget(std::make_unique<Wt::WLineEdit>());
        shipCountryInput->setStyleClass("dialog-input");

        // Freight
        auto freightGroup = grid->addWidget(std::make_unique<Wt::WContainerWidget>());
        freightGroup->setStyleClass("dialog-field-group");
        freightGroup->addWidget(std::make_unique<Wt::WText>("Freight"))
               ->setStyleClass("dialog-label");
        auto freightInput = freightGroup->addWidget(std::make_unique<Wt::WLineEdit>());
        freightInput->setStyleClass("dialog-input");
        freightInput->setText("0.00");

        // Status message (full width, below grid)
        auto statusMsg = content->addWidget(std::make_unique<Wt::WText>());
        statusMsg->setStyleClass("dialog-status");

        // Buttons
        auto btnBar = content->addWidget(std::make_unique<Wt::WContainerWidget>());
        btnBar->setStyleClass("dialog-buttons");

        auto saveBtn = btnBar->addWidget(std::make_unique<Wt::WPushButton>("Add Order"));
        saveBtn->setStyleClass("action-btn");

        saveBtn->clicked().connect([=] {
            if (custCombo->currentText().empty()) {
                statusMsg->setText("Customer is required.");
                return;
            }

            json attrs;

            std::string custName = custCombo->currentText().toUTF8();
            for (const auto& pair : customerNames_) {
                if (pair.second == custName) {
                    attrs["customer_id"] = pair.first;
                    break;
                }
            }

            if (!orderDateInput->text().empty())
                attrs["order_date"] = orderDateInput->text().toUTF8();
            if (!shipNameInput->text().empty())
                attrs["ship_name"] = shipNameInput->text().toUTF8();
            if (!shipCityInput->text().empty())
                attrs["ship_city"] = shipCityInput->text().toUTF8();
            if (!shipCountryInput->text().empty())
                attrs["ship_country"] = shipCountryInput->text().toUTF8();

            if (!freightInput->text().empty()) {
                try { attrs["freight"] = std::stod(freightInput->text().toUTF8()); }
                catch (...) {}
            }

            try {
                auto resp = ApiClient::instance().createRecord("Order", attrs);
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

    Wt::WCheckBox* unpaidCheck_;
    Wt::WComboBox* customerFilterCombo_ = nullptr;
    Wt::WComboBox* employeeFilterCombo_ = nullptr;
    std::map<std::string, std::string> customerNames_;
    std::map<std::string, std::string> employeeNames_;
};

std::unique_ptr<EntityListView> createOrderList() {
    return std::make_unique<OrderList>();
}
