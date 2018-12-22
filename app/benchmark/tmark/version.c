#include "compile.h"
#include "predefines.h"

/* FIXED STRINGS! Don't touch! */
const char tmark_banner[] =
	"TMark version " TMARK_VERSION " (" TMARK_COMPILE_BY "@"
	TMARK_COMPILE_HOST ") (" TMARK_COMPILER ") " TMARK_COMPILE_TIME "\n";

void dump_tmark_banner()
{
	bbu_printf("%s\n", tmark_banner);
}
