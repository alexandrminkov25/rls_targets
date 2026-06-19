#ifndef ARGS_H
#define ARGS_H

#include <stdbool.h>

typedef struct {
    const char *filename;
    float az1;
    float az2;
    int n;
    bool hasSector;
    bool hasTop;    
} Config;

int parseArguments(int argc, char *argv[], Config *config);

#endif
