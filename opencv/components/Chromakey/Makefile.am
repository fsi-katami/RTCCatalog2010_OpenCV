## -*- Makefile -*-

AUTOMAKE_OPTIONS = 1.9

AM_CPPFLAGS=`rtm-config --cflags` `pkg-config --cflags opencv` -I$(top_srcdir)/rtm/idl
AM_LDFLAGS=-`rtm-config --libs` `pkg-config --libs opencv` -L$(top_srcdir)/rtm/idl -lRTCSkel

#------------------------------------------------------------
# Targets
#------------------------------------------------------------
# Targets directories
#exampledir = $(pkgdatadir)/examples
exampledir = /usr/share/OpenRTM-aist/Components/OpenCV
socompdir  = $(exampledir)/rtcs
sourcedir  = $(exampledir)/src/Chromakey

# Loadable RTCs
socomp_LTLIBRARIES = Chromakey.la

Chromakey_la_SOURCES = Chromakey.cpp Chromakey.h
Chromakey_la_LDFLAGS = -module

# Standalone RTCs
example_PROGRAMS = ChromakeyComp
#example_DATA     = Chromakey.conf

ChromakeyComp_SOURCES = ChromakeyComp.cpp 
ChromakeyComp_LDADD   = Chromakey.o

#$(top_builddir)/src/lib/coil/libcoil.la 
# Example sources
source_DATA = $(ALL_SRC) $(EXTRA_DIST)

#------------------------------------------------------------
# Extra distributed files
#------------------------------------------------------------
EXTRA_DIST =                  \
	rtc.conf              \
	Makefile.Chromakey \
	README.Chromakey

ALL_SRC =                    \
	ChromakeyComp.cpp \
	Chromakey.cpp     \
	Chromakey.h

#------------------------------------------------------------
# File list for deb/ports packages
#------------------------------------------------------------
lst:
	echo $(ALL_SRC) > src.lst
	echo $(EXTRA_DIST) > other.lst

clean-local:
	rm -f \
	*.gch *.bak *.rpo *.sym lib*.*_pure_* *.lst \
	Makefile.old *core *~ *.so *.log

