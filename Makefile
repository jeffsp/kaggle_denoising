.PHONY: train

default: measure

all: denoise measure

train:
	./train.py

denoise:
	./denoise.py

measure:
	./measure.py

clean:
	rm -f *.pyc
