CREATE OR REPLACE TRIGGER Dept_Change_Trigger
BEFORE UPDATE OF Department ON Employees
FOR EACH ROW
BEGIN
    INSERT INTO Department_Change_Log (Emp_ID, Old_Department, New_Department, Change_Date)
    VALUES (:OLD.Emp_ID, :OLD.Department, :NEW.Department, SYSDATE);
END;
/