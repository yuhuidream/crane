# You can use wildcards in SRCS - they are detected and expanded by this
# make system (not make itself).  You can always use builtin wildcard
# function e.g. SRCS := $(notdir $(wildcard ...))


SRCS := *.c

CFLAGS_$(d) += -O2 -DITERATIONS=100000
FLAGS_STR = "$(filter-out -D%, $(CFLAGS) $(CFLAGS_$(d)))"
CPPFLAGS_$(d) := -DFLAGS_STR=\"$(FLAGS_STR)\"
INCLUDES += $(d)

