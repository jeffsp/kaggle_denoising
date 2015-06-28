.PHONY: train

default: denoise measure

all: train denoise measure

train:
	./train.py

denoise:
	./denoise.py

measure:
	./measure.py

clean:
	rm -f *.pyc

submit:
	./submit.py
	echo 'id, value' > submission.txt
	cat test_denoised/*.csv >> submission.txt
