#include "EntityListView.h"
#include "EntityRegistry.h"

class ProductList : public EntityListView {
public:
    ProductList()
        : EntityListView(EntityRegistry::instance().getEntity("Product")) {
    }
};

std::unique_ptr<EntityListView> createProductList() {
    return std::make_unique<ProductList>();
}
