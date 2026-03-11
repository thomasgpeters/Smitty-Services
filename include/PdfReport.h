#ifndef PDF_REPORT_H
#define PDF_REPORT_H

#include <string>
#include <vector>
#include <ctime>
#include "pdfgen.h"

struct TableColumn {
    std::string header;
    float width;     // in points
    int align;       // PDF_ALIGN_LEFT, PDF_ALIGN_RIGHT, PDF_ALIGN_CENTER
};

class PdfReport {
public:
    PdfReport(const std::string& title, float pageWidth = PDF_LETTER_WIDTH,
              float pageHeight = PDF_LETTER_HEIGHT);
    virtual ~PdfReport();

    bool save(const std::string& filename);

    // Page layout constants
    static constexpr float MARGIN_LEFT = 50.0f;
    static constexpr float MARGIN_RIGHT = 50.0f;
    static constexpr float MARGIN_TOP = 50.0f;
    static constexpr float MARGIN_BOTTOM = 60.0f;
    static constexpr float HEADER_HEIGHT = 60.0f;
    static constexpr float FOOTER_HEIGHT = 30.0f;
    static constexpr float ROW_HEIGHT = 18.0f;
    static constexpr float HEADER_ROW_HEIGHT = 22.0f;

protected:
    // Subclasses override this to generate their content
    virtual void generateContent() = 0;

    // Document helpers
    void newPage();
    float contentWidth() const;
    float contentTop() const;
    float contentBottom() const;

    // Header/footer drawn on each page
    void drawPageHeader();
    void drawPageFooter();

    // Text helpers
    void addText(const std::string& text, float size, float x, float y,
                 uint32_t colour = PDF_BLACK);
    void addTextBold(const std::string& text, float size, float x, float y,
                     uint32_t colour = PDF_BLACK);
    void addTextRight(const std::string& text, float size, float x, float y,
                      float width, uint32_t colour = PDF_BLACK);

    // Table helpers
    void drawTableHeader(const std::vector<TableColumn>& columns, float y);
    float drawTableRow(const std::vector<TableColumn>& columns,
                       const std::vector<std::string>& values, float y);
    void drawHorizontalLine(float y, float thickness = 0.5f,
                            uint32_t colour = PDF_BLACK);
    void drawSectionTitle(const std::string& title, float y);

    // Formatting helpers
    static std::string formatCurrency(double amount, const std::string& symbol = "$");
    static std::string formatDate(const std::string& isoDate);
    static std::string currentDate();

    // Check if we need a new page, returns new Y position
    float checkPageBreak(float y, float requiredSpace);

    struct pdf_doc* pdf_;
    struct pdf_object* currentPage_;
    std::string title_;
    float pageWidth_;
    float pageHeight_;
    int pageNumber_;
};

#endif // PDF_REPORT_H
