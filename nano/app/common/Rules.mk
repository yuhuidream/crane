# You can use wildcards in SRCS - they are detected and expanded by this
# make system (not make itself).  You can always use builtin wildcard
# function e.g. SRCS := $(notdir $(wildcard ...))
SRCS := bbu_help.c utils.c 

ifeq ($(CONFIG_SV_MEM_TEST),y)
SRCS += mcu_generate_pattern.c
endif

ifeq ($(CORE), ca7)
SRCS += ddr_sweep.c ddr_sweep_driver.c read_write.c
endif

ifeq ($(CORE), cr5)
SRCS += bbu_setv.c common_tests.c 
endif

SRCS += dma_nano_test.c

INCLUDES += $(d)

