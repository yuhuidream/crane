#set config and related compile flags in the configuration file
#include $(d)/bbu.conf

# ... optional BBU test configuration, default is mini
ifdef BBU_CONF
  -include $(d)/conf/bbu-$(BBU_CONF).conf
  -include $(d)/conf/bbu.conf
  -include $(d)/conf/bbu_opt.conf
CFLAGS += -DCONF_STRING='"$(BBU_CONF)"' -DCONFIG_$(BBU_CONF)
endif

VERSION := $(shell \
	echo "\#define btime \"`date +%Y-%m-%d:%H:%M:%S`\"" > $(TOP)/bbuinfo.h)

CORE ?= cr5

ifeq ($(BBU_CONF),NANO)
TARGETS = nano_$(CORE).elf
else
TARGETS = $(CORE).elf
endif


ifeq ($(BBU_CONF),NANO)
 ifneq ($(CORE),msa)
  SUBDIRS = nano
 else
  SUBDIRS = msa
 endif
else
 ifneq ($(CORE),msa)
  ifeq ($(CORE),cr5)
   SUBDIRS = app arch driver extlib
  endif
  ifeq ($(CONFIG_QPC), y)
   SUBDIRS += qpc
  endif
 else
  ifeq ($(CORE),msa)
   SUBDIRS = msa extlib
  endif
 endif
endif

ifneq ($(BBU_CONF),NANO)
cr5.elf_DEPS =  $(call subtree_tgts,$(d)/app) \
		$(call subtree_tgts,$(d)/arch) \
		$(call subtree_tgts,$(d)/driver)

msa.elf_DEPS =  $(call subtree_tgts,$(d)/msa)

ifeq ($(CONFIG_QPC), y)
$(CORE).elf_DEPS +=	$(call subtree_tgts,$(d)/qpc)
endif

else
ifneq ($(CORE),msa)
nano_$(CORE).elf_DEPS =  $(call subtree_tgts,$(d)/nano)
else
nano_$(CORE).elf_DEPS =  $(call subtree_tgts,$(d)/msa)
endif
endif


# The 3rd party libraries
# BBU_LIBS value is assigned in the extlib
BBU_LIBS :=
$(CORE).elf_LIBS = $(BBU_LIBS)

ifneq ($(CORE),msa)

ifeq ($(CONFIG_TZ),y)
tz.elf_DEPS = $(call subtree_tgts,$(d)/stz)
endif

ifeq ($(CONFIG_FAT32),y)
SUBDIRS += fs
$(CORE).elf_DEPS += $(call subtree_tgts,$(d)/fs)
endif

ifeq ($(CORE), cr5)
ARM_CORE = cortex-r5
CFLAGS +=  -DCONFIG_CR5 -marm -mthumb-interwork -mfloat-abi=soft
endif
CFLAGS += -mno-unaligned-access -mabi=aapcs -mapcs -DCORE_NUM=$(CORE_NUM)
CFLAGS += -DCR5_AREA_ST=$(CR5_LOAD_ADDR) -DCA7_AREA_ST=$(CA7_LOAD_ADDR)
else

ARM_CORE = bf533-any
CFLAGS += -DCONFIG_MSA -DMTP -DUSER_MODE_STARTUP=FALSE -DMTP_MSA -DDEBUG_HEX=1 -D__ASM__  -DSSP_COMMS -mlong-calls
endif

ARFLAGS = rs
CFLAGS += -std=c99 -fms-extensions -mcpu=$(ARM_CORE)
CFLAGS += -fdata-sections -ffunction-sections

ifeq ($(CONFIG_SQU), y)
LD_SCRIPT_cr5 := $(d)/app/main/cr5_squ.lds
else
LD_SCRIPT_cr5 := $(d)/app/main/cr5.lds
endif

LD_SCRIPT_msa := $(d)/msa/app/main/msa.lds
ifneq ($(CORE), msa)
ifeq ($(CONFIG_D2ONLY), y)
LD_SCRIPT_NANO := $(d)/nano/app/main/$(CORE)_d2.lds
else
LD_SCRIPT_NANO := $(d)/nano/app/main/$(CORE)_nano_squ.lds
endif

ifeq ($(CONFIG_NANO_BL), y)
ifeq ($(CORE), cr5)
LD_SCRIPT_NANO := $(d)/nano/app/main/cr5_nano_squ.lds
endif
endif

else
LD_SCRIPT_NANO := $(d)/msa/nano/$(CORE)_nano.lds
endif

#bbu.elf_LDFLAGS := -Wl,--skip-vendor-check -Wl,--defsym=TOTAL_CORE=$(CORE_NUM) -T$(LD_SCRIPT_BBU) -Xlinker -Map=$(OBJPATH)/bbu.map
ifeq ($(CORE), msa)
$(CORE).elf_LDFLAGS := -nostartfiles -T $(LD_SCRIPT_$(CORE)) -Xlinker -Map=$(OBJPATH)/$(CORE).map
else
$(CORE).elf_LDFLAGS := -Wl,-dT,$(LD_SCRIPT_$(CORE)) -Xlinker -Map=$(OBJPATH)/$(CORE).map
endif
$(CORE).elf_LDFLAGS += -Wl,--gc-sections #-Wl,--print-gc-sections
$(CORE).elf_LDFLAGS += -Wl,--defsym=CR5_LOAD_ADDR=$(CR5_LOAD_ADDR)

ifeq ($(CORE), ca7)
$(CORE).elf_LDFLAGS += -Wl,--skip-vendor-check -Wl,--defsym=TOTAL_CORE=$(CORE_NUM)
endif

ifeq ($(BBU_CONF), NANO)
ifeq ($(CORE), msa)
nano_$(CORE).elf_LDFLAGS := -Wl,--gc-sections -nostartfiles -nostdlib -T $(LD_SCRIPT_NANO) -Xlinker -Map=$(OBJPATH)/nano_$(CORE).map
else
nano_$(CORE).elf_LDFLAGS := -Wl,--gc-sections -Wl,-nostdlib -Wl,-dT,$(LD_SCRIPT_NANO) -Xlinker -Map=$(OBJPATH)/nano_$(CORE).map
endif
endif

	#-nostdlib \    #don't use the standard system startup files or libraries when linking.
	#-L$(QP_PORT_DIR)/$(BIN_DIR)    #links to a libary directories, -llib, -Llibdir

ifeq ($(CONFIG_TZ),y)
LD_SCRIPT_TZ := $(d)/stz/tz.lds
tz.elf_LDFLAGS := -Wl,--defsym=TOTAL_CORE=$(CORE_NUM) -Wl,-dT,$(LD_SCRIPT_TZ) -Xlinker -Map=$(OBJPATH)/tz.map
tz.elf_LDFLAGS += -Wl,--gc-sections #-Wl,--print-gc-sections
endif

INCLUDES += $(d)/arch/asr/include
INCLUDES += $(d)/arch/asr

ifneq ($(CORE), msa)
INCLUDES += $(d)

ifeq ($(CORE), ca7)

INCLUDES += $(d)/driver/common
INCLUDES += $(d)/app/fat32
ifneq ($(BBU_CONF), NANO)
INCLUDES += $(d)/app/main
endif

ifeq ($(CONFIG_FAT32),y)
INCLUDES += $(d)/fs/fullfat/src
INCLUDES += $(d)/fs/exfat
endif

endif
endif

.PHONY : tar tar-% bin

# To get ID from a tar archive just run:
#   zcat nonrec-make.tgz | git get-tar-commit-id
tar : tar-HEAD
tar-% :
        @commit=$(subst tar-,,$@);\
        tarname=bbu-driver-$$(git describe --tags $$commit).tgz;\
        echo Archiving $$commit to $$tarname;\
        git archive $$commit driver | gzip > $$tarname

#bin :
#	for c in $(d)/obj/$(BUILD_MODE)/*.elf; do $(OBJCOPY) -O binary $$c $${c%.elf}.bin; done
#	$(OBJCOPY) -O binary  $(d)/obj/$(BUILD_MODE)/bbu.elf $(d)/obj/$(BUILD_MODE)/bbu.bin

# vim: set ft=make :

