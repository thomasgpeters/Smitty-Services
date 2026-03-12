#include "EntityDetailView.h"
#include "EntityRegistry.h"
#include "ApiClient.h"
#include <Wt/WComboBox.h>
#include <map>
#include <vector>

static const std::vector<std::string> JOB_STATUSES = {
    "New", "In Progress", "Waiting Parts", "Road Test Pending", "Complete"
};

class JobDetail : public EntityDetailView {
public:
    JobDetail()
        : EntityDetailView(EntityRegistry::instance().getEntity("Job")) {
        loadLookups();
    }

protected:
    bool customEditField(Wt::WContainerWidget* content, const ColumnDef& col,
                          const std::string& value,
                          std::map<std::string, Wt::WLineEdit*>& fieldMap) override {
        if (col.name == "customer_id") {
            content->addWidget(std::make_unique<Wt::WText>("Customer"))
                   ->setStyleClass("dialog-label");
            auto combo = content->addWidget(std::make_unique<Wt::WComboBox>());
            combo->setStyleClass("dialog-input");
            combo->addItem("");
            int selectIdx = 0, idx = 1;
            for (const auto& pair : customerNames_) {
                combo->addItem(pair.second);
                if (pair.first == value) selectIdx = idx;
                idx++;
            }
            combo->setCurrentIndex(selectIdx);
            auto hidden = content->addWidget(std::make_unique<Wt::WLineEdit>());
            hidden->setHidden(true);
            hidden->setText(value);
            fieldMap[col.name] = hidden;
            combo->changed().connect([combo, hidden, this] {
                std::string name = combo->currentText().toUTF8();
                for (const auto& pair : customerNames_) {
                    if (pair.second == name) { hidden->setText(pair.first); return; }
                }
                hidden->setText("");
            });
            return true;
        }

        if (col.name == "vehicle_id") {
            content->addWidget(std::make_unique<Wt::WText>("Vehicle"))
                   ->setStyleClass("dialog-label");
            auto combo = content->addWidget(std::make_unique<Wt::WComboBox>());
            combo->setStyleClass("dialog-input");
            combo->addItem("");
            int selectIdx = 0, idx = 1;
            for (const auto& pair : vehicleNames_) {
                combo->addItem(pair.second);
                if (pair.first == value) selectIdx = idx;
                idx++;
            }
            combo->setCurrentIndex(selectIdx);
            auto hidden = content->addWidget(std::make_unique<Wt::WLineEdit>());
            hidden->setHidden(true);
            hidden->setText(value);
            fieldMap[col.name] = hidden;
            combo->changed().connect([combo, hidden, this] {
                std::string desc = combo->currentText().toUTF8();
                for (const auto& pair : vehicleNames_) {
                    if (pair.second == desc) { hidden->setText(pair.first); return; }
                }
                hidden->setText("");
            });
            return true;
        }

        if (col.name == "status") {
            content->addWidget(std::make_unique<Wt::WText>("Status"))
                   ->setStyleClass("dialog-label");
            auto combo = content->addWidget(std::make_unique<Wt::WComboBox>());
            combo->setStyleClass("dialog-input");
            int selectIdx = 0;
            for (int i = 0; i < (int)JOB_STATUSES.size(); i++) {
                combo->addItem(JOB_STATUSES[i]);
                if (JOB_STATUSES[i] == value) selectIdx = i;
            }
            combo->setCurrentIndex(selectIdx);
            auto hidden = content->addWidget(std::make_unique<Wt::WLineEdit>());
            hidden->setHidden(true);
            hidden->setText(value);
            fieldMap[col.name] = hidden;
            combo->changed().connect([combo, hidden] {
                hidden->setText(combo->currentText().toUTF8());
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

        try {
            auto resp = ApiClient::instance().fetchAll("Vehicle");
            if (resp.ok() && resp.hasData() && resp.data().is_array()) {
                for (const auto& v : resp.data()) {
                    std::string id = v.contains("id")
                        ? (v["id"].is_string() ? v["id"].get<std::string>() : v["id"].dump())
                        : "";
                    std::string desc;
                    if (v.contains("attributes") && v["attributes"].contains("description")
                        && !v["attributes"]["description"].is_null())
                        desc = v["attributes"]["description"].get<std::string>();
                    if (!id.empty() && !desc.empty())
                        vehicleNames_[id] = desc;
                }
            }
        } catch (...) {}
    }

    std::map<std::string, std::string> customerNames_;
    std::map<std::string, std::string> vehicleNames_;
};

std::unique_ptr<EntityDetailView> createJobDetail() {
    return std::make_unique<JobDetail>();
}
