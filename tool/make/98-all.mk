HELP_ME += \
"98-all.mk\n" \
"provides all and run.\n" \
"Takes in ALSO and TARGET as requirements to all/run.\n" \
"ALSO is everything written before (but not necessarily logically) TARGET.\n" \
"use .WAIT if proper ordering is not DAG implied.\n" \
"TARGET is your program output, which is modified by the origin GNUmakefile and 11-library.mk.\n" \
"run executes TARGET with ARGS, provide ARGS if you want those for your invocation.\n"

.DEFAULT_GOAL := all

all: ${ALSO} ${TARGET}

ARGS ?=
run: ${ALSO} ${TARGET}
	./${TARGET} ${ARGS}

.PHONY: all run
