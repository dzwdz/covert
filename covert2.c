#include <grp.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DIE(...) {fprintf(stderr, __VA_ARGS__); exit(1);}

#define CONFIG_PATH "/etc/covert"

gid_t groups[NGROUPS_MAX];
int og_groupamt, new_groupamt;

bool is_in_group(gid_t gid);
gid_t find_group(const char *name);
void push_group(gid_t gid);
void apply_config();


bool is_in_group(gid_t gid) {
	for (int i = 0; i < og_groupamt; i++) {
		if (groups[i] == gid) return true;
	}
	return false;
}

gid_t find_group(const char *name) {
	struct group *grp = getgrnam(name);
	if (!grp) DIE("%s isn't a valid group.\n", name);
	return grp->gr_gid;
}

void push_group(gid_t gid) {
	if (new_groupamt >= NGROUPS_MAX)
		DIE("you've hit the group limit\n");

	groups[new_groupamt++] = gid;
}

void apply_config(const char *program) {
	FILE *fp;
	char *line = NULL, *column = NULL, *group = NULL;
	char *saveptr1, *saveptr2;
	size_t len = 0, line_no = 0;
	ssize_t read;
	bool allowed;

	if (!(fp = fopen(CONFIG_PATH, "r")))
		DIE("couldn't read " CONFIG_PATH "\n");

	while ((read = getline(&line, &len, fp)) != -1) {
		line_no++;

		// first column - program name
		column = strtok_r(line, " \t\n", &saveptr1);
		if (!column || column[0] == '#') continue;
		if (strcmp(column, program) != 0) continue;

		// second column - groups that are allowed to elevate themselves
		column = strtok_r(NULL, " \t\n", &saveptr1);
		if (!column)
			DIE("syntax error @ %s:%ld\n", CONFIG_PATH, line_no);

		allowed = false;
		group = strtok_r(column, ",", &saveptr2);
		while (group != NULL) {
			if (*group == '*' || is_in_group(find_group(group))) {
				allowed = true;
				break;
			}
			group = strtok_r(NULL, ",", &saveptr2);
		}
		if (!allowed) continue;

		// third column - groups to add to the program
		column = strtok_r(NULL, " \t\n", &saveptr1);
		if (!column)
			DIE("syntax error @ %s:%ld\n", CONFIG_PATH, line_no);

		group = strtok_r(column, ",", &saveptr2);
		while (group != NULL) {
			push_group(find_group(group));
			group = strtok_r(NULL, ",", &saveptr2);
		}
	}

	fclose(fp);
}

int main(int argc, char *const *argv) {
	if (argc < 2) {
		DIE("usage: covert2 command [argv]\n\n" \
		    "Please use the 'covert' wrapper instead, this program wasn't " \
		    "intended to be run directly by end users.\n");
	}

	if ((og_groupamt = getgroups(NGROUPS_MAX, groups)) < 0)
		DIE("getgroups() call failed\n");
	new_groupamt = og_groupamt;
	
	apply_config(argv[1]);

	if (setgroups(new_groupamt, groups) < 0) {
		DIE("covert: Unable to set the supplementary GIDs. "
		    "Is the binary privileged?\n");
	}

	execv(argv[1], &argv[2]);
	DIE("execv() call failed\n");
}
