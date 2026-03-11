#include "Entity.h"

Entity::Entity(const std::string& resourceName,
               const std::string& displayName,
               const std::string& primaryKey,
               const std::vector<ColumnDef>& columns)
    : resourceName_(resourceName)
    , displayName_(displayName)
    , primaryKey_(primaryKey)
    , columns_(columns) {
}

std::vector<ColumnDef> Entity::listColumns(int maxColumns) const {
    std::vector<ColumnDef> result;
    for (const auto& col : columns_) {
        if (col.visible && col.name != primaryKey_) {
            result.push_back(col);
            if (static_cast<int>(result.size()) >= maxColumns) break;
        }
    }
    return result;
}

std::vector<ColumnDef> Entity::searchableColumns() const {
    std::vector<ColumnDef> result;
    for (const auto& col : columns_) {
        if (col.searchable) {
            result.push_back(col);
        }
    }
    return result;
}

const ColumnDef* Entity::findColumn(const std::string& name) const {
    for (const auto& col : columns_) {
        if (col.name == name) return &col;
    }
    return nullptr;
}

std::string Entity::getFieldValue(const json& record, const std::string& field) const {
    if (!record.contains("attributes")) return "";
    const auto& attrs = record["attributes"];
    if (!attrs.contains(field)) return "";
    if (attrs[field].is_null()) return "";
    if (attrs[field].is_string()) return attrs[field].get<std::string>();
    return attrs[field].dump();
}
