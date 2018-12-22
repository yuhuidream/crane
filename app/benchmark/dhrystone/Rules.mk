# You can use wildcards in SRCS - they are detected and expanded by this
# make system (not make itself).  You can always use builtin wildcard
# function e.g. SRCS := $(notdir $(wildcard ...))
SRCS := *.c
CFLAGS_$(d) := -O3 
ifeq ($(CORE), ca7)
CFLAGS_$(d) += -mthumb #--no_inline --no_multifile
endif

