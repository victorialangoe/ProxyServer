/*
 * This file implements two functions that read XML and binary information from a buffer,
 * respectively, and return pointers to Record or NULL.
 *
 * *** YOU MUST IMPLEMENT THESE FUNCTIONS ***
 *
 * The parameters and return values of the existing functions must not be changed.
 * You can add function, definition etc. as required.
 */
#include "recordFromFormat.h"
#include <stdio.h>
#include <string.h>
#include "record.h"
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>

int getCourseCode(char *course)
{
    if (strcmp(course, "IN1000") == 0)
    {
        return Course_IN1000;
    }
    else if (strcmp(course, "IN1010") == 0)
    {
        return Course_IN1010;
    }
    else if (strcmp(course, "IN1020") == 0)
    {
        return Course_IN1020;
    }
    else if (strcmp(course, "IN1030") == 0)
    {
        return Course_IN1030;
    }
    else if (strcmp(course, "IN1050") == 0)
    {
        return Course_IN1050;
    }
    else if (strcmp(course, "IN1060") == 0)
    {
        return Course_IN1060;
    }
    else if (strcmp(course, "IN1080") == 0)
    {
        return Course_IN1080;
    }
    else if (strcmp(course, "IN1140") == 0)
    {
        return Course_IN1140;
    }
    else if (strcmp(course, "IN1150") == 0)
    {
        return Course_IN1150;
    }
    else if (strcmp(course, "IN1900") == 0)
    {
        return Course_IN1900;
    }
    else if (strcmp(course, "IN1910") == 0)
    {
        return Course_IN1910;
    }
    else
    {
        return 0;
    }
}

Record *XMLtoRecord(char *buffer, int bufSize, int *bytesread)
{
    Record *record = newRecord();
    if (record == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for a Record.\n");
        return NULL;
    }

    initRecord(record);

    char *start = buffer;
    char *end = buffer + bufSize;

    if (start == NULL || end == NULL)
    {
        return NULL;
    }

    while (start < end)
    {
        if (strstr(start, "<source=\"") != NULL)
        {
            start = strstr(start, "<source=\"") + 9; // Move to the start of the source value
            char source = *start;
            setSource(record, source);
            start += 3; // Move past the source value and closing tag
        }
        else if (strstr(start, "<dest=\"") != NULL)
        {
            start = strstr(start, "<dest=\"") + 7;
            char dest = *start;
            setDest(record, dest);
            start += 3;
        }
        else if (strstr(start, "<username=\"") != NULL)
        {
            start = strstr(start, "<username=\"") + 11;
            char *end_quote = strchr(start, '"');
            if (end_quote != NULL)
            {
                int username_length = end_quote - start;                       // Calculate the length of the username
                char *username = malloc((username_length + 1) * sizeof(char)); // Allocate memory for the username string
                strncpy(username, start, username_length);                     // Copy the username from the XML data
                username[username_length] = '\0';                              // Add a null-terminating character
                setUsername(record, username);
                free(username);        // Remember to free the allocated memory
                start = end_quote + 3; // Move past the username value and closing tag
            }
        }
        else if (strstr(start, "<id=\"") != NULL)
        {
            start = strstr(start, "<id=\"") + 5;
            char *end_quote = strchr(start, '"');
            if (end_quote != NULL)
            {
                int id_length = end_quote - start;
                char *id_str = malloc((id_length + 1) * sizeof(char));
                strncpy(id_str, start, id_length);
                id_str[id_length] = '\0';
                uint32_t id = atoi(id_str);
                setId(record, id);
                free(id_str);
                start = end_quote + 3;
            }
        }
        else if (strstr(start, "<group=\"") != NULL)
        {
            start = strstr(start, "<group=\"") + 8;
            char *end_quote = strchr(start, '"');
            if (end_quote != NULL)
            {
                int group_length = end_quote - start;
                char *group_str = malloc((group_length + 1) * sizeof(char));
                strncpy(group_str, start, group_length);
                group_str[group_length] = '\0';
                uint32_t group = atoi(group_str);
                setGroup(record, group);
                free(group_str);
                start = end_quote + 3;
            }
        }
        else if (strstr(start, "<semester=\"") != NULL)
        {
            start = strstr(start, "<semester=\"") + 11;
            char *end_quote = strchr(start, '"');
            if (end_quote != NULL)
            {
                int semester_length = end_quote - start;
                char *semester_str = malloc((semester_length + 1) * sizeof(char));
                strncpy(semester_str, start, semester_length);
                semester_str[semester_length] = '\0';
                uint8_t semester = atoi(semester_str);
                setSemester(record, semester);
                free(semester_str);
                start = end_quote + 3;
            }
        }
        else if (strstr(start, "<grade=\"") != NULL)
        {
            start = strstr(start, "<grade=\"") + 8;
            char *end_quote = strchr(start, '"');
            if (end_quote != NULL)
            {
                int grade_length = end_quote - start;
                char *grade_str = malloc((grade_length + 1) * sizeof(char));
                strncpy(grade_str, start, grade_length);
                grade_str[grade_length] = '\0';
                Grade grade = Grade_None;
                if (strcmp(grade_str, "Bachelor") == 0)
                {
                    grade = Grade_Bachelor;
                }
                else if (strcmp(grade_str, "Master") == 0)
                {
                    grade = Grade_Master;
                }
                else if (strcmp(grade_str, "PhD") == 0)
                {
                    grade = Grade_PhD;
                }
                printf("grade: %d\n", grade);
                setGrade(record, grade);
                free(grade_str);
                start = end_quote + 3;
            }
        }
        else if (strstr(start, "<courses>") != NULL)
        {
            start = strstr(start, "<courses>") + 9;
            while (strstr(start, "<course=\"") != NULL)
            {
                start = strstr(start, "<course=\"") + 9;
                char *end_quote = strchr(start, '"');
                if (end_quote != NULL)
                {
                    int course_length = end_quote - start;
                    char *course_str = malloc((course_length + 1) * sizeof(char));
                    strncpy(course_str, start, course_length);
                    course_str[course_length] = '\0';
                    int course_code = getCourseCode(course_str);
                    setCourse(record, course_code);
                    free(course_str);
                    start = end_quote + 4;
                }
            }
        }
        else
        {
            start++;
        }
    }
    clearRecord(record);
    return record;
}

Record *BinaryToRecord(char *buffer, int bufSize, int *bytesread)
{
    Record *record = newRecord();

    if (record == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for a Record\n");
        return NULL;
    }

    initRecord(record);

    char *start = buffer;
    char *end = buffer + bufSize;

    if (start == NULL || end == NULL)
    {
        return NULL;
    }

    uint8_t flags = *start++;
    *bytesread = 1;

    if (flags & FLAG_SRC)
    {
        if (start + 1 > end)
        {
            return NULL;
        }
        setSource(record, *start);
        start++;
        (*bytesread)++;
    }

    if (flags & FLAG_DST)
    {
        if (start + 1 > end)
        {
            return NULL;
        }
        setDest(record, *start);
        start++;
        (*bytesread)++;
    }

    if (flags & FLAG_USERNAME)
    {
        if (start + sizeof(uint32_t) > end)
        {
            return NULL;
        }
        uint32_t username_len = ntohl(*(uint32_t *)(start));
        start += sizeof(uint32_t);
        (*bytesread) += sizeof(uint32_t);

        if (start + username_len > end)
        {
            return NULL;
        }
        char *username = malloc(username_len + 1);
        memcpy(username, start, username_len);
        username[username_len] = '\0';
        setUsername(record, username);
        start += username_len;
        (*bytesread) += username_len;
        free(username);
    }

    if (flags & FLAG_ID)
    {
        if (start + sizeof(uint32_t) > end)
        {
            return NULL;
        }
        uint32_t id = ntohl(*(uint32_t *)(start));
        setId(record, id);
        start += sizeof(uint32_t);
        (*bytesread) += sizeof(uint32_t);
    }

    if (flags & FLAG_GROUP)
    {
        if (start + sizeof(uint32_t) > end)
        {
            return NULL;
        }
        uint32_t group = ntohl(*(uint32_t *)(start));
        setGroup(record, group);
        start += sizeof(uint32_t);
        (*bytesread) += sizeof(uint32_t);
    }

    if (flags & FLAG_SEMESTER)
    {
        if (start + 1 > end)
        {
            return NULL;
        }
        setSemester(record, *start);
        start++;
        (*bytesread)++;
    }

    if (flags & FLAG_GRADE)
    {
        if (start + 1 > end)
        {
            return NULL;
        }
        Grade grade = (Grade)(*start++);
        setGrade(record, grade);
        (*bytesread)++;
    }

    if (flags & FLAG_COURSES)
    {
        if (start + sizeof(uint16_t) > end)
        {
            return NULL;
        }
        uint16_t courses = ntohs(*(uint16_t *)(start));
        record->has_courses = true;
        record->courses = courses;
        start += sizeof(uint16_t);
        (*bytesread) += sizeof(uint16_t);
    }
    clearRecord(record);
    return record;
}
