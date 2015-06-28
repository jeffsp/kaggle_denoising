# Horny Toad Utilities
#
# Copyright (C) 2008-2014 Jeffrey S. Perry
#
# contact: jeffsp@gmail.com

TARGET=horny_toad

.SUFFIXES:
.PHONY: all check clean doc release install

QFLAGS="CONFIG+=debug QMAKE_CXXFLAGS+=-Wno-write-strings"

all:
	$(MAKE) -C tests
	$(MAKE) -C examples

check: all
	$(MAKE) -C tests check

clean:
	$(MAKE) -C tests clean
	$(MAKE) -C examples clean
	rm -rf doc

doc:
	doxygen $(TARGET).doxygen

release:
	$(MAKE) QFLAGS=\"CONFIG+=release\"

install: release
	sudo mkdir -p /usr/local/include/horny_toad
	sudo cp -a *.h *.hpp /usr/local/include/horny_toad
