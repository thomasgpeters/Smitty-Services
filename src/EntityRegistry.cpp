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

    // Order entity (displayed as "Invoice")
    registerEntity(std::make_shared<Entity>(
        "Order", "Invoice", "order_id",
        std::vector<ColumnDef>{
            {"ship_name", "Ship Name", "VARCHAR", false, true, true, true},
            {"customer_id", "Customer", "VARCHAR", false, false, false, true},
            {"employee_id", "Invoiced By", "SMALLINT", false, false, false, true},
            {"order_date", "Invoice Date", "DATE", false, false, false, true},
            {"required_date", "Required Date", "DATE", false, false, false, true},
            {"shipped_date", "Shipped Date", "DATE", false, false, false, true},
            {"freight", "Freight", "FLOAT", false, false, false, true},
            {"ship_address", "Ship Address", "VARCHAR", false, false, false, true},
            {"ship_city", "Ship City", "VARCHAR", false, false, false, true},
            {"ship_region", "Ship Region", "VARCHAR", false, false, false, true},
            {"ship_postal_code", "Ship Postal Code", "VARCHAR", false, false, false, true},
            {"ship_country", "Ship Country", "VARCHAR", false, false, false, true},
            {"ship_via", "Ship Via", "SMALLINT", false, false, false, true},
            {"order_id", "Invoice ID", "SMALLINT", false, false, false, true}
        }
    ));

    // OrderDetail entity (invoice line items)
    registerEntity(std::make_shared<Entity>(
        "OrderDetail", "InvoiceDetail", "",
        std::vector<ColumnDef>{
            {"order_id", "Invoice ID", "SMALLINT", false, false, false, true},
            {"product_id", "Product ID", "SMALLINT", false, false, false, true},
            {"unit_price", "Unit Price", "FLOAT", false, false, false, true},
            {"quantity", "Quantity", "SMALLINT", false, false, false, true},
            {"discount", "Discount", "FLOAT", false, false, false, true}
        }
    ));

    // Vehicle entity
    registerEntity(std::make_shared<Entity>(
        "Vehicle", "Vehicle", "vehicle_id",
        std::vector<ColumnDef>{
            {"description", "Description", "VARCHAR", true, true, true, true},
            {"customer_id", "Owner", "VARCHAR", false, false, false, true},
            {"vin", "VIN", "VARCHAR", true, false, false, true},
            {"year", "Year", "SMALLINT", false, false, false, true},
            {"make", "Make", "VARCHAR", false, false, false, true},
            {"model", "Model", "VARCHAR", false, false, false, true},
            {"license_plate", "License Plate", "VARCHAR", false, false, false, true},
            {"notes", "Notes", "TEXT", false, false, false, true},
            {"vehicle_id", "Vehicle ID", "SMALLINT", false, false, false, true}
        }
    ));

    // Job entity
    registerEntity(std::make_shared<Entity>(
        "Job", "Job", "job_id",
        std::vector<ColumnDef>{
            {"service_description", "Service Description", "TEXT", true, true, true, true},
            {"customer_id", "Customer", "VARCHAR", false, false, false, true},
            {"vehicle_id", "Vehicle", "SMALLINT", false, false, false, true},
            {"status", "Status", "VARCHAR", true, false, false, true},
            {"created_date", "Created", "DATE", false, false, false, true},
            {"started_date", "Started", "DATE", false, false, false, true},
            {"completed_date", "Completed", "DATE", false, false, false, true},
            {"estimated_cost", "Estimated Cost", "FLOAT", false, false, false, true},
            {"actual_cost", "Actual Cost", "FLOAT", false, false, false, true},
            {"notes", "Notes", "TEXT", false, false, false, true},
            {"job_id", "Job ID", "SMALLINT", false, false, false, true}
        }
    ));

    // Purchase entity (displayed as "PO")
    registerEntity(std::make_shared<Entity>(
        "Purchase", "PO", "purchase_id",
        std::vector<ColumnDef>{
            {"supplier_id", "Supplier", "SMALLINT", false, true, false, true},
            {"status", "Status", "VARCHAR", true, false, false, true},
            {"purchase_date", "PO Date", "DATE", false, false, false, true},
            {"expected_date", "Expected Date", "DATE", false, false, false, true},
            {"received_date", "Received Date", "DATE", false, false, false, true},
            {"total_cost", "Total Cost", "FLOAT", false, false, false, true},
            {"notes", "Notes", "TEXT", false, false, false, true},
            {"purchase_id", "PO ID", "SMALLINT", false, false, false, true}
        }
    ));

    // PurchaseItem entity (PO line items)
    registerEntity(std::make_shared<Entity>(
        "PurchaseItem", "POItem", "",
        std::vector<ColumnDef>{
            {"purchase_id", "PO ID", "SMALLINT", false, false, false, true},
            {"product_id", "Product ID", "SMALLINT", false, false, false, true},
            {"unit_cost", "Unit Cost", "FLOAT", false, false, false, true},
            {"quantity", "Quantity", "SMALLINT", false, false, false, true}
        }
    ));

    // JobPurchase entity (join table)
    registerEntity(std::make_shared<Entity>(
        "JobPurchase", "JobPO", "",
        std::vector<ColumnDef>{
            {"job_id", "Job ID", "SMALLINT", false, false, false, true},
            {"purchase_id", "PO ID", "SMALLINT", false, false, false, true}
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
