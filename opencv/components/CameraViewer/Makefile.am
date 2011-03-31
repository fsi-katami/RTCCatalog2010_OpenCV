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
sourcedir  = $(exampledir)/src/CameraViewer

# Loadable RTCs
socomp_LTLIBRARIES = CameraViewer.la

CameraViewer_la_SOURCES = CameraViewer.cpp CameraViewer.h
CameraViewer_la_LDFLAGS = -module

# Standalone RTCs
example_PROGRAMS = CameraViewerComp
#example_DATA     = CameraViewer.conf

CameraViewerComp_SOURCES = CameraViewerComp.cpp 
CameraViewerComp_LDADD   = CameraViewer.o

#$(top_builddir)/src/lib/coil/libcoil.la 
# Example sources
source_DATA = $(ALL_SRC) $(EXTRA_DIST)

#------------------------------------------------------------
# Extra distributed files
#------------------------------------------------------------
EXTRA_DIST =                  \
	rtc.conf              \
	Makefile.CameraViewer \
	README.CameraViewer

ALL_SRC =                    \
	CameraViewerComp.cpp \
	CameraViewer.cpp     \
	CameraViewer.h

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

