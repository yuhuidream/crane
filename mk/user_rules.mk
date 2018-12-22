# Actually the config and rules files were meant to be updated by user
# however in order to ease keeping up with the upstream I now include
# optional user_rules.mk so the user can add it's own rules there without
# modifying original files.  

# # assembler support

# we only support .S, not .s. which will go to gcc other than as, gcc support many syntax of C-similar in .S file    
AUTO_RULES += .o:.S

#use the make builtin rules which will invoke GCC to parse and automatically call AS to compile .S file
#you can check the rules by "make -p > log.txt" and check the log.txt
#DIR_ASFLAGS = $(ASFLAGS_$(@RD))
#ASFLAGS = -g $(DIR_ASFLAGS)
#COMPILE.S = $(call echo_cmd,AS $<) $(AS) $(ASFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

#The builtin rules is not ruled by the VERBOSE defined, so still add below explicitly here
COMPILE.S = $(COMPILE.c)

#Special rule to get easily C options for given file.  This can be
# handy for your code "assistant" (e.g. clang) that needs to know
# preprocessor options in order to parse file properly.  In that case
# you can run: make /path/to/foobar.c.CFLAGS | tail -1
# to get effective flags for foobar.c

%.CFLAGS : %
	@echo $(CFLAGS)


