#include "EntityListView.h"
#include "EntityRegistry.h"
#include "ApiClient.h"
#include <Wt/WCheckBox.h>
#include <Wt/WDialog.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WComboBox.h>
#include <Wt/WTemplate.h>
#include <Wt/WText.h>
#include <map>

class ProductList : public EntityListView {
public:
    ProductList()
        : EntityListView(EntityRegistry::instance().getEntity("Product"))
        , pendingReceiptCheck_(nullptr) {
        loadLookups();
    }

protected:
    std::string includeParam() const override {
        return "category,supplier";
    }

    std::string resolveFieldValue(const json& record, const ColumnDef& col,
                                   const std::string& rawValue) const override {
        if (col.name == "category_id") {
            return lookupIncluded(record, "category", "category_name");
        }
        if (col.name == "supplier_id") {
            return lookupIncluded(record, "supplier", "company_name");
        }
        return rawValue;
    }

    void addCustomFilters(Wt::WContainerWidget* filterBar) override {
        // Category filter dropdown
        categoryFilterCombo_ = filterBar->addWidget(std::make_unique<Wt::WComboBox>());
        categoryFilterCombo_->setStyleClass("filter-combo");
        categoryFilterCombo_->addItem("All Categories");
        for (const auto& pair : categoryNames_) {
            categoryFilterCombo_->addItem(pair.second);
        }
        categoryFilterCombo_->changed().connect(this, &EntityListView::refresh);

        // Supplier filter dropdown
        supplierFilterCombo_ = filterBar->addWidget(std::make_unique<Wt::WComboBox>());
        supplierFilterCombo_->setStyleClass("filter-combo");
        supplierFilterCombo_->addItem("All Suppliers");
        for (const auto& pair : supplierNames_) {
            supplierFilterCombo_->addItem(pair.second);
        }
        supplierFilterCombo_->changed().connect(this, &EntityListView::refresh);

        // Pending Receipt checkbox
        pendingReceiptCheck_ = filterBar->addWidget(
            std::make_unique<Wt::WCheckBox>("Pending Receipt"));
        pendingReceiptCheck_->setStyleClass("filter-checkbox");
        pendingReceiptCheck_->changed().connect(this, &EntityListView::refresh);
    }

    void addActionButtons(Wt::WContainerWidget* actionBar) override {
        auto newProductBtn = actionBar->addWidget(
            std::make_unique<Wt::WPushButton>("New Product"));
        newProductBtn->setStyleClass("action-btn");
        newProductBtn->clicked().connect(this, &ProductList::showAddProductDialog);

        auto newPurchaseBtn = actionBar->addWidget(
            std::make_unique<Wt::WPushButton>("New Purchase"));
        newPurchaseBtn->setStyleClass("action-btn action-btn-secondary");
        newPurchaseBtn->clicked().connect(this, &ProductList::showNewPurchase);
    }

    bool filterRecord(const json& record) const override {
        if (!record.contains("attributes")) return false;
        const auto& attrs = record["attributes"];

        // Category filter
        if (categoryFilterCombo_ && categoryFilterCombo_->currentIndex() > 0) {
            std::string selectedCatName = categoryFilterCombo_->currentText().toUTF8();
            std::string selectedCatId;
            for (const auto& pair : categoryNames_) {
                if (pair.second == selectedCatName) { selectedCatId = pair.first; break; }
            }
            if (!selectedCatId.empty()) {
                std::string recordCatId;
                if (attrs.contains("category_id") && !attrs["category_id"].is_null())
                    recordCatId = attrs["category_id"].is_string()
                        ? attrs["category_id"].get<std::string>()
                        : attrs["category_id"].dump();
                if (recordCatId != selectedCatId) return false;
            }
        }

        // Supplier filter
        if (supplierFilterCombo_ && supplierFilterCombo_->currentIndex() > 0) {
            std::string selectedSupName = supplierFilterCombo_->currentText().toUTF8();
            std::string selectedSupId;
            for (const auto& pair : supplierNames_) {
                if (pair.second == selectedSupName) { selectedSupId = pair.first; break; }
            }
            if (!selectedSupId.empty()) {
                std::string recordSupId;
                if (attrs.contains("supplier_id") && !attrs["supplier_id"].is_null())
                    recordSupId = attrs["supplier_id"].is_string()
                        ? attrs["supplier_id"].get<std::string>()
                        : attrs["supplier_id"].dump();
                if (recordSupId != selectedSupId) return false;
            }
        }

        // Pending Receipt filter
        if (pendingReceiptCheck_ && pendingReceiptCheck_->isChecked()) {
            if (attrs.contains("units_on_order") && !attrs["units_on_order"].is_null()) {
                try {
                    int onOrder = attrs["units_on_order"].get<int>();
                    if (onOrder <= 0) return false;
                } catch (...) {
                    return false;
                }
            } else {
                return false;
            }
        }

        return true;
    }

private:
    void loadLookups() {
        try {
            auto catResp = ApiClient::instance().fetchAll("Category");
            if (catResp.ok() && catResp.hasData() && catResp.data().is_array()) {
                for (const auto& cat : catResp.data()) {
                    std::string id = cat.contains("id")
                        ? (cat["id"].is_string() ? cat["id"].get<std::string>() : cat["id"].dump())
                        : "";
                    std::string name;
                    if (cat.contains("attributes") && cat["attributes"].contains("category_name"))
                        name = cat["attributes"]["category_name"].get<std::string>();
                    if (!id.empty() && !name.empty()) {
                        categoryNames_[id] = name;
                    }
                }
            }
        } catch (...) {}

        try {
            auto supResp = ApiClient::instance().fetchAll("Supplier");
            if (supResp.ok() && supResp.hasData() && supResp.data().is_array()) {
                for (const auto& sup : supResp.data()) {
                    std::string id = sup.contains("id")
                        ? (sup["id"].is_string() ? sup["id"].get<std::string>() : sup["id"].dump())
                        : "";
                    std::string name;
                    if (sup.contains("attributes") && sup["attributes"].contains("company_name"))
                        name = sup["attributes"]["company_name"].get<std::string>();
                    if (!id.empty() && !name.empty()) {
                        supplierNames_[id] = name;
                    }
                }
            }
        } catch (...) {}
    }

    std::string lookupIncluded(const json& record, const std::string& relName,
                                const std::string& attrName) const {
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
                        if (incType == relType && incId == relId) {
                            if (inc.contains("attributes") && inc["attributes"].contains(attrName))
                                return inc["attributes"][attrName].get<std::string>();
                        }
                    }
                }
            }
        }

        // Fallback to pre-fetched lookups
        if (record.contains("attributes")) {
            const auto& attrs = record["attributes"];
            if (relName == "category" && attrs.contains("category_id") && !attrs["category_id"].is_null()) {
                std::string id = attrs["category_id"].is_string()
                    ? attrs["category_id"].get<std::string>()
                    : attrs["category_id"].dump();
                auto it = categoryNames_.find(id);
                if (it != categoryNames_.end()) return it->second;
            }
            if (relName == "supplier" && attrs.contains("supplier_id") && !attrs["supplier_id"].is_null()) {
                std::string id = attrs["supplier_id"].is_string()
                    ? attrs["supplier_id"].get<std::string>()
                    : attrs["supplier_id"].dump();
                auto it = supplierNames_.find(id);
                if (it != supplierNames_.end()) return it->second;
            }
        }

        return "-";
    }

    void showAddProductDialog() {
        auto dialog = addChild(std::make_unique<Wt::WDialog>("Add Product"));
        dialog->setStyleClass("smitty-dialog");
        dialog->setModal(true);
        dialog->setClosable(true);
        dialog->rejectWhenEscapePressed(true);

        auto content = dialog->contents();
        content->setStyleClass("dialog-content");

        // Product Name
        content->addWidget(std::make_unique<Wt::WText>("Product Name"))
               ->setStyleClass("dialog-label");
        auto nameInput = content->addWidget(std::make_unique<Wt::WLineEdit>());
        nameInput->setStyleClass("dialog-input");

        // Category dropdown
        content->addWidget(std::make_unique<Wt::WText>("Category"))
               ->setStyleClass("dialog-label");
        auto catCombo = content->addWidget(std::make_unique<Wt::WComboBox>());
        catCombo->setStyleClass("dialog-input");
        catCombo->addItem("");
        for (const auto& pair : categoryNames_) {
            catCombo->addItem(pair.second);
        }

        // Supplier dropdown
        content->addWidget(std::make_unique<Wt::WText>("Supplier"))
               ->setStyleClass("dialog-label");
        auto supCombo = content->addWidget(std::make_unique<Wt::WComboBox>());
        supCombo->setStyleClass("dialog-input");
        supCombo->addItem("");
        for (const auto& pair : supplierNames_) {
            supCombo->addItem(pair.second);
        }

        // Qty Per Unit
        content->addWidget(std::make_unique<Wt::WText>("Qty Per Unit"))
               ->setStyleClass("dialog-label");
        auto qtyInput = content->addWidget(std::make_unique<Wt::WLineEdit>());
        qtyInput->setStyleClass("dialog-input");

        // Unit Price
        content->addWidget(std::make_unique<Wt::WText>("Unit Price"))
               ->setStyleClass("dialog-label");
        auto priceInput = content->addWidget(std::make_unique<Wt::WLineEdit>());
        priceInput->setStyleClass("dialog-input");

        // Units In Stock
        content->addWidget(std::make_unique<Wt::WText>("Units In Stock"))
               ->setStyleClass("dialog-label");
        auto stockInput = content->addWidget(std::make_unique<Wt::WLineEdit>());
        stockInput->setStyleClass("dialog-input");
        stockInput->setText("0");

        // Reorder Level
        content->addWidget(std::make_unique<Wt::WText>("Reorder Level"))
               ->setStyleClass("dialog-label");
        auto reorderInput = content->addWidget(std::make_unique<Wt::WLineEdit>());
        reorderInput->setStyleClass("dialog-input");
        reorderInput->setText("0");

        // Status message
        auto statusMsg = content->addWidget(std::make_unique<Wt::WText>());
        statusMsg->setStyleClass("dialog-status");

        // Buttons
        auto btnBar = content->addWidget(std::make_unique<Wt::WContainerWidget>());
        btnBar->setStyleClass("dialog-buttons");

        auto saveBtn = btnBar->addWidget(std::make_unique<Wt::WPushButton>("Save"));
        saveBtn->setStyleClass("action-btn");

        saveBtn->clicked().connect([=] {
            if (nameInput->text().empty()) {
                statusMsg->setText("Product Name is required.");
                return;
            }

            json attrs;
            attrs["product_name"] = nameInput->text().toUTF8();
            attrs["quantity_per_unit"] = qtyInput->text().toUTF8();
            attrs["units_in_stock"] = stockInput->text().empty() ? 0 : std::stoi(stockInput->text().toUTF8());
            attrs["reorder_level"] = reorderInput->text().empty() ? 0 : std::stoi(reorderInput->text().toUTF8());
            attrs["discontinued"] = 0;

            if (!priceInput->text().empty()) {
                try { attrs["unit_price"] = std::stod(priceInput->text().toUTF8()); }
                catch (...) {}
            }

            // Resolve category name back to ID
            std::string catName = catCombo->currentText().toUTF8();
            for (const auto& pair : categoryNames_) {
                if (pair.second == catName) {
                    try { attrs["category_id"] = std::stoi(pair.first); }
                    catch (...) { attrs["category_id"] = pair.first; }
                    break;
                }
            }

            // Resolve supplier name back to ID
            std::string supName = supCombo->currentText().toUTF8();
            for (const auto& pair : supplierNames_) {
                if (pair.second == supName) {
                    try { attrs["supplier_id"] = std::stoi(pair.first); }
                    catch (...) { attrs["supplier_id"] = pair.first; }
                    break;
                }
            }

            try {
                auto resp = ApiClient::instance().createRecord("Product", attrs);
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

    void showNewPurchase() {
        // Placeholder - will navigate to Purchases page when implemented
    }

    Wt::WCheckBox* pendingReceiptCheck_;
    Wt::WComboBox* categoryFilterCombo_ = nullptr;
    Wt::WComboBox* supplierFilterCombo_ = nullptr;
    std::map<std::string, std::string> categoryNames_;
    std::map<std::string, std::string> supplierNames_;
};

std::unique_ptr<EntityListView> createProductList() {
    return std::make_unique<ProductList>();
}
