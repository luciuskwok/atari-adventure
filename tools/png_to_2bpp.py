# png_to_2bpp.py

# Uses Python 3.
# Converts an PNG file in RGB or RGBA format into a 2 bits per pixel block of data suitable for displaying on Atari.

import png, sys, zlib

# == Settings ==
# colorTable = [3, 1, 0, 2] # Tem Shop colors
# colorTable = [0, 3, 2, 1] # Battle button colors
colorTable = [3, 0, 1, 2] # Avatar colors


# == Script ==

with open(sys.argv[1], "rb") as file:
	pngReader = png.Reader(file=file)
	(width, height, pixels, info) = pngReader.read()
	bitPlanes = info["planes"];
	print("Valid PNG image {}x{}.".format(width, height))

	# Store color indexes as a flat array
	indexedData = []

	for row in pixels:
		for col in range(0, width):
			r = row[col * bitPlanes]
			g = row[col * bitPlanes + 1]
			b = row[col * bitPlanes + 2]
			lum = float(r+g+b) / (255 * 3)

			# Apply color table
			colorIndex = 0
			if lum > 0.9:
				colorIndex = colorTable[3] # white
			elif lum > 0.5:
				colorIndex = colorTable[2] # light background
			elif lum > 0.1:
				colorIndex = colorTable[1] # dark background
			else:
				colorIndex = colorTable[0] # black

			indexedData.append(colorIndex)

	# pack data into 2 bits per pixel, 4 pixels per byte
	packedData = bytearray()
	shift = 0
	packedByte = 0
	for value in indexedData:
		packedByte = (packedByte << 2) | (value & 3)
		shift += 1
		if shift >= 4:
			packedData.append(packedByte)
			packedByte = 0
			shift = 0

	print("Atari data length: {}".format(len(packedData)))

	# Print data
	count = 0
	for c in packedData: 
		sys.stdout.write("0x%0.2X,"%c)
		count += 1
		if count >= 16:
			sys.stdout.write("\n")
			count = 0
	sys.stdout.write("\n")







