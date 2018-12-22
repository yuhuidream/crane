# You can use wildcards in SRCS - they are detected and expanded by this
# make system (not make itself).  You can always use builtin wildcard
# function e.g. SRCS := $(notdir $(wildcard ...))

ifeq ($(CONFIG_MMT),y)
SUBDIRS :=usb_nano 
else
SUBDIRS := usb_sph 
INCLUDES += $(d)
endif

INCLUDES += $(d)/usb_device/usb_inc

