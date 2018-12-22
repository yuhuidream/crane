TARGETS := tmark.a
SUBDIRS := src

tmark.a_DEPS = TMark_main.o version.o $(SUBDIRS_TGTS)

INCLUDES_$(d) := $(d)/src $(d)/src/baremetal

MEMATTR_STEXCB=0x1f
TAUROS3_BASE=0x1e00a000 
PERIPH_BASE=0x40000000 
L2X0_BASE=0x58120000
ENABLE_TEXREMAP = 0
ifeq ($(CORE), cr5)
ENABLE_SL2 = 1
else
ENABLE_SL2 = 0
endif
#SET Timing way
TIME_WAY=-DBAREMETAL
VEXPRESS = 1

TAUROS3_BASE ?= 0xa0000000
L2X0_BASE   ?= 0x1e00a000
MEMATTR_STEXCB ?= 0x03	# S=0, TEX=3'b000, C=1, B=1
SMP ?= 0
PERIPH_BASE ?= 0x10100000
L2C_SIZE_MB ?= 512
ARCH ?= v7
SL2_FLUSH ?= 0
BCAST ?= 0
PJ4B_MP ?= 0
L2PREFETCH_OFFSET ?= 0xf

ifeq ($(ARCH),v7)
	 ARCH_CFLAG += -DCPU_V7
endif


ifeq ($(VEXPRESS),1)
    ARCH_CFLAG += -DVEXPRESS
else
    ARCH_CFLAG += -mtune=marvell-f -mmarvell-div
endif

#init some macros to enable some feature
CFLAGS_OPTS = -DL2C_SIZE_MB=$(L2C_SIZE_MB) -DMEMATTR_STEXCB=$(MEMATTR_STEXCB)  -DL2X0_BASE=$(L2X0_BASE) -DTAUROS3_BASE=$(TAUROS3_BASE) -DSMP=$(SMP) -DPERIPH_BASE=$(PERIPH_BASE) -DENABLE_RS=1 -DENABLE_DBP=1 -DENABLE_SBP=1 -DENABLE_STREAMING=1 -DGSHAREBIT=5 -DBCAST=$(BCAST)

ifeq ($(ENABLE_LPAE),1)
ifeq ($(SMP),1)
	LPAE_ATTR_CODE ?= 0xff
	LPAE_SHR_CODE ?= 0x3
else
	LPAE_ATTR_CODE ?= 0xee
	LPAE_SHR_CODE ?= 0x0
endif
	CFLAGS_OPTS += -DENABLE_LPAE -DLPAE_ATTR_CODE=$(LPAE_ATTR_CODE) -DLPAE_ATTR_TEST=$(LPAE_ATTR_TEST) -DLPAE_SHR_CODE=$(LPAE_SHR_CODE) -DLPAE_SHR_TEST=$(LPAE_SHR_TEST)
ifeq ($(PAGE_TABLE_4KB_ALLOCATE),1)
	CFLAGS_OPTS += -DPAGE_TABLE_4KB_ALLOCATE
endif
endif

ifeq ($(ENABLE_TEXREMAP),1)
	CFLAGS_OPTS += -DENABLE_TEXREMAP
	CFLAGS_OPTS += -DTRE_MEMATTR=$(TRE_MEMATTR)
ifeq ($(PAGE_TABLE_4KB_ALLOCATE),1)
	CFLAGS_OPTS += -DPAGE_TABLE_4KB_ALLOCATE
endif
endif

ifeq ($(ENABLE_SL2),1)
	CFLAGS_OPTS += -DENABLE_SL2=1
ifeq ($(ENABLE_L2PREFETCH),1)
	CFLAGS_OPTS += -DCONFIG_CACHE_TAUROS3_PREFETCH_ENABLE
	CFLAGS_OPTS += -DL2PREFETCH_OFFSET=$(L2PREFETCH_OFFSET)
endif
ifeq ($(SL2_FLUSH),1)
	CFLAGS_OPTS += -DSL2_FLUSH
endif
endif

ifeq ($(ENABLE_TCL2),1)
	CFLAGS_OPTS += -DENABLE_TCL2=1
ifeq ($(ENABLE_L2PREFETCH),1)
	CFLAGS_OPTS += -DTCL2_PREFETCH=1 -DTCL2_PREFETCH_BURST8=1
endif
endif


CFLAGS_$(d) += $(CFLAGS_OPTS) $(ARCH_CFLAG) -fno-builtin-printf -g -O3 --debug -fno-inline-functions -fno-jump-tables -fno-if-conversion -fno-if-conversion2 -fno-tree-loop-optimize -fno-tree-pre -fno-tree-fre -fsplit-ivs-in-unroller -falign-functions=8 -fpeel-loops -fivopts -funroll-loops -fsched2-use-traces -fno-rerun-cse-after-loop -D_TMARK_LINUX $(TIME_WAY)

# Make this flag cascade to whole subtree
INHERIT_DIR_VARS_$(d) := CFLAGS

