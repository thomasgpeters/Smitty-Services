--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;



SET default_tablespace = '';

SET default_with_oids = false;


---
--- drop tables (Smitty service tables first, then Northwind base tables)
---

-- Smitty service center tables (child → parent order)
DROP TABLE IF EXISTS job_purchases;
DROP TABLE IF EXISTS purchase_items;
DROP TABLE IF EXISTS purchases;
DROP TABLE IF EXISTS jobs;
DROP TABLE IF EXISTS vehicles;

-- Northwind base tables
DROP TABLE IF EXISTS customer_customer_demo;
DROP TABLE IF EXISTS customer_demographics;
DROP TABLE IF EXISTS employee_territories;
DROP TABLE IF EXISTS order_details;
DROP TABLE IF EXISTS orders;
DROP TABLE IF EXISTS customers;
DROP TABLE IF EXISTS products;
DROP TABLE IF EXISTS shippers;
DROP TABLE IF EXISTS suppliers;
DROP TABLE IF EXISTS territories;
DROP TABLE IF EXISTS us_states;
DROP TABLE IF EXISTS categories;
DROP TABLE IF EXISTS region;
DROP TABLE IF EXISTS employees;

--
-- Name: categories; Type: TABLE; Schema: public; Owner: -; Tablespace: 
--

CREATE TABLE categories (
    category_id smallint NOT NULL,
    category_name character varying(15) NOT NULL,
    description text,
    picture bytea
);


--
-- Name: customer_customer_demo; Type: TABLE; Schema: public; Owner: -; Tablespace: 
--

CREATE TABLE customer_customer_demo (
    customer_id character varying(5) NOT NULL,
    customer_type_id character varying(5) NOT NULL
);


--
-- Name: customer_demographics; Type: TABLE; Schema: public; Owner: -; Tablespace: 
--

CREATE TABLE customer_demographics (
    customer_type_id character varying(5) NOT NULL,
    customer_desc text
);


--
-- Name: customers; Type: TABLE; Schema: public; Owner: -; Tablespace: 
--

CREATE TABLE customers (
    customer_id character varying(5) NOT NULL,
    company_name character varying(40) NOT NULL,
    contact_name character varying(30),
    contact_title character varying(30),
    address character varying(60),
    city character varying(15),
    region character varying(15),
    postal_code character varying(10),
    country character varying(15),
    phone character varying(24),
    fax character varying(24)
);


--
-- Name: employees; Type: TABLE; Schema: public; Owner: -; Tablespace: 
--

CREATE TABLE employees (
    employee_id smallint NOT NULL,
    last_name character varying(20) NOT NULL,
    first_name character varying(10) NOT NULL,
    title character varying(30),
    title_of_courtesy character varying(25),
    birth_date date,
    hire_date date,
    address character varying(60),
    city character varying(15),
    region character varying(15),
    postal_code character varying(10),
    country character varying(15),
    home_phone character varying(24),
    extension character varying(4),
    photo bytea,
    notes text,
    reports_to smallint,
    photo_path character varying(255)
);


--
-- Name: employee_territories; Type: TABLE; Schema: public; Owner: -; Tablespace: 
--

CREATE TABLE employee_territories (
    employee_id smallint NOT NULL,
    territory_id character varying(20) NOT NULL
);




--
-- Name: order_details; Type: TABLE; Schema: public; Owner: -; Tablespace: 
--

CREATE TABLE order_details (
    order_id smallint NOT NULL,
    product_id smallint NOT NULL,
    unit_price real NOT NULL,
    quantity smallint NOT NULL,
    discount real NOT NULL
);


--
-- Name: orders; Type: TABLE; Schema: public; Owner: -; Tablespace: 
--

CREATE TABLE orders (
    order_id smallint NOT NULL,
    customer_id character varying(5),
    employee_id smallint,
    order_date date,
    required_date date,
    shipped_date date,
    ship_via smallint,
    freight real,
    ship_name character varying(40),
    ship_address character varying(60),
    ship_city character varying(15),
    ship_region character varying(15),
    ship_postal_code character varying(10),
    ship_country character varying(15)
);


--
-- Name: products; Type: TABLE; Schema: public; Owner: -; Tablespace: 
--

CREATE TABLE products (
    product_id smallint NOT NULL,
    product_name character varying(40) NOT NULL,
    supplier_id smallint,
    category_id smallint,
    quantity_per_unit character varying(20),
    unit_price real,
    units_in_stock smallint,
    units_on_order smallint,
    reorder_level smallint,
    discontinued integer NOT NULL
);


--
-- Name: region; Type: TABLE; Schema: public; Owner: -; Tablespace: 
--

CREATE TABLE region (
    region_id smallint NOT NULL,
    region_description character varying(60) NOT NULL
);


--
-- Name: shippers; Type: TABLE; Schema: public; Owner: -; Tablespace: 
--

CREATE TABLE shippers (
    shipper_id smallint NOT NULL,
    company_name character varying(40) NOT NULL,
    phone character varying(24)
);



--
-- Name: suppliers; Type: TABLE; Schema: public; Owner: -; Tablespace: 
--

CREATE TABLE suppliers (
    supplier_id smallint NOT NULL,
    company_name character varying(40) NOT NULL,
    contact_name character varying(30),
    contact_title character varying(30),
    address character varying(60),
    city character varying(15),
    region character varying(15),
    postal_code character varying(10),
    country character varying(15),
    phone character varying(24),
    fax character varying(24),
    homepage text
);


--
-- Name: territories; Type: TABLE; Schema: public; Owner: -; Tablespace: 
--

CREATE TABLE territories (
    territory_id character varying(20) NOT NULL,
    territory_description character varying(60) NOT NULL,
    region_id smallint NOT NULL
);


--
-- Name: us_states; Type: TABLE; Schema: public; Owner: -; Tablespace: 
--

CREATE TABLE us_states (
    state_id smallint NOT NULL,
    state_name character varying(100),
    state_abbr character varying(2),
    state_region character varying(50)
);


-- ==============================================================================
-- Smitty Service Center Tables
-- ==============================================================================

--
-- Name: vehicles; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE vehicles (
    vehicle_id    SMALLINT     PRIMARY KEY GENERATED BY DEFAULT AS IDENTITY,
    customer_id   VARCHAR(10)  NOT NULL,
    vin           VARCHAR(17)  NOT NULL,
    description   VARCHAR(200) NOT NULL,
    year          SMALLINT,
    make          VARCHAR(60),
    model         VARCHAR(60),
    license_plate VARCHAR(20),
    notes         TEXT,
    UNIQUE(vin)
);


--
-- Name: jobs; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE jobs (
    job_id              SMALLINT     PRIMARY KEY GENERATED BY DEFAULT AS IDENTITY,
    customer_id         VARCHAR(10)  NOT NULL,
    vehicle_id          SMALLINT     NOT NULL,
    service_description TEXT         NOT NULL,
    status              VARCHAR(30)  NOT NULL DEFAULT 'New',
    created_date        DATE         NOT NULL DEFAULT CURRENT_DATE,
    started_date        DATE,
    completed_date      DATE,
    estimated_cost      REAL,
    actual_cost         REAL,
    notes               TEXT
);


--
-- Name: purchases; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE purchases (
    purchase_id    SMALLINT     PRIMARY KEY GENERATED BY DEFAULT AS IDENTITY,
    supplier_id    SMALLINT     NOT NULL,
    purchase_date  DATE         NOT NULL DEFAULT CURRENT_DATE,
    expected_date  DATE,
    received_date  DATE,
    status         VARCHAR(30)  NOT NULL DEFAULT 'New',
    total_cost     REAL,
    notes          TEXT
);


--
-- Name: purchase_items; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE purchase_items (
    purchase_id SMALLINT NOT NULL,
    product_id  SMALLINT NOT NULL,
    unit_cost   REAL     NOT NULL,
    quantity    SMALLINT NOT NULL,
    PRIMARY KEY (purchase_id, product_id)
);


--
-- Name: job_purchases; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE job_purchases (
    job_id      SMALLINT NOT NULL,
    purchase_id SMALLINT NOT NULL,
    PRIMARY KEY (job_id, purchase_id)
);


-- ==============================================================================
-- Constraints
-- ==============================================================================

--
-- Name: pk_categories; Type: CONSTRAINT; Schema: public; Owner: -; Tablespace:
--

ALTER TABLE ONLY categories
    ADD CONSTRAINT pk_categories PRIMARY KEY (category_id);


--
-- Name: pk_customer_customer_demo; Type: CONSTRAINT; Schema: public; Owner: -; Tablespace: 
--

ALTER TABLE ONLY customer_customer_demo
    ADD CONSTRAINT pk_customer_customer_demo PRIMARY KEY (customer_id, customer_type_id);


--
-- Name: pk_customer_demographics; Type: CONSTRAINT; Schema: public; Owner: -; Tablespace: 
--

ALTER TABLE ONLY customer_demographics
    ADD CONSTRAINT pk_customer_demographics PRIMARY KEY (customer_type_id);


--
-- Name: pk_customers; Type: CONSTRAINT; Schema: public; Owner: -; Tablespace: 
--

ALTER TABLE ONLY customers
    ADD CONSTRAINT pk_customers PRIMARY KEY (customer_id);


--
-- Name: pk_employees; Type: CONSTRAINT; Schema: public; Owner: -; Tablespace: 
--

ALTER TABLE ONLY employees
    ADD CONSTRAINT pk_employees PRIMARY KEY (employee_id);


--
-- Name: pk_employee_territories; Type: CONSTRAINT; Schema: public; Owner: -; Tablespace: 
--

ALTER TABLE ONLY employee_territories
    ADD CONSTRAINT pk_employee_territories PRIMARY KEY (employee_id, territory_id);


--
-- Name: pk_order_details; Type: CONSTRAINT; Schema: public; Owner: -; Tablespace: 
--

ALTER TABLE ONLY order_details
    ADD CONSTRAINT pk_order_details PRIMARY KEY (order_id, product_id);


--
-- Name: pk_orders; Type: CONSTRAINT; Schema: public; Owner: -; Tablespace: 
--

ALTER TABLE ONLY orders
    ADD CONSTRAINT pk_orders PRIMARY KEY (order_id);


--
-- Name: pk_products; Type: CONSTRAINT; Schema: public; Owner: -; Tablespace: 
--

ALTER TABLE ONLY products
    ADD CONSTRAINT pk_products PRIMARY KEY (product_id);


--
-- Name: pk_region; Type: CONSTRAINT; Schema: public; Owner: -; Tablespace: 
--

ALTER TABLE ONLY region
    ADD CONSTRAINT pk_region PRIMARY KEY (region_id);


--
-- Name: pk_shippers; Type: CONSTRAINT; Schema: public; Owner: -; Tablespace: 
--

ALTER TABLE ONLY shippers
    ADD CONSTRAINT pk_shippers PRIMARY KEY (shipper_id);


--
-- Name: pk_suppliers; Type: CONSTRAINT; Schema: public; Owner: -; Tablespace: 
--

ALTER TABLE ONLY suppliers
    ADD CONSTRAINT pk_suppliers PRIMARY KEY (supplier_id);


--
-- Name: pk_territories; Type: CONSTRAINT; Schema: public; Owner: -; Tablespace: 
--

ALTER TABLE ONLY territories
    ADD CONSTRAINT pk_territories PRIMARY KEY (territory_id);


--
-- Name: pk_usstates; Type: CONSTRAINT; Schema: public; Owner: -; Tablespace: 
--

ALTER TABLE ONLY us_states
    ADD CONSTRAINT pk_usstates PRIMARY KEY (state_id);


--
-- Name: fk_orders_customers; Type: Constraint; Schema: -; Owner: -
--

ALTER TABLE ONLY orders
    ADD CONSTRAINT fk_orders_customers FOREIGN KEY (customer_id) REFERENCES customers;


--
-- Name: fk_orders_employees; Type: Constraint; Schema: -; Owner: -
--

ALTER TABLE ONLY orders
    ADD CONSTRAINT fk_orders_employees FOREIGN KEY (employee_id) REFERENCES employees;


--
-- Name: fk_orders_shippers; Type: Constraint; Schema: -; Owner: -
--

ALTER TABLE ONLY orders
    ADD CONSTRAINT fk_orders_shippers FOREIGN KEY (ship_via) REFERENCES shippers;


--
-- Name: fk_order_details_products; Type: Constraint; Schema: -; Owner: -
--

ALTER TABLE ONLY order_details
    ADD CONSTRAINT fk_order_details_products FOREIGN KEY (product_id) REFERENCES products;


--
-- Name: fk_order_details_orders; Type: Constraint; Schema: -; Owner: -
--

ALTER TABLE ONLY order_details
    ADD CONSTRAINT fk_order_details_orders FOREIGN KEY (order_id) REFERENCES orders;


--
-- Name: fk_products_categories; Type: Constraint; Schema: -; Owner: -
--

ALTER TABLE ONLY products
    ADD CONSTRAINT fk_products_categories FOREIGN KEY (category_id) REFERENCES categories;


--
-- Name: fk_products_suppliers; Type: Constraint; Schema: -; Owner: -
--

ALTER TABLE ONLY products
    ADD CONSTRAINT fk_products_suppliers FOREIGN KEY (supplier_id) REFERENCES suppliers;


--
-- Name: fk_territories_region; Type: Constraint; Schema: -; Owner: -
--

ALTER TABLE ONLY territories
    ADD CONSTRAINT fk_territories_region FOREIGN KEY (region_id) REFERENCES region;


--
-- Name: fk_employee_territories_territories; Type: Constraint; Schema: -; Owner: -
--

ALTER TABLE ONLY employee_territories
    ADD CONSTRAINT fk_employee_territories_territories FOREIGN KEY (territory_id) REFERENCES territories;


--
-- Name: fk_employee_territories_employees; Type: Constraint; Schema: -; Owner: -
--

ALTER TABLE ONLY employee_territories
    ADD CONSTRAINT fk_employee_territories_employees FOREIGN KEY (employee_id) REFERENCES employees;


--
-- Name: fk_customer_customer_demo_customer_demographics; Type: Constraint; Schema: -; Owner: -
--

ALTER TABLE ONLY customer_customer_demo
    ADD CONSTRAINT fk_customer_customer_demo_customer_demographics FOREIGN KEY (customer_type_id) REFERENCES customer_demographics;


--
-- Name: fk_customer_customer_demo_customers; Type: Constraint; Schema: -; Owner: -
--

ALTER TABLE ONLY customer_customer_demo
    ADD CONSTRAINT fk_customer_customer_demo_customers FOREIGN KEY (customer_id) REFERENCES customers;


--
-- Name: fk_employees_employees; Type: Constraint; Schema: -; Owner: -
--

ALTER TABLE ONLY employees
    ADD CONSTRAINT fk_employees_employees FOREIGN KEY (reports_to) REFERENCES employees;


-- ==============================================================================
-- Smitty Service Center Foreign Keys
-- ==============================================================================

--
-- Name: fk_vehicles_customers; Type: Constraint; Schema: -; Owner: -
--

ALTER TABLE ONLY vehicles
    ADD CONSTRAINT fk_vehicles_customers FOREIGN KEY (customer_id) REFERENCES customers(customer_id);


--
-- Name: fk_jobs_customers; Type: Constraint; Schema: -; Owner: -
--

ALTER TABLE ONLY jobs
    ADD CONSTRAINT fk_jobs_customers FOREIGN KEY (customer_id) REFERENCES customers(customer_id);


--
-- Name: fk_jobs_vehicles; Type: Constraint; Schema: -; Owner: -
--

ALTER TABLE ONLY jobs
    ADD CONSTRAINT fk_jobs_vehicles FOREIGN KEY (vehicle_id) REFERENCES vehicles(vehicle_id);


--
-- Name: fk_purchases_suppliers; Type: Constraint; Schema: -; Owner: -
--

ALTER TABLE ONLY purchases
    ADD CONSTRAINT fk_purchases_suppliers FOREIGN KEY (supplier_id) REFERENCES suppliers(supplier_id);


--
-- Name: fk_purchase_items_purchases; Type: Constraint; Schema: -; Owner: -
--

ALTER TABLE ONLY purchase_items
    ADD CONSTRAINT fk_purchase_items_purchases FOREIGN KEY (purchase_id) REFERENCES purchases(purchase_id);


--
-- Name: fk_purchase_items_products; Type: Constraint; Schema: -; Owner: -
--

ALTER TABLE ONLY purchase_items
    ADD CONSTRAINT fk_purchase_items_products FOREIGN KEY (product_id) REFERENCES products(product_id);


--
-- Name: fk_job_purchases_jobs; Type: Constraint; Schema: -; Owner: -
--

ALTER TABLE ONLY job_purchases
    ADD CONSTRAINT fk_job_purchases_jobs FOREIGN KEY (job_id) REFERENCES jobs(job_id);


--
-- Name: fk_job_purchases_purchases; Type: Constraint; Schema: -; Owner: -
--

ALTER TABLE ONLY job_purchases
    ADD CONSTRAINT fk_job_purchases_purchases FOREIGN KEY (purchase_id) REFERENCES purchases(purchase_id);


-- ==============================================================================
-- Smitty Service Center Indexes
-- ==============================================================================

CREATE INDEX idx_vehicle_customer_id ON vehicles(customer_id);
CREATE INDEX idx_vehicle_vin         ON vehicles(vin);
CREATE INDEX idx_job_customer_id     ON jobs(customer_id);
CREATE INDEX idx_job_vehicle_id      ON jobs(vehicle_id);
CREATE INDEX idx_job_status          ON jobs(status);
CREATE INDEX idx_job_created_date    ON jobs(created_date);
CREATE INDEX idx_purchase_supplier_id  ON purchases(supplier_id);
CREATE INDEX idx_purchase_status       ON purchases(status);
CREATE INDEX idx_purchase_item_product ON purchase_items(product_id);


--
-- PostgreSQL database dump complete
--

