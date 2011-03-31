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
sourcedir  = $(exampledir)/src/ImageCalibration

# Loadable RTCs
socomp_LTLIBRARIES = ImageCalibration.la

ImageCalibration_la_SOURCES = ImageCalibration.cpp ImageCalibration.h
ImageCalibration_la_LDFLAGS = -module

# Standalone RTCs
example_PROGRAMS = ImageCalibrationComp
#example_DATA     = ImageCalibration.conf

ImageCalibrationComp_SOURCES = ImageCalibrationComp.cpp 
ImageCalibrationComp_LDADD   = ImageCalibration.o

#$(top_builddir)/src/lib/coil/libcoil.la 
# Example sources
source_DATA = $(ALL_SRC) $(EXTRA_DIST)

#------------------------------------------------------------
# Extra distributed files
#------------------------------------------------------------
EXTRA_DIST =                  \
	rtc.conf              \
	Makefile.ImageCalibration \
	README.ImageCalibration

ALL_SRC =                    \
	ImageCalibrationComp.cpp \
	ImageCalibration.cpp     \
	ImageCalibration.h

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

