# Horny Toad Tests Makefile
#
# jsp Fri Feb  6 17:03:41 CST 2009

CCFILES=$(shell ls -1 *.cc)
TARGETS=$(basename $(CCFILES))
INCLUDEPATH=/home/jsp/Projects
DEPENDPATH=/home/jsp/Projects
EXTRA_SOURCES=
LIBS=-lfftw3 -lgomp -lopenblas

include /home/jsp/Projects/Makefile.tests

run: check
