#include <stdio.h>
#include "file.h"
#include "common.h"

int readFile(const char *filename, Record *records, int *numRecords) {
    FILE *file = fopen(filename, "r");

    if (!file) {
        fprintf(stderr, "Ошибка: файл не найден: %s\n", filename);
        return FILE_ERROR;
    }

    char buffer[256];
    int lineCount = 0;
    int acceptedCount = 0;
    int skippedCount = 0;
    *numRecords = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        lineCount++;

        if (buffer[0] == '#' || buffer[0] == '\n' || buffer[0] == '\r') {
            lineCount--;
            continue;
        }

        int timeMs, rangeM;
        unsigned int targetId;
        float azimuthDeg;
        int parsed = sscanf(buffer, "%d %u %d %f", &timeMs, &targetId, &rangeM, &azimuthDeg);

        if (parsed != 4) {
            fprintf(stderr, "Строка %d: пропущена — неверный формат (ожидалось 4 поля, найдено %d)\n", lineCount, parsed);
            skippedCount++;
            continue;
        }

        if (targetId < 1 || targetId > 255) {
            fprintf(stderr, "Строка %d: пропущена — target_id=%u вне диапазона [1, 255]\n", lineCount, targetId);
            skippedCount++;
            continue;
        }

        if (azimuthDeg < 0.0f || azimuthDeg >= 360.0f) {
            fprintf(stderr, "Строка %d: пропущена — azimuth_deg=%.1f вне диапазона [0, 360)\n", lineCount, azimuthDeg);
            skippedCount++;
            continue;
        }

        if (rangeM <= 0) {
            fprintf(stderr, "Строка %d: пропущена — range_m=%d должно быть положительным\n", lineCount, rangeM);
            skippedCount++;
            continue;
        }

        records[acceptedCount].timeMs = timeMs;
        records[acceptedCount].targetId = (uint8_t)targetId;
        records[acceptedCount].rangeM = rangeM;
        records[acceptedCount].azimuthDeg = azimuthDeg;
        acceptedCount++;
    }

    fclose(file);
    *numRecords = acceptedCount;

    if (acceptedCount == 0) {
        fprintf(stderr, "Ошибка: Нет ни одной корректной записи\n");
        return NO_RECORDS_ERROR;
    }

    fprintf(stderr, "Итого: прочитано %d строк, принято %d, пропущено %d\n", lineCount, acceptedCount, skippedCount);

    return SUCCESS;
}
