#include "EntityListView.h"
#include "EntityRegistry.h"
#include <Wt/WCheckBox.h>

class ProductList : public EntityListView {
public:
    ProductList()
        : EntityListView(EntityRegistry::instance().getEntity("Product"))
        , pendingReceiptCheck_(nullptr) {
    }

protected:
    void addCustomFilters(Wt::WContainerWidget* filterBar) override {
        pendingReceiptCheck_ = filterBar->addWidget(
            std::make_unique<Wt::WCheckBox>("Pending Receipt"));
        pendingReceiptCheck_->setStyleClass("filter-checkbox");
        pendingReceiptCheck_->changed().connect(this, &EntityListView::refresh);
    }

    bool filterRecord(const json& record) const override {
        if (!pendingReceiptCheck_ || !pendingReceiptCheck_->isChecked())
            return true;

        // Show only products with units_on_order > 0 (not yet received)
        if (!record.contains("attributes")) return false;
        const auto& attrs = record["attributes"];
        if (attrs.contains("units_on_order") && !attrs["units_on_order"].is_null()) {
            try {
                int onOrder = attrs["units_on_order"].get<int>();
                return onOrder > 0;
            } catch (...) {
                return false;
            }
        }
        return false;
    }

private:
    Wt::WCheckBox* pendingReceiptCheck_;
};

std::unique_ptr<EntityListView> createProductList() {
    return std::make_unique<ProductList>();
}
