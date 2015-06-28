# Horny Toad Examples Makefile
#
# jsp Fri Feb  6 17:13:07 CST 2009

CCFILES=$(shell ls -1 *.cc)
TARGETS=$(basename $(CCFILES))
INCLUDEPATH=/home/jsp/Projects
DEPENDPATH=/home/jsp/Projects
EXTRA_SOURCES=
LIBS=-lfftw3 -lgomp

include /home/jsp/Projects/Makefile.examples

run: all
	./bicubic_example > tmp.pgm
	display -resize 300% -filter point tmp.pgm
	rm -f tmp.pgm

doc:
	./pyramid_example0
	./pyramid_example1
	# Convert for use in documentation
	convert pyramid_example1_grayscale.pgm pyramid_example1_grayscale.png
	convert pyramid_example1_color.ppm pyramid_example1_color.png
	-display pyramid_example1_grayscale.png
	-display pyramid_example1_color.png

clean_images:
	rm -f *_example*.png
	rm -f *_example*.p?m
