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

*Future design decisions will be appended as features are added.*
