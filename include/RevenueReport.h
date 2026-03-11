#ifndef REVENUE_REPORT_H
#define REVENUE_REPORT_H

#include "PdfReport.h"
#include "json.hpp"

using json = nlohmann::json;

struct RevenueSummary {
    std::string period;       // e.g., "2026-03" or "Q1 2026"
    double totalRevenue;
    int orderCount;
    double averageOrderValue;
};

class RevenueReport : public PdfReport {
public:
    RevenueReport(const std::vector<RevenueSummary>& summaries,
                  const std::string& reportTitle,
                  const std::string& dateRange);

protected:
    void generateContent() override;

private:
    void drawSummaryCards();
    void drawRevenueTable();
    void drawNotes();

    std::vector<RevenueSummary> summaries_;
    std::string dateRange_;
    double grandTotal_;
    int totalOrders_;
};

#endif // REVENUE_REPORT_H
