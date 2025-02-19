CREATE OR REPLACE FUNCTION Update_Salary_Fn(p_Emp_ID NUMBER) RETURN NUMBER IS
    CURSOR emp_cursor IS 
        SELECT Emp_ID, Date_of_Joining, Salary, Department 
        FROM Employees WHERE Emp_ID = p_Emp_ID;
    
    v_Experience NUMBER;
    v_New_Salary NUMBER;
BEGIN
    FOR emp_rec IN emp_cursor LOOP
        v_Experience := TRUNC(MONTHS_BETWEEN(SYSDATE, emp_rec.Date_of_Joining) / 12);

        IF v_Experience < 3 THEN
            v_New_Salary := emp_rec.Salary * 1.10;
        ELSIF emp_rec.Department = 'Sales' AND emp_rec.Salary < 50000 THEN
            v_New_Salary := emp_rec.Salary * 1.15;
        ELSE
            v_New_Salary := emp_rec.Salary;
        END IF;

        -- Update salary
        IF v_New_Salary != emp_rec.Salary THEN
            UPDATE Employees SET Salary = v_New_Salary WHERE Emp_ID = emp_rec.Emp_ID;
            INSERT INTO Salary_History (Emp_ID, Old_Salary, New_Salary, Update_Date)
            VALUES (emp_rec.Emp_ID, emp_rec.Salary, v_New_Salary, SYSDATE);
        END IF;
    END LOOP;

    RETURN v_New_Salary;
END;
