# The top-level auto Makefile for Critical Mass

# require automake 1.4
AUTOMAKE_OPTIONS = 1.4

## Any directories that you want built and installed should go here.
SUBDIRS = utils utilssdl utilsgl tools game docs

## Any directories you want a part of the distribution should be listed
## here, as well as have a Makefile generated at the end of configure.in
##
## This only works for subdirectories one level deep.
DIST_SUBDIRS = $(SUBDIRS)

man_MANS = critter.6
EXTRA_DIST = $(man_MANS)

MAINTAINERCLEANFILES = Makefile.bak Makefile.in aclocal.m4 configure install-sh missing mkinstalldirs stamp-h.in config.guess config.sub depcomp defines.h.in~
