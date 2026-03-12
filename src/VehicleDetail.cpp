#include "EntityDetailView.h"
#include "EntityRegistry.h"
#include "ApiClient.h"
#include <Wt/WComboBox.h>
#include <map>

class VehicleDetail : public EntityDetailView {
public:
    VehicleDetail()
        : EntityDetailView(EntityRegistry::instance().getEntity("Vehicle")) {
        loadLookups();
    }

protected:
    bool customEditField(Wt::WContainerWidget* content, const ColumnDef& col,
                          const std::string& value,
                          std::map<std::string, Wt::WLineEdit*>& fieldMap) override {
        if (col.name == "customer_id") {
            content->addWidget(std::make_unique<Wt::WText>("Owner"))
                   ->setStyleClass("dialog-label");
            auto combo = content->addWidget(std::make_unique<Wt::WComboBox>());
            combo->setStyleClass("dialog-input");
            combo->addItem("");
            int selectIdx = 0;
            int idx = 1;
            for (const auto& pair : customerNames_) {
                combo->addItem(pair.second);
                if (pair.first == value) selectIdx = idx;
                idx++;
            }
            combo->setCurrentIndex(selectIdx);
            // Store in a hidden WLineEdit so the generic save logic picks it up
            auto hidden = content->addWidget(std::make_unique<Wt::WLineEdit>());
            hidden->setHidden(true);
            hidden->setText(value);
            fieldMap[col.name] = hidden;
            combo->changed().connect([combo, hidden, this] {
                std::string name = combo->currentText().toUTF8();
                for (const auto& pair : customerNames_) {
                    if (pair.second == name) {
                        hidden->setText(pair.first);
                        return;
                    }
                }
                hidden->setText("");
            });
            return true;
        }
        return false;
    }

private:
    void loadLookups() {
        try {
            auto resp = ApiClient::instance().fetchAll("Customer");
            if (resp.ok() && resp.hasData() && resp.data().is_array()) {
                for (const auto& cust : resp.data()) {
                    std::string id = cust.contains("id")
                        ? (cust["id"].is_string() ? cust["id"].get<std::string>() : cust["id"].dump())
                        : "";
                    std::string name;
                    if (cust.contains("attributes") && cust["attributes"].contains("company_name"))
                        name = cust["attributes"]["company_name"].get<std::string>();
                    if (!id.empty() && !name.empty())
                        customerNames_[id] = name;
                }
            }
        } catch (...) {}
    }

    std::map<std::string, std::string> customerNames_;
};

std::unique_ptr<EntityDetailView> createVehicleDetail() {
    return std::make_unique<VehicleDetail>();
}
