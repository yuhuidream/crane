# You can use wildcards in SRCS - they are detected and expanded by this
# make system (not make itself).  You can always use builtin wildcard
# function e.g. SRCS := $(notdir $(wildcard ...))
SRCS += rtc.c
SRCS += wdt.c
SRCS += clock.c
SRCS += delay-mmp.S
SRCS += timer_cr5.c

INCLUDES += $(d)

