#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "util.h"

void exit_program(const char *format, ...) {
	va_list ap;
	va_start(ap, format);
	vfprintf(stdout, format, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}
