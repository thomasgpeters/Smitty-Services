# Smitty Services - Development Log

## Phase 1: Initial Application Setup (March 2026)

### March 11, 2026 - Project Initialization

**Objective:** Create the foundational Wt (Witty) C++ web application for Smitty Services, a business management portal backed by an ApiLogicServer JSONAPI backend.

**Decisions Made:**
- Framework: C++ with Wt (Witty) web framework
- JSON Parser: nlohmann header-only JSON library
- Build System: CMake 3.16+
- API Protocol: JSONAPI (JSON:API specification)
- Backend: ApiLogicServer at `http://localhost:5656/api`

**Architecture Established:**
- Modular class hierarchy with `Entity` base class for all data entities
- `EntityRegistry` singleton for centralized entity metadata management
- `EntityListView` and `EntityDetailView` base widgets for reusable list/detail patterns
- `ApiClient` singleton for all JSONAPI HTTP communication via libcurl
- `Auth` module for session management (stub, ready for backend integration)
- `AppSettings` singleton for application-wide configuration

**UI Layout Created:**
- Navigation bar: Logo + "Smitty Services" title (left), Settings + Account buttons (right)
- Collapsible dark blue sidebar: Dashboard, Customers, Orders, Products, Settings
- Sticky black footer: "Imagery Business Systems, LLC"
- White content work area with stacked widget navigation

**Entity Pages Implemented:**
- Dashboard with summary cards (Customer/Order/Product counts)
- Customer List with filter -> Customer Detail
- Order List with filter -> Order Detail
- Product List with filter -> Product Detail
- Settings page (API endpoint, date format, currency)

**Files Created:**
- `CMakeLists.txt` - Build configuration
- `include/` - All header files (Entity.h, Auth.h, AppSettings.h, etc.)
- `src/` - All implementation files (19 .cpp files)
- `resources/css/smitty.css` - Complete application stylesheet
- `include/json.hpp` - nlohmann JSON header-only library

---

### March 11, 2026 - PDF Report Generation Module

**Objective:** Add lightweight PDF generation capability for printing Orders, Purchase Orders, Customer Monthly Statements, and Revenue Reports.

**Library Selected:** PDFGen (single .c/.h file, public domain, zero dependencies)

**Architecture:**
- `PdfReport` base class providing reusable document layout primitives:
  - Page header/footer with "Imagery Business Systems, LLC" branding
  - Table rendering (header rows, data rows, alignment, currency formatting)
  - Automatic page breaks when content exceeds page height
  - Section titles, horizontal rules, text helpers (bold, right-aligned)
- Subclass hierarchy for specific report types:
  - `OrderReport` -- Single order invoice with line items and totals
  - `PurchaseOrderReport` -- PO document with vendor info and authorization lines
  - `CustomerStatementReport` -- Monthly statement with order summary and billing address
  - `RevenueReport` -- Revenue breakdown with summary cards and period table

**Files Created:**
- `include/pdfgen.h` + `src/pdfgen.c` -- PDFGen library (single-file C library)
- `include/PdfReport.h` + `src/PdfReport.cpp` -- Base report class
- `include/OrderReport.h` + `src/OrderReport.cpp` -- Order/PO reports
- `include/CustomerStatementReport.h` + `src/CustomerStatementReport.cpp` -- Statements
- `include/RevenueReport.h` + `src/RevenueReport.cpp` -- Revenue tracking

---

*Future entries will be appended below as development continues.*
