CREATE OR REPLACE PROCEDURE Update_Salary_Proc(
    p_Emp_ID IN NUMBER,
    p_New_Salary OUT NUMBER,
    p_Old_Salary OUT NUMBER
) IS
    v_Date_of_Joining DATE;
    v_Experience NUMBER;
    v_Department VARCHAR2(50);
    
    e_Employee_Not_Found EXCEPTION;
BEGIN
    BEGIN
        SELECT Date_of_Joining, Salary, Department 
        INTO v_Date_of_Joining, p_Old_Salary, v_Department 
        FROM Employees WHERE Emp_ID = p_Emp_ID;
    EXCEPTION
        WHEN NO_DATA_FOUND THEN
            RAISE e_Employee_Not_Found;
    END;

    v_Experience := TRUNC(MONTHS_BETWEEN(SYSDATE, v_Date_of_Joining) / 12);

    IF v_Experience < 3 THEN
        p_New_Salary := p_Old_Salary * 1.10;
    ELSIF v_Department = 'Sales' AND p_Old_Salary < 50000 THEN
        p_New_Salary := p_Old_Salary * 1.15;
    ELSE
        p_New_Salary := p_Old_Salary;
    END IF;

    IF p_New_Salary != p_Old_Salary THEN
        UPDATE Employees SET Salary = p_New_Salary WHERE Emp_ID = p_Emp_ID;
        INSERT INTO Salary_History VALUES (p_Emp_ID, p_Old_Salary, p_New_Salary, SYSDATE);
    END IF;

EXCEPTION
    WHEN e_Employee_Not_Found THEN
        DBMS_OUTPUT.PUT_LINE('Error: Employee Not Found!');
END;
/