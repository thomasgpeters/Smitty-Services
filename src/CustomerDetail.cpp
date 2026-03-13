#include "EntityDetailView.h"
#include "EntityRegistry.h"
#include "ApiClient.h"
#include <Wt/WTable.h>
#include <Wt/WText.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <map>

class CustomerDetail : public EntityDetailView {
public:
    CustomerDetail()
        : EntityDetailView(EntityRegistry::instance().getEntity("Customer")) {
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
                // Primary key is read-only
                auto value = group->addWidget(std::make_unique<Wt::WText>(
                    val.empty() ? "-" : val, Wt::TextFormat::Plain));
                value->setStyleClass("form-value");
            } else {
                // Editable field
                auto input = group->addWidget(std::make_unique<Wt::WLineEdit>());
                input->setStyleClass("form-input");
                if (!val.empty() && val != "-") {
                    input->setText(val);
                }
                fieldEdits_[col.name] = input;
            }
        }

        // Save button below the form fields
        auto saveBar = fieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
        saveBar->setStyleClass("detail-save-bar");

        saveStatus_ = saveBar->addWidget(std::make_unique<Wt::WText>());
        saveStatus_->setStyleClass("detail-save-status");

        auto saveBtn = saveBar->addWidget(std::make_unique<Wt::WPushButton>("Save"));
        saveBtn->setStyleClass("action-btn");
        saveBtn->clicked().connect(this, &CustomerDetail::saveRecord);
    }

    void addChildContent(Wt::WContainerWidget* container, const json& record) override {
        std::string custId;
        if (record.contains("id")) {
            custId = record["id"].is_string()
                ? record["id"].get<std::string>()
                : record["id"].dump();
        }
        if (custId.empty()) return;

        auto section = container->addWidget(std::make_unique<Wt::WContainerWidget>());
        section->setStyleClass("child-grid-section");

        section->addWidget(std::make_unique<Wt::WText>("Customer Orders"))
               ->setStyleClass("child-grid-title");

        auto table = section->addWidget(std::make_unique<Wt::WTable>());
        table->setStyleClass("child-grid-table");
        table->setHeaderCount(1);

        // Header
        table->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Invoice ID"));
        table->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Invoice Date"));
        table->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Ship Name"));
        table->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Ship City"));
        table->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Ship Country"));
        table->elementAt(0, 5)->addWidget(std::make_unique<Wt::WText>("Shipped Date"));

        try {
            auto resp = ApiClient::instance().fetchAll("Order",
                custId, "");

            if (!resp.ok() || !resp.hasData() || !resp.data().is_array()) {
                section->addWidget(std::make_unique<Wt::WText>("No orders found."));
                return;
            }

            int row = 1;
            for (const auto& order : resp.data()) {
                if (!order.contains("attributes")) continue;
                const auto& attrs = order["attributes"];

                // Filter to only this customer's orders
                std::string orderCustId;
                if (attrs.contains("customer_id") && !attrs["customer_id"].is_null()) {
                    orderCustId = attrs["customer_id"].is_string()
                        ? attrs["customer_id"].get<std::string>()
                        : attrs["customer_id"].dump();
                }
                if (orderCustId != custId) continue;

                std::string orderId = order.contains("id")
                    ? (order["id"].is_string() ? order["id"].get<std::string>() : order["id"].dump())
                    : "-";

                auto getAttr = [&](const std::string& key) -> std::string {
                    if (attrs.contains(key) && !attrs[key].is_null())
                        return attrs[key].is_string() ? attrs[key].get<std::string>() : attrs[key].dump();
                    return "-";
                };

                table->elementAt(row, 0)->addWidget(
                    std::make_unique<Wt::WText>(orderId));
                table->elementAt(row, 1)->addWidget(
                    std::make_unique<Wt::WText>(getAttr("order_date")));
                table->elementAt(row, 2)->addWidget(
                    std::make_unique<Wt::WText>(getAttr("ship_name")));
                table->elementAt(row, 3)->addWidget(
                    std::make_unique<Wt::WText>(getAttr("ship_city")));
                table->elementAt(row, 4)->addWidget(
                    std::make_unique<Wt::WText>(getAttr("ship_country")));
                table->elementAt(row, 5)->addWidget(
                    std::make_unique<Wt::WText>(getAttr("shipped_date")));

                ++row;
            }

            if (row == 1) {
                section->addWidget(std::make_unique<Wt::WText>("No orders found."));
            }
        } catch (const std::exception& e) {
            section->addWidget(std::make_unique<Wt::WText>(
                std::string("Error loading orders: ") + e.what()));
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

std::unique_ptr<EntityDetailView> createCustomerDetail() {
    return std::make_unique<CustomerDetail>();
}
