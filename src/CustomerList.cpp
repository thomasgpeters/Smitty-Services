#include "EntityListView.h"
#include "EntityRegistry.h"
#include "ApiClient.h"
#include <Wt/WCheckBox.h>
#include <Wt/WDialog.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <set>

class CustomerList : public EntityListView {
public:
    CustomerList()
        : EntityListView(EntityRegistry::instance().getEntity("Customer"))
        , outstandingBalanceCheck_(nullptr) {
    }

protected:
    bool showRowActions() const override { return true; }

    void addCustomFilters(Wt::WContainerWidget* filterBar) override {
        outstandingBalanceCheck_ = filterBar->addWidget(
            std::make_unique<Wt::WCheckBox>("Outstanding Balance"));
        outstandingBalanceCheck_->setStyleClass("filter-checkbox");
        outstandingBalanceCheck_->changed().connect([this] {
            if (outstandingBalanceCheck_->isChecked()) {
                loadUnpaidCustomerIds();
            } else {
                unpaidCustomerIds_.clear();
            }
            refresh();
        });
    }

    void addActionButtons(Wt::WContainerWidget* actionBar) override {
        auto newCustBtn = actionBar->addWidget(
            std::make_unique<Wt::WPushButton>("New Customer"));
        newCustBtn->setStyleClass("action-btn");
        newCustBtn->clicked().connect(this, &CustomerList::showAddCustomerDialog);
    }

    bool filterRecord(const json& record) const override {
        if (!outstandingBalanceCheck_ || !outstandingBalanceCheck_->isChecked())
            return true;

        std::string custId;
        if (record.contains("id")) {
            custId = record["id"].is_string()
                ? record["id"].get<std::string>()
                : record["id"].dump();
        }
        return unpaidCustomerIds_.count(custId) > 0;
    }

private:
    void loadUnpaidCustomerIds() {
        unpaidCustomerIds_.clear();
        try {
            auto resp = ApiClient::instance().fetchAll("Order");
            if (resp.ok() && resp.hasData() && resp.data().is_array()) {
                for (const auto& order : resp.data()) {
                    if (!order.contains("attributes")) continue;
                    const auto& attrs = order["attributes"];

                    bool shipped = attrs.contains("shipped_date")
                        && !attrs["shipped_date"].is_null()
                        && attrs["shipped_date"].get<std::string>() != "";
                    if (!shipped) {
                        if (attrs.contains("customer_id") && !attrs["customer_id"].is_null()) {
                            unpaidCustomerIds_.insert(
                                attrs["customer_id"].get<std::string>());
                        }
                    }
                }
            }
        } catch (...) {}
    }

    void showAddCustomerDialog() {
        auto dialog = addChild(std::make_unique<Wt::WDialog>("Add Customer"));
        dialog->setStyleClass("smitty-dialog");
        dialog->setModal(true);
        dialog->setClosable(true);
        dialog->rejectWhenEscapePressed(true);

        auto content = dialog->contents();
        content->setStyleClass("dialog-content");

        // 3-column grid for fields
        auto grid = content->addWidget(std::make_unique<Wt::WContainerWidget>());
        grid->setStyleClass("dialog-content-grid");

        // Customer ID (required - varchar(5))
        auto idGroup = grid->addWidget(std::make_unique<Wt::WContainerWidget>());
        idGroup->setStyleClass("dialog-field-group");
        idGroup->addWidget(std::make_unique<Wt::WText>("Customer ID"))
               ->setStyleClass("dialog-label");
        auto idInput = idGroup->addWidget(std::make_unique<Wt::WLineEdit>());
        idInput->setStyleClass("dialog-input");
        idInput->setMaxLength(5);

        // Company Name (required)
        auto companyGroup = grid->addWidget(std::make_unique<Wt::WContainerWidget>());
        companyGroup->setStyleClass("dialog-field-group");
        companyGroup->addWidget(std::make_unique<Wt::WText>("Company Name"))
               ->setStyleClass("dialog-label");
        auto companyInput = companyGroup->addWidget(std::make_unique<Wt::WLineEdit>());
        companyInput->setStyleClass("dialog-input");

        // Contact Name
        auto contactGroup = grid->addWidget(std::make_unique<Wt::WContainerWidget>());
        contactGroup->setStyleClass("dialog-field-group");
        contactGroup->addWidget(std::make_unique<Wt::WText>("Contact Name"))
               ->setStyleClass("dialog-label");
        auto contactInput = contactGroup->addWidget(std::make_unique<Wt::WLineEdit>());
        contactInput->setStyleClass("dialog-input");

        // City
        auto cityGroup = grid->addWidget(std::make_unique<Wt::WContainerWidget>());
        cityGroup->setStyleClass("dialog-field-group");
        cityGroup->addWidget(std::make_unique<Wt::WText>("City"))
               ->setStyleClass("dialog-label");
        auto cityInput = cityGroup->addWidget(std::make_unique<Wt::WLineEdit>());
        cityInput->setStyleClass("dialog-input");

        // Country
        auto countryGroup = grid->addWidget(std::make_unique<Wt::WContainerWidget>());
        countryGroup->setStyleClass("dialog-field-group");
        countryGroup->addWidget(std::make_unique<Wt::WText>("Country"))
               ->setStyleClass("dialog-label");
        auto countryInput = countryGroup->addWidget(std::make_unique<Wt::WLineEdit>());
        countryInput->setStyleClass("dialog-input");

        // Phone
        auto phoneGroup = grid->addWidget(std::make_unique<Wt::WContainerWidget>());
        phoneGroup->setStyleClass("dialog-field-group");
        phoneGroup->addWidget(std::make_unique<Wt::WText>("Phone"))
               ->setStyleClass("dialog-label");
        auto phoneInput = phoneGroup->addWidget(std::make_unique<Wt::WLineEdit>());
        phoneInput->setStyleClass("dialog-input");

        // Status message (full width, below grid)
        auto statusMsg = content->addWidget(std::make_unique<Wt::WText>());
        statusMsg->setStyleClass("dialog-status");

        // Buttons
        auto btnBar = content->addWidget(std::make_unique<Wt::WContainerWidget>());
        btnBar->setStyleClass("dialog-buttons");

        auto saveBtn = btnBar->addWidget(std::make_unique<Wt::WPushButton>("Add Customer"));
        saveBtn->setStyleClass("action-btn");

        saveBtn->clicked().connect([=] {
            if (idInput->text().empty()) {
                statusMsg->setText("Customer ID is required.");
                return;
            }
            if (companyInput->text().empty()) {
                statusMsg->setText("Company Name is required.");
                return;
            }

            json attrs;
            attrs["customer_id"] = idInput->text().toUTF8();
            attrs["company_name"] = companyInput->text().toUTF8();
            if (!contactInput->text().empty())
                attrs["contact_name"] = contactInput->text().toUTF8();
            if (!cityInput->text().empty())
                attrs["city"] = cityInput->text().toUTF8();
            if (!countryInput->text().empty())
                attrs["country"] = countryInput->text().toUTF8();
            if (!phoneInput->text().empty())
                attrs["phone"] = phoneInput->text().toUTF8();

            try {
                auto resp = ApiClient::instance().createRecord("Customer", attrs);
                if (resp.ok()) {
                    dialog->accept();
                    refresh();
                } else {
                    statusMsg->setText("Error: " + resp.errorMessage());
                }
            } catch (const std::exception& e) {
                statusMsg->setText(std::string("Error: ") + e.what());
            }
        });

        dialog->finished().connect([this, dialog](Wt::DialogCode) {
            removeChild(dialog);
        });

        dialog->show();
    }

    Wt::WCheckBox* outstandingBalanceCheck_;
    std::set<std::string> unpaidCustomerIds_;
};

std::unique_ptr<EntityListView> createCustomerList() {
    return std::make_unique<CustomerList>();
}
