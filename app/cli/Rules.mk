# You can use wildcards in SRCS - they are detected and expanded by this
# make system (not make itself).  You can always use builtin wildcard
# function e.g. SRCS := $(notdir $(wildcard ...))
SRCS += cli.c
SRCS += cli_comp.c
ifeq ($(CONFIG_QS_EXT),y)
SRCS += cli_qspy_comp.c
SRCS += spy_multi_filter.c
endif
INCLUDES += $(d)

