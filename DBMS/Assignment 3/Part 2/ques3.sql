CREATE OR REPLACE PACKAGE Salary_Package IS
    FUNCTION Update_Salary_Fn(p_Emp_ID NUMBER) RETURN NUMBER;
    PROCEDURE Update_Salary_Proc(p_Emp_ID IN NUMBER, p_New_Salary OUT NUMBER, p_Old_Salary OUT NUMBER);
END Salary_Package;
/