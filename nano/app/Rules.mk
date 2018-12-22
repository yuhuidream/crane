SUBDIRS := cli main memory common

#ifneq ($(CORE), cr5)
#SUBDIRS += memory common
#else
#INCLUDES += $(d)/memory $(d)/common
#endif
