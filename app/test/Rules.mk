SRCS := *.c *.S *.h
SUBDIRS := common

ifeq ($(CONFIG_ATE),y)
SUBDIRS += ate
endif

ifeq ($(CONFIG_SCREEN_CHIP),y)
SUBDIRS += screen
endif

INCLUDES_$(d) := $(d)

