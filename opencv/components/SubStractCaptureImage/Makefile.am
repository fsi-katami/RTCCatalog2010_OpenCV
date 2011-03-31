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
sourcedir  = $(exampledir)/src/SubStractCaptureImage

# Loadable RTCs
socomp_LTLIBRARIES = SubStractCaptureImage.la

SubStractCaptureImage_la_SOURCES = SubStractCaptureImage.cpp SubStractCaptureImage.h
SubStractCaptureImage_la_LDFLAGS = -module

# Standalone RTCs
example_PROGRAMS = SubStractCaptureImageComp
#example_DATA     = SubStractCaptureImage.conf

SubStractCaptureImageComp_SOURCES = SubStractCaptureImageComp.cpp 
SubStractCaptureImageComp_LDADD   = SubStractCaptureImage.o

#$(top_builddir)/src/lib/coil/libcoil.la 
# Example sources
source_DATA = $(ALL_SRC) $(EXTRA_DIST)

#------------------------------------------------------------
# Extra distributed files
#------------------------------------------------------------
EXTRA_DIST =                  \
	rtc.conf              \
	Makefile.SubStractCaptureImage \
	README.SubStractCaptureImage

ALL_SRC =                    \
	SubStractCaptureImageComp.cpp \
	SubStractCaptureImage.cpp     \
	SubStractCaptureImage.h

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

