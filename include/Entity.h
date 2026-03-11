#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <vector>
#include <map>
#include "json.hpp"

using json = nlohmann::json;

struct ColumnDef {
    std::string name;
    std::string label;
    std::string type;     // VARCHAR, SMALLINT, FLOAT, DATE, TEXT, BLOB, INTEGER
    bool required = false;
    bool searchable = false;
    bool sortable = false;
    bool visible = true;
};

class Entity {
public:
    Entity(const std::string& resourceName,
           const std::string& displayName,
           const std::string& primaryKey,
           const std::vector<ColumnDef>& columns);
    virtual ~Entity() = default;

    const std::string& resourceName() const { return resourceName_; }
    const std::string& displayName() const { return displayName_; }
    const std::string& primaryKey() const { return primaryKey_; }
    const std::vector<ColumnDef>& columns() const { return columns_; }

    std::vector<ColumnDef> listColumns(int maxColumns = 8) const;
    std::vector<ColumnDef> searchableColumns() const;
    const ColumnDef* findColumn(const std::string& name) const;

    std::string getFieldValue(const json& record, const std::string& field) const;

private:
    std::string resourceName_;
    std::string displayName_;
    std::string primaryKey_;
    std::vector<ColumnDef> columns_;
};

#endif // ENTITY_H
