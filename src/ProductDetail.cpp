#include "EntityDetailView.h"
#include "EntityRegistry.h"

class ProductDetail : public EntityDetailView {
public:
    ProductDetail()
        : EntityDetailView(EntityRegistry::instance().getEntity("Product")) {
    }
};

std::unique_ptr<EntityDetailView> createProductDetail() {
    return std::make_unique<ProductDetail>();
}
