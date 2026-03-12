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
#include <Wt/WTable.h>
#include <Wt/WContainerWidget.h>
#include <map>
#include <vector>
#include <algorithm>

struct ProductInfo {
    std::string id;
    std::string name;
    std::string categoryId;
    std::string supplierId;
    double unitPrice = 0.0;
};

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

        // Load products for order lines
        try {
            auto resp = ApiClient::instance().fetchAll("Product");
            if (resp.ok() && resp.hasData() && resp.data().is_array()) {
                for (const auto& prod : resp.data()) {
                    ProductInfo pi;
                    pi.id = prod.contains("id")
                        ? (prod["id"].is_string() ? prod["id"].get<std::string>() : prod["id"].dump())
                        : "";
                    if (prod.contains("attributes")) {
                        const auto& attrs = prod["attributes"];
                        if (attrs.contains("product_name") && !attrs["product_name"].is_null())
                            pi.name = attrs["product_name"].get<std::string>();
                        if (attrs.contains("unit_price") && !attrs["unit_price"].is_null()) {
                            try { pi.unitPrice = attrs["unit_price"].get<double>(); }
                            catch (...) {}
                        }
                        if (attrs.contains("category_id") && !attrs["category_id"].is_null())
                            pi.categoryId = attrs["category_id"].is_string()
                                ? attrs["category_id"].get<std::string>()
                                : attrs["category_id"].dump();
                        if (attrs.contains("supplier_id") && !attrs["supplier_id"].is_null())
                            pi.supplierId = attrs["supplier_id"].is_string()
                                ? attrs["supplier_id"].get<std::string>()
                                : attrs["supplier_id"].dump();
                    }
                    if (!pi.id.empty() && !pi.name.empty()) {
                        products_.push_back(pi);
                    }
                }
            }
        } catch (...) {}

        // Load categories for line item filters
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

        // Load suppliers for line item filters
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

    // Rebuild the product combo based on selected category/supplier filters
    void rebuildProductCombo(Wt::WComboBox* productCombo,
                             Wt::WComboBox* catFilter,
                             Wt::WComboBox* supFilter) {
        productCombo->clear();
        productCombo->addItem("");

        std::string selectedCat;
        if (catFilter->currentIndex() > 0) {
            std::string catName = catFilter->currentText().toUTF8();
            for (const auto& pair : categoryNames_) {
                if (pair.second == catName) { selectedCat = pair.first; break; }
            }
        }

        std::string selectedSup;
        if (supFilter->currentIndex() > 0) {
            std::string supName = supFilter->currentText().toUTF8();
            for (const auto& pair : supplierNames_) {
                if (pair.second == supName) { selectedSup = pair.first; break; }
            }
        }

        for (const auto& prod : products_) {
            if (!selectedCat.empty() && prod.categoryId != selectedCat) continue;
            if (!selectedSup.empty() && prod.supplierId != selectedSup) continue;
            productCombo->addItem(prod.name);
        }
    }

    void showAddOrderDialog() {
        auto dialog = addChild(std::make_unique<Wt::WDialog>("Add Invoice"));
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

        // Ship Name
        content->addWidget(std::make_unique<Wt::WText>("Ship Name"))
               ->setStyleClass("dialog-label");
        auto shipNameInput = content->addWidget(std::make_unique<Wt::WLineEdit>());
        shipNameInput->setStyleClass("dialog-input");

        // Order Date
        content->addWidget(std::make_unique<Wt::WText>("Invoice Date"))
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

        // ===== ORDER LINES SECTION =====
        auto linesSection = content->addWidget(std::make_unique<Wt::WContainerWidget>());
        linesSection->setStyleClass("order-lines-section");

        // Header with title and Add Line button
        auto linesHeader = linesSection->addWidget(std::make_unique<Wt::WContainerWidget>());
        linesHeader->setStyleClass("order-lines-header");
        linesHeader->addWidget(std::make_unique<Wt::WText>("Invoice Lines"))
                   ->setStyleClass("order-lines-title");
        auto addLineBtn = linesHeader->addWidget(std::make_unique<Wt::WPushButton>("+ Add Line"));
        addLineBtn->setStyleClass("order-lines-add-btn");

        // Category / Supplier filter row
        auto filterRow = linesSection->addWidget(std::make_unique<Wt::WContainerWidget>());
        filterRow->setStyleClass("order-lines-filters");

        auto catFilterCombo = filterRow->addWidget(std::make_unique<Wt::WComboBox>());
        catFilterCombo->setStyleClass("order-lines-filter-combo");
        catFilterCombo->addItem("All Categories");
        for (const auto& pair : categoryNames_) {
            catFilterCombo->addItem(pair.second);
        }

        auto supFilterCombo = filterRow->addWidget(std::make_unique<Wt::WComboBox>());
        supFilterCombo->setStyleClass("order-lines-filter-combo");
        supFilterCombo->addItem("All Suppliers");
        for (const auto& pair : supplierNames_) {
            supFilterCombo->addItem(pair.second);
        }

        // Lines table
        auto linesTable = linesSection->addWidget(std::make_unique<Wt::WTable>());
        linesTable->setStyleClass("order-lines-table");
        linesTable->setHeaderCount(1);
        linesTable->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Product"));
        linesTable->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Unit Price"));
        linesTable->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Qty"));
        linesTable->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Discount"));
        linesTable->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>(""));

        // Shared line row data structures
        struct LineRow {
            Wt::WComboBox* productCombo;
            Wt::WLineEdit* priceInput;
            Wt::WLineEdit* qtyInput;
            Wt::WLineEdit* discountInput;
            int tableRow;
        };
        auto lineRows = std::make_shared<std::vector<LineRow>>();

        // Lambda to add a line row
        auto addLineRow = [this, linesTable, lineRows, catFilterCombo, supFilterCombo]() {
            int row = linesTable->rowCount();

            auto productCombo = linesTable->elementAt(row, 0)->addWidget(
                std::make_unique<Wt::WComboBox>());
            productCombo->setStyleClass("line-combo");
            rebuildProductCombo(productCombo, catFilterCombo, supFilterCombo);

            auto priceInput = linesTable->elementAt(row, 1)->addWidget(
                std::make_unique<Wt::WLineEdit>());
            priceInput->setStyleClass("line-input");
            priceInput->setText("0.00");

            auto qtyInput = linesTable->elementAt(row, 2)->addWidget(
                std::make_unique<Wt::WLineEdit>());
            qtyInput->setStyleClass("line-input");
            qtyInput->setText("1");

            auto discountInput = linesTable->elementAt(row, 3)->addWidget(
                std::make_unique<Wt::WLineEdit>());
            discountInput->setStyleClass("line-input");
            discountInput->setText("0");

            auto removeBtn = linesTable->elementAt(row, 4)->addWidget(
                std::make_unique<Wt::WPushButton>("×"));
            removeBtn->setStyleClass("line-remove-btn");

            LineRow lr{productCombo, priceInput, qtyInput, discountInput, row};
            lineRows->push_back(lr);

            // Auto-fill unit price when product changes
            productCombo->changed().connect([this, productCombo, priceInput] {
                std::string prodName = productCombo->currentText().toUTF8();
                for (const auto& prod : products_) {
                    if (prod.name == prodName) {
                        char buf[32];
                        snprintf(buf, sizeof(buf), "%.2f", prod.unitPrice);
                        priceInput->setText(buf);
                        break;
                    }
                }
            });

            // Remove row
            removeBtn->clicked().connect([linesTable, lineRows, row] {
                // Hide the row instead of deleting (simpler with Wt tables)
                for (int c = 0; c < 5; c++) {
                    linesTable->elementAt(row, c)->hide();
                }
                // Mark as removed by clearing the product combo
                for (auto& lr : *lineRows) {
                    if (lr.tableRow == row) {
                        lr.productCombo = nullptr;
                        break;
                    }
                }
            });
        };

        addLineBtn->clicked().connect(addLineRow);

        // When category or supplier filter changes, rebuild product combos in all visible rows
        auto rebuildAllProductCombos = [this, lineRows, catFilterCombo, supFilterCombo] {
            for (auto& lr : *lineRows) {
                if (lr.productCombo) {
                    std::string currentSelection = lr.productCombo->currentText().toUTF8();
                    rebuildProductCombo(lr.productCombo, catFilterCombo, supFilterCombo);
                    // Try to restore selection
                    for (int i = 0; i < lr.productCombo->count(); i++) {
                        if (lr.productCombo->itemText(i).toUTF8() == currentSelection) {
                            lr.productCombo->setCurrentIndex(i);
                            break;
                        }
                    }
                }
            }
        };
        catFilterCombo->changed().connect(rebuildAllProductCombos);
        supFilterCombo->changed().connect(rebuildAllProductCombos);

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
                    // Get new order ID from response
                    std::string newOrderId;
                    if (resp.hasData()) {
                        const auto& data = resp.data();
                        if (data.contains("id")) {
                            newOrderId = data["id"].is_string()
                                ? data["id"].get<std::string>()
                                : data["id"].dump();
                        }
                    }

                    // Save each order line
                    if (!newOrderId.empty()) {
                        for (const auto& lr : *lineRows) {
                            if (!lr.productCombo) continue; // removed row
                            std::string prodName = lr.productCombo->currentText().toUTF8();
                            if (prodName.empty()) continue;

                            // Find product ID
                            std::string productId;
                            for (const auto& prod : products_) {
                                if (prod.name == prodName) {
                                    productId = prod.id;
                                    break;
                                }
                            }
                            if (productId.empty()) continue;

                            json lineAttrs;
                            try { lineAttrs["order_id"] = std::stoi(newOrderId); }
                            catch (...) { lineAttrs["order_id"] = newOrderId; }
                            try { lineAttrs["product_id"] = std::stoi(productId); }
                            catch (...) { lineAttrs["product_id"] = productId; }

                            if (!lr.priceInput->text().empty()) {
                                try { lineAttrs["unit_price"] = std::stod(lr.priceInput->text().toUTF8()); }
                                catch (...) { lineAttrs["unit_price"] = 0.0; }
                            }
                            if (!lr.qtyInput->text().empty()) {
                                try { lineAttrs["quantity"] = std::stoi(lr.qtyInput->text().toUTF8()); }
                                catch (...) { lineAttrs["quantity"] = 1; }
                            }
                            if (!lr.discountInput->text().empty()) {
                                try { lineAttrs["discount"] = std::stod(lr.discountInput->text().toUTF8()); }
                                catch (...) { lineAttrs["discount"] = 0.0; }
                            }

                            ApiClient::instance().createRecord("OrderDetail", lineAttrs);
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

    Wt::WCheckBox* unpaidCheck_;
    Wt::WComboBox* customerFilterCombo_ = nullptr;
    Wt::WComboBox* employeeFilterCombo_ = nullptr;
    std::map<std::string, std::string> customerNames_;
    std::map<std::string, std::string> employeeNames_;
    std::map<std::string, std::string> categoryNames_;
    std::map<std::string, std::string> supplierNames_;
    std::vector<ProductInfo> products_;
};

std::unique_ptr<EntityListView> createOrderList() {
    return std::make_unique<OrderList>();
}
