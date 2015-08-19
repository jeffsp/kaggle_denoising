# Jack Rabbit Examples Makefile
# jsp Fri Feb  6 17:42:54 CST 2009

CCFILES=$(shell ls -1 *.cc)
TARGETS=$(basename $(CCFILES))
INCLUDEPATH=/home/jsp/Projects
DEPENDPATH=/home/jsp/Projects
EXTRA_SOURCES=
LIBS=

include /home/jsp/Projects/Makefile.examples

doc:
	# Convert for use in documentation
	./raster_example0
	./raster_example1
	./raster_example2
	convert raster_example1.pgm raster_example1.png
	convert raster_example2.ppm raster_example2.png
	-display raster_example1.png
	-display raster_example2.png

clean_images:
	rm -f *_example*.png
	rm -f *_example*.p?m
