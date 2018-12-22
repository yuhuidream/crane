# You can use wildcards in SRCS - they are detected and expanded by this
# make system (not make itself).  You can always use builtin wildcard
# function e.g. SRCS := $(notdir $(wildcard ...))
SRCS := *.c *.S

#trick to include the binary file
CFLAGS_$(d) += -Wa,-I,$(d)/cp/driver/pmu
SRCS_EXCLUDES := msabin.S
INCLUDES += $(d)

