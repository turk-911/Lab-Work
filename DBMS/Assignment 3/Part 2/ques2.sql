CREATE OR REPLACE PROCEDURE update_marks_for_young_students_proc(
    p_roll_no IN NUMBER, 
    p_updated_marks OUT NUMBER, 
    p_message OUT VARCHAR2
) IS
    -- Cursor to fetch student details
    CURSOR student_cursor IS 
        SELECT s.dob, m.marks, m.course_id 
        FROM students s 
        JOIN marks m ON s.roll_no = m.roll_no 
        JOIN courses c ON m.course_id = c.course_id 
        WHERE s.roll_no = p_roll_no AND c.course_name = 'DBMS';

    -- Variables
    v_dob DATE;
    v_age NUMBER;
    v_marks NUMBER;
    v_course_id NUMBER;
    v_no_student EXCEPTION; -- User-defined exception
    v_updated_marks NUMBER;

    -- Function to calculate age
    FUNCTION calculate_age(p_dob DATE) RETURN NUMBER IS
    BEGIN
        RETURN TRUNC(MONTHS_BETWEEN(SYSDATE, p_dob) / 12);
    END;

BEGIN
    -- Open cursor and process student details
    FOR student_rec IN student_cursor LOOP
        v_dob := student_rec.dob;
        v_marks := student_rec.marks;
        v_course_id := student_rec.course_id;

        -- Calculate age
        v_age := calculate_age(v_dob);

        -- Check if age < 18
        IF v_age < 18 THEN
            -- Increase marks by 10%
            v_updated_marks := v_marks * 1.1;

            -- Update marks in the database
            UPDATE marks 
            SET marks = v_updated_marks 
            WHERE roll_no = p_roll_no AND course_id = v_course_id;

            p_updated_marks := v_updated_marks;
            p_message := 'Marks updated successfully to ' || v_updated_marks;
        ELSE
            p_updated_marks := v_marks;
            p_message := 'Marks not updated as age is 18 or above.';
        END IF;

        RETURN;
    END LOOP;

    -- If no student found, raise a user-defined exception
    RAISE v_no_student;

EXCEPTION
    WHEN v_no_student THEN
        p_message := 'Error: Student not found or not enrolled in DBMS.';
        p_updated_marks := NULL;
    WHEN OTHERS THEN
        p_message := 'An error occurred: ' || SQLERRM;
        p_updated_marks := NULL;
END update_marks_for_young_students_proc;
/