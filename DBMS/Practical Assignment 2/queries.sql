-- 1) Customers with balance > 9000 and in "HOUSEHOLD" market segment
SELECT c_name 
FROM CUSTOMER 
WHERE c_acctbal > 9000 
AND c_mktsegment = 'HOUSEHOLD';

-- 2) Parts supplied by at least one supplier from the "MIDDLE EAST"
SELECT DISTINCT p.p_partkey, p.p_name
FROM PART p
JOIN PARTSUPP ps ON p.p_partkey = ps.ps_partkey
JOIN SUPPLIER s ON ps.ps_suppkey = s.s_suppkey
JOIN NATION n ON s.s_nationkey = n.n_nationkey
JOIN REGION r ON n.n_regionkey = r.r_regionkey
WHERE r.r_name = 'MIDDLE EAST'
ORDER BY p.p_partkey;

-- 3) Count of distinct parts supplied by European suppliers
SELECT COUNT(DISTINCT ps.ps_partkey) AS distinct_parts
FROM PARTSUPP ps
JOIN SUPPLIER s ON ps.ps_suppkey = s.s_suppkey
JOIN NATION n ON s.s_nationkey = n.n_nationkey
JOIN REGION r ON n.n_regionkey = r.r_regionkey
WHERE r.r_name = 'EUROPE';

-- 4) Parts not supplied by any supplier from EUROPE
SELECT DISTINCT p.p_name
FROM PART p
WHERE p.p_partkey NOT IN (
    SELECT ps.ps_partkey
    FROM PARTSUPP ps
    JOIN SUPPLIER s ON ps.ps_suppkey = s.s_suppkey
    JOIN NATION n ON s.s_nationkey = n.n_nationkey
    JOIN REGION r ON n.n_regionkey = r.r_regionkey
    WHERE r.r_name = 'EUROPE'
);

-- 5) Customers who ordered parts ONLY from suppliers in the same region
SELECT DISTINCT c.c_name, c.c_phone, r.r_name
FROM CUSTOMER c
JOIN ORDERS o ON c.c_custkey = o.o_custkey
JOIN LINEITEM l ON o.o_orderkey = l.l_orderkey
JOIN SUPPLIER s ON l.l_suppkey = s.s_suppkey
JOIN NATION cn ON c.c_nationkey = cn.n_nationkey
JOIN NATION sn ON s.s_nationkey = sn.n_nationkey
JOIN REGION r ON cn.n_regionkey = r.r_regionkey
WHERE cn.n_regionkey = sn.n_regionkey;

-- 6) Highest extended price for parts with discount > tax
SELECT MAX(l.l_extendedprice)
FROM LINEITEM l
WHERE l.l_discount > l.l_tax;

-- 7) Number of orders with all items received within 2 weeks
SELECT COUNT(DISTINCT o.o_orderkey)
FROM ORDERS o
JOIN LINEITEM l ON o.o_orderkey = l.l_orderkey
WHERE NOT EXISTS (
    SELECT 1
    FROM LINEITEM l2
    WHERE l2.l_orderkey = o.o_orderkey
    AND DATEDIFF(l2.l_shipdate, l2.l_commitdate) > 14
);

-- 8) Number of customers who did not order any part from an American supplier
SELECT COUNT(DISTINCT c.c_custkey)
FROM CUSTOMER c
WHERE c.c_custkey NOT IN (
    SELECT DISTINCT o.o_custkey
    FROM ORDERS o
    JOIN LINEITEM l ON o.o_orderkey = l.l_orderkey
    JOIN SUPPLIER s ON l.l_suppkey = s.s_suppkey
    JOIN NATION n ON s.s_nationkey = n.n_nationkey
    JOIN REGION r ON n.n_regionkey = r.r_regionkey
    WHERE r.r_name = 'AMERICA'
);

-- 9) Nation of the customer with the highest balance
SELECT n.n_name
FROM CUSTOMER c
JOIN NATION n ON c.c_nationkey = n.n_nationkey
WHERE c.c_acctbal = (SELECT MAX(c_acctbal) FROM CUSTOMER);

-- 10) Countries where parts took more than 29 days to ship within the same country
SELECT DISTINCT n.n_name
FROM LINEITEM l
JOIN ORDERS o ON l.l_orderkey = o.o_orderkey
JOIN CUSTOMER c ON o.o_custkey = c.c_custkey
JOIN SUPPLIER s ON l.l_suppkey = s.s_suppkey
JOIN NATION n ON c.c_nationkey = n.n_nationkey
WHERE s.s_nationkey = c.c_nationkey
AND DATEDIFF(l.l_shipdate, l.l_commitdate) > 29;

-- 11) European customers with balance < 8000
SELECT COUNT(DISTINCT c.c_custkey)
FROM CUSTOMER c
JOIN NATION n ON c.c_nationkey = n.n_nationkey
JOIN REGION r ON n.n_regionkey = r.r_regionkey
WHERE r.r_name = 'EUROPE' AND c.c_acctbal < 8000;

-- 12) Region that supplies the most distinct parts
SELECT r.r_name, COUNT(DISTINCT ps.ps_partkey) AS part_count
FROM PARTSUPP ps
JOIN SUPPLIER s ON ps.ps_suppkey = s.s_suppkey
JOIN NATION n ON s.s_nationkey = n.n_nationkey
JOIN REGION r ON n.n_regionkey = r.r_regionkey
GROUP BY r.r_name
ORDER BY part_count DESC
LIMIT 1;

-- 13) Average shipping time by TRUCK and items taking less than average time
WITH AvgShippingTime AS (
    SELECT AVG(DATEDIFF(l.l_shipdate, l.l_commitdate)) AS avg_time
    FROM LINEITEM l
    WHERE l.l_shipmode = 'TRUCK'
)
SELECT l.l_quantity, l.l_orderkey
FROM LINEITEM l, AvgShippingTime
WHERE l.l_shipmode = 'TRUCK' AND DATEDIFF(l.l_shipdate, l.l_commitdate) < avg_time;

-- 14) Items with no discount and not sold to Europe
    SELECT DISTINCT l.*
    FROM LINEITEM l
    JOIN ORDERS o ON l.l_orderkey = o.o_orderkey
    JOIN CUSTOMER c ON o.o_custkey = c.c_custkey
    JOIN NATION n ON c.c_nationkey = n.n_nationkey
    JOIN REGION r ON n.n_regionkey = r.r_regionkey
    WHERE l.l_discount = 0 AND r.r_name <> 'EUROPE';

-- 15) Customers who have not placed any orders
    SELECT c.c_name
    FROM CUSTOMER c
    WHERE c.c_custkey NOT IN (SELECT DISTINCT o.o_custkey FROM ORDERS o);

-- 16) Distinct items ordered by customers from the country with the highest average supplier balance
WITH HighestAvgBalanceNation AS (
    SELECT n.n_nationkey
    FROM SUPPLIER s
    JOIN NATION n ON s.s_nationkey = n.n_nationkey
    GROUP BY n.n_nationkey
    ORDER BY AVG(s.s_acctbal) DESC
    LIMIT 1
)
SELECT DISTINCT l.l_partkey
FROM LINEITEM l
JOIN ORDERS o ON l.l_orderkey = o.o_orderkey
JOIN CUSTOMER c ON o.o_custkey = c.c_custkey
WHERE c.c_nationkey IN (SELECT n_nationkey FROM HighestAvgBalanceNation);

-- 17) Average price of orders with no parts larger than size 40
SELECT AVG(o.o_totalprice)
FROM ORDERS o
WHERE NOT EXISTS (
    SELECT 1
    FROM LINEITEM l
    JOIN PART p ON l.l_partkey = p.p_partkey
    WHERE l.l_orderkey = o.o_orderkey AND p.p_size > 40
);

-- 18) Suppliers supplying more than 5 parts
SELECT s.s_name, COUNT(DISTINCT ps.ps_partkey) AS part_count
FROM SUPPLIER s
JOIN PARTSUPP ps ON s.s_suppkey = ps.ps_suppkey
GROUP BY s.s_name
HAVING COUNT(DISTINCT ps.ps_partkey) > 5;

-- 19) Distinct parts receiving the highest discount
SELECT DISTINCT l.l_partkey
FROM LINEITEM l
WHERE l.l_discount = (SELECT MAX(l_discount) FROM LINEITEM);