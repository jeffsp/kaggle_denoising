# RMS Application Makefile

TARGET=rms
INCLUDEPATH=../.. ../../../jack_rabbit
DEPENDPATH=../.. ../../../jack_rabbit
EXTRA_SOURCES=../../argv.cc
LIBS=

include ../../../Makefile.app

check:
	./rms --help
	./rms < noise.pgm > /dev/null
	./rms -w8 noise.pgm > /dev/null
	./rms --mean noise.pgm > /dev/null
