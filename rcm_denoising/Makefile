BUILD=DEBUG

waf:
	waf

all: lut denoise convert

file_lists:
	find ../input/*.pgm | sort > input.txt
	find ../input_cleaned/*.pgm | sort > input_cleaned.txt

lut: waf file_lists
	paste input_cleaned.txt input.txt | ./build/$(BUILD)/mklut > denoise.lut

denoise: waf
	ls ../input/*.pgm | xargs -I{} basename {} .pgm | \
	xargs -P 1 -I {} sh -c "./build/$(BUILD)/denoise denoise.lut < ../input/{}.pgm > ../input_denoised/{}.pgm"

# convert up to png
convert:
	ls ../input_denoised/*.pgm | xargs -I{} basename {} .pgm | \
	xargs -I {} sh -c "convert ../input_denoised/{}.pgm ../input_denoised/{}.png"

# denoise and convert the test set
submit: lut
	ls ../test/*.pgm | xargs -I{} basename {} .pgm | \
	xargs -P 1 -I {} sh -c "./build/$(BUILD)/denoise denoise.lut < ../test/{}.pgm > ../test_denoised/{}.pgm"
	ls ../test_denoised/*.pgm | xargs -I{} basename {} .pgm | \
	xargs -I {} sh -c "convert ../test_denoised/{}.pgm ../test_denoised/{}.png"
