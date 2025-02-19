DECLARE
    v_Emp_ID        NUMBER;
    v_Date_of_Joining DATE;
    v_Experience    NUMBER;
    v_Salary        NUMBER;
    v_Department    VARCHAR2(50);
    v_New_Salary    NUMBER;
BEGIN
    v_Emp_ID := &Enter_Employee_ID;
    SELECT Date_of_Joining, Salary, Department 
    INTO v_Date_of_Joining, v_Salary, v_Department 
    FROM Employees 
    WHERE Emp_ID = v_Emp_ID;
    v_Experience := TRUNC(MONTHS_BETWEEN(SYSDATE, v_Date_of_Joining) / 12);
    IF v_Experience < 3 THEN
        v_New_Salary := v_Salary * 1.10;
    ELSIF v_Department = 'Sales' AND v_Salary < 50000 THEN
        v_New_Salary := v_Salary * 1.15; 
    ELSE
        v_New_Salary := v_Salary;
    END IF;
    IF v_New_Salary != v_Salary THEN
        UPDATE Employees SET Salary = v_New_Salary WHERE Emp_ID = v_Emp_ID;
        INSERT INTO Salary_History (Emp_ID, Old_Salary, New_Salary, Update_Date)
        VALUES (v_Emp_ID, v_Salary, v_New_Salary, SYSDATE);
        DBMS_OUTPUT.PUT_LINE('Salary updated to: ' || v_New_Salary);
    ELSE
        DBMS_OUTPUT.PUT_LINE('No salary update needed.');
    END IF;
END;
