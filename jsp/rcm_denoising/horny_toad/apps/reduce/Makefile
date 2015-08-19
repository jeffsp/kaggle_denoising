# Reduce Application Makefile
# jsp Fri Feb  6 17:49:00 CST 2009

TARGET=reduce
INCLUDEPATH=../.. ../../../horny_toad
DEPENDPATH=../.. ../../../horny_toad
EXTRA_SOURCES=../../../horny_toad/argv.cc
LIBS=

include ../../../Makefile.app

check:
	convert xc:yellow -resize 640x480! \
		-pointsize 80 \
		-gravity "Center" \
		-depth 8 \
		-draw "text 0,0 '$(TARGET) 8bpp'" \
		/tmp/tmp-$(TARGET).pgm
	-display /tmp/tmp-$(TARGET).pgm
	./reduce -k4 -l0 -x4 /tmp/tmp-$(TARGET).pgm > /tmp/tmp-$(TARGET).out.pgm
	-display /tmp/tmp-$(TARGET).out.pgm
	convert xc:yellow -resize 160x120! \
		-pointsize 20 \
		-gravity "Center" \
		-depth 8 \
		-draw "text 0,0 '$(TARGET) 8bpp'" \
		/tmp/tmp-$(TARGET).pgm
	-display /tmp/tmp-$(TARGET).pgm
	./reduce -k3 -i2 -l0 -x4 /tmp/tmp-$(TARGET).pgm > /tmp/tmp-$(TARGET).out.pgm
	-display /tmp/tmp-$(TARGET).out.pgm
	rm /tmp/tmp-$(TARGET).pgm
	rm /tmp/tmp-$(TARGET).out.pgm
	convert xc:yellow -resize 640x480! \
		-pointsize 80 \
		-gravity "Center" \
		-depth 16 \
		-draw "text 0,0 '$(TARGET) 16bpp'" \
		/tmp/tmp-$(TARGET).pgm
	-display /tmp/tmp-$(TARGET).pgm
	./reduce -k4 -l0 -x2 /tmp/tmp-$(TARGET).pgm > /tmp/tmp-$(TARGET).out.pgm
	-display /tmp/tmp-$(TARGET).out.pgm
	convert xc:yellow -resize 160x120! \
		-pointsize 20 \
		-gravity "Center" \
		-depth 16 \
		-draw "text 0,0 '$(TARGET) 16bpp'" \
		/tmp/tmp-$(TARGET).pgm
	-display /tmp/tmp-$(TARGET).pgm
	./reduce -k4 -i2 -l0 -x2 /tmp/tmp-$(TARGET).pgm > /tmp/tmp-$(TARGET).out.pgm
	-display /tmp/tmp-$(TARGET).out.pgm

blah:
	convert xc:yellow -resize 640x480! \
		-pointsize 80 \
		-gravity "Center" \
		-depth 16 \
		-draw "text 0,0 '$(TARGET) 16bpp'" \
		tmp-$(TARGET).pgm
	-display tmp-$(TARGET).pgm
	./reduce -k4 -l0 -x2 tmp-$(TARGET).pgm > tmp-$(TARGET).out.pgm
	-display tmp-$(TARGET).out.pgm
