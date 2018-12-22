ifeq ($(CONFIG_D2ONLY),y)
SUBDIRS := d2cp
else
SUBDIRS := arch app driver
ifeq ($(CONFIG_SWP_DDR_ONLY),y)
CFLAGS += -DCONFIG_SWP_DDR_ONLY
endif
endif
