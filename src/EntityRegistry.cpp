#include "EntityRegistry.h"

EntityRegistry& EntityRegistry::instance() {
    static EntityRegistry registry;
    return registry;
}

EntityRegistry::EntityRegistry() {
    initializeEntities();
}

void EntityRegistry::registerEntity(std::shared_ptr<Entity> entity) {
    entities_[entity->resourceName()] = entity;
}

std::shared_ptr<Entity> EntityRegistry::getEntity(const std::string& resourceName) const {
    auto it = entities_.find(resourceName);
    if (it != entities_.end()) return it->second;
    return nullptr;
}

std::vector<std::string> EntityRegistry::entityNames() const {
    std::vector<std::string> names;
    for (const auto& pair : entities_) {
        names.push_back(pair.first);
    }
    return names;
}

void EntityRegistry::initializeEntities() {
    // Customer entity
    registerEntity(std::make_shared<Entity>(
        "Customer", "Customer", "customer_id",
        std::vector<ColumnDef>{
            {"company_name", "Company Name", "VARCHAR", true, true, true, true},
            {"contact_name", "Contact Name", "VARCHAR", false, false, false, true},
            {"contact_title", "Contact Title", "VARCHAR", false, false, false, true},
            {"address", "Address", "VARCHAR", false, false, false, true},
            {"city", "City", "VARCHAR", false, false, false, true},
            {"region", "Region", "VARCHAR", false, false, false, false},
            {"postal_code", "Postal Code", "VARCHAR", false, false, false, true},
            {"country", "Country", "VARCHAR", false, false, false, false},
            {"phone", "Phone", "VARCHAR", false, false, false, true},
            {"fax", "Fax", "VARCHAR", false, false, false, true},
            {"customer_id", "Customer ID", "VARCHAR", true, false, false, true}
        }
    ));

    // Order entity
    registerEntity(std::make_shared<Entity>(
        "Order", "Order", "order_id",
        std::vector<ColumnDef>{
            {"ship_name", "Ship Name", "VARCHAR", false, true, true, true},
            {"customer_id", "Customer", "VARCHAR", false, false, false, true},
            {"employee_id", "Ordered By", "SMALLINT", false, false, false, true},
            {"order_date", "Order Date", "DATE", false, false, false, true},
            {"required_date", "Required Date", "DATE", false, false, false, true},
            {"shipped_date", "Shipped Date", "DATE", false, false, false, true},
            {"freight", "Freight", "FLOAT", false, false, false, true},
            {"ship_address", "Ship Address", "VARCHAR", false, false, false, true},
            {"ship_city", "Ship City", "VARCHAR", false, false, false, true},
            {"ship_region", "Ship Region", "VARCHAR", false, false, false, true},
            {"ship_postal_code", "Ship Postal Code", "VARCHAR", false, false, false, true},
            {"ship_country", "Ship Country", "VARCHAR", false, false, false, true},
            {"ship_via", "Ship Via", "SMALLINT", false, false, false, true},
            {"order_id", "Order ID", "SMALLINT", false, false, false, true}
        }
    ));

    // OrderDetail entity (line items)
    registerEntity(std::make_shared<Entity>(
        "OrderDetail", "OrderDetail", "",
        std::vector<ColumnDef>{
            {"order_id", "Order ID", "SMALLINT", false, false, false, true},
            {"product_id", "Product ID", "SMALLINT", false, false, false, true},
            {"unit_price", "Unit Price", "FLOAT", false, false, false, true},
            {"quantity", "Quantity", "SMALLINT", false, false, false, true},
            {"discount", "Discount", "FLOAT", false, false, false, true}
        }
    ));

    // Product entity
    registerEntity(std::make_shared<Entity>(
        "Product", "Product", "product_id",
        std::vector<ColumnDef>{
            {"product_name", "Product Name", "VARCHAR", true, true, true, true},
            {"category_id", "Category", "SMALLINT", false, false, false, true},
            {"supplier_id", "Supplier", "SMALLINT", false, false, false, true},
            {"quantity_per_unit", "Qty Per Unit", "VARCHAR", false, false, false, true},
            {"unit_price", "Unit Price", "FLOAT", false, false, false, true},
            {"units_in_stock", "In Stock", "SMALLINT", false, false, false, true},
            {"units_on_order", "On Order", "SMALLINT", false, false, false, true},
            {"reorder_level", "Reorder Level", "SMALLINT", false, false, false, true},
            {"product_id", "Product ID", "SMALLINT", false, false, false, true},
            {"discontinued", "Discontinued", "INTEGER", true, false, false, true}
        }
    ));
}
