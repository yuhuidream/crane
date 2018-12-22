ifeq ($(PLATFORM), haps)
SRCS := lpddr2_haps.c
endif
ifeq ($(PLATFORM), zebu)
SRCS := init_lpddr2.c
endif
ifeq ($(PLATFORM), z1)
SRCS := z1_ddr_init.c
endif
