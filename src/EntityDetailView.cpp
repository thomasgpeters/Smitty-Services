#include "EntityDetailView.h"
#include "ApiClient.h"
#include "AppSettings.h"

EntityDetailView::EntityDetailView(std::shared_ptr<Entity> entity)
    : entity_(entity)
    , fieldsContainer_(nullptr)
    , childContainer_(nullptr)
    , titleText_(nullptr) {
    buildUI();
}

void EntityDetailView::setBackCallback(std::function<void()> callback) {
    backCallback_ = callback;
}

void EntityDetailView::buildUI() {
    // Back button
    auto backBtn = addWidget(std::make_unique<Wt::WPushButton>("\xE2\x86\x90 Back to List"));
    backBtn->setStyleClass("back-btn");
    backBtn->clicked().connect([this] {
        if (backCallback_) backCallback_();
    });

    // Page header with action buttons
    auto header = addWidget(std::make_unique<Wt::WContainerWidget>());
    header->setStyleClass("page-header");
    titleText_ = header->addWidget(std::make_unique<Wt::WText>(entity_->displayName() + " Detail"));
    titleText_->setStyleClass("page-title");

    auto headerActions = header->addWidget(std::make_unique<Wt::WContainerWidget>());
    headerActions->setStyleClass("action-buttons");

    auto editBtn = headerActions->addWidget(std::make_unique<Wt::WPushButton>("Edit"));
    editBtn->setStyleClass("action-btn");
    editBtn->clicked().connect(this, &EntityDetailView::showEditDialog);

    auto deleteBtn = headerActions->addWidget(std::make_unique<Wt::WPushButton>("Delete"));
    deleteBtn->setStyleClass("action-btn action-btn-danger");
    deleteBtn->clicked().connect(this, &EntityDetailView::confirmDelete);

    // Fields container (3-column grid)
    fieldsContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    fieldsContainer_->setStyleClass("detail-form detail-form-grid");

    // Child content container (for subclass grids below the form)
    childContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    childContainer_->setStyleClass("detail-child-content");
}

void EntityDetailView::loadRecord(const std::string& id) {
    currentRecordId_ = id;
    fieldsContainer_->clear();
    childContainer_->clear();

    try {
        auto resp = ApiClient::instance().fetchOne(entity_->resourceName(), id);
        if (!resp.ok()) {
            fieldsContainer_->addWidget(
                std::make_unique<Wt::WText>("API error: " + resp.errorMessage(), Wt::TextFormat::Plain));
            return;
        }
        if (resp.hasData()) {
            currentRecord_ = resp.data();
            populateFields(currentRecord_);
            addChildContent(childContainer_, currentRecord_);
        }
    } catch (const std::exception& e) {
        fieldsContainer_->addWidget(
            std::make_unique<Wt::WText>(std::string("Error loading record: ") + e.what(), Wt::TextFormat::Plain));
    }
}

void EntityDetailView::populateFields(const json& record) {
    for (const auto& col : entity_->columns()) {
        auto group = fieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
        group->setStyleClass("form-group");

        auto label = group->addWidget(std::make_unique<Wt::WText>(col.label));
        label->setStyleClass("form-label");

        std::string val = entity_->getFieldValue(record, col.name);
        std::string formatted = formatFieldValue(col, val);

        auto value = group->addWidget(std::make_unique<Wt::WText>(formatted, Wt::TextFormat::Plain));
        value->setStyleClass("form-value");
    }
}

std::string EntityDetailView::formatFieldValue(const ColumnDef& col, const std::string& value) {
    if (value.empty()) return "-";
    if (col.type == "FLOAT") {
        try {
            double d = std::stod(value);
            char buf[64];
            snprintf(buf, sizeof(buf), "%.2f", d);
            return AppSettings::instance().currencySymbol() + std::string(buf);
        } catch (...) {
            return value;
        }
    }
    return value;
}

bool EntityDetailView::customEditField(Wt::WContainerWidget* /*content*/,
                                        const ColumnDef& /*col*/,
                                        const std::string& /*value*/,
                                        std::map<std::string, Wt::WLineEdit*>& /*fieldMap*/) {
    return false;
}

void EntityDetailView::addChildContent(Wt::WContainerWidget* /*container*/,
                                        const json& /*record*/) {
    // Default: no child content. Subclasses override.
}

void EntityDetailView::showEditDialog() {
    auto dialog = addChild(std::make_unique<Wt::WDialog>("Edit " + entity_->displayName()));
    dialog->setStyleClass("smitty-dialog");
    dialog->setModal(true);
    dialog->setClosable(true);
    dialog->rejectWhenEscapePressed(true);

    auto content = dialog->contents();
    content->setStyleClass("dialog-content");

    std::map<std::string, Wt::WLineEdit*> fieldMap;

    for (const auto& col : entity_->columns()) {
        // Skip the primary key field - not editable
        if (col.name == entity_->primaryKey()) continue;

        std::string currentVal = entity_->getFieldValue(currentRecord_, col.name);

        // Let subclasses handle special fields (dropdowns, etc.)
        if (customEditField(content, col, currentVal, fieldMap)) continue;

        content->addWidget(std::make_unique<Wt::WText>(col.label))
               ->setStyleClass("dialog-label");
        auto input = content->addWidget(std::make_unique<Wt::WLineEdit>());
        input->setStyleClass("dialog-input");
        if (currentVal != "-" && !currentVal.empty()) {
            input->setText(currentVal);
        }
        fieldMap[col.name] = input;
    }

    auto statusMsg = content->addWidget(std::make_unique<Wt::WText>());
    statusMsg->setStyleClass("dialog-status");

    auto btnBar = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    btnBar->setStyleClass("dialog-buttons");

    auto saveBtn = btnBar->addWidget(std::make_unique<Wt::WPushButton>("Save"));
    saveBtn->setStyleClass("action-btn");

    saveBtn->clicked().connect([this, dialog, fieldMap, statusMsg] {
        json attrs;
        for (const auto& col : entity_->columns()) {
            if (col.name == entity_->primaryKey()) continue;
            auto it = fieldMap.find(col.name);
            if (it == fieldMap.end()) continue;

            std::string val = it->second->text().toUTF8();
            if (val.empty()) continue;

            if (col.type == "FLOAT") {
                try { attrs[col.name] = std::stod(val); }
                catch (...) { attrs[col.name] = val; }
            } else if (col.type == "SMALLINT" || col.type == "INTEGER") {
                try { attrs[col.name] = std::stoi(val); }
                catch (...) { attrs[col.name] = val; }
            } else {
                attrs[col.name] = val;
            }
        }

        try {
            auto resp = ApiClient::instance().updateRecord(
                entity_->resourceName(), currentRecordId_, attrs);
            if (resp.ok()) {
                dialog->accept();
                loadRecord(currentRecordId_);
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

void EntityDetailView::confirmDelete() {
    auto dialog = addChild(std::make_unique<Wt::WDialog>("Confirm Delete"));
    dialog->setStyleClass("smitty-dialog");
    dialog->setModal(true);
    dialog->setClosable(true);
    dialog->rejectWhenEscapePressed(true);

    auto content = dialog->contents();
    content->setStyleClass("dialog-content");

    content->addWidget(std::make_unique<Wt::WText>(
        "Are you sure you want to delete this " + entity_->displayName() + "?"))
        ->setStyleClass("dialog-confirm-text");

    auto statusMsg = content->addWidget(std::make_unique<Wt::WText>());
    statusMsg->setStyleClass("dialog-status");

    auto btnBar = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    btnBar->setStyleClass("dialog-buttons");

    auto deleteBtn = btnBar->addWidget(std::make_unique<Wt::WPushButton>("Delete"));
    deleteBtn->setStyleClass("action-btn action-btn-danger");

    deleteBtn->clicked().connect([this, dialog, statusMsg] {
        try {
            auto resp = ApiClient::instance().deleteRecord(
                entity_->resourceName(), currentRecordId_);
            if (resp.ok()) {
                dialog->accept();
                if (backCallback_) backCallback_();
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
