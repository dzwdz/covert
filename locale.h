#define ERR_badgroup "\"%s\" isn't a valid group.\n"
#define ERR_exec "execv() call failed.\n"
#define ERR_fopen "Couldn't open %s.\n"
#define ERR_getgroups "getgroups() call failed.\n"
#define ERR_ngroups "You've exceeded the group limit.\n"
#define ERR_syntax "syntax error @ %s:%ld\n"

#define ERR_argc \
	"usage: covert2 command [argv]\n\n" \
	"Please use the 'covert' wrapper instead, this program wasn't " \
	"intended to be run directly by end users.\n"

#define ERR_setgroups \
	"covert: Unable to set the supplementary GIDs. " \
	"Is the binary privileged?\n"
