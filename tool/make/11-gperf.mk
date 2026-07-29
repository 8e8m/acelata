HELP_ME += \
"11-gperf.mk\n" \
"Automatic gperf support. By default function shape: filename_lookup hash_lookup\n" \
"Change 11-gperf for different function shape\n" \
"Chains in GPERF.orig.\n" \
"Provides GPERF / GPERFLAGS configurable for overrides.\n"

GPERF      ?= gperf
GPERFLAGS  = --null-strings -tEI
GPERF.orig := $(call search, gperf)
GPERF.tmp  := $(GPERF.orig)
$(call flatten,GPERF.tmp)
GPERF.tmp := $(addprefix ${OBJECT.dir}/,${GPERF.tmp})
$(call convert,GPERF.tmp,h,gperf)
ALSO       += ${GPERF.tmp}

vpath %.gperf   ${SEARCH.dir}

${OBJECT.dir}/%.h: %.gperf
	$(call quiet_echo,GPERF	$<)
	${QUIET}${GPERF} ${GPERFLAGS} -H $(shell basename $(basename $<))_hash -N $(shell basename $(basename $<))_lookup < $< > $@

HELP_ME += "Default gperf flags: $(GPERFLAGS)" \
"\n"
