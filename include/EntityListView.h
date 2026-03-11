#ifndef ENTITY_LIST_VIEW_H
#define ENTITY_LIST_VIEW_H

#include <Wt/WContainerWidget.h>
#include <Wt/WTable.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WCheckBox.h>
#include <Wt/WText.h>
#include <string>
#include <functional>
#include <memory>
#include "Entity.h"
#include "json.hpp"

using json = nlohmann::json;

class EntityListView : public Wt::WContainerWidget {
public:
    EntityListView(std::shared_ptr<Entity> entity);

    void setRowClickCallback(std::function<void(const std::string& id)> callback);
    void refresh();

protected:
    void buildUI();
    void applyFilter();
    void clearFilter();
    void populateTable(const json& data);
    virtual std::string formatCellValue(const ColumnDef& col, const std::string& value);

    // Hook for subclasses to add custom filter widgets (checkboxes, etc.)
    // Called during buildUI after the main filter bar is created.
    virtual void addCustomFilters(Wt::WContainerWidget* filterBar);

    // Hook for subclasses to filter records client-side after API fetch.
    // Default returns true for all records (no filtering).
    virtual bool filterRecord(const json& record) const;

    std::shared_ptr<Entity> entity_;
    Wt::WLineEdit* filterInput_;
    Wt::WTable* table_;
    Wt::WText* statusText_;
    std::function<void(const std::string&)> rowClickCallback_;
    std::string currentFilter_;
};

#endif // ENTITY_LIST_VIEW_H
