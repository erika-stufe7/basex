#include "../include/basex.h"
#include <stdio.h>

// Common utilities shared across encodings

const char* basex_version(void) {
    static char version[32];
    snprintf(version, sizeof(version), "%d.%d.%d",
             BASEX_VERSION_MAJOR, BASEX_VERSION_MINOR, BASEX_VERSION_PATCH);
    return version;
}
