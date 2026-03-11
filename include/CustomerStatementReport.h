#ifndef CUSTOMER_STATEMENT_REPORT_H
#define CUSTOMER_STATEMENT_REPORT_H

#include "PdfReport.h"
#include "json.hpp"

using json = nlohmann::json;

class CustomerStatementReport : public PdfReport {
public:
    CustomerStatementReport(const json& customer, const json& orders,
                            const std::string& statementMonth);

protected:
    void generateContent() override;

private:
    void drawCustomerInfo();
    void drawOrderSummaryTable();
    void drawBalanceSummary();

    json customer_;
    json orders_;
    std::string statementMonth_;
};

#endif // CUSTOMER_STATEMENT_REPORT_H
