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

### March 11, 2026 - Build, Deployment & DevOps

**Objective:** Establish a complete build and deployment pipeline with Docker orchestration.

**Infrastructure Created:**
- `docker-compose.yml` - Three-tier orchestration:
  - Tier 1: PostgreSQL 16-Alpine database with schema auto-init
  - Tier 2: ApiLogicServer backend API (middleware)
  - Tier 3: Smitty Services Wt frontend
- `docker/Dockerfile.frontend` - Frontend container build
- `database/schema.sql` - Full Northwind-based PostgreSQL schema
- `database/seed_data.sql` - Initial data population
- `scripts/env.sh` + `scripts/start.sh` - Local development helpers
- `.gitignore` - Build artifacts and CMake output exclusions

**Key Fixes Applied:**
- Fixed `WTableRow` click handler (no `clicked()` on `WTableRow`; use cell-level handlers)
- Fixed `WCssDecorationStyle` incomplete type by switching to CSS class-based styling
- Fixed XHTML parse errors by using `Wt::TextFormat::Plain` for all API-sourced data
- Suppressed third-party header warnings by marking Wt includes as SYSTEM in CMake
- Bundled Wt framework resources (`wt-resources/`) with `--resources-dir` configuration

---

### March 11, 2026 - API Client Improvements

**Objective:** Improve API connectivity, logging, and configurability.

**Changes:**
- `ApiClient` reads `SMITTY_API_ENDPOINT` environment variable at startup, falling back to the configured default
- Added structured request/response logging via `Wt::log()`:
  - Logs HTTP method, URL, status code, elapsed time, and record count
  - Errors logged with full JSONAPI error message extraction
- `ApiResponse` struct enhanced with `ok()`, `hasData()`, `hasErrors()`, and `errorMessage()` helpers
- JSONAPI error synthesis for HTTP 4xx/5xx responses without standard error bodies

---

### March 11, 2026 - UI Polish & Styling

**Objective:** Refine the visual design for a professional, modern appearance.

**Changes:**
- Light grey (`#f0f2f5`) background with white card-based content areas
- Cards use `border-radius: 12px` and subtle box-shadows
- Sidebar menu items use bold (`font-weight: 600`) typography
- Dashboard cards have colored left-border accents per entity type
- Data tables use separated borders with hover highlight (`#f1f5ff`)
- Filter bar, detail forms, and settings all use consistent card styling

---

### March 11, 2026 - Default API Port Change

**Objective:** Change the default middleware port from 5656 to 5659 to avoid conflicts with other ALS servers.

**Files Updated:**
- `src/AppSettings.cpp` - Default `apiEndpoint_` changed to `http://localhost:5659/api`
- `src/ApiClient.cpp` - Fallback URL changed to port 5659
- `model/app_model.yaml` - `api_endpoint` updated to port 5659
- `docker-compose.yml` - `APILOGICPROJECT_PORT` and port mapping changed to 5659

---

### March 11, 2026 - Dashboard Stat Cards & List Filters

**Objective:** Add financial visibility with unpaid revenue tracking, outstanding purchase order monitoring, order aging analysis, and filtered list views.

**Dashboard Enhancements:**
- **Unpaid Revenue** card - Sums freight from all unshipped (unpaid) orders, displayed with currency symbol
- **Outstanding POs** card - Counts products with `units_on_order > 0` (awaiting receipt into inventory)
- **Order Aging** section with three cards:
  - Aging > 1 Week: unshipped orders older than 7 days
  - Aging > 1 Month: unshipped orders older than 30 days
  - Aging > 1 Quarter: unshipped orders older than 90 days

**List Filter Checkboxes:**
- **Customer List** - "Outstanding Balance" checkbox: fetches all orders, identifies customer IDs with unshipped orders, filters the customer table to only those customers
- **Product List** - "Pending Receipt" checkbox: filters to products where `units_on_order > 0`
- **Order List** - "Unpaid Orders" checkbox: filters to orders with no `shipped_date`

**Architecture Changes:**
- Added `addCustomFilters()` virtual hook to `EntityListView` - subclasses override to inject custom filter widgets into the filter bar
- Added `filterRecord()` virtual hook to `EntityListView` - subclasses override for client-side row filtering after API fetch
- `WCheckBox` included in `EntityListView.h` for subclass use

**CSS Additions:**
- New card accent colors: red (unpaid revenue), purple (outstanding POs), yellow/orange/dark-red (aging tiers)
- `.filter-checkbox` style for inline checkbox alignment in filter bars

---

*Future entries will be appended below as development continues.*
