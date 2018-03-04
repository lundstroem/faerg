#include "faerg_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void f_log(const char *fmt,...) {
    int rnd_string = 0;
    if(f_log_enabled) {
        size_t count = 255;
        char chars[256] = {0};
        va_list ap = {0};
        va_start(ap, fmt);
        vsnprintf(chars, count, fmt, ap);
        va_end(ap);
        rnd_string = rand()%INT8_MAX+100;
        printf("f_log:[%d]%s\n", rnd_string, chars);
    }
}
