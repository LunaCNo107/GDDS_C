#include "../include/error.h"
#include <stdio.h>

bool gdds_error_enabled = false;
bool gdds_debug_enabled = false;

static char gdds_error_buffer[256] = "No error";

void gdds_enable_errors(void) { gdds_error_enabled = true; }
void gdds_enable_debug(void)  { gdds_debug_enabled = true; }

void gdds_set_error(const char *msg, const char *file, int line) {
    snprintf(gdds_error_buffer, sizeof(gdds_error_buffer), "Error: %s (%s:%d)", msg, file, line);
}

const char* gdds_get_error(void) {
    return gdds_error_buffer;
}