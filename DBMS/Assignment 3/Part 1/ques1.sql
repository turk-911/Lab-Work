DECLARE
    v_roll_no NUMBER;
    v_dob DATE;
    v_age NUMBER;
    v_marks NUMBER;
    v_updated_marks NUMBER;
    v_course_id NUMBER;
    
    FUNCTION calculate_age(p_dob DATE) RETURN NUMBER IS
        v_years NUMBER;
    BEGIN
        v_years := TRUNC(MONTHS_BETWEEN(SYSDATE, p_dob) / 12);
        RETURN v_years;
    END;

BEGIN

    DBMS_OUTPUT.PUT_LINE('Enter Roll Number:');
    v_roll_no := &roll_no;

    SELECT dob INTO v_dob FROM students WHERE roll_no = v_roll_no;
    
    v_age := calculate_age(v_dob);
    
    SELECT marks INTO v_marks 
    FROM marks 
    WHERE roll_no = v_roll_no AND course_id = (SELECT course_id FROM courses WHERE course_name = 'DBMS');

    IF v_age < 18 THEN
        v_updated_marks := v_marks * 1.1;

        UPDATE marks 
        SET marks = v_updated_marks 
        WHERE roll_no = v_roll_no AND course_id = (SELECT course_id FROM courses WHERE course_name = 'DBMS');

        DBMS_OUTPUT.PUT_LINE('Marks updated successfully.');
        DBMS_OUTPUT.PUT_LINE('Updated Marks: ' || v_updated_marks);
    ELSE
        DBMS_OUTPUT.PUT_LINE('Marks not updated as age is 18 or above.');
    END IF;

    COMMIT;

EXCEPTION
    WHEN NO_DATA_FOUND THEN
        DBMS_OUTPUT.PUT_LINE('Student not found.');
    WHEN OTHERS THEN
        DBMS_OUTPUT.PUT_LINE('An error occurred: ' || SQLERRM);
END;
/