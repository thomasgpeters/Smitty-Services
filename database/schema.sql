-- ==============================================================================
-- Smitty Services — Full Database Schema
-- ==============================================================================
-- Northwind base tables + Smitty service center extensions.
-- Creates tables with primary keys only. Foreign keys are added by
-- seed_data.sql after all data has been loaded.
-- ==============================================================================

SET statement_timeout = 0;
SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

SET default_tablespace = '';
SET default_with_oids = false;


-- ==============================================================================
-- Drop all tables (child → parent order)
-- ==============================================================================

-- Smitty service center tables
DROP TABLE IF EXISTS job_purchases CASCADE;
DROP TABLE IF EXISTS purchase_items CASCADE;
DROP TABLE IF EXISTS purchases CASCADE;
DROP TABLE IF EXISTS jobs CASCADE;
DROP TABLE IF EXISTS vehicles CASCADE;

-- Northwind tables
DROP TABLE IF EXISTS customer_customer_demo CASCADE;
DROP TABLE IF EXISTS customer_demographics CASCADE;
DROP TABLE IF EXISTS employee_territories CASCADE;
DROP TABLE IF EXISTS order_details CASCADE;
DROP TABLE IF EXISTS orders CASCADE;
DROP TABLE IF EXISTS customers CASCADE;
DROP TABLE IF EXISTS products CASCADE;
DROP TABLE IF EXISTS shippers CASCADE;
DROP TABLE IF EXISTS suppliers CASCADE;
DROP TABLE IF EXISTS territories CASCADE;
DROP TABLE IF EXISTS us_states CASCADE;
DROP TABLE IF EXISTS categories CASCADE;
DROP TABLE IF EXISTS region CASCADE;
DROP TABLE IF EXISTS employees CASCADE;


-- ==============================================================================
-- Northwind Base Tables (PKs only — FKs added after seed data)
-- ==============================================================================

CREATE TABLE categories (
    category_id smallint PRIMARY KEY,
    category_name character varying(15) NOT NULL,
    description text,
    picture bytea
);

CREATE TABLE customer_demographics (
    customer_type_id character varying(5) PRIMARY KEY,
    customer_desc text
);

CREATE TABLE customers (
    customer_id character varying(5) PRIMARY KEY,
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

CREATE TABLE customer_customer_demo (
    customer_id character varying(5) NOT NULL,
    customer_type_id character varying(5) NOT NULL,
    PRIMARY KEY (customer_id, customer_type_id)
);

CREATE TABLE employees (
    employee_id smallint PRIMARY KEY,
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

CREATE TABLE region (
    region_id smallint PRIMARY KEY,
    region_description character varying(60) NOT NULL
);

CREATE TABLE territories (
    territory_id character varying(20) PRIMARY KEY,
    territory_description character varying(60) NOT NULL,
    region_id smallint NOT NULL
);

CREATE TABLE employee_territories (
    employee_id smallint NOT NULL,
    territory_id character varying(20) NOT NULL,
    PRIMARY KEY (employee_id, territory_id)
);

CREATE TABLE shippers (
    shipper_id smallint PRIMARY KEY,
    company_name character varying(40) NOT NULL,
    phone character varying(24)
);

CREATE TABLE suppliers (
    supplier_id smallint PRIMARY KEY,
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

CREATE TABLE products (
    product_id smallint PRIMARY KEY,
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

CREATE TABLE orders (
    order_id smallint PRIMARY KEY,
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

CREATE TABLE order_details (
    order_id smallint NOT NULL,
    product_id smallint NOT NULL,
    unit_price real NOT NULL,
    quantity smallint NOT NULL,
    discount real NOT NULL,
    PRIMARY KEY (order_id, product_id)
);

CREATE TABLE us_states (
    state_id smallint PRIMARY KEY,
    state_name character varying(100),
    state_abbr character varying(2),
    state_region character varying(50)
);


-- ==============================================================================
-- Smitty Service Center Tables (PKs only — FKs added after seed data)
-- ==============================================================================

CREATE TABLE vehicles (
    vehicle_id    SMALLINT     PRIMARY KEY GENERATED BY DEFAULT AS IDENTITY,
    customer_id   VARCHAR(10)  NOT NULL,
    vin           VARCHAR(17)  NOT NULL UNIQUE,
    description   VARCHAR(200) NOT NULL,
    year          SMALLINT,
    make          VARCHAR(60),
    model         VARCHAR(60),
    license_plate VARCHAR(20),
    notes         TEXT
);

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

CREATE TABLE purchase_items (
    purchase_id SMALLINT NOT NULL,
    product_id  SMALLINT NOT NULL,
    unit_cost   REAL     NOT NULL,
    quantity    SMALLINT NOT NULL,
    PRIMARY KEY (purchase_id, product_id)
);

CREATE TABLE job_purchases (
    job_id      SMALLINT NOT NULL,
    purchase_id SMALLINT NOT NULL,
    PRIMARY KEY (job_id, purchase_id)
);
