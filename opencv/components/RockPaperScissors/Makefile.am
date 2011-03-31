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
sourcedir  = $(exampledir)/src/RockPaperScissors

# Loadable RTCs
socomp_LTLIBRARIES = RockPaperScissors.la

RockPaperScissors_la_SOURCES = RockPaperScissors.cpp RockPaperScissors.h
RockPaperScissors_la_LDFLAGS = -module

# Standalone RTCs
example_PROGRAMS = RockPaperScissorsComp
#example_DATA     = RockPaperScissors.conf

RockPaperScissorsComp_SOURCES = RockPaperScissorsComp.cpp 
RockPaperScissorsComp_LDADD   = RockPaperScissors.o

#$(top_builddir)/src/lib/coil/libcoil.la 
# Example sources
source_DATA = $(ALL_SRC) $(EXTRA_DIST)

#------------------------------------------------------------
# Extra distributed files
#------------------------------------------------------------
EXTRA_DIST =                  \
	rtc.conf              \
	Makefile.RockPaperScissors \
	README.RockPaperScissors

ALL_SRC =                    \
	RockPaperScissorsComp.cpp \
	RockPaperScissors.cpp     \
	RockPaperScissors.h

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

