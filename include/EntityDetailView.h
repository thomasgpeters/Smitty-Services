#ifndef ENTITY_DETAIL_VIEW_H
#define ENTITY_DETAIL_VIEW_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <string>
#include <functional>
#include <memory>
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

    std::shared_ptr<Entity> entity_;
    Wt::WContainerWidget* fieldsContainer_;
    Wt::WText* titleText_;
    std::function<void()> backCallback_;
};

#endif // ENTITY_DETAIL_VIEW_H
