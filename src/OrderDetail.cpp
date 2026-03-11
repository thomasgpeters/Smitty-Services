#include "EntityDetailView.h"
#include "EntityRegistry.h"

class OrderDetail : public EntityDetailView {
public:
    OrderDetail()
        : EntityDetailView(EntityRegistry::instance().getEntity("Order")) {
    }
};

std::unique_ptr<EntityDetailView> createOrderDetail() {
    return std::make_unique<OrderDetail>();
}
