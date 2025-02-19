CREATE OR REPLACE TRIGGER Salary_Update_Trigger
AFTER UPDATE OF Salary ON Employees
BEGIN
    INSERT INTO Salary_History (Emp_ID, Old_Salary, New_Salary, Update_Date)
    SELECT Emp_ID, :OLD.Salary, :NEW.Salary, SYSDATE FROM Employees WHERE Emp_ID = :OLD.Emp_ID;
END;
/