#include "EntityListView.h"
#include "EntityRegistry.h"

class CustomerList : public EntityListView {
public:
    CustomerList()
        : EntityListView(EntityRegistry::instance().getEntity("Customer")) {
    }
};

// Factory function for SmittyApplication
std::unique_ptr<EntityListView> createCustomerList() {
    return std::make_unique<CustomerList>();
}
