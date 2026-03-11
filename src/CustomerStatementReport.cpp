#include "CustomerStatementReport.h"

CustomerStatementReport::CustomerStatementReport(const json& customer,
                                                   const json& orders,
                                                   const std::string& statementMonth)
    : PdfReport("Customer Monthly Statement")
    , customer_(customer)
    , orders_(orders)
    , statementMonth_(statementMonth) {
}

void CustomerStatementReport::generateContent() {
    float y = contentTop();
    drawCustomerInfo();
    drawOrderSummaryTable();
    drawBalanceSummary();
}

void CustomerStatementReport::drawCustomerInfo() {
    float y = contentTop();

    addTextBold("MONTHLY STATEMENT", 16, MARGIN_LEFT, y);
    y -= 20;
    addText("Statement Period: " + statementMonth_, 10, MARGIN_LEFT, y);
    y -= 30;

    // Customer details box
    pdf_add_filled_rectangle(pdf_, currentPage_,
                             MARGIN_LEFT, y - 60, contentWidth() / 2.0f, 70,
                             0.5f, PDF_RGB(0xf8, 0xf9, 0xfa), PDF_RGB(0xe8, 0xe8, 0xe8));

    addTextBold("Bill To:", 9, MARGIN_LEFT + 8, y);
    y -= 14;

    if (customer_.contains("attributes")) {
        const auto& a = customer_["attributes"];
        auto val = [&](const std::string& key) -> std::string {
            if (a.contains(key) && !a[key].is_null() && a[key].is_string())
                return a[key].get<std::string>();
            return "";
        };

        addText(val("company_name"), 10, MARGIN_LEFT + 8, y); y -= 12;
        addText(val("contact_name"), 9, MARGIN_LEFT + 8, y); y -= 12;
        addText(val("address"), 9, MARGIN_LEFT + 8, y); y -= 12;

        std::string cityLine = val("city");
        if (!val("region").empty()) cityLine += ", " + val("region");
        if (!val("postal_code").empty()) cityLine += " " + val("postal_code");
        addText(cityLine, 9, MARGIN_LEFT + 8, y); y -= 12;
        addText(val("country"), 9, MARGIN_LEFT + 8, y);
    }
}

void CustomerStatementReport::drawOrderSummaryTable() {
    float y = contentTop() - 160;

    drawSectionTitle("Orders This Period", y);
    y -= 20;

    std::vector<TableColumn> columns = {
        {"Order #", 70, PDF_ALIGN_LEFT},
        {"Order Date", 100, PDF_ALIGN_LEFT},
        {"Ship Name", 150, PDF_ALIGN_LEFT},
        {"Ship City", 100, PDF_ALIGN_LEFT},
        {"Freight", 90, PDF_ALIGN_RIGHT}
    };

    drawTableHeader(columns, y);
    y -= HEADER_ROW_HEIGHT;

    double totalFreight = 0.0;
    int orderCount = 0;

    if (orders_.contains("data") && orders_["data"].is_array()) {
        for (const auto& order : orders_["data"]) {
            y = checkPageBreak(y, ROW_HEIGHT * 2);

            std::string ordId = "N/A", oDate = "", shipName = "", shipCity = "", freight = "0";

            if (order.contains("attributes")) {
                const auto& a = order["attributes"];
                if (a.contains("order_id") && !a["order_id"].is_null())
                    ordId = a["order_id"].dump();
                if (a.contains("order_date") && !a["order_date"].is_null() && a["order_date"].is_string())
                    oDate = a["order_date"].get<std::string>();
                if (a.contains("ship_name") && !a["ship_name"].is_null() && a["ship_name"].is_string())
                    shipName = a["ship_name"].get<std::string>();
                if (a.contains("ship_city") && !a["ship_city"].is_null() && a["ship_city"].is_string())
                    shipCity = a["ship_city"].get<std::string>();
                if (a.contains("freight") && !a["freight"].is_null())
                    freight = a["freight"].dump();
            }

            double f = 0;
            try { f = std::stod(freight); } catch (...) {}
            totalFreight += f;
            orderCount++;

            std::vector<std::string> row = {
                ordId, formatDate(oDate), shipName, shipCity, formatCurrency(f)
            };
            y = drawTableRow(columns, row, y);
        }
    }

    // Summary
    y -= 10;
    drawHorizontalLine(y, 1.0f, PDF_BLACK);
    y -= 16;
    addTextBold("Total Orders: " + std::to_string(orderCount), 10, MARGIN_LEFT, y);

    char buf[64];
    snprintf(buf, sizeof(buf), "Total Freight: %s", formatCurrency(totalFreight).c_str());
    addTextBold(buf, 10, MARGIN_LEFT + 300, y);
}

void CustomerStatementReport::drawBalanceSummary() {
    // Placeholder for balance summary - can be extended with payment tracking
    float y = contentBottom() + 80;

    drawHorizontalLine(y + 20, 0.5f, PDF_RGB(0xcc, 0xcc, 0xcc));
    addText("This statement is for informational purposes. "
            "Please contact us with any questions.", 8, MARGIN_LEFT, y);
}
