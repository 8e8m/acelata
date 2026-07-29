HELP_ME += \
"03-depend.mk\n" \
"Automatically generates header dependencies for C/C++. Ensures effectively correct rebuilds.\n" \
"WILL CAUSE EXCESS EXTRA REBUILDS THAT ARE USELESS,\n" \
"if you run make over and over again, it won't produce the desired \"Nothing to be done for 'all'.\" for a number of iterations.\n" \
"provides DEPEND, which is included at the parse-time of each make invocation (i.e. second build required for initial .depend data to be useful)\n" \
"\n"

CFLAGS   += -MMD -MP
CXXFLAGS += -MMD -MP
DEPEND := $(wildcard ${OBJECT.dir}/*.d)

-include ${DEPEND}
