bin_PROGRAMS = proxy_vision_tester

# -ansi


AM_CFLAGS = \
            -g -W -Wall -pedantic -Wbad-function-cast -Wcast-align \
            -Wcast-qual -Wchar-subscripts -Winline \
            -Wmissing-prototypes -Wnested-externs -Wpointer-arith \
            -Wredundant-decls -Wshadow -Wstrict-prototypes \
            -Wpointer-arith -Wno-long-long \
            -Wcomment -Wmissing-include-dirs \
            -O3 -Wuninitialized \
            -I../ \
            -Icommon \
            -Iproto



proxy_vision_tester_SOURCES = \
        multithread.c
proxy_vision_tester_LDFLAGS = 
proxy_vision_tester_LDADD = -lpthread -lcurl

