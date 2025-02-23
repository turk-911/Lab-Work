ALTER TABLE SUPPLIER ADD rating NUMBER DEFAULT 0;
CREATE OR REPLACE TRIGGER trg_update_supplier_rating
AFTER UPDATE OF status ON ORDERS
WHEN (NEW.status = 'SHIPPED')
BEGIN
    UPDATE SUPPLIER s
    SET s.rating = s.rating + 1
    WHERE s.suppkey IN (
        SELECT DISTINCT l.suppkey
        FROM LINEITEM l
        WHERE l.orderkey IN (
            SELECT orderkey FROM ORDERS WHERE status = 'SHIPPED'
        )
    );
END;
/