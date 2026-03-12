# Smitty Services - Service Center Guide

Smitty Services is a local business that repairs and services commercial vehicles. This guide documents the service center data model, workflows, and application features.

## Overview

The service center manages a small fleet of active jobs at any given time (typically 3-10 vehicles being worked on simultaneously). The application tracks:

- **Customers** who own vehicles and request service
- **Vehicles** registered to customers with VIN tracking
- **Jobs** representing individual service/repair engagements
- **Purchases** (orders) for parts and supplies linked to jobs

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

### Job-Purchase Link (job_purchase)

Links jobs to purchase orders for parts and supplies.

| Field | Type | Description |
|-------|------|-------------|
| job_id | SMALLINT FK | References Job |
| order_id | SMALLINT FK | References Order (purchase) |

Composite primary key: (job_id, order_id)

## Job Status Workflow

Jobs progress through these statuses:

```
New --> In Progress --> Waiting Parts --> In Progress --> Road Test Pending --> Complete
```

| Status | Description |
|--------|-------------|
| **New** | Job created, work not yet started |
| **In Progress** | Technician actively working on the vehicle |
| **Waiting Parts** | Work paused pending parts delivery (linked to a Purchase) |
| **Road Test Pending** | Repair complete, needs road test verification |
| **Complete** | Job finished, vehicle ready for pickup |

### Status Transitions

- **New -> In Progress**: Technician begins work, `started_date` is set
- **In Progress -> Waiting Parts**: Parts needed; create a Purchase Order and link it to the job
- **Waiting Parts -> In Progress**: Parts received, work resumes
- **In Progress -> Road Test Pending**: Repair work finished, road test scheduled
- **Road Test Pending -> Complete**: Road test passed, `completed_date` is set
- **Road Test Pending -> In Progress**: Road test failed, additional work needed

## Application Features

### Sidebar Navigation

The sidebar includes two new entries in the Service Center section:

- **Jobs** - View and manage service jobs
- **Vehicles** - View and manage registered vehicles

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
7. **Linked Purchases section** - Link existing purchase orders to the job
   - "+ Link Purchase" button adds a row with an order dropdown
   - Creates `job_purchase` records on save

### Job Detail Page

- Full job information display
- **Edit**: Opens edit dialog with Customer/Vehicle/Status dropdowns
- **Delete**: Confirmation dialog, removes job record

## Database Setup

The schema additions are in `database/schema.sql`. To add the new tables to an existing database:

```sql
-- Run the full schema (safe with IF NOT EXISTS)
psql -U smitty -d smitty_services -f database/schema.sql
```

Or apply just the new tables:

```sql
CREATE TABLE IF NOT EXISTS vehicle (
    vehicle_id    SMALLINT     PRIMARY KEY GENERATED BY DEFAULT AS IDENTITY,
    customer_id   VARCHAR(10)  NOT NULL REFERENCES customer(customer_id),
    vin           VARCHAR(17)  NOT NULL,
    description   VARCHAR(200) NOT NULL,
    year          SMALLINT,
    make          VARCHAR(60),
    model         VARCHAR(60),
    license_plate VARCHAR(20),
    notes         TEXT,
    UNIQUE(vin)
);

CREATE TABLE IF NOT EXISTS job (
    job_id             SMALLINT     PRIMARY KEY GENERATED BY DEFAULT AS IDENTITY,
    customer_id        VARCHAR(10)  NOT NULL REFERENCES customer(customer_id),
    vehicle_id         SMALLINT     NOT NULL REFERENCES vehicle(vehicle_id),
    service_description TEXT        NOT NULL,
    status             VARCHAR(30)  NOT NULL DEFAULT 'New',
    created_date       DATE         NOT NULL DEFAULT CURRENT_DATE,
    started_date       DATE,
    completed_date     DATE,
    estimated_cost     REAL,
    actual_cost        REAL,
    notes              TEXT
);

CREATE TABLE IF NOT EXISTS job_purchase (
    job_id   SMALLINT NOT NULL REFERENCES job(job_id),
    order_id SMALLINT NOT NULL REFERENCES "order"(order_id),
    PRIMARY KEY (job_id, order_id)
);
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
| POST | /api/JobPurchase | Link a purchase to a job |
| DELETE | /api/JobPurchase/{id} | Unlink a purchase from a job |

## Typical Workflow

1. **Customer arrives** with a vehicle needing service
2. **Register the vehicle** (if new) via Vehicles > New Vehicle
3. **Create a job** via Jobs > New Job, selecting the customer and their vehicle
4. **Describe the service** needed and provide an estimated cost
5. **Begin work** - update status to "In Progress"
6. **Order parts** if needed - create a Purchase Order via Orders > New Order, then link it to the job
7. **Update status** to "Waiting Parts" while waiting for delivery
8. **Parts arrive** - update status back to "In Progress"
9. **Complete repair** - update status to "Road Test Pending"
10. **Road test passes** - update status to "Complete", set actual cost
11. **Customer picks up** vehicle
