CREATE OR REPLACE FUNCTION update_marks_for_young_students(p_roll_no NUMBER) RETURN VARCHAR2 IS
    CURSOR student_cursor IS 
        SELECT s.dob, m.marks, m.course_id 
        FROM students s 
        JOIN marks m ON s.roll_no = m.roll_no 
        JOIN courses c ON m.course_id = c.course_id 
        WHERE s.roll_no = p_roll_no AND c.course_name = 'DBMS';

    v_dob DATE;
    v_age NUMBER;
    v_marks NUMBER;
    v_updated_marks NUMBER;
    v_course_id NUMBER;
    v_message VARCHAR2(100);

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

        -- Check if age is less than 18
        IF v_age < 18 THEN
            -- Increase marks by 10%
            v_updated_marks := v_marks * 1.1;

            -- Update marks in the database
            UPDATE marks 
            SET marks = v_updated_marks 
            WHERE roll_no = p_roll_no AND course_id = v_course_id;

            v_message := 'Marks updated to ' || v_updated_marks;
        ELSE
            v_message := 'Marks not updated as age is 18 or above.';
        END IF;
    END LOOP;

    COMMIT;

    RETURN v_message;

EXCEPTION
    WHEN NO_DATA_FOUND THEN
        RETURN 'Student not found.';
    WHEN OTHERS THEN
        RETURN 'An error occurred: ' || SQLERRM;
END update_marks_for_young_students;
/