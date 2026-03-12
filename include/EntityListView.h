#ifndef ENTITY_LIST_VIEW_H
#define ENTITY_LIST_VIEW_H

#include <Wt/WContainerWidget.h>
#include <Wt/WTable.h>
#include <Wt/WTableCell.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WCheckBox.h>
#include <Wt/WText.h>
#include <Wt/WDialog.h>
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

    // Hook for subclasses to add action buttons (New, etc.) to the right side of filter bar.
    virtual void addActionButtons(Wt::WContainerWidget* actionBar);

    // Hook for subclasses to specify JSONAPI include parameter (e.g. "category,supplier").
    virtual std::string includeParam() const;

    // Hook for subclasses to resolve a field value using included relationship data.
    virtual std::string resolveFieldValue(const json& record, const ColumnDef& col,
                                          const std::string& rawValue) const;

    // Hook for subclasses to add custom widgets to a cell (e.g. clickable links).
    // Return true if the cell was handled (no default text will be added).
    virtual bool customRenderCell(Wt::WTableCell* cell, const json& record,
                                  const ColumnDef& col, const std::string& value);

    // Hook for subclasses to filter records client-side after API fetch.
    // Default returns true for all records (no filtering).
    virtual bool filterRecord(const json& record) const;

    // Return true to show Edit (pencil) and Delete (trashcan) buttons on each row.
    virtual bool showRowActions() const;

    // Generic edit dialog: fetches the record, shows editable fields, PATCHes on save.
    void showEditDialog(const std::string& id);

    // Confirm-and-delete: shows a confirmation dialog, DELETEs on confirm, refreshes.
    void confirmDelete(const std::string& id);

    std::shared_ptr<Entity> entity_;
    Wt::WLineEdit* filterInput_;
    Wt::WTable* table_;
    Wt::WText* statusText_;
    std::function<void(const std::string&)> rowClickCallback_;
    std::string currentFilter_;
    json lastResponseBody_;
};

#endif // ENTITY_LIST_VIEW_H
