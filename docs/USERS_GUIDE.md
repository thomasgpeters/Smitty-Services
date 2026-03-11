# Smitty Services - User's Guide

## Getting Started

Smitty Services is a business management portal for viewing and managing your customers, orders, and products. The application connects to your ApiLogicServer middleware to provide a clean, modern interface for everyday business operations.

### Accessing the Application

Open your web browser and navigate to the application URL (default: `http://localhost:8080`). The Dashboard will load automatically as your home page.

### Navigation

The application uses a sidebar for navigation. Click any menu item to switch between pages:

- **Dashboard** - Overview of your business at a glance
- **Customers** - View and manage customer records
- **Orders** - View and manage order records
- **Products** - View and manage product inventory
- **Settings** - Configure application preferences

Use the hamburger menu icon in the top-left corner of the navigation bar to collapse or expand the sidebar, giving you more screen space when needed.

---

## Dashboard

The Dashboard provides a real-time summary of your business data.

### Summary Cards

The top row of cards shows key counts:

| Card | Description |
|------|-------------|
| **Customers** | Total number of customer records |
| **Orders** | Total number of order records |
| **Products** | Total number of product records |
| **Unpaid Revenue** | Total freight value of all unshipped orders |
| **Outstanding POs** | Number of products with units on order awaiting receipt |

### Order Aging

Below the summary cards, the Order Aging section breaks down unshipped orders by how long they have been open:

| Card | Description |
|------|-------------|
| **Aging > 1 Week** | Unshipped orders placed more than 7 days ago |
| **Aging > 1 Month** | Unshipped orders placed more than 30 days ago |
| **Aging > 1 Quarter** | Unshipped orders placed more than 90 days ago |

These aging buckets help you identify overdue orders that may need attention. Orders in the "1 Quarter" bucket are the most critical.

Click **Refresh** to reload all dashboard data from the server.

---

## Customer List

The Customer List displays all customer records in a sortable table showing company name, contact information, address, and other details.

### Filtering

- **Text Filter**: Type a search term in the filter box and click **Filter** (or press Enter) to search for customers by name or other fields. Click **Clear** to reset the filter.
- **Outstanding Balance**: Check this box to show only customers who have unpaid (unshipped) orders. This is useful for identifying customers with open balances that may need follow-up.

### Viewing Details

Click any row in the customer table to view the full customer record with all fields displayed.

---

## Order List

The Order List shows all orders with ship name, customer ID, dates, freight charges, and shipping details.

### Filtering

- **Text Filter**: Type a search term to filter orders by ship name or other fields.
- **Unpaid Orders**: Check this box to show only orders that have not yet been shipped. Unshipped orders are considered unpaid/outstanding.

### Understanding Order Status

- An order with a **Shipped Date** is considered fulfilled and paid.
- An order without a **Shipped Date** is considered open/unpaid and will appear in the Dashboard's unpaid revenue and aging calculations.

### Viewing Details

Click any row to view the complete order record.

---

## Product List

The Product List displays your product catalog including product name, category, supplier, pricing, and inventory levels.

### Filtering

- **Text Filter**: Type a search term to filter products by name or other fields.
- **Pending Receipt**: Check this box to show only products that have units on order from suppliers but not yet received into inventory. These are your outstanding purchase orders.

### Understanding Inventory Fields

| Field | Description |
|-------|-------------|
| **Unit Price** | Current selling price per unit |
| **In Stock** | Units currently available in inventory |
| **On Order** | Units ordered from suppliers, awaiting delivery |
| **Reorder Level** | Minimum stock level that triggers a reorder |
| **Discontinued** | Whether the product is no longer being sold |

Products with **On Order > 0** have purchase orders outstanding with suppliers. Until the goods are received and **In Stock** is updated, they appear in the "Outstanding POs" count on the Dashboard and are shown when the **Pending Receipt** filter is active.

### Viewing Details

Click any row to view the complete product record.

---

## Settings

The Settings page lets you configure application preferences.

### API Configuration

| Setting | Description |
|---------|-------------|
| **API Endpoint** | The URL of the ApiLogicServer middleware (default: `http://localhost:5659/api`). Change this if your API server runs on a different host or port. |
| **Date Format** | The date display format used throughout the application (default: `YYYY-MM-DD`). |
| **Currency Symbol** | The currency symbol shown alongside monetary values (default: `$`). |

### Display

| Setting | Description |
|---------|-------------|
| **Theme** | The current visual theme (light). |
| **Locale** | The application locale for language and formatting (default: `en`). |

Click **Save Settings** to apply your changes. Note that API Endpoint changes take effect immediately for subsequent API calls within your session.

---

## Tips

- **Refresh data**: Each list page loads data when you navigate to it. Use the Dashboard's Refresh button to update summary statistics.
- **Combine filters**: You can use the text filter and checkbox filter together. For example, type a customer name and check "Outstanding Balance" to find a specific customer with unpaid orders.
- **Maximize screen space**: Collapse the sidebar using the hamburger menu when working with wide tables.
- **Multiple ALS servers**: If you run multiple ApiLogicServer instances, update the API Endpoint in Settings to point to the correct server (default port: 5659).
