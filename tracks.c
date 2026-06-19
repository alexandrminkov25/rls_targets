#include <stdio.h>
#include "common.h"
#include "tracks.h"

void sortPointsByTime(Point* points, int n) {
    for (int i = 1; i < n; i++) {
        Point key = points[i];
        int j = i - 1;

        while (j >= 0 && points[j].timeMs > key.timeMs) {
            points[j + 1] = points[j];
            j--;
        }

        points[j + 1] = key;
    }
}

void sortTargetsByRange(Target *targets, int n) {
    for (int i = 0; i < n - 1; i++) {
        int k = i;

        for (int j = i + 1; j < n; j++) {
            if (targets[j].minRange < targets[k].minRange) {
                k = j;
            }
        }

        Target temp = targets[i]; 
        targets[i] = targets[k]; 
        targets[k] = temp;
    }
}

void groupData(Record *records, int numRecords, Target *targets, int *targetCount) {
    *targetCount = 0;

    for (int i = 0; i < numRecords; i++) {
        int index = -1;
        for (int j = 0; j < *targetCount; j++) {
            if (targets[j].id == records[i].targetId) { 
                index = j;
                break;
            }
        }

        if (index == -1) {
            if (*targetCount >= MAX_TARGETS) {
                continue;
            }

            index = (*targetCount)++;
            targets[index].id = records[i].targetId;
            targets[index].count = 0;
            targets[index].duplicates = 0;
        }

        Target *t = &targets[index];
        bool isDuplicate = false;

        for (int p = 0; p < t->count; p++) {
            if (t->points[p].timeMs == records[i].timeMs) {
                isDuplicate = true;
                t->duplicates++;

                fprintf(stderr, "Дубликат: ID=%d, время %d мс. ", t->id, t->points[p].timeMs);

                if (records[i].rangeM < t->points[p].rangeM) {
                    t->points[p].rangeM = records[i].rangeM;
                    t->points[p].azimuthDeg = records[i].azimuthDeg;

                    fprintf(stderr, "Обновлено (range %d).\n", records[i].rangeM);
                } 
                else {
                    fprintf(stderr, "Пропущено.\n");
                }

                break;
            }
        }

        if (!isDuplicate && t->count < MAX_POINTS_PER_TARGET) {
            t->points[t->count].timeMs = records[i].timeMs;
            t->points[t->count].rangeM = records[i].rangeM;
            t->points[t->count].azimuthDeg = records[i].azimuthDeg;
            t->count++;
        }
    }
}

void processTargets(Target *targets, int targetCount) {
    for (int i = 0; i < targetCount; i++) {
        Target *t = &targets[i];
        
        sortPointsByTime(t->points, t->count);

        t->minRange = t->points[0].rangeM;
        t->maxRange = t->points[0].rangeM;
        t->minRangeTime = t->points[0].timeMs;

        float azSum = 0;

        for (int j = 0; j < t->count; j++) {
            if (t->points[j].rangeM < t->minRange) {
                t->minRange = t->points[j].rangeM;
                t->minRangeTime = t->points[j].timeMs;
            }

            if (t->points[j].rangeM > t->maxRange) {
                t->maxRange = t->points[j].rangeM;
            }

            azSum += t->points[j].azimuthDeg;
        }

        t->avgAzimuth = azSum / t->count;

        if (t->count >= 3) {
            float dR = (float)(t->points[t->count - 1].rangeM - t->points[0].rangeM);
            float dT = (float)(t->points[t->count - 1].timeMs - t->points[0].timeMs) / 1000.0f;

            t->speed = (dT > 0) ? (-dR / dT) : 0;
            t->hasSpeed = true;

        } 
        else {
            t->hasSpeed = false;
        }
    }

    sortTargetsByRange(targets, targetCount);
}

void printReport(Config config, Target *targets, int targetCount) {
    int totalRecords = 0;
    int totalDuplicates = 0;
    int targetsPassingSector = 0;

    for (int i = 0; i < targetCount; i++) {
        totalRecords += targets[i].count;
        totalDuplicates += targets[i].duplicates;

        if (config.hasSector) {
            if (targets[i].avgAzimuth >= config.az1 && targets[i].avgAzimuth <= config.az2) {
                targetsPassingSector++;
            }
        } 
        else {
            targetsPassingSector++;
        }
    }

    printf("=== Отчёт по журналу: %s ===\n", config.filename);
    
    if (config.hasSector) {
        int shown = (config.hasTop && config.n < targetsPassingSector) ? config.n : targetsPassingSector;

        printf("Фильтр: сектор %.1f° — %.1f°  (показаны %d из %d целей)\n", 
            config.az1, config.az2, shown, targetCount);
    }

    printf("Всего целей: %-2d |  Всего отметок: %-3d |  Дублей: %-2d\n\n", 
        targetCount, totalRecords, totalDuplicates);

    printf("ID  Отметок  Дальн.мин(м)  Дальн.макс(м)  Азимут.ср(°)  Скорость(м/с)\n");
    printf("---  -------  ------------  -------------  ------------  -------------\n");

    int printedCount = 0;
    int index = -1; 

    for (int i = 0; i < targetCount; i++) {
        if (config.hasSector) {
            if (targets[i].avgAzimuth < config.az1 || targets[i].avgAzimuth > config.az2) {
                continue; 
            }
        }

        if (config.hasTop && printedCount >= config.n) {
            break;
        }

        if (index == -1) {
            index = i;
        }

        printf("%-3u  %7d  %12d  %13d  %11.1f°   ", 
               targets[i].id, 
               targets[i].count, 
               targets[i].minRange, 
               targets[i].maxRange, 
               targets[i].avgAzimuth);

        if (targets[i].hasSpeed) {
            printf("%+13.1f\n", targets[i].speed);
        } 
        else {
            printf("           --\n");
        }

        printedCount++;
    }

    if (index != -1) {
        printf("\nБлижайшая цель: ID=%u, минимальная дальность %d м (время %d мс)\n", 
            targets[index].id, 
            targets[index].minRange, 
            targets[index].minRangeTime);
    }
}
