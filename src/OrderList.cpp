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
#include <Wt/WAnchor.h>
#include <map>

class OrderList : public EntityListView {
public:
    OrderList()
        : EntityListView(EntityRegistry::instance().getEntity("Order"))
        , unpaidCheck_(nullptr) {
        loadLookups();
    }

protected:
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
            // Get the customer ID for the detail link
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
        unpaidCheck_ = filterBar->addWidget(
            std::make_unique<Wt::WCheckBox>("Unpaid Orders"));
        unpaidCheck_->setStyleClass("filter-checkbox");
        unpaidCheck_->changed().connect(this, &EntityListView::refresh);
    }

    void addActionButtons(Wt::WContainerWidget* actionBar) override {
        auto newOrderBtn = actionBar->addWidget(
            std::make_unique<Wt::WPushButton>("New Order"));
        newOrderBtn->setStyleClass("action-btn");
        newOrderBtn->clicked().connect(this, &OrderList::showAddOrderDialog);
    }

    bool filterRecord(const json& record) const override {
        if (!unpaidCheck_ || !unpaidCheck_->isChecked())
            return true;

        if (!record.contains("attributes")) return false;
        const auto& attrs = record["attributes"];

        bool shipped = attrs.contains("shipped_date")
            && !attrs["shipped_date"].is_null()
            && attrs["shipped_date"].get<std::string>() != "";
        return !shipped;
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
        // Try relationship data first
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
        // Fallback to attribute
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
        // Try JSONAPI included relationships first
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

        auto btnBar = content->addWidget(std::make_unique<Wt::WContainerWidget>());
        btnBar->setStyleClass("dialog-buttons");

        auto closeBtn = btnBar->addWidget(std::make_unique<Wt::WPushButton>("Close"));
        closeBtn->setStyleClass("action-btn");
        closeBtn->clicked().connect(dialog, &Wt::WDialog::accept);

        dialog->finished().connect([this, dialog](Wt::DialogCode) {
            removeChild(dialog);
        });

        dialog->show();
    }

    void showAddOrderDialog() {
        auto dialog = addChild(std::make_unique<Wt::WDialog>("Add Order"));
        dialog->setStyleClass("smitty-dialog");
        dialog->setModal(true);
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

        // Ship Name
        content->addWidget(std::make_unique<Wt::WText>("Ship Name"))
               ->setStyleClass("dialog-label");
        auto shipNameInput = content->addWidget(std::make_unique<Wt::WLineEdit>());
        shipNameInput->setStyleClass("dialog-input");

        // Order Date
        content->addWidget(std::make_unique<Wt::WText>("Order Date"))
               ->setStyleClass("dialog-label");
        auto orderDateInput = content->addWidget(std::make_unique<Wt::WLineEdit>());
        orderDateInput->setStyleClass("dialog-input");
        orderDateInput->setPlaceholderText("YYYY-MM-DD");

        // Required Date
        content->addWidget(std::make_unique<Wt::WText>("Required Date"))
               ->setStyleClass("dialog-label");
        auto reqDateInput = content->addWidget(std::make_unique<Wt::WLineEdit>());
        reqDateInput->setStyleClass("dialog-input");
        reqDateInput->setPlaceholderText("YYYY-MM-DD");

        // Ship Address
        content->addWidget(std::make_unique<Wt::WText>("Ship Address"))
               ->setStyleClass("dialog-label");
        auto shipAddrInput = content->addWidget(std::make_unique<Wt::WLineEdit>());
        shipAddrInput->setStyleClass("dialog-input");

        // Ship City
        content->addWidget(std::make_unique<Wt::WText>("Ship City"))
               ->setStyleClass("dialog-label");
        auto shipCityInput = content->addWidget(std::make_unique<Wt::WLineEdit>());
        shipCityInput->setStyleClass("dialog-input");

        // Ship Country
        content->addWidget(std::make_unique<Wt::WText>("Ship Country"))
               ->setStyleClass("dialog-label");
        auto shipCountryInput = content->addWidget(std::make_unique<Wt::WLineEdit>());
        shipCountryInput->setStyleClass("dialog-input");

        // Freight
        content->addWidget(std::make_unique<Wt::WText>("Freight"))
               ->setStyleClass("dialog-label");
        auto freightInput = content->addWidget(std::make_unique<Wt::WLineEdit>());
        freightInput->setStyleClass("dialog-input");
        freightInput->setText("0.00");

        // Status message
        auto statusMsg = content->addWidget(std::make_unique<Wt::WText>());
        statusMsg->setStyleClass("dialog-status");

        // Buttons
        auto btnBar = content->addWidget(std::make_unique<Wt::WContainerWidget>());
        btnBar->setStyleClass("dialog-buttons");

        auto saveBtn = btnBar->addWidget(std::make_unique<Wt::WPushButton>("Save"));
        saveBtn->setStyleClass("action-btn");

        auto cancelBtn = btnBar->addWidget(std::make_unique<Wt::WPushButton>("Cancel"));
        cancelBtn->setStyleClass("action-btn action-btn-secondary");
        cancelBtn->clicked().connect(dialog, &Wt::WDialog::reject);

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

            if (!shipNameInput->text().empty())
                attrs["ship_name"] = shipNameInput->text().toUTF8();
            if (!orderDateInput->text().empty())
                attrs["order_date"] = orderDateInput->text().toUTF8();
            if (!reqDateInput->text().empty())
                attrs["required_date"] = reqDateInput->text().toUTF8();
            if (!shipAddrInput->text().empty())
                attrs["ship_address"] = shipAddrInput->text().toUTF8();
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
    std::map<std::string, std::string> customerNames_;
    std::map<std::string, std::string> employeeNames_;
};

std::unique_ptr<EntityListView> createOrderList() {
    return std::make_unique<OrderList>();
}
