#ifndef ORDER_REPORT_H
#define ORDER_REPORT_H

#include "PdfReport.h"
#include "json.hpp"

using json = nlohmann::json;

class OrderReport : public PdfReport {
public:
    // Generate a single order invoice/printout
    OrderReport(const json& orderData, const json& orderDetails);

protected:
    void generateContent() override;

private:
    void drawOrderInfo();
    void drawLineItems();
    void drawTotals();

    json order_;
    json details_;
};

// Generate a Purchase Order PDF
class PurchaseOrderReport : public PdfReport {
public:
    PurchaseOrderReport(const json& orderData, const json& orderDetails,
                        const std::string& vendorName);

protected:
    void generateContent() override;

private:
    json order_;
    json details_;
    std::string vendorName_;
};

#endif // ORDER_REPORT_H
