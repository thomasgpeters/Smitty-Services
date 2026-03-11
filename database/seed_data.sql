-- ==============================================================================
-- Smitty Services - Seed Data (PostgreSQL)
-- ==============================================================================
-- Northwind sample data for development and testing.
--
-- Usage:
--   psql -U smitty -d smitty_services -f seed_data.sql
--
-- Prerequisites: schema.sql must be run first.
-- ==============================================================================

BEGIN;

-- ------------------------------------------------------------------------------
-- Regions
-- ------------------------------------------------------------------------------
INSERT INTO region (region_id, region_description) VALUES
    (1, 'Eastern'),
    (2, 'Western'),
    (3, 'Northern'),
    (4, 'Southern')
ON CONFLICT DO NOTHING;

-- ------------------------------------------------------------------------------
-- Territories
-- ------------------------------------------------------------------------------
INSERT INTO territory (territory_id, territory_description, region_id) VALUES
    ('01581', 'Westboro', 1),
    ('01730', 'Bedford', 1),
    ('01833', 'Georgetown', 1),
    ('02116', 'Boston', 1),
    ('02139', 'Cambridge', 1),
    ('02184', 'Braintree', 1),
    ('02903', 'Providence', 1),
    ('03049', 'Hollis', 3),
    ('03801', 'Portsmouth', 3),
    ('06897', 'Wilton', 1),
    ('07960', 'Morristown', 1),
    ('08837', 'Edison', 1),
    ('10019', 'New York', 1),
    ('10038', 'New York', 1),
    ('11747', 'Mellville', 1),
    ('14450', 'Fairport', 1),
    ('19428', 'Philadelphia', 3),
    ('20852', 'Rockville', 1),
    ('27403', 'Greensboro', 1),
    ('27511', 'Cary', 1),
    ('29202', 'Columbia', 4),
    ('30346', 'Atlanta', 4),
    ('31406', 'Savannah', 4),
    ('32859', 'Orlando', 4),
    ('33607', 'Tampa', 4),
    ('40222', 'Louisville', 1),
    ('44122', 'Beachwood', 3),
    ('45839', 'Findlay', 3),
    ('48075', 'Southfield', 3),
    ('48084', 'Troy', 3),
    ('48304', 'Bloomfield Hills', 3),
    ('53404', 'Racine', 3),
    ('55113', 'Roseville', 3),
    ('55439', 'Minneapolis', 3),
    ('60179', 'Hoffman Estates', 2),
    ('60601', 'Chicago', 2),
    ('72716', 'Bentonville', 4),
    ('75234', 'Dallas', 4),
    ('78759', 'Austin', 4),
    ('80202', 'Denver', 2),
    ('80909', 'Colorado Springs', 2),
    ('85014', 'Phoenix', 2),
    ('85251', 'Scottsdale', 2),
    ('90405', 'Santa Monica', 2),
    ('94025', 'Menlo Park', 2),
    ('94105', 'San Francisco', 2),
    ('95008', 'Campbell', 2),
    ('95054', 'Santa Clara', 2),
    ('95060', 'Santa Cruz', 2),
    ('98004', 'Bellevue', 2),
    ('98052', 'Redmond', 2),
    ('98104', 'Seattle', 2)
ON CONFLICT DO NOTHING;

-- ------------------------------------------------------------------------------
-- US States
-- ------------------------------------------------------------------------------
INSERT INTO us_state (state_id, state_name, state_abbr, state_region) VALUES
    (1, 'Alabama', 'AL', 'south'),
    (2, 'Alaska', 'AK', 'north'),
    (3, 'Arizona', 'AZ', 'west'),
    (4, 'Arkansas', 'AR', 'south'),
    (5, 'California', 'CA', 'west'),
    (6, 'Colorado', 'CO', 'west'),
    (7, 'Connecticut', 'CT', 'east'),
    (8, 'Delaware', 'DE', 'east'),
    (9, 'District of Columbia', 'DC', 'east'),
    (10, 'Florida', 'FL', 'south'),
    (11, 'Georgia', 'GA', 'south'),
    (12, 'Hawaii', 'HI', 'west'),
    (13, 'Idaho', 'ID', 'west'),
    (14, 'Illinois', 'IL', 'midwest'),
    (15, 'Indiana', 'IN', 'midwest'),
    (16, 'Iowa', 'IA', 'midwest'),
    (17, 'Kansas', 'KS', 'midwest'),
    (18, 'Kentucky', 'KY', 'south'),
    (19, 'Louisiana', 'LA', 'south'),
    (20, 'Maine', 'ME', 'east'),
    (21, 'Maryland', 'MD', 'east'),
    (22, 'Massachusetts', 'MA', 'east'),
    (23, 'Michigan', 'MI', 'midwest'),
    (24, 'Minnesota', 'MN', 'midwest'),
    (25, 'Mississippi', 'MS', 'south'),
    (26, 'Missouri', 'MO', 'midwest'),
    (27, 'Montana', 'MT', 'west'),
    (28, 'Nebraska', 'NE', 'midwest'),
    (29, 'Nevada', 'NV', 'west'),
    (30, 'New Hampshire', 'NH', 'east'),
    (31, 'New Jersey', 'NJ', 'east'),
    (32, 'New Mexico', 'NM', 'west'),
    (33, 'New York', 'NY', 'east'),
    (34, 'North Carolina', 'NC', 'south'),
    (35, 'North Dakota', 'ND', 'midwest'),
    (36, 'Ohio', 'OH', 'midwest'),
    (37, 'Oklahoma', 'OK', 'south'),
    (38, 'Oregon', 'OR', 'west'),
    (39, 'Pennsylvania', 'PA', 'east'),
    (40, 'Rhode Island', 'RI', 'east'),
    (41, 'South Carolina', 'SC', 'south'),
    (42, 'South Dakota', 'SD', 'midwest'),
    (43, 'Tennessee', 'TN', 'south'),
    (44, 'Texas', 'TX', 'south'),
    (45, 'Utah', 'UT', 'west'),
    (46, 'Vermont', 'VT', 'east'),
    (47, 'Virginia', 'VA', 'south'),
    (48, 'Washington', 'WA', 'west'),
    (49, 'West Virginia', 'WV', 'south'),
    (50, 'Wisconsin', 'WI', 'midwest'),
    (51, 'Wyoming', 'WY', 'west')
ON CONFLICT DO NOTHING;

-- ------------------------------------------------------------------------------
-- Categories
-- ------------------------------------------------------------------------------
INSERT INTO category (category_id, category_name, description) VALUES
    (1, 'Beverages', 'Soft drinks, coffees, teas, beers, and ales'),
    (2, 'Condiments', 'Sweet and savory sauces, relishes, spreads, and seasonings'),
    (3, 'Confections', 'Desserts, candies, and sweet breads'),
    (4, 'Dairy Products', 'Cheeses'),
    (5, 'Grains/Cereals', 'Breads, crackers, pasta, and cereal'),
    (6, 'Meat/Poultry', 'Prepared meats'),
    (7, 'Produce', 'Dried fruit and bean curd'),
    (8, 'Seafood', 'Seaweed and fish')
ON CONFLICT DO NOTHING;

-- ------------------------------------------------------------------------------
-- Suppliers
-- ------------------------------------------------------------------------------
INSERT INTO supplier (supplier_id, company_name, contact_name, contact_title, address, city, region, postal_code, country, phone, fax, homepage) VALUES
    (1, 'Exotic Liquids', 'Charlotte Cooper', 'Purchasing Manager', '49 Gilbert St.', 'London', NULL, 'EC1 4SD', 'UK', '(171) 555-2222', NULL, NULL),
    (2, 'New Orleans Cajun Delights', 'Shelley Burke', 'Order Administrator', 'P.O. Box 78934', 'New Orleans', 'LA', '70117', 'USA', '(100) 555-4822', NULL, '#CAJUN.HTM#'),
    (3, 'Grandma Kelly''s Homestead', 'Regina Murphy', 'Sales Representative', '707 Oxford Rd.', 'Ann Arbor', 'MI', '48104', 'USA', '(313) 555-5735', '(313) 555-3349', NULL),
    (4, 'Tokyo Traders', 'Yoshi Nagase', 'Marketing Manager', '9-8 Sekimai Musashino-shi', 'Tokyo', NULL, '100', 'Japan', '(03) 3555-5011', NULL, NULL),
    (5, 'Cooperativa de Quesos ''Las Cabras''', 'Antonio del Valle Saavedra', 'Export Administrator', 'Calle del Rosal 4', 'Oviedo', 'Asturias', '33007', 'Spain', '(98) 598 76 54', NULL, NULL),
    (6, 'Mayumi''s', 'Mayumi Ohno', 'Marketing Representative', '92 Setsuko Chuo-ku', 'Osaka', NULL, '545', 'Japan', '(06) 431-7877', NULL, 'Mayumi''s (on the World Wide Web)#http://www.microsoft.com/accessdev/sampleapps/mayumi.htm#'),
    (7, 'Pavlova, Ltd.', 'Ian Devling', 'Marketing Manager', '74 Rose St. Moonie Ponds', 'Melbourne', 'Victoria', '3058', 'Australia', '(03) 444-2343', '(03) 444-6588', NULL),
    (8, 'Specialty Biscuits, Ltd.', 'Peter Wilson', 'Sales Representative', '29 King''s Way', 'Manchester', NULL, 'M14 GSD', 'UK', '(161) 555-4448', NULL, NULL),
    (9, 'PB Knackebrod AB', 'Lars Peterson', 'Sales Agent', 'Kaloadagatan 13', 'Goteborg', NULL, 'S-345 67', 'Sweden', '031-987 65 43', '031-987 65 91', NULL),
    (10, 'Refrescos Americanas LTDA', 'Carlos Diaz', 'Marketing Manager', 'Av. das Americanas 12.890', 'Sao Paulo', NULL, '5442', 'Brazil', '(11) 555 4640', NULL, NULL),
    (11, 'Heli Sussswaren GmbH & Co. KG', 'Petra Winkler', 'Sales Manager', 'Tiergartenstrasse 5', 'Berlin', NULL, '10785', 'Germany', '(010) 9984510', NULL, NULL),
    (12, 'Plutzer Lebensmittelgrosmarkte AG', 'Martin Bein', 'International Marketing Mgr.', 'Bogenallee 51', 'Frankfurt', NULL, '60439', 'Germany', '(069) 992755', NULL, 'Plutzer (on the World Wide Web)#http://www.microsoft.com/accessdev/sampleapps/plutzer.htm#'),
    (13, 'Nord-Ost-Fisch Handelsgesellschaft mbH', 'Sven Petersen', 'Coordinator Foreign Markets', 'Frahmredder 112a', 'Cuxhaven', NULL, '27478', 'Germany', '(04721) 8713', '(04721) 8714', NULL),
    (14, 'Formaggi Fortini s.r.l.', 'Elio Rossi', 'Sales Representative', 'Viale Dante, 75', 'Ravenna', NULL, '48100', 'Italy', '(0544) 60323', '(0544) 60603', '#FORMAGGI.HTM#'),
    (15, 'Norske Meierier', 'Beate Vileid', 'Marketing Manager', 'Hatlevegen 5', 'Sandvika', NULL, '1320', 'Norway', '(0)2-953010', NULL, NULL),
    (16, 'Bigfoot Breweries', 'Cheryl Saylor', 'Regional Account Rep.', '3400 - 8th Avenue Suite 210', 'Bend', 'OR', '97101', 'USA', '(503) 555-9931', NULL, NULL),
    (17, 'Svensk Sjofoda AB', 'Michael Bjorn', 'Sales Representative', 'Brovallavagen 231', 'Stockholm', NULL, 'S-123 45', 'Sweden', '08-123 45 67', NULL, NULL),
    (18, 'Aux joyeux ecclesiastiques', 'Guylene Nodier', 'Sales Manager', '203, Rue des Francs-Bourgeois', 'Paris', NULL, '75004', 'France', '(1) 03.83.00.68', '(1) 03.83.00.62', NULL),
    (19, 'New England Seafood Cannery', 'Robb Merchant', 'Wholesale Account Agent', 'Order Processing Dept. 2100 Paul Revere Blvd.', 'Boston', 'MA', '02134', 'USA', '(617) 555-3267', '(617) 555-3389', NULL),
    (20, 'Leka Trading', 'Chandra Leka', 'Owner', '471 Serangoon Loop, Suite #402', 'Singapore', NULL, '0512', 'Singapore', '555-8787', NULL, NULL),
    (21, 'Lyngbysild', 'Niels Petersen', 'Sales Manager', 'Lyngbysild Fiskevej 10', 'Lyngby', NULL, '2800', 'Denmark', '43844108', '43844115', NULL),
    (22, 'Zaanse Snoepfabriek', 'Dirk Luchte', 'Accounting Manager', 'Verkoop Rijnweg 22', 'Zaandam', NULL, '9999 ZZ', 'Netherlands', '(12345) 1212', '(12345) 1210', NULL),
    (23, 'Karkki Oy', 'Anne Heikkonen', 'Product Manager', 'Valtakatu 12', 'Lappeenranta', NULL, '53120', 'Finland', '(953) 10956', NULL, NULL),
    (24, 'G''day, Mate', 'Wendy Mackenzie', 'Sales Representative', '170 Prince Edward Parade Hunter''s Hill', 'Sydney', 'NSW', '2042', 'Australia', '(02) 555-5914', '(02) 555-4873', 'G''day Mate (on the World Wide Web)#http://www.microsoft.com/accessdev/sampleapps/gdaymate.htm#'),
    (25, 'Ma Maison', 'Jean-Guy Lauzon', 'Marketing Manager', '2960 Rue St. Laurent', 'Montreal', 'Quebec', 'H1J 1C3', 'Canada', '(514) 555-9022', NULL, NULL),
    (26, 'Pasta Buttini s.r.l.', 'Giovanni Giudici', 'Order Administrator', 'Via dei Gelsomini, 153', 'Salerno', NULL, '84100', 'Italy', '(089) 6547665', '(089) 6547667', NULL),
    (27, 'Escargots Nouveaux', 'Marie Delamare', 'Sales Manager', '22, rue H. Voiron', 'Montceau', NULL, '71300', 'France', '85.57.00.07', NULL, NULL),
    (28, 'Gai paturage', 'Eliane Noz', 'Sales Representative', 'Bat. B 3, rue des Alpes', 'Annecy', NULL, '74000', 'France', '38.76.98.06', '38.76.98.58', NULL),
    (29, 'Forets d''erables', 'Chantal Goulet', 'Accounting Manager', '148 rue Chasseur', 'Ste-Hyacinthe', 'Quebec', 'J2S 7S8', 'Canada', '(514) 555-2955', '(514) 555-2921', NULL)
ON CONFLICT DO NOTHING;

-- ------------------------------------------------------------------------------
-- Shippers
-- ------------------------------------------------------------------------------
INSERT INTO shipper (shipper_id, company_name, phone) VALUES
    (1, 'Speedy Express', '(503) 555-9831'),
    (2, 'United Package', '(503) 555-3199'),
    (3, 'Federal Shipping', '(503) 555-9931')
ON CONFLICT DO NOTHING;

-- ------------------------------------------------------------------------------
-- Customers (representative sample - 30 customers)
-- ------------------------------------------------------------------------------
INSERT INTO customer (customer_id, company_name, contact_name, contact_title, address, city, region, postal_code, country, phone, fax) VALUES
    ('ALFKI', 'Alfreds Futterkiste', 'Maria Anders', 'Sales Representative', 'Obere Str. 57', 'Berlin', NULL, '12209', 'Germany', '030-0074321', '030-0076545'),
    ('ANATR', 'Ana Trujillo Emparedados y helados', 'Ana Trujillo', 'Owner', 'Avda. de la Constitucion 2222', 'Mexico D.F.', NULL, '05021', 'Mexico', '(5) 555-4729', '(5) 555-3745'),
    ('ANTON', 'Antonio Moreno Taqueria', 'Antonio Moreno', 'Owner', 'Mataderos 2312', 'Mexico D.F.', NULL, '05023', 'Mexico', '(5) 555-3932', NULL),
    ('AROUT', 'Around the Horn', 'Thomas Hardy', 'Sales Representative', '120 Hanover Sq.', 'London', NULL, 'WA1 1DP', 'UK', '(171) 555-7788', '(171) 555-6750'),
    ('BERGS', 'Berglunds snabbkop', 'Christina Berglund', 'Order Administrator', 'Berguvsvagen 8', 'Lulea', NULL, 'S-958 22', 'Sweden', '0921-12 34 65', '0921-12 34 67'),
    ('BLAUS', 'Blauer See Delikatessen', 'Hanna Moos', 'Sales Representative', 'Forsterstr. 57', 'Mannheim', NULL, '68306', 'Germany', '0621-08460', '0621-08924'),
    ('BLONP', 'Blondel pere et fils', 'Frederique Citeaux', 'Marketing Manager', '24, place Kleber', 'Strasbourg', NULL, '67000', 'France', '88.60.15.31', '88.60.15.32'),
    ('BOLID', 'Bolido Comidas preparadas', 'Martin Sommer', 'Owner', 'C/ Araquil, 67', 'Madrid', NULL, '28023', 'Spain', '(91) 555 22 82', '(91) 555 91 99'),
    ('BONAP', 'Bon app''', 'Laurence Lebihan', 'Owner', '12, rue des Bouchers', 'Marseille', NULL, '13008', 'France', '91.24.45.40', '91.24.45.41'),
    ('BOTTM', 'Bottom-Dollar Markets', 'Elizabeth Lincoln', 'Accounting Manager', '23 Tsawassen Blvd.', 'Tsawassen', 'BC', 'T2F 8M4', 'Canada', '(604) 555-4729', '(604) 555-3745'),
    ('BSBEV', 'B''s Beverages', 'Victoria Ashworth', 'Sales Representative', 'Fauntleroy Circus', 'London', NULL, 'EC2 5NT', 'UK', '(171) 555-1212', NULL),
    ('CACTU', 'Cactus Comidas para llevar', 'Patricio Simpson', 'Sales Agent', 'Cerrito 333', 'Buenos Aires', NULL, '1010', 'Argentina', '(1) 135-5555', '(1) 135-4892'),
    ('CENTC', 'Centro comercial Moctezuma', 'Francisco Chang', 'Marketing Manager', 'Sierras de Granada 9993', 'Mexico D.F.', NULL, '05022', 'Mexico', '(5) 555-3392', '(5) 555-7293'),
    ('CHOPS', 'Chop-suey Chinese', 'Yang Wang', 'Owner', 'Hauptstr. 29', 'Bern', NULL, '3012', 'Switzerland', '0452-076545', NULL),
    ('COMMI', 'Comercio Mineiro', 'Pedro Afonso', 'Sales Associate', 'Av. dos Lusiadas, 23', 'Sao Paulo', 'SP', '05432-043', 'Brazil', '(11) 555-7647', NULL),
    ('CONSH', 'Consolidated Holdings', 'Elizabeth Brown', 'Sales Representative', 'Berkeley Gardens 12 Brewery', 'London', NULL, 'WX1 6LT', 'UK', '(171) 555-2282', '(171) 555-9199'),
    ('DRACD', 'Drachenblut Delikatessen', 'Sven Ottlieb', 'Order Administrator', 'Walserweg 21', 'Aachen', NULL, '52066', 'Germany', '0241-039123', '0241-059428'),
    ('DUMON', 'Du monde entier', 'Janine Labrune', 'Owner', '67, rue des Cinquante Otages', 'Nantes', NULL, '44000', 'France', '40.67.88.88', '40.67.89.89'),
    ('EASTC', 'Eastern Connection', 'Ann Devon', 'Sales Agent', '35 King George', 'London', NULL, 'WX3 6FW', 'UK', '(171) 555-0297', '(171) 555-3373'),
    ('ERNSH', 'Ernst Handel', 'Roland Mendel', 'Sales Manager', 'Kirchgasse 6', 'Graz', NULL, '8010', 'Austria', '7675-3425', '7675-3426'),
    ('FAMIA', 'Familia Arquibaldo', 'Aria Cruz', 'Marketing Assistant', 'Rua Oros, 92', 'Sao Paulo', 'SP', '05442-030', 'Brazil', '(11) 555-9857', NULL),
    ('FISSA', 'FISSA Fabrica Inter. Salchichas S.A.', 'Diego Roel', 'Accounting Manager', 'C/ Moralzarzal, 86', 'Madrid', NULL, '28034', 'Spain', '(91) 555 94 44', '(91) 555 55 93'),
    ('FOLIG', 'Folies gourmandes', 'Martine Rance', 'Assistant Sales Agent', '184, chaussee de Tournai', 'Lille', NULL, '59000', 'France', '20.16.10.16', '20.16.10.17'),
    ('FRANK', 'Frankenversand', 'Peter Franken', 'Marketing Manager', 'Berliner Platz 43', 'Munchen', NULL, '80805', 'Germany', '089-0877310', '089-0877451'),
    ('FRANR', 'France restauration', 'Carine Schmitt', 'Marketing Manager', '54, rue Royale', 'Nantes', NULL, '44000', 'France', '40.32.21.21', '40.32.21.20'),
    ('FRANS', 'Franchi S.p.A.', 'Paolo Accorti', 'Sales Representative', 'Via Monte Bianco 34', 'Torino', NULL, '10100', 'Italy', '011-4988260', '011-4988261'),
    ('GALED', 'Galeria del gastronomo', 'Eduardo Saavedra', 'Marketing Manager', 'Rambla de Cataluna, 23', 'Barcelona', NULL, '08022', 'Spain', '(93) 203 4560', '(93) 203 4561'),
    ('GODOS', 'Godos Cocina Tipica', 'Jose Pedro Freyre', 'Sales Manager', 'C/ Romero, 33', 'Sevilla', NULL, '41101', 'Spain', '(95) 555 82 82', NULL),
    ('GOURL', 'Gourmet Lanchonetes', 'Andre Fonseca', 'Sales Associate', 'Av. Brasil, 442', 'Campinas', 'SP', '04876-786', 'Brazil', '(11) 555-9482', NULL),
    ('GREAL', 'Great Lakes Food Market', 'Howard Snyder', 'Marketing Manager', '2732 Baker Blvd.', 'Eugene', 'OR', '97403', 'USA', '(503) 555-7555', NULL)
ON CONFLICT DO NOTHING;

-- ------------------------------------------------------------------------------
-- Employees
-- ------------------------------------------------------------------------------
INSERT INTO employee (employee_id, last_name, first_name, title, title_of_courtesy, birth_date, hire_date, address, city, region, postal_code, country, home_phone, extension, notes, photo_path, reports_to) VALUES
    (1, 'Davolio', 'Nancy', 'Sales Representative', 'Ms.', '1968-12-08', '1992-05-01', '507 - 20th Ave. E. Apt. 2A', 'Seattle', 'WA', '98122', 'USA', '(206) 555-9857', '5467', 'Education includes a BA in psychology from Colorado State University.', 'http://accweb/emmployees/davolio.bmp', 2),
    (2, 'Fuller', 'Andrew', 'Vice President, Sales', 'Dr.', '1952-02-19', '1992-08-14', '908 W. Capital Way', 'Tacoma', 'WA', '98401', 'USA', '(206) 555-9482', '3457', 'Andrew received his BTS commercial and a Ph.D. in international marketing from the University of Dallas.', 'http://accweb/emmployees/fuller.bmp', NULL),
    (3, 'Leverling', 'Janet', 'Sales Representative', 'Ms.', '1963-08-30', '1992-04-01', '722 Moss Bay Blvd.', 'Kirkland', 'WA', '98033', 'USA', '(206) 555-3412', '3355', 'Janet has a BS degree in chemistry from Boston College.', 'http://accweb/emmployees/leverling.bmp', 2),
    (4, 'Peacock', 'Margaret', 'Sales Representative', 'Mrs.', '1958-09-19', '1993-05-03', '4110 Old Redmond Rd.', 'Redmond', 'WA', '98052', 'USA', '(206) 555-8122', '5176', 'Margaret holds a BA in English literature from Concordia College and an MA from the American Institute of Culinary Arts.', 'http://accweb/emmployees/peacock.bmp', 2),
    (5, 'Buchanan', 'Steven', 'Sales Manager', 'Mr.', '1955-03-04', '1993-10-17', '14 Garrett Hill', 'London', NULL, 'SW1 8JR', 'UK', '(71) 555-4848', '3453', 'Steven Buchanan graduated from St. Andrews University, Scotland.', 'http://accweb/emmployees/buchanan.bmp', 2),
    (6, 'Suyama', 'Michael', 'Sales Representative', 'Mr.', '1963-07-02', '1993-10-17', 'Coventry House Miner Rd.', 'London', NULL, 'EC2 7JR', 'UK', '(71) 555-7773', '428', 'Michael is a graduate of Sussex University (MA, economics) and the University of California at Los Angeles (MBA, marketing).', 'http://accweb/emmployees/davolio.bmp', 5),
    (7, 'King', 'Robert', 'Sales Representative', 'Mr.', '1960-05-29', '1994-01-02', 'Edgeham Hollow Winchester Way', 'London', NULL, 'RG1 9SP', 'UK', '(71) 555-5598', '465', 'Robert King served in the Peace Corps and traveled extensively before completing his degree in English at the University of Michigan.', 'http://accweb/emmployees/davolio.bmp', 5),
    (8, 'Callahan', 'Laura', 'Inside Sales Coordinator', 'Ms.', '1958-01-09', '1994-03-05', '4726 - 11th Ave. N.E.', 'Seattle', 'WA', '98105', 'USA', '(206) 555-1189', '2344', 'Laura received a BA in psychology from the University of Washington.', 'http://accweb/emmployees/davolio.bmp', 2),
    (9, 'Dodsworth', 'Anne', 'Sales Representative', 'Ms.', '1969-07-02', '1994-11-15', '7 Houndstooth Rd.', 'London', NULL, 'WG2 7LT', 'UK', '(71) 555-4444', '452', 'Anne has a BA degree in English from St. Lawrence College.', 'http://accweb/emmployees/davolio.bmp', 5)
ON CONFLICT DO NOTHING;

-- ------------------------------------------------------------------------------
-- Employee Territories
-- ------------------------------------------------------------------------------
INSERT INTO employee_territory (employee_id, territory_id) VALUES
    (1, '06897'), (1, '19713'),
    (2, '01581'), (2, '01730'), (2, '01833'), (2, '02116'), (2, '02139'), (2, '02184'), (2, '40222'),
    (3, '30346'), (3, '31406'), (3, '32859'), (3, '33607'),
    (4, '20852'), (4, '27403'), (4, '27511'),
    (5, '02903'), (5, '07960'), (5, '08837'), (5, '10019'), (5, '10038'), (5, '11747'), (5, '14450'),
    (6, '44122'), (6, '44122'), (6, '45839'), (6, '48075'), (6, '48084'), (6, '48304'),
    (7, '60179'), (7, '60601'), (7, '80202'), (7, '80909'), (7, '85014'), (7, '85251'),
    (8, '98004'), (8, '98052'), (8, '98104'),
    (9, '03049'), (9, '03801'), (9, '48084')
ON CONFLICT DO NOTHING;

-- ------------------------------------------------------------------------------
-- Products
-- ------------------------------------------------------------------------------
INSERT INTO product (product_id, product_name, supplier_id, category_id, quantity_per_unit, unit_price, units_in_stock, units_on_order, reorder_level, discontinued) VALUES
    (1, 'Chai', 1, 1, '10 boxes x 20 bags', 18, 39, 0, 10, 0),
    (2, 'Chang', 1, 1, '24 - 12 oz bottles', 19, 17, 40, 25, 0),
    (3, 'Aniseed Syrup', 1, 2, '12 - 550 ml bottles', 10, 13, 70, 25, 0),
    (4, 'Chef Anton''s Cajun Seasoning', 2, 2, '48 - 6 oz jars', 22, 53, 0, 0, 0),
    (5, 'Chef Anton''s Gumbo Mix', 2, 2, '36 boxes', 21.35, 0, 0, 0, 1),
    (6, 'Grandma''s Boysenberry Spread', 3, 2, '12 - 8 oz jars', 25, 120, 0, 25, 0),
    (7, 'Uncle Bob''s Organic Dried Pears', 3, 7, '12 - 1 lb pkgs.', 30, 15, 0, 10, 0),
    (8, 'Northwoods Cranberry Sauce', 3, 2, '12 - 12 oz jars', 40, 6, 0, 0, 0),
    (9, 'Mishi Kobe Niku', 4, 6, '18 - 500 g pkgs.', 97, 29, 0, 0, 1),
    (10, 'Ikura', 4, 8, '12 - 200 ml jars', 31, 31, 0, 0, 0),
    (11, 'Queso Cabrales', 5, 4, '1 kg pkg.', 21, 22, 30, 30, 0),
    (12, 'Queso Manchego La Pastora', 5, 4, '10 - 500 g pkgs.', 38, 86, 0, 0, 0),
    (13, 'Konbu', 6, 8, '2 kg box', 6, 24, 0, 5, 0),
    (14, 'Tofu', 6, 7, '40 - 100 g pkgs.', 23.25, 35, 0, 0, 0),
    (15, 'Genen Shouyu', 6, 2, '24 - 250 ml bottles', 15.5, 39, 0, 5, 0),
    (16, 'Pavlova', 7, 3, '32 - 500 g boxes', 17.45, 29, 0, 10, 0),
    (17, 'Alice Mutton', 7, 6, '20 - 1 kg tins', 39, 0, 0, 0, 1),
    (18, 'Carnarvon Tigers', 7, 8, '16 kg pkg.', 62.5, 42, 0, 0, 0),
    (19, 'Teatime Chocolate Biscuits', 8, 3, '10 boxes x 12 pieces', 9.2, 25, 0, 5, 0),
    (20, 'Sir Rodney''s Marmalade', 8, 3, '30 gift boxes', 81, 40, 0, 0, 0),
    (21, 'Sir Rodney''s Scones', 8, 3, '24 pkgs. x 4 pieces', 10, 3, 40, 5, 0),
    (22, 'Gustaf''s Knackebrod', 9, 5, '24 - 500 g pkgs.', 21, 104, 0, 25, 0),
    (23, 'Tunnbrod', 9, 5, '12 - 250 g pkgs.', 9, 61, 0, 25, 0),
    (24, 'Guarana Fantastica', 10, 1, '12 - 355 ml cans', 4.5, 20, 0, 0, 1),
    (25, 'NuNuCa Nuss-Nougat-Creme', 11, 3, '20 - 450 g glasses', 14, 76, 0, 30, 0),
    (26, 'Gumbar Gummibarchen', 11, 3, '100 - 250 g bags', 31.23, 15, 0, 0, 0),
    (27, 'Schoggi Schokolade', 11, 3, '100 - 100 g pieces', 43.9, 49, 0, 30, 0),
    (28, 'Rossle Sauerkraut', 12, 7, '25 - 825 g cans', 45.6, 26, 0, 0, 0),
    (29, 'Thuringer Rostbratwurst', 12, 6, '50 bags x 30 sausgs.', 123.79, 0, 0, 0, 1),
    (30, 'Nord-Ost Matjeshering', 13, 8, '10 - 200 g glasses', 25.89, 10, 0, 15, 0),
    (31, 'Gorgonzola Telino', 14, 4, '12 - 100 g pkgs', 12.5, 0, 70, 20, 0),
    (32, 'Mascarpone Fabioli', 14, 4, '24 - 200 g pkgs.', 32, 9, 40, 25, 0),
    (33, 'Geitost', 15, 4, '500 g', 2.5, 112, 0, 20, 0),
    (34, 'Sasquatch Ale', 16, 1, '24 - 12 oz bottles', 14, 111, 0, 15, 0),
    (35, 'Steeleye Stout', 16, 1, '24 - 12 oz bottles', 18, 20, 0, 15, 0),
    (36, 'Inlagd Sill', 17, 8, '24 - 250 g jars', 19, 112, 0, 20, 0),
    (37, 'Gravad lax', 17, 8, '12 - 500 g pkgs.', 26, 11, 50, 25, 0),
    (38, 'Cote de Blaye', 18, 1, '12 - 75 cl bottles', 263.5, 17, 0, 15, 0),
    (39, 'Chartreuse verte', 18, 1, '750 cc per bottle', 18, 69, 0, 5, 0),
    (40, 'Boston Crab Meat', 19, 8, '24 - 4 oz tins', 18.4, 123, 0, 30, 0),
    (41, 'Jack''s New England Clam Chowder', 19, 8, '12 - 12 oz cans', 9.65, 85, 0, 10, 0),
    (42, 'Singaporean Hokkien Fried Mee', 20, 5, '32 - 1 kg pkgs.', 14, 26, 0, 0, 1),
    (43, 'Ipoh Coffee', 20, 1, '16 - 500 g tins', 46, 17, 10, 25, 0),
    (44, 'Gula Malacca', 20, 2, '20 - 2 kg bags', 19.45, 27, 0, 15, 0),
    (45, 'Rogede sild', 21, 8, '1k pkg.', 9.5, 5, 70, 15, 0),
    (46, 'Spegesild', 21, 8, '4 - 450 g glasses', 12, 95, 0, 0, 0),
    (47, 'Zaanse koeken', 22, 3, '10 - 4 oz boxes', 9.5, 36, 0, 0, 0),
    (48, 'Chocolade', 22, 3, '10 pkgs.', 12.75, 15, 70, 25, 0),
    (49, 'Maxilaku', 23, 3, '24 - 50 g pkgs.', 20, 10, 60, 15, 0),
    (50, 'Valkoinen suklaa', 23, 3, '12 - 100 g bars', 16.25, 65, 0, 30, 0),
    (51, 'Manjimup Dried Apples', 24, 7, '50 - 300 g pkgs.', 53, 20, 0, 10, 0),
    (52, 'Filo Mix', 24, 5, '16 - 2 kg boxes', 7, 38, 0, 25, 0),
    (53, 'Perth Pasties', 24, 6, '48 pieces', 32.8, 0, 0, 0, 1),
    (54, 'Tourtiere', 25, 6, '16 pies', 7.45, 21, 0, 10, 0),
    (55, 'Pate chinois', 25, 6, '24 boxes x 2 pies', 24, 115, 0, 20, 0),
    (56, 'Gnocchi di nonna Alice', 26, 5, '24 - 250 g pkgs.', 38, 21, 10, 30, 0),
    (57, 'Ravioli Angelo', 26, 5, '24 - 250 g pkgs.', 19.5, 36, 0, 20, 0),
    (58, 'Escargots de Bourgogne', 27, 8, '24 pieces', 13.25, 62, 0, 20, 0),
    (59, 'Raclette Courdavault', 28, 4, '5 kg pkg.', 55, 79, 0, 0, 0),
    (60, 'Camembert Pierrot', 28, 4, '15 - 300 g rounds', 34, 19, 0, 0, 0),
    (61, 'Sirop d''erable', 29, 2, '24 - 500 ml bottles', 28.5, 113, 0, 25, 0),
    (62, 'Tarte au sucre', 29, 3, '48 pies', 49.3, 17, 0, 0, 0),
    (63, 'Vegie-spread', 7, 2, '15 - 625 g jars', 43.9, 24, 0, 5, 0),
    (64, 'Wimmers gute Semmelknodel', 12, 5, '20 bags x 4 pieces', 33.25, 22, 80, 30, 0),
    (65, 'Louisiana Fiery Hot Pepper Sauce', 2, 2, '32 - 8 oz bottles', 21.05, 76, 0, 0, 0),
    (66, 'Louisiana Hot Spiced Okra', 2, 2, '24 - 8 oz jars', 17, 4, 100, 20, 0),
    (67, 'Laughing Lumberjack Lager', 16, 1, '24 - 12 oz bottles', 14, 52, 0, 10, 0),
    (68, 'Scottish Longbreads', 8, 3, '10 boxes x 8 pieces', 12.5, 6, 10, 15, 0),
    (69, 'Gudbrandsdalsost', 15, 4, '10 kg pkg.', 36, 26, 0, 15, 0),
    (70, 'Outback Lager', 7, 1, '24 - 355 ml bottles', 15, 15, 10, 30, 0),
    (71, 'Flotemysost', 15, 4, '10 - 500 g pkgs.', 21.5, 26, 0, 0, 0),
    (72, 'Mozzarella di Giovanni', 14, 4, '24 - 200 g pkgs.', 34.8, 14, 0, 0, 0),
    (73, 'Rod Kaviar', 17, 8, '24 - 150 g jars', 15, 101, 0, 5, 0),
    (74, 'Longlife Tofu', 4, 7, '5 kg pkg.', 10, 4, 20, 5, 0),
    (75, 'Rhonbrau Klosterbier', 12, 1, '24 - 0.5 l bottles', 7.75, 125, 0, 25, 0),
    (76, 'Lakkalikoori', 23, 1, '500 ml', 18, 57, 0, 20, 0),
    (77, 'Original Frankfurter grune Sosse', 12, 2, '12 boxes', 13, 32, 0, 15, 0)
ON CONFLICT DO NOTHING;

-- ------------------------------------------------------------------------------
-- Orders (representative sample - 30 orders)
-- ------------------------------------------------------------------------------
INSERT INTO "order" (order_id, customer_id, employee_id, order_date, required_date, shipped_date, ship_via, freight, ship_name, ship_address, ship_city, ship_region, ship_postal_code, ship_country) VALUES
    (10248, 'ALFKI', 5, '1996-07-04', '1996-08-01', '1996-07-16', 3, 32.38, 'Alfreds Futterkiste', 'Obere Str. 57', 'Berlin', NULL, '12209', 'Germany'),
    (10249, 'ANATR', 6, '1996-07-05', '1996-08-16', '1996-07-10', 1, 11.61, 'Ana Trujillo Emparedados y helados', 'Avda. de la Constitucion 2222', 'Mexico D.F.', NULL, '05021', 'Mexico'),
    (10250, 'BLAUS', 4, '1996-07-08', '1996-08-05', '1996-07-12', 2, 65.83, 'Blauer See Delikatessen', 'Forsterstr. 57', 'Mannheim', NULL, '68306', 'Germany'),
    (10251, 'BOLID', 3, '1996-07-08', '1996-08-05', '1996-07-15', 1, 41.34, 'Bolido Comidas preparadas', 'C/ Araquil, 67', 'Madrid', NULL, '28023', 'Spain'),
    (10252, 'BONAP', 4, '1996-07-09', '1996-08-06', '1996-07-11', 2, 51.30, 'Bon app''', '12, rue des Bouchers', 'Marseille', NULL, '13008', 'France'),
    (10253, 'BOTTM', 3, '1996-07-10', '1996-07-24', '1996-07-16', 2, 58.17, 'Bottom-Dollar Markets', '23 Tsawassen Blvd.', 'Tsawassen', 'BC', 'T2F 8M4', 'Canada'),
    (10254, 'CHOPS', 5, '1996-07-11', '1996-08-08', '1996-07-23', 2, 22.98, 'Chop-suey Chinese', 'Hauptstr. 29', 'Bern', NULL, '3012', 'Switzerland'),
    (10255, 'BLONP', 9, '1996-07-12', '1996-08-09', '1996-07-15', 3, 148.33, 'Blondel pere et fils', '24, place Kleber', 'Strasbourg', NULL, '67000', 'France'),
    (10256, 'ERNSH', 3, '1996-07-15', '1996-08-12', '1996-07-17', 2, 13.97, 'Ernst Handel', 'Kirchgasse 6', 'Graz', NULL, '8010', 'Austria'),
    (10257, 'BERGS', 4, '1996-07-16', '1996-08-13', '1996-07-22', 3, 81.91, 'Berglunds snabbkop', 'Berguvsvagen 8', 'Lulea', NULL, 'S-958 22', 'Sweden'),
    (10258, 'ERNSH', 1, '1996-07-17', '1996-08-14', '1996-07-23', 1, 140.51, 'Ernst Handel', 'Kirchgasse 6', 'Graz', NULL, '8010', 'Austria'),
    (10259, 'CENTC', 4, '1996-07-18', '1996-08-15', '1996-07-25', 3, 3.25, 'Centro comercial Moctezuma', 'Sierras de Granada 9993', 'Mexico D.F.', NULL, '05022', 'Mexico'),
    (10260, 'BOTTM', 4, '1996-07-19', '1996-08-16', '1996-07-29', 1, 55.09, 'Bottom-Dollar Markets', '23 Tsawassen Blvd.', 'Tsawassen', 'BC', 'T2F 8M4', 'Canada'),
    (10261, 'BLAUS', 4, '1996-07-19', '1996-08-16', '1996-07-30', 2, 3.05, 'Blauer See Delikatessen', 'Forsterstr. 57', 'Mannheim', NULL, '68306', 'Germany'),
    (10262, 'ANATR', 8, '1996-07-22', '1996-08-19', '1996-07-25', 3, 48.29, 'Ana Trujillo Emparedados y helados', 'Avda. de la Constitucion 2222', 'Mexico D.F.', NULL, '05021', 'Mexico'),
    (10263, 'ERNSH', 9, '1996-07-23', '1996-08-20', '1996-07-31', 3, 146.06, 'Ernst Handel', 'Kirchgasse 6', 'Graz', NULL, '8010', 'Austria'),
    (10264, 'AROUT', 6, '1996-07-24', '1996-08-21', '1996-08-23', 3, 3.67, 'Around the Horn', '120 Hanover Sq.', 'London', NULL, 'WA1 1DP', 'UK'),
    (10265, 'BLONP', 2, '1996-07-25', '1996-08-22', '1996-08-12', 1, 55.28, 'Blondel pere et fils', '24, place Kleber', 'Strasbourg', NULL, '67000', 'France'),
    (10266, 'BONAP', 3, '1996-07-26', '1996-09-06', '1996-07-31', 3, 25.73, 'Bon app''', '12, rue des Bouchers', 'Marseille', NULL, '13008', 'France'),
    (10267, 'FRANK', 4, '1996-07-29', '1996-08-26', '1996-08-06', 1, 208.58, 'Frankenversand', 'Berliner Platz 43', 'Munchen', NULL, '80805', 'Germany'),
    (10268, 'GREAL', 8, '1996-07-30', '1996-08-27', '1996-08-02', 3, 66.29, 'Great Lakes Food Market', '2732 Baker Blvd.', 'Eugene', 'OR', '97403', 'USA'),
    (10269, 'BOTTM', 5, '1996-07-31', '1996-08-14', '1996-08-09', 1, 4.56, 'Bottom-Dollar Markets', '23 Tsawassen Blvd.', 'Tsawassen', 'BC', 'T2F 8M4', 'Canada'),
    (10270, 'BONAP', 1, '1996-08-01', '1996-08-29', '1996-08-02', 1, 136.54, 'Bon app''', '12, rue des Bouchers', 'Marseille', NULL, '13008', 'France'),
    (10271, 'AROUT', 6, '1996-08-01', '1996-08-29', '1996-08-30', 2, 4.54, 'Around the Horn', '120 Hanover Sq.', 'London', NULL, 'WA1 1DP', 'UK'),
    (10272, 'ALFKI', 6, '1996-08-02', '1996-08-30', '1996-08-06', 2, 98.03, 'Alfreds Futterkiste', 'Obere Str. 57', 'Berlin', NULL, '12209', 'Germany'),
    (10273, 'BOTTM', 3, '1996-08-05', '1996-09-02', '1996-08-12', 3, 76.07, 'Bottom-Dollar Markets', '23 Tsawassen Blvd.', 'Tsawassen', 'BC', 'T2F 8M4', 'Canada'),
    (10274, 'ALFKI', 6, '1996-08-06', '1996-09-03', '1996-08-16', 1, 6.01, 'Alfreds Futterkiste', 'Obere Str. 57', 'Berlin', NULL, '12209', 'Germany'),
    (10275, 'BERGS', 1, '1996-08-07', '1996-09-04', '1996-08-09', 1, 26.93, 'Berglunds snabbkop', 'Berguvsvagen 8', 'Lulea', NULL, 'S-958 22', 'Sweden'),
    (10276, 'BLAUS', 8, '1996-08-08', '1996-08-22', '1996-08-14', 3, 13.84, 'Blauer See Delikatessen', 'Forsterstr. 57', 'Mannheim', NULL, '68306', 'Germany'),
    (10277, 'BERGS', 2, '1996-08-09', '1996-09-06', '1996-08-13', 3, 125.77, 'Berglunds snabbkop', 'Berguvsvagen 8', 'Lulea', NULL, 'S-958 22', 'Sweden')
ON CONFLICT DO NOTHING;

-- ------------------------------------------------------------------------------
-- Order Details (line items for above orders)
-- ------------------------------------------------------------------------------
INSERT INTO order_detail (order_id, product_id, unit_price, quantity, discount) VALUES
    (10248, 11, 14, 12, 0),
    (10248, 42, 9.8, 10, 0),
    (10248, 72, 34.8, 5, 0),
    (10249, 14, 18.6, 9, 0),
    (10249, 51, 42.4, 40, 0),
    (10250, 41, 7.7, 10, 0),
    (10250, 51, 42.4, 35, 0.15),
    (10250, 65, 16.8, 15, 0.15),
    (10251, 22, 16.8, 6, 0.05),
    (10251, 57, 15.6, 15, 0.05),
    (10251, 65, 16.8, 20, 0),
    (10252, 20, 64.8, 40, 0.05),
    (10252, 33, 2, 25, 0.05),
    (10252, 60, 27.2, 40, 0),
    (10253, 31, 10, 20, 0),
    (10253, 39, 14.4, 42, 0),
    (10253, 49, 16, 40, 0),
    (10254, 24, 3.6, 15, 0.15),
    (10254, 55, 19.2, 21, 0.15),
    (10254, 74, 8, 21, 0),
    (10255, 2, 15.2, 20, 0),
    (10255, 16, 13.9, 35, 0),
    (10255, 36, 15.2, 25, 0),
    (10255, 59, 44, 30, 0),
    (10256, 53, 26.2, 15, 0),
    (10256, 77, 10.4, 12, 0),
    (10257, 27, 35.1, 25, 0),
    (10257, 39, 14.4, 6, 0),
    (10257, 77, 10.4, 15, 0),
    (10258, 2, 15.2, 50, 0.2),
    (10258, 5, 17.0, 65, 0.2),
    (10258, 32, 25.6, 6, 0.2),
    (10259, 21, 8, 10, 0),
    (10259, 37, 20.8, 1, 0),
    (10260, 41, 7.7, 16, 0.25),
    (10260, 57, 15.6, 50, 0),
    (10260, 62, 39.4, 15, 0.25),
    (10260, 70, 12, 21, 0.25),
    (10261, 21, 8, 20, 0),
    (10261, 35, 14.4, 20, 0),
    (10262, 5, 17.0, 12, 0.2),
    (10262, 7, 24, 15, 0),
    (10262, 56, 30.4, 2, 0),
    (10263, 16, 13.9, 60, 0.25),
    (10263, 24, 3.6, 28, 0),
    (10263, 30, 20.7, 60, 0.25),
    (10263, 74, 8, 36, 0.25),
    (10264, 2, 15.2, 35, 0),
    (10264, 41, 7.7, 25, 0.15),
    (10265, 17, 31.2, 30, 0),
    (10265, 70, 12, 20, 0),
    (10266, 12, 30.4, 12, 0.05),
    (10267, 40, 14.7, 50, 0),
    (10267, 59, 44, 70, 0.15),
    (10267, 76, 14.4, 15, 0.15),
    (10268, 29, 99.0, 10, 0),
    (10268, 72, 27.8, 4, 0),
    (10269, 33, 2, 60, 0.05),
    (10269, 72, 27.8, 20, 0.05),
    (10270, 36, 15.2, 30, 0),
    (10270, 43, 36.8, 25, 0),
    (10271, 33, 2, 24, 0),
    (10272, 20, 64.8, 6, 0),
    (10272, 31, 10, 40, 0),
    (10272, 72, 27.8, 24, 0),
    (10273, 10, 24.8, 24, 0.05),
    (10273, 31, 10, 15, 0.05),
    (10273, 33, 2, 20, 0),
    (10273, 40, 14.7, 60, 0.05),
    (10274, 71, 17.2, 20, 0),
    (10274, 72, 27.8, 7, 0),
    (10275, 24, 3.6, 12, 0.05),
    (10275, 59, 44, 6, 0.05),
    (10276, 10, 24.8, 15, 0),
    (10276, 13, 4.8, 10, 0),
    (10277, 28, 36.4, 20, 0),
    (10277, 62, 39.4, 12, 0)
ON CONFLICT DO NOTHING;

COMMIT;
