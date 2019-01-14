# png_to_zlib.py

# Uses Python 3
# Uses PyPNG module. Use `pip install pypng`.

import png, sys, zlib

# == Settings ==
colorTable = [3, 1, 0, 2] # Tem Shop colors
# colorTable = [0, 3, 2, 1] # Battle button colors
# colorTable = [3, 0, 1, 2] # Avatar colors


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

	print("Packed data len={}".format(len(packedData)))

	# Compress with DEFLATE
	compressor = zlib.compressobj(level=9, method=zlib.DEFLATED, strategy=zlib.Z_DEFAULT_STRATEGY)
		# strategy: zlib.Z_DEFAULT_STRATEGY, zlib.Z_FIXED
	zlibData = compressor.compress(packedData)
	zlibData += compressor.flush()

	# Print stats
	compLength = len(zlibData)
	compRatio = 1.0 - (float(compLength) / float(len(packedData)))
	percentStr = "%1.1f%%"%(compRatio*100)
	print ("Length: {}, Compression: {}%.".format(len(zlibData), percentStr))

	# Print data
	count = 0
	for c in bytearray(zlibData): 
		sys.stdout.write("0x%0.2X,"%c)
		count += 1
		if count >= 16:
			sys.stdout.write("\n")
			count = 0
	sys.stdout.write("\n")







