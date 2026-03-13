#include "EntityDetailView.h"
#include "EntityRegistry.h"
#include "ApiClient.h"
#include "AppSettings.h"
#include <Wt/WTable.h>
#include <Wt/WText.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <map>

class OrderDetailView : public EntityDetailView {
public:
    OrderDetailView()
        : EntityDetailView(EntityRegistry::instance().getEntity("Order")) {
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
        saveBtn->clicked().connect(this, &OrderDetailView::saveRecord);
    }

    void addChildContent(Wt::WContainerWidget* container, const json& record) override {
        std::string orderId;
        if (record.contains("id")) {
            orderId = record["id"].is_string()
                ? record["id"].get<std::string>()
                : record["id"].dump();
        }
        if (orderId.empty()) return;

        // Pre-fetch product names for display
        std::map<std::string, std::string> productNames;
        try {
            auto prodResp = ApiClient::instance().fetchAll("Product");
            if (prodResp.ok() && prodResp.hasData() && prodResp.data().is_array()) {
                for (const auto& prod : prodResp.data()) {
                    std::string pid = prod.contains("id")
                        ? (prod["id"].is_string() ? prod["id"].get<std::string>() : prod["id"].dump())
                        : "";
                    if (prod.contains("attributes") && prod["attributes"].contains("product_name"))
                        productNames[pid] = prod["attributes"]["product_name"].get<std::string>();
                }
            }
        } catch (...) {}

        auto section = container->addWidget(std::make_unique<Wt::WContainerWidget>());
        section->setStyleClass("child-grid-section");

        section->addWidget(std::make_unique<Wt::WText>("Invoice Line Items"))
               ->setStyleClass("child-grid-title");

        auto table = section->addWidget(std::make_unique<Wt::WTable>());
        table->setStyleClass("child-grid-table");
        table->setHeaderCount(1);

        // Header
        table->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Product"));
        table->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Unit Price"));
        table->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Quantity"));
        table->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Discount"));
        table->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Line Total"));

        try {
            auto resp = ApiClient::instance().fetchAll("OrderDetail",
                orderId, "");

            if (!resp.ok() || !resp.hasData() || !resp.data().is_array()) {
                section->addWidget(std::make_unique<Wt::WText>("No line items found."));
                return;
            }

            std::string currency = AppSettings::instance().currencySymbol();
            int row = 1;
            for (const auto& item : resp.data()) {
                if (!item.contains("attributes")) continue;
                const auto& attrs = item["attributes"];

                // Filter to only this order's items
                std::string itemOrderId;
                if (attrs.contains("order_id") && !attrs["order_id"].is_null()) {
                    itemOrderId = attrs["order_id"].is_string()
                        ? attrs["order_id"].get<std::string>()
                        : attrs["order_id"].dump();
                }
                if (itemOrderId != orderId) continue;

                // Product name lookup
                std::string productId;
                if (attrs.contains("product_id") && !attrs["product_id"].is_null()) {
                    productId = attrs["product_id"].is_string()
                        ? attrs["product_id"].get<std::string>()
                        : attrs["product_id"].dump();
                }
                auto pIt = productNames.find(productId);
                std::string productName = (pIt != productNames.end()) ? pIt->second : productId;

                double unitPrice = 0;
                if (attrs.contains("unit_price") && !attrs["unit_price"].is_null()) {
                    try { unitPrice = attrs["unit_price"].get<double>(); } catch (...) {}
                }

                int quantity = 0;
                if (attrs.contains("quantity") && !attrs["quantity"].is_null()) {
                    try { quantity = attrs["quantity"].get<int>(); } catch (...) {}
                }

                double discount = 0;
                if (attrs.contains("discount") && !attrs["discount"].is_null()) {
                    try { discount = attrs["discount"].get<double>(); } catch (...) {}
                }

                double lineTotal = unitPrice * quantity * (1.0 - discount);

                char priceBuf[64], totalBuf[64], discBuf[64];
                snprintf(priceBuf, sizeof(priceBuf), "%.2f", unitPrice);
                snprintf(totalBuf, sizeof(totalBuf), "%.2f", lineTotal);
                snprintf(discBuf, sizeof(discBuf), "%.0f%%", discount * 100);

                table->elementAt(row, 0)->addWidget(
                    std::make_unique<Wt::WText>(productName));
                table->elementAt(row, 1)->addWidget(
                    std::make_unique<Wt::WText>(currency + priceBuf));
                table->elementAt(row, 2)->addWidget(
                    std::make_unique<Wt::WText>(std::to_string(quantity)));
                table->elementAt(row, 3)->addWidget(
                    std::make_unique<Wt::WText>(discBuf));
                table->elementAt(row, 4)->addWidget(
                    std::make_unique<Wt::WText>(currency + totalBuf));

                ++row;
            }

            if (row == 1) {
                section->addWidget(std::make_unique<Wt::WText>("No line items found."));
            }
        } catch (const std::exception& e) {
            section->addWidget(std::make_unique<Wt::WText>(
                std::string("Error loading line items: ") + e.what()));
        }
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

std::unique_ptr<EntityDetailView> createOrderDetail() {
    return std::make_unique<OrderDetailView>();
}
