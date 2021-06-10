#include "types.h"
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"

_Noreturn void die(const char *err, ...);
gid_t find_group(const char *name);
void add_group(gid_t gid);
const char *full_path(const char *name);

gid_t groups[NGROUPS_MAX];
int groups_amt;

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

void add_group(gid_t gid) {
	if (groups_amt > NGROUPS_MAX) die("too many groups\n");

	groups[groups_amt++] = gid;
}

const char *full_path(const char *name) {
	// TODO traverse PATH
	return name;
}

int main(int argc, char *const *argv) {
	if (argc < 2) {
		printf("usage: %s command [arg ...]\n", argv[0]);
		return 1;
	}

	const char *path = full_path(argv[1]);

	groups_amt = getgroups(NGROUPS_MAX, groups);
	if (groups_amt == -1) die("getgroups() call failed\n");

	for (int i = 0; perms[i].exe != NULL; i++)
		if (strcmp(perms[i].exe, path) == 0)
			add_group(find_group(perms[i].group));

	if (setgroups(groups_amt, groups) == -1)
		die("setgroups() call failed: %s.\nis the binary privileged?\n", strerror(errno));

	execv(path, &argv[1]);

	die("execv() call failed\n");
}
