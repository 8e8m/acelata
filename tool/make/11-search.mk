HELP_ME += \
"11-search.mk\n" \
"Builds SEARCH.object tree." \
"\n"

SEARCH.object := ${addprefix ${OBJECT.dir}/,${SEARCH.dir:${SOURCE.dir}/%=%}}
ALSO += ${SEARCH.object}

${SEARCH.object}:
	@mkdir -p $@

${OBJECT.orig}: ${SEARCH.object}
