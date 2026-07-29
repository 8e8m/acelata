HELP_ME += \
"03-hardened.mk\n" \
"uses DEBUG=0 to add compiler security features.\n" \
"Check file for details. Targeted towards C.\n" \
"\n"

ifneq (${DEBUG},1)
        CFLAGS   += -fstack-protector-strong
        CXXFLAGS += -fstack-protector-strong
        ifneq (${PLATFORM},Mac)
                CFLAGS   += -fstack-clash-protection
                CXXFLAGS += -fstack-clash-protection
                ifeq (${IS_GCC},1)
                        CFLAGS   += -fPIE -pie
                        CXXFLAGS += -fPIE -pie
                endif
                LDFLAGS  += -Wl,-z,relro,-z,now
        endif
        CPPFLAGS += -D_FORTIFY_SOURCE=3
endif
