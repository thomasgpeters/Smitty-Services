#include "EntityDetailView.h"
#include "EntityRegistry.h"
#include "ApiClient.h"
#include <Wt/WComboBox.h>
#include <map>
#include <vector>

static const std::vector<std::string> PURCHASE_STATUSES = {
    "New", "Ordered", "Partial", "Received", "Cancelled"
};

class PurchaseDetail : public EntityDetailView {
public:
    PurchaseDetail()
        : EntityDetailView(EntityRegistry::instance().getEntity("Purchase")) {
        loadLookups();
    }

protected:
    bool customEditField(Wt::WContainerWidget* content, const ColumnDef& col,
                          const std::string& value,
                          std::map<std::string, Wt::WLineEdit*>& fieldMap) override {
        if (col.name == "supplier_id") {
            content->addWidget(std::make_unique<Wt::WText>("Supplier"))
                   ->setStyleClass("dialog-label");
            auto combo = content->addWidget(std::make_unique<Wt::WComboBox>());
            combo->setStyleClass("dialog-input");
            combo->addItem("");
            int selectIdx = 0, idx = 1;
            for (const auto& pair : supplierNames_) {
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
                for (const auto& pair : supplierNames_) {
                    if (pair.second == name) { hidden->setText(pair.first); return; }
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
            for (int i = 0; i < (int)PURCHASE_STATUSES.size(); i++) {
                combo->addItem(PURCHASE_STATUSES[i]);
                if (PURCHASE_STATUSES[i] == value) selectIdx = i;
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
            auto resp = ApiClient::instance().fetchAll("Supplier");
            if (resp.ok() && resp.hasData() && resp.data().is_array()) {
                for (const auto& sup : resp.data()) {
                    std::string id = sup.contains("id")
                        ? (sup["id"].is_string() ? sup["id"].get<std::string>() : sup["id"].dump())
                        : "";
                    std::string name;
                    if (sup.contains("attributes") && sup["attributes"].contains("company_name"))
                        name = sup["attributes"]["company_name"].get<std::string>();
                    if (!id.empty() && !name.empty())
                        supplierNames_[id] = name;
                }
            }
        } catch (...) {}
    }

    std::map<std::string, std::string> supplierNames_;
};

std::unique_ptr<EntityDetailView> createPurchaseDetail() {
    return std::make_unique<PurchaseDetail>();
}
