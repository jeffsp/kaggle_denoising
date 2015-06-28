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
	xargs -P 2 -I {} sh -c "./build/$(BUILD)/denoise denoise.lut < ../input/{}.pgm > ../input_denoised/{}.pgm"

# convert up to png
convert:
	ls ../input_denoised/*.pgm | xargs -I{} basename {} .pgm | \
	xargs -I {} sh -c "convert ../input_denoised/{}.pgm ../input_denoised/{}.png"
