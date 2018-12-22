# You can use wildcards in SRCS - they are detected and expanded by this
# make system (not make itself).  You can always use builtin wildcard
# function e.g. SRCS := $(notdir $(wildcard ...))

ifeq ($(CONFIG_NANO_BL), y)
SRCS := bl_main.c
else
SRCS := main.c
endif

INCLUDES += $(d)

