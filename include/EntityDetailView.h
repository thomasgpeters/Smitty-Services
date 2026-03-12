#ifndef ENTITY_DETAIL_VIEW_H
#define ENTITY_DETAIL_VIEW_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WDialog.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <string>
#include <functional>
#include <memory>
#include <map>
#include "Entity.h"
#include "json.hpp"

using json = nlohmann::json;

class EntityDetailView : public Wt::WContainerWidget {
public:
    EntityDetailView(std::shared_ptr<Entity> entity);

    void loadRecord(const std::string& id);
    void setBackCallback(std::function<void()> callback);

protected:
    void buildUI();
    void populateFields(const json& record);
    virtual std::string formatFieldValue(const ColumnDef& col, const std::string& value);

    // CRUD hooks for subclasses
    virtual void showEditDialog();
    virtual void confirmDelete();

    // Hook for subclasses to customize edit dialog fields (e.g. combo boxes).
    // Return true if the field was handled (no default input will be added).
    virtual bool customEditField(Wt::WContainerWidget* content, const ColumnDef& col,
                                  const std::string& value,
                                  std::map<std::string, Wt::WLineEdit*>& fieldMap);

    // Hook for subclasses to add child content (grids, etc.) below the detail fields.
    // Called after populateFields when a record is loaded.
    virtual void addChildContent(Wt::WContainerWidget* container, const json& record);

    std::shared_ptr<Entity> entity_;
    Wt::WContainerWidget* fieldsContainer_;
    Wt::WContainerWidget* childContainer_;
    Wt::WText* titleText_;
    std::function<void()> backCallback_;
    std::string currentRecordId_;
    json currentRecord_;
};

#endif // ENTITY_DETAIL_VIEW_H
