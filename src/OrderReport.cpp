#include "OrderReport.h"

// ===== Order Invoice Report =====

OrderReport::OrderReport(const json& orderData, const json& orderDetails)
    : PdfReport("Order Invoice")
    , order_(orderData)
    , details_(orderDetails) {
}

void OrderReport::generateContent() {
    float y = contentTop();
    drawOrderInfo();
    y -= 120;
    drawLineItems();
}

void OrderReport::drawOrderInfo() {
    float y = contentTop();

    // Order number and date
    std::string orderId = "N/A";
    if (order_.contains("attributes")) {
        const auto& attrs = order_["attributes"];
        if (attrs.contains("order_id") && !attrs["order_id"].is_null())
            orderId = attrs["order_id"].dump();
    }

    addTextBold("Order #" + orderId, 14, MARGIN_LEFT, y);
    y -= 24;

    if (order_.contains("attributes")) {
        const auto& attrs = order_["attributes"];

        auto addField = [&](const std::string& label, const std::string& key) {
            if (attrs.contains(key) && !attrs[key].is_null()) {
                std::string val = attrs[key].is_string() ?
                    attrs[key].get<std::string>() : attrs[key].dump();
                addTextBold(label + ":", 9, MARGIN_LEFT, y);
                addText(val, 9, MARGIN_LEFT + 100, y);
                y -= 14;
            }
        };

        addField("Customer", "customer_id");
        addField("Order Date", "order_date");
        addField("Required Date", "required_date");
        addField("Shipped Date", "shipped_date");
        addField("Ship Name", "ship_name");
        addField("Ship Address", "ship_address");
        addField("Ship City", "ship_city");
        addField("Ship Country", "ship_country");
        addField("Freight", "freight");
    }
}

void OrderReport::drawLineItems() {
    float y = contentTop() - 170;

    drawSectionTitle("Line Items", y);
    y -= 20;

    std::vector<TableColumn> columns = {
        {"Product ID", 80, PDF_ALIGN_LEFT},
        {"Unit Price", 100, PDF_ALIGN_RIGHT},
        {"Quantity", 80, PDF_ALIGN_RIGHT},
        {"Discount", 80, PDF_ALIGN_RIGHT},
        {"Line Total", 120, PDF_ALIGN_RIGHT}
    };

    drawTableHeader(columns, y);
    y -= HEADER_ROW_HEIGHT;

    double grandTotal = 0.0;

    if (details_.contains("data") && details_["data"].is_array()) {
        for (const auto& item : details_["data"]) {
            y = checkPageBreak(y, ROW_HEIGHT * 2);

            std::string prodId = "N/A", price = "0", qty = "0", disc = "0";

            if (item.contains("attributes")) {
                const auto& a = item["attributes"];
                if (a.contains("product_id") && !a["product_id"].is_null())
                    prodId = a["product_id"].dump();
                if (a.contains("unit_price") && !a["unit_price"].is_null())
                    price = a["unit_price"].dump();
                if (a.contains("quantity") && !a["quantity"].is_null())
                    qty = a["quantity"].dump();
                if (a.contains("discount") && !a["discount"].is_null())
                    disc = a["discount"].dump();
            }

            double p = 0, q = 0, d = 0;
            try { p = std::stod(price); } catch (...) {}
            try { q = std::stod(qty); } catch (...) {}
            try { d = std::stod(disc); } catch (...) {}

            double lineTotal = p * q * (1.0 - d);
            grandTotal += lineTotal;

            std::vector<std::string> row = {
                prodId,
                formatCurrency(p),
                qty,
                disc,
                formatCurrency(lineTotal)
            };

            y = drawTableRow(columns, row, y);
        }
    }

    // Grand total
    y -= 10;
    drawHorizontalLine(y, 1.0f, PDF_BLACK);
    y -= 16;
    addTextBold("Total:", 11, MARGIN_LEFT + 240, y);
    addTextBold(formatCurrency(grandTotal), 11,
                MARGIN_LEFT + 340, y);
}

void OrderReport::drawTotals() {
    // Handled inline in drawLineItems
}

// ===== Purchase Order Report =====

PurchaseOrderReport::PurchaseOrderReport(const json& orderData, const json& orderDetails,
                                         const std::string& vendorName)
    : PdfReport("Purchase Order")
    , order_(orderData)
    , details_(orderDetails)
    , vendorName_(vendorName) {
}

void PurchaseOrderReport::generateContent() {
    float y = contentTop();

    // PO Header
    addTextBold("PURCHASE ORDER", 18, MARGIN_LEFT, y);
    y -= 30;

    addTextBold("Vendor:", 10, MARGIN_LEFT, y);
    addText(vendorName_, 10, MARGIN_LEFT + 60, y);
    y -= 16;

    std::string orderId = "N/A";
    if (order_.contains("attributes") && order_["attributes"].contains("order_id"))
        orderId = order_["attributes"]["order_id"].dump();

    addTextBold("PO Number:", 10, MARGIN_LEFT, y);
    addText(orderId, 10, MARGIN_LEFT + 80, y);
    y -= 16;

    addTextBold("Date:", 10, MARGIN_LEFT, y);
    addText(currentDate(), 10, MARGIN_LEFT + 80, y);
    y -= 30;

    // Line items table
    drawSectionTitle("Items", y);
    y -= 20;

    std::vector<TableColumn> columns = {
        {"Product ID", 100, PDF_ALIGN_LEFT},
        {"Description", 180, PDF_ALIGN_LEFT},
        {"Qty", 60, PDF_ALIGN_RIGHT},
        {"Unit Price", 80, PDF_ALIGN_RIGHT},
        {"Amount", 90, PDF_ALIGN_RIGHT}
    };

    drawTableHeader(columns, y);
    y -= HEADER_ROW_HEIGHT;

    double total = 0.0;

    if (details_.contains("data") && details_["data"].is_array()) {
        for (const auto& item : details_["data"]) {
            y = checkPageBreak(y, ROW_HEIGHT * 2);

            std::string prodId = "N/A", price = "0", qty = "0";

            if (item.contains("attributes")) {
                const auto& a = item["attributes"];
                if (a.contains("product_id") && !a["product_id"].is_null())
                    prodId = a["product_id"].dump();
                if (a.contains("unit_price") && !a["unit_price"].is_null())
                    price = a["unit_price"].dump();
                if (a.contains("quantity") && !a["quantity"].is_null())
                    qty = a["quantity"].dump();
            }

            double p = 0, q = 0;
            try { p = std::stod(price); } catch (...) {}
            try { q = std::stod(qty); } catch (...) {}
            double amount = p * q;
            total += amount;

            std::vector<std::string> row = {
                prodId, "-", qty, formatCurrency(p), formatCurrency(amount)
            };
            y = drawTableRow(columns, row, y);
        }
    }

    y -= 10;
    drawHorizontalLine(y, 1.0f, PDF_BLACK);
    y -= 16;
    addTextBold("TOTAL:", 11, MARGIN_LEFT + 340, y);
    addTextBold(formatCurrency(total), 11, MARGIN_LEFT + 420, y);

    // Authorization line
    y -= 50;
    addText("Authorized By: ____________________________", 10, MARGIN_LEFT, y);
    y -= 20;
    addText("Date: ____________________________", 10, MARGIN_LEFT, y);
}
