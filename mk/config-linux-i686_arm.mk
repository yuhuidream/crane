# How the "cross tools" should be invoked
CROSS_COMPILE = arm-none-eabi-
CC := $(CROSS_COMPILE)gcc
CXX := $(CROSS_COMPILE)g++
LINK := $(CROSS_COMPILE)gcc
AS := $(CROSS_COMPILE)as
AR := $(CROSS_COMPILE)ar
RANLIB := $(CROSS_COMPILE)ranlib
OBJCOPY := $(CROSS_COMPILE)objcopy

# Since we are compiling for a "remote target" we have to set this manually
ENDIAN := little

# Any other target specific settings
#CPPFLAGS += -DARM
