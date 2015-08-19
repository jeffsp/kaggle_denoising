# Jack Rabbit Makefile
#
# Copyright (C) 2008-2014 Jeffrey S. Perry
#
# contact: jeffsp@gmail.com

TARGET=jack_rabbit

.SUFFIXES:
.PHONY: all check clean doc release install

QFLAGS="CONFIG+=debug"

all:
	$(MAKE) -C tests
	$(MAKE) -C examples

check: all
	$(MAKE) -C tests check
	$(MAKE) -C examples check

clean:
	$(MAKE) -C tests clean
	$(MAKE) -C examples clean clean_images
	rm -rf doc

doc: check
	$(MAKE) -C examples doc
	doxygen $(TARGET).doxygen

release:
	$(MAKE) QFLAGS=\"CONFIG+=release\"

install: release check
	sudo mkdir -p /usr/local/include/jack_rabbit
	sudo cp -a  *.h /usr/local/include/jack_rabbit
