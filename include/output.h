#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdbool.h>

void set_scripting(bool enabled);
bool is_scripting();

void tell(const char *str);
void tell_ln(const char *str);
void tellf(const char *fmt, ...);

#define TELL(...) tellf(__VA_ARGS__)
#define CR tellf("\n")
#define CRLF tellf("\n")

#endif
