#ifndef _SRC_LOGGER_H
#define _SRC_LOGGER_H

#include <stdio.h>

#define LOGGER_INFO(LABEL, FORMAT, ...) \
	fprintf(stdout, "[INFO@%s]: " FORMAT, LABEL, ## __VA_ARGS__)

#endif
