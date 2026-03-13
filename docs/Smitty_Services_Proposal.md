# Smitty Services
## Business Management Solution Proposal

**Prepared by:** Imagery Business Systems, LLC
**Date:** March 2026

---

# About Imagery Business Systems

Imagery Business Systems, LLC delivers custom business management software
that replaces spreadsheets, paper forms, and disconnected tools with a
single, modern web application tailored to your operations.

---

# The Challenge

Running a service-oriented business means juggling customers, orders,
inventory, vehicle repairs, parts procurement, and financials across
disconnected systems. This leads to:

- Lost or duplicate customer records
- Orders and invoices tracked in spreadsheets with no aging visibility
- Inventory surprises: stockouts, over-ordering, missed reorder points
- Service jobs tracked on whiteboards or paper with no cost visibility
- No single source of truth for business performance

---

# The Solution: Smitty Services

Smitty Services is a modern, web-based business management platform that
unifies your operations into one clean, easy-to-use application.

Access it from any device with a browser. No software to install on
workstations. Real-time data. One source of truth.

---

# Key Capabilities

## 1. Real-Time Dashboard
- Total customers, orders, products at a glance
- Unpaid revenue tracking
- Order aging analysis (7-day, 30-day, 90-day buckets)
- Pending inventory receipts
- One-click refresh for live data

## 2. Customer Management
- Centralized customer database with contact info and addresses
- Full order history per customer
- Outstanding balance filtering
- Search across all customer fields

## 3. Order & Invoice Management
- Create, edit, and track sales orders
- Line-item detail with products, quantities, pricing, and discounts
- Unpaid invoice filtering for accounts receivable
- Customer and employee name lookups (no memorizing IDs)

## 4. Product & Inventory Management
- Complete product catalog with category and supplier tracking
- Stock level monitoring: units in stock, on order, reorder level
- Pending receipt tracking for incoming inventory
- Filter by category or supplier
- Discontinued product flagging

## 5. Service Center Operations
- **Job Workflow:** New -> In Progress -> Waiting Parts -> Road Test -> Complete
- Track estimated vs. actual costs per job
- Link jobs to specific vehicles and customers
- Vehicle database with VIN, make, model, year, license plate

## 6. Purchase Order Management
- **PO Workflow:** New -> Ordered -> Partial -> Received
- Supplier-linked purchase orders for parts and materials
- Auto-calculated totals from line items
- Link POs directly to service jobs for cost tracking
- Open PO filtering to monitor outstanding orders

## 7. Reporting & Documents
- Invoice and purchase order PDF generation
- Customer billing statements
- Revenue analysis reports

## 8. Configurable Settings
- Customizable date format, currency symbol, and locale
- API endpoint configuration for backend flexibility
- Changes take effect immediately, no restart required

---

# Value Proposition

| Benefit | Impact |
|---------|--------|
| **Single Source of Truth** | Eliminate duplicate data entry and conflicting spreadsheets |
| **Real-Time Visibility** | Dashboard shows business health at a glance -- unpaid invoices, aging orders, inventory status |
| **Faster Service Operations** | Job status workflow keeps technicians and managers aligned; parts ordering linked directly to jobs |
| **Inventory Control** | Reorder-level alerts and pending receipt tracking prevent stockouts and over-ordering |
| **Accounts Receivable** | Order aging analysis and unpaid invoice filters improve cash flow management |
| **Browser-Based Access** | No desktop software to install or maintain; works on any device |
| **Extensible Architecture** | New entities, reports, and workflows can be added as your business grows |
| **Professional Documents** | Generate invoices, POs, and statements as PDFs directly from the system |
| **On-Premise Infrastructure** | TNAS server keeps data on your network -- no cloud hosting fees, no internet dependency for daily operations |
| **Built-In File Server** | Replace USB drives and email attachments with organized, permissioned network shares |
| **Automated Backups** | Nightly database snapshots, file versioning, and weekly full-system backup to external drive -- disaster recovery without the worry |

---

# Why Smitty Services?

**Built for your business, not a one-size-fits-all SaaS product.**

- Tailored to your specific workflows and terminology
- Your data stays under your control -- hosted on your infrastructure
- No per-user licensing fees or surprise price increases
- Direct relationship with the development team for support and enhancements
- Modern, responsive interface your team will actually want to use

---

# Infrastructure: TerraMaster NAS Server

To give Smitty a reliable, always-on platform for the application, file
sharing, and data protection, we recommend deploying on a TerraMaster
F4-424 Pro NAS server installed on-premise at your location.

## Why a TNAS?

- **All-in-one server:** Runs Smitty Services in Docker containers,
  serves as your office file server, and handles automated backups --
  one device, three critical roles
- **No cloud dependency:** Your business data stays on your network,
  under your control, with no recurring cloud hosting fees
- **Enterprise-grade reliability:** Intel i3-N305 8-core CPU, 32GB DDR5
  RAM, dual 2.5GbE networking -- far more power than needed, leaving
  room to grow
- **Docker Compose native:** TOS (TerraMaster OS) includes a built-in
  Docker manager, purpose-built for running containerized applications
  like Smitty Services
- **Energy efficient:** Low-power design runs 24/7 at a fraction of the
  cost of a traditional server

## Deployment Architecture

```
 TerraMaster F4-424 Pro
 +-------------------------------------------------+
 |                                                 |
 |  Docker Compose Platform                        |
 |  +-------------------------------------------+  |
 |  | PostgreSQL Database      (Tier 1)         |  |
 |  | ApiLogicServer Backend   (Tier 2)         |  |
 |  | Smitty Services Frontend (Tier 3)         |  |
 |  +-------------------------------------------+  |
 |                                                 |
 |  File Server (SMB/CIFS Shares)                  |
 |  +-------------------------------------------+  |
 |  | Shared Documents & Business Files         |  |
 |  | User Home Folders                         |  |
 |  +-------------------------------------------+  |
 |                                                 |
 |  Automated Backup                               |
 |  +-------------------------------------------+  |
 |  | Nightly database snapshots                |  |
 |  | File server versioned backups             |  |
 |  | Optional off-site replication (USB/Cloud) |  |
 |  +-------------------------------------------+  |
 |                                                 |
 |  Storage: RAID 1 Mirror (2x 4TB WD Red Plus)    |
 |  Capacity: 4TB usable, fully redundant          |
 +-------------------------------------------------+
         |                    |
     2.5GbE LAN          2.5GbE LAN
         |                    |
    Office Network     (failover/link agg)
```

## TNAS Specifications

| Component | Detail |
|-----------|--------|
| **Model** | TerraMaster F4-424 Pro |
| **CPU** | Intel Core i3-N305, 8-core / 8-thread, up to 3.8 GHz |
| **RAM** | 32 GB DDR5 4800 MHz |
| **Drive Bays** | 4x 3.5" SATA + 2x M.2 NVMe (SSD cache) |
| **Network** | 2x 2.5 Gigabit Ethernet |
| **OS** | TOS (TerraMaster OS) with Docker Manager |
| **Form Factor** | Compact desktop tower |

## Storage Plan

| Drive | Purpose | Configuration |
|-------|---------|---------------|
| **Bay 1:** WD Red Plus 4TB | Primary data | RAID 1 (mirror) |
| **Bay 2:** WD Red Plus 4TB | Mirror copy | RAID 1 (mirror) |
| **Bay 3-4:** | Available for future expansion | -- |

RAID 1 mirroring means every byte is written to both drives
simultaneously. If either drive fails, the system continues running
on the surviving drive with zero data loss.

**Usable capacity:** 4 TB (documents, database, backups)

## File Server

The TNAS replaces any existing file-sharing workarounds (USB drives,
email attachments, local folders) with a proper network file server:

- **Shared folders** accessible from any PC on the network (Windows,
  Mac, Linux)
- **User-level permissions** to control who can read, write, or
  administer each folder
- **Recycle bin** protection against accidental deletion
- **Access from anywhere** via TerraMaster's secure remote access
  (TNAS.online) when needed

## Backup Plan

| What | How | When |
|------|-----|------|
| **Smitty Services Database** | Automated PostgreSQL dump to local backup folder | Nightly at 2:00 AM |
| **Docker Volumes** | Snapshot of all container data | Nightly at 2:30 AM |
| **Shared Files** | Versioned backup with 7-day retention | Nightly at 3:00 AM |
| **Full System Backup** | Complete TNAS backup to external USB drive | Weekly (Sunday 1:00 AM) |
| **Off-Site Copy (optional)** | Encrypted replication to cloud storage or second location | Configurable |

- Database backups are retained for 30 days (rolling)
- File versioning lets you recover accidentally deleted or overwritten
  files from the past 7 days
- External USB drive provides disaster recovery in case of hardware
  failure, theft, or fire
- All backups run automatically with no manual intervention required

---

# Pricing Schedule

## Hardware Investment (One-Time)

| Item | Est. Cost |
|------|-----------|
| **TerraMaster F4-424 Pro** (4-bay, diskless) | $700.00 |
| **WD Red Plus 4TB NAS Drive** x 2 (RAID 1) | $200.00 |
| **Ethernet Cable / Accessories** | $25.00 |
| | |
| **Total Hardware** | **$925.00** |

*Hardware is purchased directly by the client. Imagery Business Systems
will specify exact models and can assist with ordering. Prices are
estimated and subject to current retail availability.*

---

## First-Year Software & Services: $3,000.00

| Item | Amount | When Due |
|------|--------|----------|
| **Project Deposit** | $500.00 | Upon signed agreement |
| **Installation & Configuration** | $300.00 | At deployment |
| **Software Subscription** (Month 1) | $183.33 | 30 days after install |
| **Software Subscription** (Month 2) | $183.33 | 60 days after install |
| **Software Subscription** (Month 3) | $183.33 | 90 days after install |
| **Software Subscription** (Month 4) | $183.33 | 120 days after install |
| **Software Subscription** (Month 5) | $183.33 | 150 days after install |
| **Software Subscription** (Month 6) | $183.33 | 180 days after install |
| **Software Subscription** (Month 7) | $183.33 | 210 days after install |
| **Software Subscription** (Month 8) | $183.33 | 240 days after install |
| **Software Subscription** (Month 9) | $183.33 | 270 days after install |
| **Software Subscription** (Month 10) | $183.33 | 300 days after install |
| **Software Subscription** (Month 11) | $183.33 | 330 days after install |
| **Software Subscription** (Month 12) | $183.37 | 360 days after install |
| | | |
| **Total First Year** | **$3,000.00** | |

*Month 12 adjusted to $183.37 to ensure exact $3,000.00 total.*

---

## What's Included

### Project Deposit ($500.00)
- Requirements review and confirmation
- Project scheduling and kickoff
- Development environment setup

### Installation & Configuration ($300.00)
- TNAS server hardware setup and RAID configuration
- Docker Compose deployment of all three application tiers
- Database setup and initial data migration
- File server shared folders and user permissions
- Automated backup schedule configuration
- Network integration and connectivity testing
- System testing and verification
- Hands-on training session for your team

### Monthly Software Subscription ($183.33/mo)
- Full access to all application features
- Bug fixes and security patches
- Email and phone support during business hours
- Minor enhancements and configuration adjustments
- Automated nightly database and file backups
- TNAS system health monitoring and alerts
- Backup verification and recovery testing (quarterly)

---

## Total First-Year Investment Summary

| Category | Amount |
|----------|--------|
| Hardware (TNAS + Drives, purchased by client) | $925.00 |
| Software & Services (deposit + install + 12 months) | $3,000.00 |
| **Total First-Year Investment** | **$3,925.00** |

*After year one, ongoing cost is the monthly subscription only --
no hardware costs, no per-user fees, no cloud hosting bills.*

---

## Renewal Terms

After the initial 12-month term, the software subscription renews at the
then-current monthly rate. No long-term lock-in -- cancel with 30 days
written notice.

---

# Implementation Timeline

| Phase | Duration | Activities |
|-------|----------|------------|
| **Kickoff** | Week 1 | Requirements review, project plan, deposit due, hardware ordered |
| **Hardware Setup** | Week 2 | TNAS unboxing, drive install, RAID config, TOS setup |
| **Server Config** | Week 2-3 | Docker Compose deployment, file shares, backup schedules |
| **Data Migration** | Week 3 | Database setup, initial data import, install fee due |
| **Testing** | Week 3-4 | End-to-end system testing, backup/restore verification |
| **Training** | Week 4 | User training: application, file server, backup recovery |
| **Go Live** | Week 5 | Production launch, subscription billing begins |

---

# Next Steps

1. Review this proposal
2. Schedule a live demonstration
3. Sign the service agreement
4. Submit the $500.00 project deposit
5. We begin!

---

# Contact

**Imagery Business Systems, LLC**

We look forward to partnering with you to streamline your business operations
with Smitty Services.

---

*This proposal is valid for 30 days from the date above.*
