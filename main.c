#include <stdio.h>
#include "args.h"
#include "common.h"
#include "tracks.h"
#include "file.h"

static Record records[MAX_TOTAL_RECORDS];
static Target targets[MAX_TARGETS];

int main(int argc, char *argv[]) {
    int numRaw = 0;
    int targetCount = 0;
    Config config;

    int resultCode = parseArguments(argc, argv, &config);
    if (resultCode != SUCCESS) {
        return resultCode;
    }

    resultCode = readFile(config.filename, records, &numRaw);
    if (resultCode != SUCCESS) {
        return resultCode;
    }

    groupData(records, numRaw, targets, &targetCount);
    processTargets(targets, targetCount);
    printReport(config, targets, targetCount);

    return SUCCESS;
}
