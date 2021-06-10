#include <sys/types.h>

_Noreturn void die(const char *err, ...);
gid_t find_group(const char *name);
const char *full_path(const char *name);
