#ifndef _DEFAULT_CALL_H
#define _DEFAULT_CALL_H

#include "types.h"

extern int32_t default_open(const int8_t* filename);
extern int32_t default_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t default_write(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t default_close(int32_t fd);

#endif
