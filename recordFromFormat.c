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
#include "record.h"

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
        fprintf(stderr, "Failed to allocate memory.\n");
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
                if (courseCode == 0)
                {
                    break;
                }
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
    Record *record = newRecord();

    if (record == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for a Record\n");
        return NULL;
    }

    char *start = buffer;
    char *end = buffer + bufSize;
    uint8_t flags = *start++;
    int have_grade = 0;

    while (start < end)
    {
        if (flags & 0x01)
        {
            setSource(record, *start);
            start++;
            flags &= ~0x01;
        }
        else if (flags & 0x02)
        {
            setDest(record, *start);
            start++;
            flags &= ~0x02;
        }
        else if (flags & 0x04)
        {
            uint32_t username_len = ntohl(*(uint32_t *)(start));
            start += sizeof(uint32_t);
            setUsername(record, start);
            start += username_len;
            flags &= ~0x04;
        }
        else if (flags & 0x08)
        {
            uint32_t id = ntohl(*(uint32_t *)(start));
            setId(record, id);
            start += sizeof(uint32_t);
            flags &= ~0x08;
        }
        else if (flags & 0x10)
        {
            uint32_t group = ntohl(*(uint32_t *)(start));
            setGroup(record, group);
            start += sizeof(uint32_t);
            flags &= ~0x10;
        }
        else if (flags & 0x20)
        {
            setSemester(record, *start);
            start++;
            flags &= ~0x20;
        }
        else if (flags & 0x40)
        {
            have_grade = 1;
            Grade grade = (Grade)(*start++);
            setGrade(record, grade);
            flags &= ~0x40;
        }
        else if (flags & 0x80)
        {
            uint16_t courses = ntohs(*(uint16_t *)(start));
            setCourse(record, courses);
            start += sizeof(uint16_t);
            flags &= ~0x80;
        }
        else
        {
            break;
        }
    }

    if (!have_grade)
    {
        setGrade(record, Grade_None);
    }

    if (bytesread != NULL)
    {
        *bytesread = start - buffer;
    }

    return record;
}
