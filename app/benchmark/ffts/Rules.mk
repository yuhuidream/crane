SRCS := *.c *.S

CFLAGS_$(d) += -mfloat-abi=softfp -mfpu=neon
