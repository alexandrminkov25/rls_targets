#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "args.h"
#include "common.h"

int parseArguments(int argc, char *argv[], Config *config) {
    if (argc < 2) {
        fprintf(stderr, "Использование: %s <файл> [--sector <az1> <az2>] [--top <N>] [--min-marks <minMarks>]\n", argv[0]);
        return ARGUMENT_ERROR;
    }

    config->filename = argv[1];
    config->hasSector = false;
    config->hasTop = false;
    config->hasMinMarks = false;
    config->az1 = 0.0f;
    config->az2 = 0.0f;
    config->n = 0;
    config->hasMinMarks = 0;

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--sector") == 0) {
            if (config->hasSector) {
                fprintf(stderr, "Ошибка: --sector уже указан\n");
                return ARGUMENT_ERROR;
            }

            if (i + 2 < argc) {
                char *endptr1, *endptr2;
                config->az1 = strtof(argv[++i], &endptr1);
                config->az2 = strtof(argv[++i], &endptr2);

                if (*endptr1 != '\0' || *endptr2 != '\0') {
                    fprintf(stderr, "Ошибка: Неверный формат аргументов для --sector\n");
                    return ARGUMENT_ERROR;
                }
                
                config->hasSector = true;
            }
            else {
                fprintf(stderr, "Ошибка: Неверное количество аргументов для --sector\n");
                return ARGUMENT_ERROR;
            }
        }
        else if (strcmp(argv[i], "--top") == 0) {
            if (config->hasTop) {
                fprintf(stderr, "Ошибка: --top уже указан\n");
                return ARGUMENT_ERROR;
            }

            if (i + 1 < argc) {
                char *endptr;
                config->n = (int)strtol(argv[++i], &endptr, 10);

                if (*endptr != '\0' || config->n < 0) {
                    fprintf(stderr, "Ошибка: Неверный формат аргумента для --top\n");
                    return ARGUMENT_ERROR;
                }
                
                config->hasTop = true;
            }
            else {
                fprintf(stderr, "Ошибка: Неверное количество аргументов для --top\n");
                return ARGUMENT_ERROR;
            }
        } 
        else if (strcmp(argv[i], "--min-marks") == 0) {
            if (config->hasMinMarks)  {
                fprintf(stderr, "Ошибка: --min-marks уже указан\n");
                return ARGUMENT_ERROR;
            }

            if (i + 1 < argc) {
                char *endptr;
                config->minMarks = (int)strtol(argv[++i], &endptr, 10);

                if (*endptr != '\0' || config->minMarks < 0) {
                    fprintf(stderr, "Ошибка: Неверный формат аргумента для --min-marks\n");
                    return ARGUMENT_ERROR;
                }

                config->hasMinMarks = true;
            } 
            else {
                fprintf(stderr, "Ошибка: Неверное количество аргументов для --min-marks\n");
                return ARGUMENT_ERROR;
            }
        }
        else {
            fprintf(stderr, "Ошибка: Неизвестный аргумент %s\n", argv[i]);
            return ARGUMENT_ERROR;
        }
    }
    
    return SUCCESS;
}
