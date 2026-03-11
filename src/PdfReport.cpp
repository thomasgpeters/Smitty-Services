#include "PdfReport.h"
#include <cstdio>
#include <sstream>
#include <iomanip>
#include <cstring>

PdfReport::PdfReport(const std::string& title, float pageWidth, float pageHeight)
    : pdf_(nullptr)
    , currentPage_(nullptr)
    , title_(title)
    , pageWidth_(pageWidth)
    , pageHeight_(pageHeight)
    , pageNumber_(0) {
    struct pdf_info info;
    memset(&info, 0, sizeof(info));
    strncpy(info.creator, "Smitty Services", sizeof(info.creator) - 1);
    strncpy(info.producer, "Smitty Services / PDFGen", sizeof(info.producer) - 1);
    strncpy(info.title, title.c_str(), sizeof(info.title) - 1);
    strncpy(info.author, "Imagery Business Systems, LLC", sizeof(info.author) - 1);
    strncpy(info.subject, title.c_str(), sizeof(info.subject) - 1);

    std::string date = currentDate();
    strncpy(info.date, date.c_str(), sizeof(info.date) - 1);

    pdf_ = pdf_create(pageWidth, pageHeight, &info);
}

PdfReport::~PdfReport() {
    if (pdf_) {
        pdf_destroy(pdf_);
        pdf_ = nullptr;
    }
}

bool PdfReport::save(const std::string& filename) {
    if (!pdf_) return false;

    newPage();
    generateContent();

    int result = pdf_save(pdf_, filename.c_str());
    return result >= 0;
}

void PdfReport::newPage() {
    currentPage_ = pdf_append_page(pdf_);
    pageNumber_++;
    drawPageHeader();
    drawPageFooter();
}

float PdfReport::contentWidth() const {
    return pageWidth_ - MARGIN_LEFT - MARGIN_RIGHT;
}

float PdfReport::contentTop() const {
    return pageHeight_ - MARGIN_TOP - HEADER_HEIGHT;
}

float PdfReport::contentBottom() const {
    return MARGIN_BOTTOM + FOOTER_HEIGHT;
}

void PdfReport::drawPageHeader() {
    float top = pageHeight_ - MARGIN_TOP;

    // Company name
    pdf_set_font(pdf_, "Helvetica-Bold");
    pdf_add_text(pdf_, currentPage_, "Imagery Business Systems, LLC",
                 10, MARGIN_LEFT, top, PDF_RGB(0x66, 0x66, 0x66));

    // Report title
    pdf_set_font(pdf_, "Helvetica-Bold");
    pdf_add_text(pdf_, currentPage_, title_.c_str(),
                 16, MARGIN_LEFT, top - 22, PDF_BLACK);

    // Date on the right
    pdf_set_font(pdf_, "Helvetica");
    std::string date = currentDate();
    float textW = 0;
    pdf_get_font_text_width(pdf_, "Helvetica", date.c_str(), 9, &textW);
    pdf_add_text(pdf_, currentPage_, date.c_str(),
                 9, pageWidth_ - MARGIN_RIGHT - textW, top, PDF_RGB(0x99, 0x99, 0x99));

    // Separator line
    float lineY = top - HEADER_HEIGHT + 10;
    pdf_add_line(pdf_, currentPage_,
                 MARGIN_LEFT, lineY,
                 pageWidth_ - MARGIN_RIGHT, lineY,
                 1.0f, PDF_RGB(0x16, 0x21, 0x3e));
}

void PdfReport::drawPageFooter() {
    float y = MARGIN_BOTTOM;

    // Separator line
    pdf_add_line(pdf_, currentPage_,
                 MARGIN_LEFT, y + FOOTER_HEIGHT - 5,
                 pageWidth_ - MARGIN_RIGHT, y + FOOTER_HEIGHT - 5,
                 0.5f, PDF_RGB(0xcc, 0xcc, 0xcc));

    // Footer text
    pdf_set_font(pdf_, "Helvetica");
    pdf_add_text(pdf_, currentPage_, "Imagery Business Systems, LLC",
                 8, MARGIN_LEFT, y + 8, PDF_RGB(0x99, 0x99, 0x99));

    // Page number on the right
    char pageStr[32];
    snprintf(pageStr, sizeof(pageStr), "Page %d", pageNumber_);
    float textW = 0;
    pdf_get_font_text_width(pdf_, "Helvetica", pageStr, 8, &textW);
    pdf_add_text(pdf_, currentPage_, pageStr,
                 8, pageWidth_ - MARGIN_RIGHT - textW, y + 8,
                 PDF_RGB(0x99, 0x99, 0x99));
}

void PdfReport::addText(const std::string& text, float size, float x, float y,
                        uint32_t colour) {
    pdf_set_font(pdf_, "Helvetica");
    pdf_add_text(pdf_, currentPage_, text.c_str(), size, x, y, colour);
}

void PdfReport::addTextBold(const std::string& text, float size, float x, float y,
                            uint32_t colour) {
    pdf_set_font(pdf_, "Helvetica-Bold");
    pdf_add_text(pdf_, currentPage_, text.c_str(), size, x, y, colour);
}

void PdfReport::addTextRight(const std::string& text, float size, float x, float y,
                             float width, uint32_t colour) {
    pdf_set_font(pdf_, "Helvetica");
    float textW = 0;
    pdf_get_font_text_width(pdf_, "Helvetica", text.c_str(), size, &textW);
    float xPos = x + width - textW;
    pdf_add_text(pdf_, currentPage_, text.c_str(), size, xPos, y, colour);
}

void PdfReport::drawTableHeader(const std::vector<TableColumn>& columns, float y) {
    float x = MARGIN_LEFT;

    // Header background
    pdf_add_filled_rectangle(pdf_, currentPage_,
                             MARGIN_LEFT, y - 4,
                             contentWidth(), HEADER_ROW_HEIGHT,
                             0, PDF_RGB(0xf0, 0xf2, 0xf5), PDF_RGB(0xf0, 0xf2, 0xf5));

    pdf_set_font(pdf_, "Helvetica-Bold");
    for (const auto& col : columns) {
        if (col.align == PDF_ALIGN_RIGHT) {
            float textW = 0;
            pdf_get_font_text_width(pdf_, "Helvetica-Bold", col.header.c_str(), 9, &textW);
            pdf_add_text(pdf_, currentPage_, col.header.c_str(),
                         9, x + col.width - textW, y + 2, PDF_RGB(0x33, 0x33, 0x33));
        } else {
            pdf_add_text(pdf_, currentPage_, col.header.c_str(),
                         9, x + 4, y + 2, PDF_RGB(0x33, 0x33, 0x33));
        }
        x += col.width;
    }

    // Bottom border
    pdf_add_line(pdf_, currentPage_,
                 MARGIN_LEFT, y - 4,
                 pageWidth_ - MARGIN_RIGHT, y - 4,
                 1.0f, PDF_RGB(0xde, 0xe2, 0xe6));
}

float PdfReport::drawTableRow(const std::vector<TableColumn>& columns,
                              const std::vector<std::string>& values, float y) {
    float x = MARGIN_LEFT;

    pdf_set_font(pdf_, "Helvetica");
    for (size_t i = 0; i < columns.size() && i < values.size(); ++i) {
        const auto& col = columns[i];
        const auto& val = values[i];

        if (col.align == PDF_ALIGN_RIGHT) {
            float textW = 0;
            pdf_get_font_text_width(pdf_, "Helvetica", val.c_str(), 9, &textW);
            pdf_add_text(pdf_, currentPage_, val.c_str(),
                         9, x + col.width - textW, y + 2, PDF_RGB(0x55, 0x55, 0x55));
        } else {
            pdf_add_text(pdf_, currentPage_, val.c_str(),
                         9, x + 4, y + 2, PDF_RGB(0x55, 0x55, 0x55));
        }
        x += col.width;
    }

    // Row bottom border
    pdf_add_line(pdf_, currentPage_,
                 MARGIN_LEFT, y - 4,
                 pageWidth_ - MARGIN_RIGHT, y - 4,
                 0.25f, PDF_RGB(0xee, 0xee, 0xee));

    return y - ROW_HEIGHT;
}

void PdfReport::drawHorizontalLine(float y, float thickness, uint32_t colour) {
    pdf_add_line(pdf_, currentPage_,
                 MARGIN_LEFT, y,
                 pageWidth_ - MARGIN_RIGHT, y,
                 thickness, colour);
}

void PdfReport::drawSectionTitle(const std::string& title, float y) {
    pdf_set_font(pdf_, "Helvetica-Bold");
    pdf_add_text(pdf_, currentPage_, title.c_str(), 12, MARGIN_LEFT, y, PDF_BLACK);
    drawHorizontalLine(y - 4, 0.5f, PDF_RGB(0xe8, 0xe8, 0xe8));
}

std::string PdfReport::formatCurrency(double amount, const std::string& symbol) {
    char buf[64];
    snprintf(buf, sizeof(buf), "%s%.2f", symbol.c_str(), amount);
    return std::string(buf);
}

std::string PdfReport::formatDate(const std::string& isoDate) {
    // Pass through ISO dates as-is for now; can be extended with locale formatting
    return isoDate;
}

std::string PdfReport::currentDate() {
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d", t);
    return std::string(buf);
}

float PdfReport::checkPageBreak(float y, float requiredSpace) {
    if (y - requiredSpace < contentBottom()) {
        newPage();
        return contentTop();
    }
    return y;
}
