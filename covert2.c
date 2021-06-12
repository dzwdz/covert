#include "locale.h"
#include <grp.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DIE(...) {fprintf(stderr, __VA_ARGS__); exit(1);}

#define CONFIG_PATH "/etc/covert"
#define WHITESPACE " \t\n"

gid_t groups[NGROUPS_MAX];
int og_groupamt, new_groupamt;

bool is_in_group(gid_t gid);
gid_t find_group(const char *name);
void push_group(gid_t gid);
void apply_rule(char *rule, const char *program, size_t line_no);
void apply_config();


bool is_in_group(gid_t gid) {
	for (int i = 0; i < og_groupamt; i++) {
		if (groups[i] == gid) return true;
	}
	return false;
}

gid_t find_group(const char *name) {
	struct group *grp = getgrnam(name);
	if (!grp) DIE(ERR_badgroup, name);
	return grp->gr_gid;
}

void push_group(gid_t gid) {
	if (new_groupamt >= NGROUPS_MAX) DIE(ERR_ngroups);
	groups[new_groupamt++] = gid;
}

void apply_rule(char *rule, const char *program, size_t line_no) {
	bool privileged = false;
	char *column = NULL, *group = NULL;
	char *sp1, *sp2;

	// first column - program name
	column = strtok_r(rule, WHITESPACE, &sp1);
	if (!column || column[0] == '#') return;
	if (strcmp(column, program) != 0) return;

	// second column - groups that are allowed to elevate themselves
	column = strtok_r(NULL, WHITESPACE, &sp1);
	if (!column) DIE(ERR_syntax, CONFIG_PATH, line_no);

	group = strtok_r(column, ",", &sp2);
	while (group != NULL && !privileged) {
		privileged |= (*group == '*') // * is a catchall "group"
		           || is_in_group(find_group(group));

		group = strtok_r(NULL, ",", &sp2);
	}
	if (!privileged) return;

	// third column - groups to add to the program
	column = strtok_r(NULL, WHITESPACE, &sp1);
	if (!column) DIE(ERR_syntax, CONFIG_PATH, line_no);

	group = strtok_r(column, ",", &sp2);
	while (group != NULL) {
		push_group(find_group(group));
		group = strtok_r(NULL, ",", &sp2);
	}
}

void apply_config(const char *program) {
	FILE *fp;
	char *buf = NULL;
	size_t buflen = 0, line_no = 0;

	if (!(fp = fopen(CONFIG_PATH, "r")))
		DIE(ERR_fopen, CONFIG_PATH);

	while (getline(&buf, &buflen, fp) != -1)
		apply_rule(buf, program, ++line_no);

	fclose(fp);
}

int main(int argc, char *const *argv) {
	if (argc < 2)
		DIE(ERR_argc);

	new_groupamt = og_groupamt = getgroups(NGROUPS_MAX, groups);
	if (og_groupamt < 0) DIE(ERR_getgroups);
	
	apply_config(argv[1]);

	if (setgroups(new_groupamt, groups) < 0)
		DIE(ERR_setgroups);

	execv(argv[1], &argv[2]);
	DIE(ERR_exec);
}
