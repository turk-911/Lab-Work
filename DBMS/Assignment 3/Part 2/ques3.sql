CREATE OR REPLACE PACKAGE student_marks_pkg AS
    -- Function to update marks based on age
    FUNCTION update_marks_for_young_students(p_roll_no NUMBER) RETURN VARCHAR2;

    -- Procedure to update marks and return multiple values
    PROCEDURE update_marks_for_young_students_proc(
        p_roll_no IN NUMBER, 
        p_updated_marks OUT NUMBER, 
        p_message OUT VARCHAR2
    );
END student_marks_pkg;
/

CREATE OR REPLACE PACKAGE BODY student_marks_pkg AS

    -- Function to update marks based on age
    FUNCTION update_marks_for_young_students(p_roll_no NUMBER) RETURN VARCHAR2 IS
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

    -- Procedure to update marks and return multiple values
    PROCEDURE update_marks_for_young_students_proc(
        p_roll_no IN NUMBER, 
        p_updated_marks OUT NUMBER, 
        p_message OUT VARCHAR2
    ) IS
        CURSOR student_cursor IS 
            SELECT s.dob, m.marks, m.course_id 
            FROM students s 
            JOIN marks m ON s.roll_no = m.roll_no 
            JOIN courses c ON m.course_id = c.course_id 
            WHERE s.roll_no = p_roll_no AND c.course_name = 'DBMS';

        v_dob DATE;
        v_age NUMBER;
        v_marks NUMBER;
        v_course_id NUMBER;
        v_no_student EXCEPTION;
        v_updated_marks NUMBER;

        -- Function to calculate age
        FUNCTION calculate_age(p_dob DATE) RETURN NUMBER IS
        BEGIN
            RETURN TRUNC(MONTHS_BETWEEN(SYSDATE, p_dob) / 12);
        END;

    BEGIN
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

        -- Raise exception if no student found
        RAISE v_no_student;

    EXCEPTION
        WHEN v_no_student THEN
            p_message := 'Error: Student not found or not enrolled in DBMS.';
            p_updated_marks := NULL;
        WHEN OTHERS THEN
            p_message := 'An error occurred: ' || SQLERRM;
            p_updated_marks := NULL;
    END update_marks_for_young_students_proc;

END student_marks_pkg;
/

DECLARE
    v_roll_no NUMBER := 101;  -- Change to a valid roll number
    v_updated_marks NUMBER;
    v_message VARCHAR2(100);
BEGIN
    -- Call the function
    v_message := student_marks_pkg.update_marks_for_young_students(v_roll_no);
    DBMS_OUTPUT.PUT_LINE('Function Call Output: ' || v_message);

    -- Call the procedure
    student_marks_pkg.update_marks_for_young_students_proc(v_roll_no, v_updated_marks, v_message);
    DBMS_OUTPUT.PUT_LINE('Procedure Call Output: ' || v_message);
    
    IF v_updated_marks IS NOT NULL THEN
        DBMS_OUTPUT.PUT_LINE('Updated Marks: ' || v_updated_marks);
    END IF;
END;
/