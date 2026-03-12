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

static const std::vector<std::string> PURCHASE_STATUSES = {
    "New", "Ordered", "Partial", "Received", "Cancelled"
};

struct ProductInfo {
    std::string id;
    std::string name;
    std::string categoryId;
    std::string supplierId;
    double unitPrice = 0.0;
};

class PurchaseList : public EntityListView {
public:
    PurchaseList()
        : EntityListView(EntityRegistry::instance().getEntity("Purchase")) {
        loadLookups();
    }

protected:
    std::string includeParam() const override {
        return "supplier";
    }

    std::string resolveFieldValue(const json& record, const ColumnDef& col,
                                   const std::string& rawValue) const override {
        if (col.name == "supplier_id") {
            return lookupRelationship(record, "supplier", "company_name");
        }
        return rawValue;
    }

    void addCustomFilters(Wt::WContainerWidget* filterBar) override {
        // Status filter
        statusFilterCombo_ = filterBar->addWidget(std::make_unique<Wt::WComboBox>());
        statusFilterCombo_->setStyleClass("filter-combo");
        statusFilterCombo_->addItem("All Statuses");
        for (const auto& s : PURCHASE_STATUSES) {
            statusFilterCombo_->addItem(s);
        }
        statusFilterCombo_->changed().connect(this, &EntityListView::refresh);

        // Supplier filter
        supplierFilterCombo_ = filterBar->addWidget(std::make_unique<Wt::WComboBox>());
        supplierFilterCombo_->setStyleClass("filter-combo");
        supplierFilterCombo_->addItem("All Suppliers");
        for (const auto& pair : supplierNames_) {
            supplierFilterCombo_->addItem(pair.second);
        }
        supplierFilterCombo_->changed().connect(this, &EntityListView::refresh);

        // Open only checkbox
        openOnlyCheck_ = filterBar->addWidget(
            std::make_unique<Wt::WCheckBox>("Open Only"));
        openOnlyCheck_->setStyleClass("filter-checkbox");
        openOnlyCheck_->setChecked(true);
        openOnlyCheck_->changed().connect(this, &EntityListView::refresh);
    }

    void addActionButtons(Wt::WContainerWidget* actionBar) override {
        auto newBtn = actionBar->addWidget(
            std::make_unique<Wt::WPushButton>("New Purchase"));
        newBtn->setStyleClass("action-btn");
        newBtn->clicked().connect(this, &PurchaseList::showAddPurchaseDialog);
    }

    bool filterRecord(const json& record) const override {
        if (!record.contains("attributes")) return false;
        const auto& attrs = record["attributes"];

        // Status filter
        if (statusFilterCombo_ && statusFilterCombo_->currentIndex() > 0) {
            std::string selected = statusFilterCombo_->currentText().toUTF8();
            std::string recordStatus;
            if (attrs.contains("status") && !attrs["status"].is_null())
                recordStatus = attrs["status"].get<std::string>();
            if (recordStatus != selected) return false;
        }

        // Supplier filter
        if (supplierFilterCombo_ && supplierFilterCombo_->currentIndex() > 0) {
            std::string selectedName = supplierFilterCombo_->currentText().toUTF8();
            std::string selectedId;
            for (const auto& pair : supplierNames_) {
                if (pair.second == selectedName) { selectedId = pair.first; break; }
            }
            if (!selectedId.empty()) {
                std::string recordId;
                if (attrs.contains("supplier_id") && !attrs["supplier_id"].is_null())
                    recordId = attrs["supplier_id"].is_string()
                        ? attrs["supplier_id"].get<std::string>()
                        : attrs["supplier_id"].dump();
                if (recordId != selectedId) return false;
            }
        }

        // Open only (exclude Received and Cancelled)
        if (openOnlyCheck_ && openOnlyCheck_->isChecked()) {
            std::string status;
            if (attrs.contains("status") && !attrs["status"].is_null())
                status = attrs["status"].get<std::string>();
            if (status == "Received" || status == "Cancelled") return false;
        }

        return true;
    }

private:
    void loadLookups() {
        // Suppliers
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

        // Products (for purchase items)
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
                    if (!pi.id.empty() && !pi.name.empty())
                        products_.push_back(pi);
                }
            }
        } catch (...) {}

        // Categories (for product filter in purchase items)
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

        // Fallback
        if (record.contains("attributes")) {
            const auto& attrs = record["attributes"];
            std::string fieldName = relName + "_id";
            if (attrs.contains(fieldName) && !attrs[fieldName].is_null()) {
                std::string id = attrs[fieldName].is_string()
                    ? attrs[fieldName].get<std::string>()
                    : attrs[fieldName].dump();
                if (relName == "supplier") {
                    auto it = supplierNames_.find(id);
                    if (it != supplierNames_.end()) return it->second;
                }
            }
        }
        return "-";
    }

    // Rebuild product combo based on category/supplier filters
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

    void showAddPurchaseDialog() {
        auto dialog = addChild(std::make_unique<Wt::WDialog>("Add Purchase"));
        dialog->setStyleClass("smitty-dialog smitty-dialog-wide");
        dialog->setModal(true);
        dialog->setClosable(true);
        dialog->rejectWhenEscapePressed(true);

        auto content = dialog->contents();
        content->setStyleClass("dialog-content");

        // Supplier dropdown
        content->addWidget(std::make_unique<Wt::WText>("Supplier"))
               ->setStyleClass("dialog-label");
        auto supplierCombo = content->addWidget(std::make_unique<Wt::WComboBox>());
        supplierCombo->setStyleClass("dialog-input");
        supplierCombo->addItem("");
        for (const auto& pair : supplierNames_) {
            supplierCombo->addItem(pair.second);
        }

        // Purchase Date
        content->addWidget(std::make_unique<Wt::WText>("Purchase Date"))
               ->setStyleClass("dialog-label");
        auto purchDateInput = content->addWidget(std::make_unique<Wt::WLineEdit>());
        purchDateInput->setStyleClass("dialog-input");
        purchDateInput->setPlaceholderText("YYYY-MM-DD");

        // Expected Date
        content->addWidget(std::make_unique<Wt::WText>("Expected Date"))
               ->setStyleClass("dialog-label");
        auto expectedDateInput = content->addWidget(std::make_unique<Wt::WLineEdit>());
        expectedDateInput->setStyleClass("dialog-input");
        expectedDateInput->setPlaceholderText("YYYY-MM-DD");

        // Status dropdown
        content->addWidget(std::make_unique<Wt::WText>("Status"))
               ->setStyleClass("dialog-label");
        auto statusCombo = content->addWidget(std::make_unique<Wt::WComboBox>());
        statusCombo->setStyleClass("dialog-input");
        for (const auto& s : PURCHASE_STATUSES) {
            statusCombo->addItem(s);
        }

        // Notes
        content->addWidget(std::make_unique<Wt::WText>("Notes"))
               ->setStyleClass("dialog-label");
        auto notesInput = content->addWidget(std::make_unique<Wt::WLineEdit>());
        notesInput->setStyleClass("dialog-input");

        // ===== PURCHASE ITEMS SECTION =====
        auto itemsSection = content->addWidget(std::make_unique<Wt::WContainerWidget>());
        itemsSection->setStyleClass("order-lines-section");

        auto itemsHeader = itemsSection->addWidget(std::make_unique<Wt::WContainerWidget>());
        itemsHeader->setStyleClass("order-lines-header");
        itemsHeader->addWidget(std::make_unique<Wt::WText>("Purchase Items"))
                   ->setStyleClass("order-lines-title");
        auto addItemBtn = itemsHeader->addWidget(
            std::make_unique<Wt::WPushButton>("+ Add Item"));
        addItemBtn->setStyleClass("order-lines-add-btn");

        // Category / Supplier filter row for product selection
        auto filterRow = itemsSection->addWidget(std::make_unique<Wt::WContainerWidget>());
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

        // Items table
        auto itemsTable = itemsSection->addWidget(std::make_unique<Wt::WTable>());
        itemsTable->setStyleClass("order-lines-table");
        itemsTable->setHeaderCount(1);
        itemsTable->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Product"));
        itemsTable->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Unit Cost"));
        itemsTable->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Qty"));
        itemsTable->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>(""));

        struct ItemRow {
            Wt::WComboBox* productCombo;
            Wt::WLineEdit* costInput;
            Wt::WLineEdit* qtyInput;
            int tableRow;
        };
        auto itemRows = std::make_shared<std::vector<ItemRow>>();

        auto addItemRow = [this, itemsTable, itemRows, catFilterCombo, supFilterCombo]() {
            int row = itemsTable->rowCount();

            auto productCombo = itemsTable->elementAt(row, 0)->addWidget(
                std::make_unique<Wt::WComboBox>());
            productCombo->setStyleClass("line-combo");
            rebuildProductCombo(productCombo, catFilterCombo, supFilterCombo);

            auto costInput = itemsTable->elementAt(row, 1)->addWidget(
                std::make_unique<Wt::WLineEdit>());
            costInput->setStyleClass("line-input");
            costInput->setText("0.00");

            auto qtyInput = itemsTable->elementAt(row, 2)->addWidget(
                std::make_unique<Wt::WLineEdit>());
            qtyInput->setStyleClass("line-input");
            qtyInput->setText("1");

            auto removeBtn = itemsTable->elementAt(row, 3)->addWidget(
                std::make_unique<Wt::WPushButton>("×"));
            removeBtn->setStyleClass("line-remove-btn");

            ItemRow ir{productCombo, costInput, qtyInput, row};
            itemRows->push_back(ir);

            // Auto-fill unit cost from product unit_price
            productCombo->changed().connect([this, productCombo, costInput] {
                std::string prodName = productCombo->currentText().toUTF8();
                for (const auto& prod : products_) {
                    if (prod.name == prodName) {
                        char buf[32];
                        snprintf(buf, sizeof(buf), "%.2f", prod.unitPrice);
                        costInput->setText(buf);
                        break;
                    }
                }
            });

            removeBtn->clicked().connect([itemsTable, itemRows, row] {
                for (int c = 0; c < 4; c++)
                    itemsTable->elementAt(row, c)->hide();
                for (auto& ir : *itemRows) {
                    if (ir.tableRow == row) { ir.productCombo = nullptr; break; }
                }
            });
        };

        addItemBtn->clicked().connect(addItemRow);

        // Rebuild product combos when filters change
        auto rebuildAll = [this, itemRows, catFilterCombo, supFilterCombo] {
            for (auto& ir : *itemRows) {
                if (ir.productCombo) {
                    std::string current = ir.productCombo->currentText().toUTF8();
                    rebuildProductCombo(ir.productCombo, catFilterCombo, supFilterCombo);
                    for (int i = 0; i < ir.productCombo->count(); i++) {
                        if (ir.productCombo->itemText(i).toUTF8() == current) {
                            ir.productCombo->setCurrentIndex(i);
                            break;
                        }
                    }
                }
            }
        };
        catFilterCombo->changed().connect(rebuildAll);
        supFilterCombo->changed().connect(rebuildAll);

        // Status message
        auto statusMsg = content->addWidget(std::make_unique<Wt::WText>());
        statusMsg->setStyleClass("dialog-status");

        // Buttons
        auto btnBar = content->addWidget(std::make_unique<Wt::WContainerWidget>());
        btnBar->setStyleClass("dialog-buttons");
        auto saveBtn = btnBar->addWidget(std::make_unique<Wt::WPushButton>("Save"));
        saveBtn->setStyleClass("action-btn");

        saveBtn->clicked().connect([=] {
            if (supplierCombo->currentText().empty()) {
                statusMsg->setText("Supplier is required.");
                return;
            }

            json attrs;

            // Resolve supplier ID
            std::string supName = supplierCombo->currentText().toUTF8();
            for (const auto& pair : supplierNames_) {
                if (pair.second == supName) {
                    try { attrs["supplier_id"] = std::stoi(pair.first); }
                    catch (...) { attrs["supplier_id"] = pair.first; }
                    break;
                }
            }

            attrs["status"] = statusCombo->currentText().toUTF8();

            if (!purchDateInput->text().empty())
                attrs["purchase_date"] = purchDateInput->text().toUTF8();
            if (!expectedDateInput->text().empty())
                attrs["expected_date"] = expectedDateInput->text().toUTF8();
            if (!notesInput->text().empty())
                attrs["notes"] = notesInput->text().toUTF8();

            // Calculate total cost from items
            double totalCost = 0.0;
            for (const auto& ir : *itemRows) {
                if (!ir.productCombo) continue;
                if (ir.productCombo->currentText().empty()) continue;
                try {
                    double cost = std::stod(ir.costInput->text().toUTF8());
                    int qty = std::stoi(ir.qtyInput->text().toUTF8());
                    totalCost += cost * qty;
                } catch (...) {}
            }
            if (totalCost > 0) attrs["total_cost"] = totalCost;

            try {
                auto resp = ApiClient::instance().createRecord("Purchase", attrs);
                if (resp.ok()) {
                    // Get new purchase ID
                    std::string newPurchaseId;
                    if (resp.hasData()) {
                        const auto& data = resp.data();
                        if (data.contains("id"))
                            newPurchaseId = data["id"].is_string()
                                ? data["id"].get<std::string>() : data["id"].dump();
                    }

                    // Save each purchase item
                    if (!newPurchaseId.empty()) {
                        for (const auto& ir : *itemRows) {
                            if (!ir.productCombo) continue;
                            std::string prodName = ir.productCombo->currentText().toUTF8();
                            if (prodName.empty()) continue;

                            std::string productId;
                            for (const auto& prod : products_) {
                                if (prod.name == prodName) { productId = prod.id; break; }
                            }
                            if (productId.empty()) continue;

                            json itemAttrs;
                            try { itemAttrs["purchase_id"] = std::stoi(newPurchaseId); }
                            catch (...) { itemAttrs["purchase_id"] = newPurchaseId; }
                            try { itemAttrs["product_id"] = std::stoi(productId); }
                            catch (...) { itemAttrs["product_id"] = productId; }

                            if (!ir.costInput->text().empty()) {
                                try { itemAttrs["unit_cost"] = std::stod(ir.costInput->text().toUTF8()); }
                                catch (...) { itemAttrs["unit_cost"] = 0.0; }
                            }
                            if (!ir.qtyInput->text().empty()) {
                                try { itemAttrs["quantity"] = std::stoi(ir.qtyInput->text().toUTF8()); }
                                catch (...) { itemAttrs["quantity"] = 1; }
                            }

                            ApiClient::instance().createRecord("PurchaseItem", itemAttrs);
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
    Wt::WComboBox* supplierFilterCombo_ = nullptr;
    Wt::WCheckBox* openOnlyCheck_ = nullptr;
    std::map<std::string, std::string> supplierNames_;
    std::map<std::string, std::string> categoryNames_;
    std::vector<ProductInfo> products_;
};

std::unique_ptr<EntityListView> createPurchaseList() {
    return std::make_unique<PurchaseList>();
}
