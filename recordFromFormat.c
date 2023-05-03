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
#include <libxml/parser.h>

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
        return 0; // no course found
    }
}

Record *XMLtoRecord(char *buffer, int bufSize, int *bytesread)
{
    Record *record = malloc(sizeof(Record));
    if (record == NULL)
    {
        fprintf(stderr, "%s:%d Failed to allocate memory for a Record\n", __FILE__, __LINE__);
        return NULL;
    }

    memset(record, 0, sizeof(Record));
    char *start = buffer;
    char *end = buffer + bufSize;
    int have_grade = 0;

    if (start == NULL || end == NULL)
    {
        return NULL;
    }

    while (start < end)
    {
        if (strncmp(start, "<source=", 8) == 0)
        {
            start += 8;
            setSource(record, *start);
            start += 3;
        }
        else if (strncmp(start, "<dest=", 6) == 0)
        {
            start += 6;
            setDest(record, *start);
            start += 3;
        }
        else if (strncmp(start, "<username=", 10) == 0)
        {
            start += 10;
            char *username = NULL;
            sscanf(start, "\"%m[^\"]\"", &username);
            setUsername(record, username);
            free(username);
            start += strlen(record->username) + 2;
        }
        else if (strncmp(start, "<id", 4) == 0)
        {
            start += 4;
            uint32_t id = 0;
            sscanf(start, "\"%u\"", &id);
            setId(record, id);
            start += strlen(start) + 1;
        }
        else if (strncmp(start, "<group=", 7) == 0)
        {
            start += 7;
            uint32_t group = 0;
            sscanf(start, "\"%u\"", &group);
            setGroup(record, group);
            start += strlen(start) + 1;
        }
        else if (strncmp(start, "<grade=", 7) == 0)
        {
            have_grade = 1;
            start += 7;
            if (strncmp(start, "PhD", 3) == 0)
            {
                setGrade(record, Grade_PhD);
                start += 3;
            }
            else if (strncmp(start, "Master", 3) == 0)
            {
                setGrade(record, Grade_Master);
                start += 3;
            }
            else if (strncmp(start, "Bachelor", 3) == 0)
            {
                setGrade(record, Grade_Bachelor);
                start += 3;
            }
        }
        else if (strncmp(start, "<course=", 8) == 0)
        {
            start += 9;
            while (strncmp(start, "</courses>", 10) != 0)
            {
                char course[8] = "";
                sscanf(start, "\"%7[^\"]\"", course);
                start += strlen(course) + 4;
                int courseCode = getCourseCode(course);
                setCourse(record, courseCode);
            }
        }
        else
        {
            start = strchr(start, '>') + 1;
        }
    }
    if (!have_grade)
    {
        setGrade(record, Grade_None);
    }

    return record;
}

Record *BinaryToRecord(char *buffer, int bufSize, int *bytesread)
{
    /* TO BE IMPLEMENTED */
    return NULL;
}
