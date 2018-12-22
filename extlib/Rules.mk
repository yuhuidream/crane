# The libraries
ifeq ($(CONFIG_AUDIO),y)
BBU_LIBS += -L$(d) -lcodecmp3dec
endif

ifeq ($(CONFIG_OMX),y)
BBU_LIBS += -L$(d)/OMXlib -lomxIC
BBU_LIBS += -L$(d)/OMXlib -lomxAC
BBU_LIBS += -L$(d)/OMXlib -lomxIP
BBU_LIBS += -L$(d)/OMXlib -lomxVC
BBU_LIBS += -L$(d)/OMXlib -lomxSP
endif

ifneq ($(CONFIG_QPC), y)
ifeq ($(CORE), ca7)
BBU_LIBS += -L$(d) -lqpc_ca7
else ifeq ($(CORE), msa)
BBU_LIBS += -L$(d) -lqpc_msa
else
BBU_LIBS += -L$(d) -lqpc_cr5
endif
endif

ifeq ($(CONFIG_MLUCAS),y)
	ifeq ($(CONFIG_MLUCAS_LIB),y)
	   ifeq ($(CORE), cr5)
	      BBU_LIBS += -L$(d) -lmlucas_cr5
	   else
	      BBU_LIBS += -L$(d) -lmlucas_ca7
	   endif
	endif
endif

ifeq ($(CONFIG_ISP),y)
    ifeq ($(CONFIG_FVTS),y)
    BBU_LIBS += -L$(d)/ISP_FVTSlib -lDxOISP_Firmware
    BBU_LIBS += -L$(d)/ISP_FVTSlib -lcorrData
    BBU_LIBS += -L$(d)/ISP_FVTSlib -lDxOISP_MicroCode
    else ifeq ($(CONFIG_FVTS_FS),y)
    BBU_LIBS += -L$(d)/ISP_FVTSlib -lDxOISP_Firmware
    BBU_LIBS += -L$(d)/ISP_FVTSlib -lcorrData
    BBU_LIBS += -L$(d)/ISP_FVTSlib -lDxOISP_MicroCode
    else
    BBU_LIBS += -L$(d)/ISPlib -lDxOISP_Firmware
    BBU_LIBS += -L$(d)/ISPlib -lcorrData
    BBU_LIBS += -L$(d)/ISPlib -lDxOISP_MicroCode
    endif
endif

INCLUDES += $(d)/inc/qpc/include
ifeq ($(CONFIG_QK),y)
INCLUDES += $(d)/inc/qpc/ports/arm-cortex/qk/gnu
else
INCLUDES += $(d)/inc/qpc/ports/arm-cortex/vanilla/gnu
endif


