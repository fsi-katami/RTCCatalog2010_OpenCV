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
sourcedir  = $(exampledir)/src/DilationErosion

# Loadable RTCs
socomp_LTLIBRARIES = DilationErosion.la

DilationErosion_la_SOURCES = DilationErosion.cpp DilationErosion.h
DilationErosion_la_LDFLAGS = -module

# Standalone RTCs
example_PROGRAMS = DilationErosionComp
#example_DATA     = DilationErosion.conf

DilationErosionComp_SOURCES = DilationErosionComp.cpp 
DilationErosionComp_LDADD   = DilationErosion.o

#$(top_builddir)/src/lib/coil/libcoil.la 
# Example sources
source_DATA = $(ALL_SRC) $(EXTRA_DIST)

#------------------------------------------------------------
# Extra distributed files
#------------------------------------------------------------
EXTRA_DIST =                  \
	rtc.conf              \
	Makefile.DilationErosion \
	README.DilationErosion

ALL_SRC =                    \
	DilationErosionComp.cpp \
	DilationErosion.cpp     \
	DilationErosion.h

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

