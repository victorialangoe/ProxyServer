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

Record* XMLtoRecord( char* buffer, int bufSize, int* bytesread )
{
    Record* record = malloc(sizeof(Record));
    memset(record, 0, sizeof(Record));
    char* start = buffer;
    char* end = buffer+bufSize;
    
    if (start == NULL || end == NULL) {
        return NULL;
    }

    while (start < end) {
        if (strncmp(start, "<source=",8)==0 ){
            record->has_source = 1;
            start += 8;
            record->source = *start;
            start += 3;
        } else if(strncmp(start,"<dest=",6)==0){
            record->has_dest = 1;
            start += 6;
            record->dest = *start;
            start += 3;
        } else if(strncmp(start,"<username=",10)==0){
            record->has_username = 1;
            start += 10;
            

        }
    }


    return NULL;
}

Record* BinaryToRecord( char* buffer, int bufSize, int* bytesread )
{
    /* TO BE IMPLEMENTED */
    return NULL;
}

