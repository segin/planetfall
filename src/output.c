#include <stdio.h>
#include <stdarg.h>
#include "output.h"

static bool scripting_enabled = false;
static FILE* script_file = NULL;

void set_scripting(bool enabled) {
    if (enabled) {
        if (!scripting_enabled) {
            script_file = fopen("planetfall.scr", "a");
            if (script_file) {
                scripting_enabled = true;
                printf("Scripting enabled. Output being written to planetfall.scr\n");
            } else {
                printf("Failed to open script file.\n");
            }
        } else {
            printf("Scripting already enabled.\n");
        }
    } else {
        if (scripting_enabled) {
            if (script_file) fclose(script_file);
            script_file = NULL;
            scripting_enabled = false;
            printf("Scripting disabled.\n");
        } else {
            printf("Scripting already disabled.\n");
        }
    }
}

bool is_scripting() {
    return scripting_enabled;
}

void tell(const char* str) {
    printf("%s", str);
    if (scripting_enabled && script_file) {
        fprintf(script_file, "%s", str);
        fflush(script_file);
    }
}

void tell_ln(const char* str) {
    printf("%s\n", str);
    if (scripting_enabled && script_file) {
        fprintf(script_file, "%s\n", str);
        fflush(script_file);
    }
}

void tellf(const char* fmt, ...) {
    va_list args;
    
    // Print to stdout
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    
    // Print to file
    if (scripting_enabled && script_file) {
        va_start(args, fmt);
        vfprintf(script_file, fmt, args);
        va_end(args);
        fflush(script_file);
    }
}
