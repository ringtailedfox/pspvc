bindir = $(prefix)/bin

bin_PROGRAMS = pspvc

pspvc_SOURCES = \
    main.c \
    ui_main.c \
    ui_convert.c \
    ui_about.c \
    ffmpeg.c \
    profile.c \
    support.c

AM_CPPFLAGS = -I$(top_srcdir)/include -DPKGDATADIR=\"$(pkgdatadir)\"

pspvc_CFLAGS = $(GTK_CFLAGS)
pspvc_LDADD  = $(GTK_LIBS) $(INTLLIBS)

