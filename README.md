# Smitty Services

## Executive Summary

Smitty Services is a business management web application built with C++ and the Wt (Witty) framework. It provides a modern, responsive portal for managing Customers, Orders, and Products through a clean admin-style interface backed by an ApiLogicServer JSONAPI backend.

The application is designed for incremental feature adoption -- new entities, workflows, and capabilities can be added modularly as business needs evolve.

## Architecture

### Technology Stack
- **Frontend/Server:** C++ 17 with Wt (Witty) web framework
- **JSON Parsing:** nlohmann header-only JSON library
- **Backend API:** ApiLogicServer (JSONAPI specification)
- **Build System:** CMake 3.16+
- **HTTP Client:** libcurl
- **PDF Generation:** PDFGen (single-file C library, zero dependencies)

### Modular Design

The application follows a modular architecture designed for extensibility:

```
SmittyApplication          -- Main app shell (navbar, sidebar, footer, routing)
  |-- Entity               -- Base class defining entity metadata & columns
  |-- EntityRegistry       -- Centralized entity definitions (Customer, Order, Product, ...)
  |-- EntityListView       -- Reusable list widget with filtering & row selection
  |-- EntityDetailView     -- Reusable detail widget with field rendering
  |-- ApiClient            -- JSONAPI HTTP client (libcurl)
  |-- Auth                 -- Authentication & session management
  |-- AppSettings          -- Application-wide configuration
  +-- PdfReport            -- PDF generation (OrderReport, CustomerStatement, RevenueReport)
```

**Adding a new entity** requires only:
1. Register the entity definition in `EntityRegistry`
2. Create thin List/Detail wrapper classes
3. Wire into the sidebar and content stack

This pattern ensures that as Smitty adopts new features (Categories, Suppliers, Employees, Territories, etc.), they can be added with minimal code changes and no architectural rework.

### UI Layout
- **Navbar:** Logo + title (left-justified), Settings + Account buttons (float right)
- **Sidebar:** Dark blue, collapsible. Dashboard > Customers, Orders, Products > Settings
- **Content Area:** White workspace for list/detail views
- **Footer:** Black, sticky bottom -- "Imagery Business Systems, LLC"

## Building

```bash
mkdir build && cd build
cmake ..
make
```

## Running

```bash
./smitty_services --docroot resources --http-listen 0.0.0.0:8080
```

Then open `http://localhost:8080` in your browser.

The application expects the ApiLogicServer backend running at `http://localhost:5656/api` (configurable in Settings).

## Project Structure

```
Smitty-Services/
|-- CMakeLists.txt              # Build configuration
|-- model/
|   +-- app_model.yaml          # ApiLogicServer entity/API model
|-- include/
|   |-- json.hpp                # nlohmann JSON (header-only)
|   |-- SmittyApplication.h     # Main application class
|   |-- Entity.h                # Entity base class & ColumnDef
|   |-- EntityRegistry.h        # Centralized entity definitions
|   |-- EntityListView.h        # Reusable list widget
|   |-- EntityDetailView.h      # Reusable detail widget
|   |-- ApiClient.h             # JSONAPI HTTP client
|   |-- Auth.h                  # Authentication module
|   |-- AppSettings.h           # Application settings
|   |-- NavBar.h                # Navigation bar widget
|   |-- SideBar.h               # Sidebar navigation widget
|   |-- Footer.h                # Footer widget
|   |-- Dashboard.h             # Dashboard page
|   |-- SettingsView.h          # Settings page
|   |-- pdfgen.h                # PDFGen library (public domain)
|   |-- PdfReport.h             # Base report class
|   |-- OrderReport.h           # Order invoice & PO reports
|   |-- CustomerStatementReport.h # Monthly customer statements
|   +-- RevenueReport.h         # Revenue tracking reports
|-- src/
|   |-- main.cpp                # Entry point
|   |-- SmittyApplication.cpp   # App shell & routing
|   |-- Entity.cpp              # Entity metadata logic
|   |-- EntityRegistry.cpp      # Entity definitions
|   |-- EntityListView.cpp      # List rendering & filtering
|   |-- EntityDetailView.cpp    # Detail field rendering
|   |-- ApiClient.cpp           # HTTP/JSONAPI client
|   |-- Auth.cpp                # Auth session management
|   |-- AppSettings.cpp         # Settings management
|   |-- NavBar.cpp              # Navbar widget
|   |-- SideBar.cpp             # Sidebar widget
|   |-- Footer.cpp              # Footer widget
|   |-- Dashboard.cpp           # Dashboard page
|   |-- CustomerList.cpp        # Customer list page
|   |-- CustomerDetail.cpp      # Customer detail page
|   |-- OrderList.cpp           # Order list page
|   |-- OrderDetail.cpp         # Order detail page
|   |-- ProductList.cpp         # Product list page
|   |-- ProductDetail.cpp       # Product detail page
|   |-- Settings.cpp            # Settings page
|   |-- pdfgen.c                # PDFGen library source
|   |-- PdfReport.cpp           # Base report implementation
|   |-- OrderReport.cpp         # Order/PO report generation
|   |-- CustomerStatementReport.cpp # Statement generation
|   +-- RevenueReport.cpp       # Revenue report generation
+-- resources/
    +-- css/
        +-- smitty.css          # Application stylesheet
```

## Documentation

- [Development Log](DEVELOPMENT_LOG.md) -- Chronological record of development activity
- [Design Decisions](DESIGN_DECISIONS.md) -- Architectural decisions and rationale
