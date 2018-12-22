SRCS := *.c *.S
ifeq ($(CORE), ca7)
SUBDIRS := ca7 
endif

ifeq ($(CORE), cr5)
SUBDIRS := cr5 
endif
