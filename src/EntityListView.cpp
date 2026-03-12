#include "EntityListView.h"
#include "ApiClient.h"
#include "AppSettings.h"
#include <Wt/WDialog.h>
#include <Wt/WLineEdit.h>
#include <Wt/WContainerWidget.h>

EntityListView::EntityListView(std::shared_ptr<Entity> entity)
    : entity_(entity)
    , filterInput_(nullptr)
    , table_(nullptr)
    , statusText_(nullptr) {
    buildUI();
}

void EntityListView::setRowClickCallback(std::function<void(const std::string& id)> callback) {
    rowClickCallback_ = callback;
}

void EntityListView::buildUI() {
    // Page header
    auto header = addWidget(std::make_unique<Wt::WContainerWidget>());
    header->setStyleClass("page-header");
    header->addWidget(std::make_unique<Wt::WText>(entity_->displayName() + " List"))
          ->setStyleClass("page-title");

    // Filter bar
    auto filterBar = addWidget(std::make_unique<Wt::WContainerWidget>());
    filterBar->setStyleClass("filter-bar");

    filterInput_ = filterBar->addWidget(std::make_unique<Wt::WLineEdit>());
    filterInput_->setStyleClass("filter-input");
    filterInput_->setPlaceholderText("Filter " + entity_->displayName() + "s...");
    filterInput_->enterPressed().connect(this, &EntityListView::applyFilter);

    auto filterBtn = filterBar->addWidget(std::make_unique<Wt::WPushButton>("Filter"));
    filterBtn->setStyleClass("filter-btn");
    filterBtn->clicked().connect(this, &EntityListView::applyFilter);

    auto clearBtn = filterBar->addWidget(std::make_unique<Wt::WPushButton>("Clear"));
    clearBtn->setStyleClass("clear-btn");
    clearBtn->clicked().connect(this, &EntityListView::clearFilter);

    // Allow subclasses to add custom filter widgets
    addCustomFilters(filterBar);

    // Action buttons container (floated right)
    auto actionBar = filterBar->addWidget(std::make_unique<Wt::WContainerWidget>());
    actionBar->setStyleClass("action-buttons");
    addActionButtons(actionBar);

    // Status text
    statusText_ = addWidget(std::make_unique<Wt::WText>());

    // Data table
    table_ = addWidget(std::make_unique<Wt::WTable>());
    table_->setStyleClass("data-table");
    table_->setHeaderCount(1);

    refresh();
}

void EntityListView::addCustomFilters(Wt::WContainerWidget* /*filterBar*/) {
    // Default: no custom filters. Subclasses override this.
}

void EntityListView::addActionButtons(Wt::WContainerWidget* /*actionBar*/) {
    // Default: no action buttons. Subclasses override this.
}

std::string EntityListView::includeParam() const {
    // Default: no includes. Subclasses override for relationship loading.
    return "";
}

std::string EntityListView::resolveFieldValue(const json& /*record*/, const ColumnDef& /*col*/,
                                               const std::string& rawValue) const {
    // Default: return the raw value. Subclasses override to resolve relationships.
    return rawValue;
}

bool EntityListView::customRenderCell(Wt::WTableCell* /*cell*/, const json& /*record*/,
                                       const ColumnDef& /*col*/, const std::string& /*value*/) {
    // Default: no custom rendering. Subclasses override to add clickable links, etc.
    return false;
}

bool EntityListView::filterRecord(const json& /*record*/) const {
    // Default: include all records.
    return true;
}

bool EntityListView::showRowActions() const {
    // Default: no row-level action buttons. Subclasses override to enable.
    return false;
}

void EntityListView::applyFilter() {
    currentFilter_ = filterInput_->text().toUTF8();
    refresh();
}

void EntityListView::clearFilter() {
    filterInput_->setText("");
    currentFilter_ = "";
    refresh();
}

void EntityListView::refresh() {
    try {
        auto resp = ApiClient::instance().fetchAll(entity_->resourceName(), currentFilter_,
                                                     includeParam());
        if (!resp.ok()) {
            statusText_->setTextFormat(Wt::TextFormat::Plain);
            statusText_->setText("API error: " + resp.errorMessage());
            return;
        }
        lastResponseBody_ = resp.body;
        populateTable(resp.body);
    } catch (const std::exception& e) {
        statusText_->setTextFormat(Wt::TextFormat::Plain);
        statusText_->setText(std::string("Error loading data: ") + e.what());
    }
}

void EntityListView::populateTable(const json& data) {
    table_->clear();

    auto cols = entity_->listColumns(AppSettings::instance().maxListColumns());
    bool hasActions = showRowActions();

    // Header row
    for (size_t c = 0; c < cols.size(); ++c) {
        table_->elementAt(0, static_cast<int>(c))
              ->addWidget(std::make_unique<Wt::WText>(cols[c].label));
    }
    if (hasActions) {
        auto actionsHeader = table_->elementAt(0, static_cast<int>(cols.size()));
        actionsHeader->addWidget(std::make_unique<Wt::WText>(""));
        actionsHeader->setStyleClass("row-actions-header");
    }

    // Data rows
    if (!data.contains("data") || !data["data"].is_array()) {
        statusText_->setText("No records found.");
        return;
    }

    const auto& records = data["data"];
    int row = 1;
    for (const auto& record : records) {
        // Apply client-side filter from subclass
        if (!filterRecord(record)) continue;

        for (size_t c = 0; c < cols.size(); ++c) {
            std::string val = entity_->getFieldValue(record, cols[c].name);
            val = resolveFieldValue(record, cols[c], val);
            std::string formatted = formatCellValue(cols[c], val);
            auto cell = table_->elementAt(row, static_cast<int>(c));
            if (!customRenderCell(cell, record, cols[c], formatted)) {
                cell->addWidget(std::make_unique<Wt::WText>(formatted, Wt::TextFormat::Plain));
            }
        }

        // Click handler for row
        std::string id;
        if (record.contains("id")) {
            id = record["id"].is_string() ? record["id"].get<std::string>() : record["id"].dump();
        }

        if (!id.empty()) {
            for (size_t c = 0; c < cols.size(); ++c) {
                auto cell = table_->elementAt(row, static_cast<int>(c));
                cell->clicked().connect([this, id] {
                    if (rowClickCallback_) rowClickCallback_(id);
                });
            }

            // Row action buttons: Edit (pencil) and Delete (trashcan)
            if (hasActions) {
                auto actionsCell = table_->elementAt(row, static_cast<int>(cols.size()));
                actionsCell->setStyleClass("row-actions-cell");

                auto editBtn = actionsCell->addWidget(
                    std::make_unique<Wt::WPushButton>());
                editBtn->setTextFormat(Wt::TextFormat::XHTML);
                editBtn->setText("<span class=\"row-action-icon\">&#9998;</span>");
                editBtn->setStyleClass("row-action-btn row-action-edit");
                editBtn->setToolTip("Edit");
                editBtn->clicked().connect([this, id] {
                    showEditDialog(id);
                });

                auto deleteBtn = actionsCell->addWidget(
                    std::make_unique<Wt::WPushButton>());
                deleteBtn->setTextFormat(Wt::TextFormat::XHTML);
                deleteBtn->setText("<span class=\"row-action-icon\">&#128465;</span>");
                deleteBtn->setStyleClass("row-action-btn row-action-delete");
                deleteBtn->setToolTip("Delete");
                deleteBtn->clicked().connect([this, id] {
                    confirmDelete(id);
                });
            }
        }

        ++row;
    }

    statusText_->setText(std::to_string(row - 1) + " record(s) loaded.");
}

void EntityListView::showEditDialog(const std::string& id) {
    auto dialog = addChild(std::make_unique<Wt::WDialog>("Edit " + entity_->displayName()));
    dialog->setStyleClass("smitty-dialog");
    dialog->setModal(true);
    dialog->setClosable(true);
    dialog->rejectWhenEscapePressed(true);

    auto content = dialog->contents();
    content->setStyleClass("dialog-content");

    auto statusMsg = content->addWidget(std::make_unique<Wt::WText>());
    statusMsg->setStyleClass("dialog-status");

    // Fetch the current record
    try {
        auto resp = ApiClient::instance().fetchOne(entity_->resourceName(), id);
        if (!resp.ok() || !resp.hasData()) {
            statusMsg->setText("Error loading record: " + resp.errorMessage());
            dialog->finished().connect([this, dialog](Wt::DialogCode) { removeChild(dialog); });
            dialog->show();
            return;
        }

        const auto& record = resp.data();
        const auto& allCols = entity_->columns();

        // Build editable fields (skip primary key)
        struct FieldEntry {
            std::string name;
            Wt::WLineEdit* input;
        };
        auto fields = std::make_shared<std::vector<FieldEntry>>();

        for (const auto& col : allCols) {
            if (col.name == entity_->primaryKey()) continue;

            content->addWidget(std::make_unique<Wt::WText>(col.label))
                   ->setStyleClass("dialog-label");
            auto input = content->addWidget(std::make_unique<Wt::WLineEdit>());
            input->setStyleClass("dialog-input");

            std::string val = entity_->getFieldValue(record, col.name);
            input->setText(val);

            fields->push_back({col.name, input});
        }

        // Move status message after fields
        content->removeWidget(statusMsg);
        statusMsg = content->addWidget(std::make_unique<Wt::WText>());
        statusMsg->setStyleClass("dialog-status");

        // Buttons
        auto btnBar = content->addWidget(std::make_unique<Wt::WContainerWidget>());
        btnBar->setStyleClass("dialog-buttons");

        auto saveBtn = btnBar->addWidget(std::make_unique<Wt::WPushButton>("Save"));
        saveBtn->setStyleClass("action-btn");

        saveBtn->clicked().connect([this, id, fields, statusMsg, dialog] {
            json attrs;
            for (const auto& f : *fields) {
                std::string val = f.input->text().toUTF8();
                if (!val.empty()) {
                    attrs[f.name] = val;
                }
            }

            try {
                auto resp = ApiClient::instance().updateRecord(
                    entity_->resourceName(), id, attrs);
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
    } catch (const std::exception& e) {
        statusMsg->setText(std::string("Error: ") + e.what());
    }

    dialog->finished().connect([this, dialog](Wt::DialogCode) {
        removeChild(dialog);
    });

    dialog->show();
}

void EntityListView::confirmDelete(const std::string& id) {
    auto dialog = addChild(std::make_unique<Wt::WDialog>("Confirm Delete"));
    dialog->setStyleClass("smitty-dialog");
    dialog->setModal(true);
    dialog->setClosable(true);
    dialog->rejectWhenEscapePressed(true);

    auto content = dialog->contents();
    content->setStyleClass("dialog-content");

    content->addWidget(std::make_unique<Wt::WText>(
        "Are you sure you want to delete this " + entity_->displayName() + "?"))
        ->setStyleClass("dialog-label");

    auto statusMsg = content->addWidget(std::make_unique<Wt::WText>());
    statusMsg->setStyleClass("dialog-status");

    auto btnBar = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    btnBar->setStyleClass("dialog-buttons");

    auto deleteBtn = btnBar->addWidget(std::make_unique<Wt::WPushButton>("Delete"));
    deleteBtn->setStyleClass("action-btn action-btn-danger");

    auto cancelBtn = btnBar->addWidget(std::make_unique<Wt::WPushButton>("Cancel"));
    cancelBtn->setStyleClass("action-btn action-btn-secondary");

    deleteBtn->clicked().connect([this, id, statusMsg, dialog] {
        try {
            auto resp = ApiClient::instance().deleteRecord(entity_->resourceName(), id);
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

    cancelBtn->clicked().connect([dialog] {
        dialog->reject();
    });

    dialog->finished().connect([this, dialog](Wt::DialogCode) {
        removeChild(dialog);
    });

    dialog->show();
}

std::string EntityListView::formatCellValue(const ColumnDef& col, const std::string& value) {
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
