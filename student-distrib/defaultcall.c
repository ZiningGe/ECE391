#include "defaultcall.h"

/*--------------------------Default system calls---------------------------*/
int32_t default_open(const int8_t* filename) {
    return -1;
}
int32_t default_read(int32_t fd, void* buf, int32_t nbytes){
    return -1;
}
int32_t default_write(int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}
int32_t default_close(int32_t fd){
    return -1;
}

