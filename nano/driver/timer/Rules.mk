# You can use wildcards in SRCS - they are detected and expanded by this
# make system (not make itself).  You can always use builtin wildcard
# function e.g. SRCS := $(notdir $(wildcard ...))

ifeq ($(CORE),ca7)
SRCS += timer_ca7.c
endif
ifeq ($(CORE),cr5)
SRCS += wdt.c timer_cr5.c
endif
INCLUDES += $(d)

