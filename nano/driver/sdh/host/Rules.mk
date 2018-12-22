# You can use wildcards in SRCS - they are detected and expanded by this
# make system (not make itself).  You can always use builtin wildcard
# function e.g. SRCS := $(notdir $(wildcard ...))
SRCS := *.c
ifneq ($(CONFIG_MMC_CAP_HS200), y)
SRCS_EXCLUDES := sdh_tune.c
endif

INCLUDES += $(d)

