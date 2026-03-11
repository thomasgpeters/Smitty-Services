#include "EntityListView.h"
#include "EntityRegistry.h"

class OrderList : public EntityListView {
public:
    OrderList()
        : EntityListView(EntityRegistry::instance().getEntity("Order")) {
    }
};

std::unique_ptr<EntityListView> createOrderList() {
    return std::make_unique<OrderList>();
}
