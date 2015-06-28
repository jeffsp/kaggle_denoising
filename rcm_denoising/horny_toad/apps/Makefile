# Horny Toad Apps Makefile
#
# Copyright (C) 2009 Jeffrey S. Perry
#
# This program is free software: you can redistribute it
# and/or modify it under the terms of the GNU General
# Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your
# option) any later version.
#
# This program is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public
# License along with this program.  If not, see
# <http://www.gnu.org/licenses/>.
#
# contact: jeffsp@gmail.com

.SUFFIXES:
.PHONY: all check clean

all:
	$(MAKE) -C rms
	$(MAKE) -C foveate
	$(MAKE) -C reduce

check:
	$(MAKE) -C rms check
	$(MAKE) -C reduce check

clean:
	$(MAKE) -C rms clean
	$(MAKE) -C foveate clean
	$(MAKE) -C reduce clean
