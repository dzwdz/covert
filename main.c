#include <grp.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DIE(...) {fprintf(stderr, __VA_ARGS__); exit(1);}

#define CONFIG_PATH "/etc/covert"

gid_t groups[NGROUPS_MAX];
int groups_amt;

gid_t find_group(const char *name);
void push_group(gid_t gid);
void apply_config();


gid_t find_group(const char *name) {
	struct group *grp = getgrnam(name);
	if (!grp) DIE("%s isn't a valid group.\n", name);
	return grp->gr_gid;
}

void push_group(gid_t gid) {
	if (groups_amt >= NGROUPS_MAX)
		DIE("you've hit the group limit\n");

	groups[groups_amt++] = gid;
}

void apply_config(const char *program) {
	FILE *fp;
	char *line = NULL, *column = NULL, *group = NULL;
	char *saveptr1, *saveptr2;
	size_t len = 0;
	ssize_t read;

	if (!(fp = fopen(CONFIG_PATH, "r")))
		DIE("couldn't read " CONFIG_PATH "\n");

	while ((read = getline(&line, &len, fp)) != -1) {
		column = strtok_r(line, " \t\n", &saveptr1);
		if (!column || column[0] == '#') continue;

		if (strcmp(column, program) == 0) {
			column = strtok_r(NULL, " \t\n", &saveptr1);
			if (!column) continue; // TODO warn about misconfiguration

			group = strtok_r(column, ",", &saveptr2);
			while (group != NULL) {
				push_group(find_group(group));
				group = strtok_r(NULL, ",", &saveptr2);
			}
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

	if ((groups_amt = getgroups(NGROUPS_MAX, groups)) < 0)
		DIE("getgroups() call failed\n");
	
	apply_config(argv[1]);

	if (setgroups(groups_amt, groups) < 0) {
		DIE("covert: Unable to set the supplementary GIDs. "
		    "Is the binary privileged?\n");
	}

	execv(argv[1], &argv[2]);
	DIE("execv() call failed\n");
}
