#include "EntityListView.h"
#include "ApiClient.h"
#include "AppSettings.h"

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

    // Status text
    statusText_ = addWidget(std::make_unique<Wt::WText>());

    // Data table
    table_ = addWidget(std::make_unique<Wt::WTable>());
    table_->setStyleClass("data-table");
    table_->setHeaderCount(1);

    refresh();
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
        auto resp = ApiClient::instance().fetchAll(entity_->resourceName(), currentFilter_);
        if (!resp.ok()) {
            statusText_->setTextFormat(Wt::TextFormat::Plain);
            statusText_->setText("API error: " + resp.errorMessage());
            return;
        }
        populateTable(resp.body);
    } catch (const std::exception& e) {
        statusText_->setTextFormat(Wt::TextFormat::Plain);
        statusText_->setText(std::string("Error loading data: ") + e.what());
    }
}

void EntityListView::populateTable(const json& data) {
    table_->clear();

    auto cols = entity_->listColumns(AppSettings::instance().maxListColumns());

    // Header row
    for (size_t c = 0; c < cols.size(); ++c) {
        table_->elementAt(0, static_cast<int>(c))
              ->addWidget(std::make_unique<Wt::WText>(cols[c].label));
    }

    // Data rows
    if (!data.contains("data") || !data["data"].is_array()) {
        statusText_->setText("No records found.");
        return;
    }

    const auto& records = data["data"];
    int row = 1;
    for (const auto& record : records) {
        for (size_t c = 0; c < cols.size(); ++c) {
            std::string val = entity_->getFieldValue(record, cols[c].name);
            std::string formatted = formatCellValue(cols[c], val);
            auto text = std::make_unique<Wt::WText>(formatted, Wt::TextFormat::Plain);
            table_->elementAt(row, static_cast<int>(c))
                  ->addWidget(std::move(text));
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
        }

        ++row;
    }

    statusText_->setText(std::to_string(row - 1) + " record(s) loaded.");
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
