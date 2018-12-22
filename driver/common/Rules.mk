# You can use wildcards in SRCS - they are detected and expanded by this
# make system (not make itself).  You can always use builtin wildcard
# function e.g. SRCS := $(notdir $(wildcard ...))
SRCS := *.c

ifeq ($(CONFIG_NATO), y)
SRCS_EXCLUDES += interrupt.c
else
SRCS_EXCLUDES += interrupt_nato.c
endif

CFLAGS_$(d) := -DCONFIG_ARCH_MMP
INCLUDES += $(d)

