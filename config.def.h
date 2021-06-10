// yes, this include is pretty useless. it's only there to make editors shut up
// about the incomplete struct definition
#include "types.h"

const struct perms perms[] = {
	{"/bin/id", "floppy"},
	{"/bin/capsh", "scanner"},
	NULL
};
