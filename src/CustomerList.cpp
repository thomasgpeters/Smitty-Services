#include "EntityListView.h"
#include "EntityRegistry.h"
#include "ApiClient.h"
#include <Wt/WCheckBox.h>
#include <set>

class CustomerList : public EntityListView {
public:
    CustomerList()
        : EntityListView(EntityRegistry::instance().getEntity("Customer"))
        , outstandingBalanceCheck_(nullptr) {
    }

protected:
    void addCustomFilters(Wt::WContainerWidget* filterBar) override {
        outstandingBalanceCheck_ = filterBar->addWidget(
            std::make_unique<Wt::WCheckBox>("Outstanding Balance"));
        outstandingBalanceCheck_->setStyleClass("filter-checkbox");
        outstandingBalanceCheck_->changed().connect([this] {
            if (outstandingBalanceCheck_->isChecked()) {
                loadUnpaidCustomerIds();
            } else {
                unpaidCustomerIds_.clear();
            }
            refresh();
        });
    }

    bool filterRecord(const json& record) const override {
        if (!outstandingBalanceCheck_ || !outstandingBalanceCheck_->isChecked())
            return true;

        // Only show customers who have unpaid orders
        std::string custId;
        if (record.contains("id")) {
            custId = record["id"].is_string()
                ? record["id"].get<std::string>()
                : record["id"].dump();
        }
        return unpaidCustomerIds_.count(custId) > 0;
    }

private:
    void loadUnpaidCustomerIds() {
        unpaidCustomerIds_.clear();
        try {
            auto resp = ApiClient::instance().fetchAll("Order");
            if (resp.ok() && resp.hasData() && resp.data().is_array()) {
                for (const auto& order : resp.data()) {
                    if (!order.contains("attributes")) continue;
                    const auto& attrs = order["attributes"];

                    bool shipped = attrs.contains("shipped_date")
                        && !attrs["shipped_date"].is_null()
                        && attrs["shipped_date"].get<std::string>() != "";
                    if (!shipped) {
                        if (attrs.contains("customer_id") && !attrs["customer_id"].is_null()) {
                            unpaidCustomerIds_.insert(
                                attrs["customer_id"].get<std::string>());
                        }
                    }
                }
            }
        } catch (...) {}
    }

    Wt::WCheckBox* outstandingBalanceCheck_;
    std::set<std::string> unpaidCustomerIds_;
};

// Factory function for SmittyApplication
std::unique_ptr<EntityListView> createCustomerList() {
    return std::make_unique<CustomerList>();
}
