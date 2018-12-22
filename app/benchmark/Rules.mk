# You can use wildcards in SRCS - they are detected and expanded by this
# make system (not make itself).  You can always use builtin wildcard
# function e.g. SRCS := $(notdir $(wildcard ...))
SUBDIRS := coremark linpack superpi dhrystone llcbench whetstone tmark

ifeq ($(CORE), ca7)
SUBDIRS += memcpy
endif

ifeq ($(CONFIG_FFTS), y)
SUBDIRS += ffts
endif

