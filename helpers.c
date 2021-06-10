#include "helpers.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

_Noreturn void die(const char *err, ...) {
	va_list argp;
	va_start(argp, err);
	vprintf(err, argp);
	va_end(argp);

	exit(1);
}

gid_t find_group(const char *name) {
	FILE *fp = fopen("/etc/group", "r");
	if (fp == NULL) die("couldn't open /etc/group\n");

	char *line = NULL, *token = NULL;
	size_t len = 0;
	ssize_t read;

	while ((read = getline(&line, &len, fp)) != -1) {
		token = strtok(line, ":");
		if (strcmp(token, name) == 0) {
			token = strtok(NULL, ":");
			token = strtok(NULL, ":");
			return atoi(token);
		}
	}

	die("unknown group: %s\n", name);
}

const char *full_path(const char *name) {
	// TODO traverse PATH
	return name;
}
