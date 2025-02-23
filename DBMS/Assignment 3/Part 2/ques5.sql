ALTER TABLE PARTSUPP ADD available_stock NUMBER DEFAULT 100;
CREATE OR REPLACE TRIGGER trg_update_part_stock
AFTER UPDATE OF quantity ON LINEITEM
FOR EACH ROW
WHEN (NEW.quantity > OLD.quantity) 
BEGIN
    UPDATE PARTSUPP
    SET available_stock = available_stock - (NEW.quantity - OLD.quantity)
    WHERE partkey = :NEW.partkey
    AND suppkey = :NEW.suppkey;
END;
/