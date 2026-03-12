# Smitty Services - Service Center Guide

Smitty Services is a local business that repairs and services commercial vehicles. This guide documents the service center data model, workflows, and application features.

## Overview

The service center manages a small fleet of active jobs at any given time (typically 3-10 vehicles being worked on simultaneously). The application tracks:

- **Customers** who own vehicles and request service
- **Vehicles** registered to customers with VIN tracking
- **Jobs** representing individual service/repair engagements
- **POs** (Purchase Orders) for parts and supplies from suppliers, linked to jobs

## Data Model

### Vehicle

Represents a commercial vehicle registered in the system.

| Field | Type | Description |
|-------|------|-------------|
| vehicle_id | SMALLINT PK | Auto-generated identifier |
| customer_id | VARCHAR(10) FK | Owner (references Customer) |
| vin | VARCHAR(17) UNIQUE | Vehicle Identification Number |
| description | VARCHAR(200) | Free-text description (e.g. "2019 Ford F-350 White Flatbed") |
| year | SMALLINT | Model year |
| make | VARCHAR(60) | Manufacturer (e.g. Ford, Freightliner, Peterbilt) |
| model | VARCHAR(60) | Model name (e.g. F-350, Cascadia) |
| license_plate | VARCHAR(20) | Current license plate |
| notes | TEXT | Additional notes |

### Job

Represents a service or repair engagement for a specific vehicle.

| Field | Type | Description |
|-------|------|-------------|
| job_id | SMALLINT PK | Auto-generated identifier |
| customer_id | VARCHAR(10) FK | Customer requesting service |
| vehicle_id | SMALLINT FK | Vehicle being serviced |
| service_description | TEXT | Description of work to be performed |
| status | VARCHAR(30) | Current job status (see workflow below) |
| created_date | DATE | Date job was created (defaults to today) |
| started_date | DATE | Date work began |
| completed_date | DATE | Date work was finished |
| estimated_cost | REAL | Estimated cost quoted to customer |
| actual_cost | REAL | Final actual cost |
| notes | TEXT | Internal notes about the job |

### Purchase (displayed as "PO" in the UI)

Represents a purchase order placed with a supplier for parts and supplies.

| Field | Type | Description |
|-------|------|-------------|
| purchase_id | SMALLINT PK | Auto-generated identifier |
| supplier_id | SMALLINT FK | Supplier (references Supplier) |
| purchase_date | DATE | Date purchase was created (defaults to today) |
| expected_date | DATE | Expected delivery date |
| received_date | DATE | Actual date parts were received |
| status | VARCHAR(30) | Purchase status (see below) |
| total_cost | REAL | Total cost (auto-calculated from items) |
| notes | TEXT | Additional notes |

**Purchase Statuses:** New, Ordered, Partial, Received, Cancelled

### Purchase Item (purchase_item)

Line items on a purchase order. Each item references a Product.

| Field | Type | Description |
|-------|------|-------------|
| purchase_id | SMALLINT FK | References Purchase |
| product_id | SMALLINT FK | References Product |
| unit_cost | REAL | Cost per unit from supplier |
| quantity | SMALLINT | Number of units ordered |

Composite primary key: (purchase_id, product_id)

### Job-Purchase Link (job_purchase)

Links jobs to purchase orders for parts needed for service work.

| Field | Type | Description |
|-------|------|-------------|
| job_id | SMALLINT FK | References Job |
| purchase_id | SMALLINT FK | References Purchase |

Composite primary key: (job_id, purchase_id)

## Job Status Workflow

Jobs progress through these statuses:

```
New --> In Progress --> Waiting Parts --> In Progress --> Road Test Pending --> Complete
```

| Status | Description |
|--------|-------------|
| **New** | Job created, work not yet started |
| **In Progress** | Technician actively working on the vehicle |
| **Waiting Parts** | Work paused pending parts delivery (linked to a PO) |
| **Road Test Pending** | Repair complete, needs road test verification |
| **Complete** | Job finished, vehicle ready for pickup |

### Status Transitions

- **New -> In Progress**: Technician begins work, `started_date` is set
- **In Progress -> Waiting Parts**: Parts needed; create a PO and link it to the job
- **Waiting Parts -> In Progress**: Parts received, work resumes
- **In Progress -> Road Test Pending**: Repair work finished, road test scheduled
- **Road Test Pending -> Complete**: Road test passed, `completed_date` is set
- **Road Test Pending -> In Progress**: Road test failed, additional work needed

## Application Features

### Sidebar Navigation

The sidebar includes three entries in the Service Center section:

- **Jobs** - View and manage service jobs
- **Vehicles** - View and manage registered vehicles
- **POs** - View and manage supplier purchase orders

### Vehicle List Page

- **Owner filter dropdown**: Filter vehicles by customer/owner
- **New Vehicle button**: Opens dialog to register a new vehicle
- **Owner column**: Resolves `customer_id` to company name via JSONAPI `?include=customer`
- **Click-through**: Click a vehicle row to see full detail with Edit/Delete

### Vehicle Detail Page

- Full vehicle information display
- **Edit**: Opens edit dialog with Owner dropdown (customer lookup)
- **Delete**: Confirmation dialog, removes vehicle record

### Job List Page

- **Status filter dropdown**: Filter by job status (New, In Progress, Waiting Parts, Road Test Pending, Complete)
- **Customer filter dropdown**: Filter by customer
- **Active Only checkbox**: Checked by default, hides completed jobs
- **New Job button**: Opens the Add Job dialog
- **Customer/Vehicle columns**: Resolve FK IDs to names via JSONAPI includes
- **Click-through**: Click a job row to see full detail with Edit/Delete

### Add Job Dialog

The Add Job dialog includes:

1. **Customer dropdown** - Select the customer
2. **Vehicle dropdown** - Filtered to show only vehicles owned by the selected customer
3. **Service Description** - Free-text description of work needed
4. **Status dropdown** - Defaults to "New"
5. **Estimated Cost** - Quote amount
6. **Notes** - Internal notes
7. **Linked POs section** - Link existing purchase orders to the job
   - "+ Link PO" button adds a row with a PO dropdown
   - Dropdown shows supplier name + PO ID + status
   - Creates `job_purchase` records on save

### Job Detail Page

- Full job information display
- **Edit**: Opens edit dialog with Customer/Vehicle/Status dropdowns
- **Delete**: Confirmation dialog, removes job record

### PO List Page

- **Status filter dropdown**: Filter by PO status (New, Ordered, Partial, Received, Cancelled)
- **Supplier filter dropdown**: Filter by supplier
- **Open Only checkbox**: Checked by default, hides Received and Cancelled POs
- **New PO button**: Opens the Add PO dialog
- **Supplier column**: Resolves `supplier_id` to company name via JSONAPI `?include=supplier`
- **Click-through**: Click a PO row to see full detail with Edit/Delete

### Add PO Dialog

The Add PO dialog includes:

1. **Supplier dropdown** - Select the parts supplier
2. **PO Date** - Date the order is placed
3. **Expected Date** - When parts are expected to arrive
4. **Status dropdown** - Defaults to "New"
5. **Notes** - Additional notes
6. **PO Items section** - Add line items for parts being ordered
   - Category/Supplier filter dropdowns to narrow the product list
   - "+ Add Item" button adds a row: Product dropdown, Unit Cost (auto-fills from product), Quantity, Remove
   - Total cost is auto-calculated from items on save
   - Creates `purchase_item` records after saving the PO

### PO Detail Page

- Full PO information display
- **Edit**: Opens edit dialog with Supplier/Status dropdowns
- **Delete**: Confirmation dialog, removes PO record

## Database Setup

The schema additions are applied via patch files:

```bash
# Apply in order:
psql -U smitty -d smitty_services -f database/PATCH_FILE_001.sql  # Vehicle, Job
psql -U smitty -d smitty_services -f database/PATCH_FILE_002.sql  # Purchase, PurchaseItem, JobPurchase
```

Or run the full schema (includes everything):

```bash
psql -U smitty -d smitty_services -f database/schema.sql
```

## API Endpoints (JSONAPI via ALS)

Once the tables are created and ALS is configured, the following endpoints become available:

| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | /api/Vehicle | List all vehicles |
| GET | /api/Vehicle/{id} | Get vehicle detail |
| GET | /api/Vehicle?include=customer | List vehicles with owner info |
| POST | /api/Vehicle | Create a new vehicle |
| PATCH | /api/Vehicle/{id} | Update a vehicle |
| DELETE | /api/Vehicle/{id} | Delete a vehicle |
| GET | /api/Job | List all jobs |
| GET | /api/Job/{id} | Get job detail |
| GET | /api/Job?include=customer,vehicle | List jobs with related info |
| POST | /api/Job | Create a new job |
| PATCH | /api/Job/{id} | Update a job (status change, costs, etc.) |
| DELETE | /api/Job/{id} | Delete a job |
| GET | /api/Purchase | List all POs |
| GET | /api/Purchase/{id} | Get PO detail |
| GET | /api/Purchase?include=supplier | List POs with supplier info |
| POST | /api/Purchase | Create a new PO |
| PATCH | /api/Purchase/{id} | Update a PO (status, dates, etc.) |
| DELETE | /api/Purchase/{id} | Delete a PO |
| POST | /api/PurchaseItem | Add a line item to a PO |
| POST | /api/JobPurchase | Link a PO to a job |
| DELETE | /api/JobPurchase/{id} | Unlink a PO from a job |

## Typical Workflow

1. **Customer arrives** with a vehicle needing service
2. **Register the vehicle** (if new) via Vehicles > New Vehicle
3. **Create a job** via Jobs > New Job, selecting the customer and their vehicle
4. **Describe the service** needed and provide an estimated cost
5. **Begin work** - update status to "In Progress"
6. **Order parts** if needed - create a PO via POs > New PO, selecting the supplier and adding product line items
7. **Link the PO** to the job (via the Add Job dialog or by editing the job)
8. **Update job status** to "Waiting Parts" while waiting for delivery
9. **Parts arrive** - update PO status to "Received", update job status back to "In Progress"
10. **Complete repair** - update job status to "Road Test Pending"
11. **Road test passes** - update job status to "Complete", set actual cost
12. **Customer picks up** vehicle
