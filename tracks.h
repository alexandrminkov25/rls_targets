#ifndef TRACKS_H
#define TRACKS_H

#include <stdbool.h>
#include "file.h"
#include "args.h"

typedef struct {
    int timeMs;
    int rangeM;
    float azimuthDeg;
} Point;

typedef struct {
    uint8_t id;
    Point points[MAX_POINTS_PER_TARGET];
    int count;
    int duplicates;
    
    int minRange;
    int maxRange;
    int minRangeTime;
    float avgAzimuth;
    float speed;
    bool hasSpeed;
} Target;

void groupData(Record *records, int numRecords, Target *targets, int *targetCount);
void processTargets(Target *targets, int targetCount);
void printReport(Config config, Target *targets, int targetCount);

#endif
