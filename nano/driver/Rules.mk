SUBDIRS := common printer timer uart qspi

ifeq ($(CORE), cr5)
ifeq ($(CONFIG_NANO_SDH), y)
SUBDIRS += sdh
endif
SUBDIRS += pmic i2c geu ddr
INCLUDES += $(d)/pmu
#pmu
endif

ifeq ($(CORE), ca7)
SUBDIRS += amp ddr
INCLUDES += $(d)/i2c $(d)/pmu $(d)/pmic
endif

ifeq ($(CONFIG_MMT), y)
SUBDIRS += usb_nano
endif
