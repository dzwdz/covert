#include "helpers.h"
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

struct perms {
	const char *exe;
	const char *group;
};

const struct perms perms[] = {
	{"/bin/id", "floppy"},
	{"/bin/capsh", "scanner"},
	NULL
};

gid_t groups[NGROUPS_MAX];
int groups_amt;

void add_group(gid_t gid) {
	if (groups_amt > NGROUPS_MAX) die("too many groups\n");

	groups[groups_amt++] = gid;
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
