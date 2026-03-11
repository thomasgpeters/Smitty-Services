#include "EntityDetailView.h"
#include "EntityRegistry.h"

class CustomerDetail : public EntityDetailView {
public:
    CustomerDetail()
        : EntityDetailView(EntityRegistry::instance().getEntity("Customer")) {
    }
};

std::unique_ptr<EntityDetailView> createCustomerDetail() {
    return std::make_unique<CustomerDetail>();
}
