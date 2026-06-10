#ifndef GDDS_ERROR_DEBUG_H
#define GDDS_ERROR_DEBUG_H

#include <stdbool.h>
#include <assert.h>

void gdds_enable_errors(void);
void gdds_enable_debug(void);

void gdds_set_error(const char *msg, const char *file, int line);
const char* gdds_get_error(void);

// Shared control flags exposed via extern
extern bool gdds_error_enabled;
extern bool gdds_debug_enabled;

#define GDDS_SET_ERROR(msg) do { if (gdds_error_enabled) gdds_set_error((msg), __FILE__, __LINE__); } while(0)
#define GDDS_ASSERT(x)      do { if (gdds_debug_enabled) assert((x)); } while(0)

#endif