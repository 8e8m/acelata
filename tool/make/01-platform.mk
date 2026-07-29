HELP_ME += \
"01-platform.mk\n" \
"provides PLATFORM. Note that skel depends deeply on Unix assumptions\n" \
"PLATFORM is set to the uname which should be the standard assumption for platform.\n" \
"For Example, \"Linux\", \"Darwin\", \"OpenBSD\", \"FreeBSD\"" \
"This tool really isn't designed for Windows as it is a janky operating system.\n" \
"\n"

ifneq ($(UNAME),)
  PLATFORM ?= $(shell uname)
endif

PLATFORM ?= Unknown
