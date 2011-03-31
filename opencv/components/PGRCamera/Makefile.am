## -*- Makefile -*-

AUTOMAKE_OPTIONS = 1.9

AM_CPPFLAGS=`rtm-config --cflags` `pkg-config --cflags opencv` -I$(top_srcdir)/rtm/idl -I/usr/include/flycapture
AM_LDFLAGS=-`rtm-config --libs` `pkg-config --libs opencv` -L$(top_srcdir)/rtm/idl -lRTCSkel -lflycapture${D}
 
#------------------------------------------------------------
# Targets
#------------------------------------------------------------
# Targets directories
#exampledir = $(pkgdatadir)/examples
exampledir = /usr/share/OpenRTM-aist/Components/OpenCV
socompdir  = $(exampledir)/rtcs
sourcedir  = $(exampledir)/src/PGRCamera

# Loadable RTCs
socomp_LTLIBRARIES = PGRCamera.la

PGRCamera_la_SOURCES = PGRCamera.cpp PGRCamera.h
PGRCamera_la_LDFLAGS = -module $(AM_LDFLAGS)

# Standalone RTCs
example_PROGRAMS = PGRCameraComp
#example_DATA     = PGRCamera.conf

PGRCameraComp_SOURCES = PGRCameraComp.cpp 
PGRCameraComp_LDADD   = PGRCamera.o

#$(top_builddir)/src/lib/coil/libcoil.la 
# Example sources
source_DATA = $(ALL_SRC) $(EXTRA_DIST)

#------------------------------------------------------------
# Extra distributed files
#------------------------------------------------------------
EXTRA_DIST =                  \
	rtc.conf              \
	Makefile.PGRCamera \
	README.PGRCamera

ALL_SRC =                    \
	PGRCameraComp.cpp \
	PGRCamera.cpp     \
	PGRCamera.h

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

