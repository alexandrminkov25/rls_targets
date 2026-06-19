#ifndef FILE_H
#define FILE_H

#include <stdint.h>

#define MAX_TARGETS 64
#define MAX_POINTS_PER_TARGET 1000
#define MAX_TOTAL_RECORDS MAX_TARGETS * MAX_POINTS_PER_TARGET

typedef struct {
    int timeMs;
    uint8_t targetId;
    int rangeM;
    float azimuthDeg;
} Record;

int readFile(const char *filename, Record *records, int *numRecords);

#endif