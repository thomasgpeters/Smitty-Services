#include "EntityListView.h"
#include "EntityRegistry.h"
#include <Wt/WCheckBox.h>

class OrderList : public EntityListView {
public:
    OrderList()
        : EntityListView(EntityRegistry::instance().getEntity("Order"))
        , unpaidCheck_(nullptr) {
    }

protected:
    void addCustomFilters(Wt::WContainerWidget* filterBar) override {
        unpaidCheck_ = filterBar->addWidget(
            std::make_unique<Wt::WCheckBox>("Unpaid Orders"));
        unpaidCheck_->setStyleClass("filter-checkbox");
        unpaidCheck_->changed().connect(this, &EntityListView::refresh);
    }

    bool filterRecord(const json& record) const override {
        if (!unpaidCheck_ || !unpaidCheck_->isChecked())
            return true;

        // Show only orders that have not been shipped (unpaid)
        if (!record.contains("attributes")) return false;
        const auto& attrs = record["attributes"];

        bool shipped = attrs.contains("shipped_date")
            && !attrs["shipped_date"].is_null()
            && attrs["shipped_date"].get<std::string>() != "";
        return !shipped;
    }

private:
    Wt::WCheckBox* unpaidCheck_;
};

std::unique_ptr<EntityListView> createOrderList() {
    return std::make_unique<OrderList>();
}
