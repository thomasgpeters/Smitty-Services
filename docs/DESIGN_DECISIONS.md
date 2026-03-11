# Smitty Services - Design Decisions

## DD-001: C++ with Wt Framework

**Decision:** Use C++ with the Wt (Witty) web framework for the application UI.

**Rationale:** Wt provides a widget-based server-side rendering model similar to desktop GUI frameworks. This allows building rich web applications entirely in C++ without requiring separate frontend/backend stacks. The server-side approach simplifies deployment and reduces attack surface.

**Trade-offs:** Requires C++ compilation environment. Less frontend developer ecosystem compared to JavaScript frameworks. Benefits include type safety, performance, and single-language stack.

---

## DD-002: nlohmann Header-Only JSON Parser

**Decision:** Use nlohmann/json as the JSON parsing library.

**Rationale:** Header-only means zero additional build dependencies. Industry-standard C++ JSON library with intuitive API, excellent documentation, and broad adoption. Simplifies JSONAPI response parsing throughout the application.

---

## DD-003: Modular Entity Architecture

**Decision:** Create a base `Entity` class with `ColumnDef` metadata, a centralized `EntityRegistry`, and reusable `EntityListView`/`EntityDetailView` base widgets.

**Rationale:** The app_model.yaml defines multiple entities (Customer, Order, Product, etc.) that all follow the same patterns: list with filter, detail view, JSONAPI data source. A modular base class approach means:
- Adding new entities requires minimal code (inherit from base, register in EntityRegistry)
- Column metadata drives both list and detail rendering automatically
- Filter, sort, and display logic is shared across all entity types
- Future entities (Category, Supplier, Employee, etc.) can be added incrementally as Smitty purchases additional features

**Extensibility:** When new entities are needed, the process is:
1. Add entity definition to `EntityRegistry::initializeEntities()`
2. Create entity-specific List/Detail .cpp files (thin wrappers)
3. Register pages in `SmittyApplication::createLayout()`
4. Add sidebar navigation button

---

## DD-004: Singleton Service Layer

**Decision:** Use singleton pattern for `ApiClient`, `Auth`, `AppSettings`, and `EntityRegistry`.

**Rationale:** These represent application-wide shared state that should have exactly one instance per process. Wt applications run server-side where each user session gets its own `WApplication` instance, but services like the API client and settings are shared infrastructure. The singleton ensures consistent configuration across the application.

---

## DD-005: JSONAPI Protocol

**Decision:** Communicate with the backend using the JSONAPI specification (application/vnd.api+json).

**Rationale:** The ApiLogicServer backend exposes a JSONAPI-compliant REST API. The `ApiClient` class issues requests with proper JSONAPI content-type headers and parses the standard `{ "data": [...], "attributes": {...} }` response format. This allows seamless integration with the existing middleware.

---

## DD-006: Collapsible Sidebar Navigation

**Decision:** Implement a dark blue sidebar with collapsible toggle, separating entity navigation from system functions.

**Rationale:** Follows the Admin Portal wireframe pattern established in Student-Onboarding. The sidebar groups navigation logically: Dashboard (overview), then entity actions (Customers, Orders, Products), then system (Settings). The hamburger toggle allows users to maximize work area on smaller screens.

---

## DD-007: Stacked Widget Page Navigation

**Decision:** Use `WStackedWidget` for page navigation rather than URL-based routing.

**Rationale:** Wt's stacked widget provides instant page switching without full page reloads. Each page (Dashboard, Customer List, Customer Detail, etc.) is pre-created and swapped into view. This gives a desktop-application feel with smooth transitions. Detail views are loaded on-demand when a list row is clicked.

---

## DD-008: CSS-Only Styling (No Framework)

**Decision:** Use a custom CSS stylesheet rather than Bootstrap or other CSS frameworks.

**Rationale:** Keeps the application lightweight with no external CSS dependencies. The stylesheet is purpose-built for the exact layout requirements: navbar, sidebar, footer, data tables, filter bars, detail forms, and dashboard cards. Custom CSS gives full control over the Imagery Business Systems branding.

---

## DD-009: PDFGen for PDF Generation

**Decision:** Use PDFGen (single .c/.h file, public domain) for all PDF report generation.

**Rationale:** PDFGen is the lightest possible PDF library -- a single C source file with zero external dependencies. It provides the drawing primitives needed for business documents (text, lines, rectangles, images) while keeping the build simple. The `PdfReport` base class wraps PDFGen's C API in a C++ class hierarchy with reusable table layout, page break management, and branding.

**Alternatives Considered:**
- libHaru: More features but adds zlib/libpng dependencies
- PDF-Writer (PDFHummus): Full-featured but heavy (FreeType, zlib, libpng, libjpeg, libtiff)

**Trade-offs:** PDFGen has no advanced typography or embedded font support beyond the 14 standard PDF fonts. For business documents (invoices, statements, reports), this is sufficient. If rich formatting is needed later, the `PdfReport` base class can be re-targeted to a different backend without changing the report subclasses.

---

## DD-010: Report Class Hierarchy

**Decision:** Create a `PdfReport` abstract base class with virtual `generateContent()`, with concrete subclasses for each report type (OrderReport, CustomerStatementReport, RevenueReport).

**Rationale:** All reports share common structure: page header with company branding, page footer with page numbers, table rendering, currency formatting, and automatic page breaks. The base class encapsulates these patterns while subclasses focus only on their specific data layout. Adding a new report type requires only inheriting from `PdfReport` and implementing `generateContent()`.

---

## DD-011: Environment-Based API Configuration

**Decision:** Read the API endpoint from the `SMITTY_API_ENDPOINT` environment variable at startup, falling back to the compiled default.

**Rationale:** In a Docker/multi-server environment, the API endpoint differs between development, staging, and production. Environment variables are the standard 12-factor approach. The `AppSettings` singleton holds the default, and `ApiClient` checks the environment at construction time. The Settings page also allows runtime changes within a session.

---

## DD-012: Three-Tier Docker Compose Architecture

**Decision:** Orchestrate all three tiers (PostgreSQL, ApiLogicServer, Wt frontend) in a single `docker-compose.yml` with environment-variable overrides for ports.

**Rationale:** A single compose file lets developers `docker compose up -d` to get the full stack running. Port overrides (`SMITTY_APP_PORT`, `SMITTY_API_PORT`, `SMITTY_DB_PORT`) allow running multiple instances side-by-side -- critical when multiple ALS servers are in use. Default API port is 5659 to avoid conflicts with other ALS instances on the same host.

---

## DD-013: Client-Side Record Filtering via Virtual Hooks

**Decision:** Add `addCustomFilters()` and `filterRecord()` virtual methods to `EntityListView` rather than server-side API filter parameters.

**Rationale:** The JSONAPI backend does not expose computed fields like "has unpaid orders" or "has pending receipt." Client-side filtering keeps the architecture simple:
- `addCustomFilters()` lets subclasses inject checkboxes or other widgets into the existing filter bar
- `filterRecord()` lets subclasses accept/reject individual records after the API response arrives
- No changes needed to the backend API or data model
- The base class calls both hooks automatically during `buildUI()` and `populateTable()`

**Trade-offs:** Client-side filtering fetches all records then filters in-memory, which works well for the current data volumes (hundreds of records). For very large datasets, server-side filtering with custom API endpoints would be more efficient.

---

## DD-014: Unpaid Order Detection via shipped_date

**Decision:** Treat orders with a null or empty `shipped_date` as "unpaid" for revenue tracking and customer balance calculations.

**Rationale:** The Northwind data model does not have an explicit "paid" flag. In the standard Northwind workflow, an order is fulfilled when it ships. Orders without a `shipped_date` represent open/unfulfilled orders -- the closest proxy for "unpaid." This convention is used consistently across:
- Dashboard unpaid revenue card
- Dashboard order aging calculations
- Customer outstanding balance filter
- Order list unpaid filter

---

## DD-015: Order Aging Buckets

**Decision:** Define three aging tiers based on `order_date` age relative to the current date: >7 days (1 week), >30 days (1 month), >90 days (1 quarter).

**Rationale:** These are standard accounts-receivable aging buckets used in business accounting. Each unpaid order falls into exactly one bucket (the most recent applicable tier), giving a quick visual breakdown of how stale outstanding orders are. The dashboard displays counts per tier with color-coded cards (yellow for week, orange for month, red for quarter) to draw attention to older items.

---

## DD-016: Customer Outstanding Balance via Cross-Entity Lookup

**Decision:** When the "Outstanding Balance" checkbox is checked on the Customer list, fetch all orders to build a set of customer IDs with unpaid orders, then filter the customer table client-side.

**Rationale:** The JSONAPI backend serves each entity independently -- there is no "customers with unpaid orders" endpoint. Rather than adding custom server endpoints, we perform a second API call to fetch orders, extract the `customer_id` from unshipped orders, and use `filterRecord()` to include only matching customers. This keeps the backend generic and the logic in the frontend where it's visible and modifiable.

**Trade-offs:** Requires an extra API call when the checkbox is toggled. For the current dataset size this is negligible. The order data is not cached across checkbox toggles to ensure freshness.

---

*Future design decisions will be appended as features are added.*
