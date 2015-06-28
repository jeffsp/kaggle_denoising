BUILD=DEBUG

waf:
	waf

run: lut denoise

file_lists:
	find train/*.pgm | sort > train.txt
	find train_cleaned/*.pgm | sort > train_cleaned.txt

lut: waf file_lists
	paste train_cleaned.txt train.txt | ./build/$(BUILD)/mklut > denoise.lut

denoise: waf
	ls train/*.pgm | xargs -I{} basename {} .pgm | \
	xargs -I {} sh -c "./build/$(BUILD)/denoise denoise.lut < train/{}.pgm > train_denoised/{}.pgm"

# convert up to parent directory
convert:
	ls train_denoised/*.pgm | xargs -I{} basename {} .pgm | \
	xargs -I {} sh -c "convert train_denoised/{}.pgm ../train_denoised/{}.png"
