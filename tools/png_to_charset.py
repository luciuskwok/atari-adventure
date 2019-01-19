# png_to_charset.py

# Uses Python 3

# Given a PNG that is 128 px wide, in RGB format without alpha, generate ASM source for a character set or font.

# Uses PyPNG module. Use `pip install pypng`.

import png, sys, zlib

# == Script ==

with open(sys.argv[1], "rb") as file:
	pngReader = png.Reader(file=file)
	(width, height, pixels, info) = pngReader.read()
	bitPlaneCount = info["planes"];
	print("Valid PNG image {}x{}x{}.".format(width, height, bitPlaneCount))

	charBlock = []
	rowIndex = 0
	for row in pixels:
		colIndex = 0
		value = 0
		for col in range(0, width):
			r = row[col * bitPlaneCount]
			g = row[col * bitPlaneCount + 1]
			b = row[col * bitPlaneCount + 2]
			lum = float(r+g+b) / (255 * 3)

			value = (value << 1) 
			if lum > 0.5:
				value |= 1

			colIndex += 1
			if colIndex % 8 == 0:
				charBlock.append(value)
				value = 0 

		rowIndex += 1
		if rowIndex % 8 == 0:
			charCount = int(len(charBlock)/8)
			for charIndex in range(0, charCount):
				sys.stdout.write(".byte ")
				for y in range(0, 8):
					c = charBlock[charIndex + y * charCount]
					sys.stdout.write("$%0.2X"%c)
					if y < 7: 
						sys.stdout.write(", ")
				sys.stdout.write("\n")
			charBlock = []


