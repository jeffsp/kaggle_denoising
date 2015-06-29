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
	rm test_denoised/*
	$(MAKE) -C ./rcm_denoising submit
	./submit.py
	echo 'id,value' > submission.csv
	cat test_denoised/*.csv >> submission.csv
