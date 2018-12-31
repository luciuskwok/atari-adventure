# tiff_to_deflate.py

# (Updated for Python 3.)

# Given a filename in command line, opens TIFF image, converts to 2-bpp screen data, and uses DEFLATE, which is output as C code


import sys, zlib

# Color table mapping
# colorTable = [3, 1, 0, 2] # Tem Shop colors
# colorTable = [0, 3, 2, 1] # Battle button colors
colorTable = [3, 0, 1, 2] # Avatar colors


def validateTIFFHeader(ifh):
	isBigEndian = False
	isHeaderValid = False
	string = ifh[0:2].decode("ascii")

	if string == "II":
		isHeaderValid = True
		isBigEndian = False
	elif string == "MM":
		isHeaderValid = True
		isBigEndian = True

	if isHeaderValid:
		magic = intFromEndianData(ifh[2:4], isBigEndian)
		if magic != 42:
			isHeaderValid = False

	return (isHeaderValid, isBigEndian)
#end def validateTIFFHeader()


def intFromEndianData(bytes, isBigEndian):
	index = 0;
	length = len(bytes);
	result = 0;
	while index < length:
		result = result << 8;
		if isBigEndian:
			result = result | bytes[length - index - 1] 
		else:
			result = result | bytes[index]
		index += 1
	return result
#end def intFromEndianData()


def valueForTag(tag, ifd, isBigEndian):
	ifdCount = len(ifd) // 12
	for index in range(0,ifdCount):
		ifdOffset = index*12
		entry = ifd[ifdOffset : ifdOffset + 12]
		if tag == intFromEndianData(entry[0:2], isBigEndian):
			valueType = intFromEndianData(entry[2:4], isBigEndian)
			valueCount = intFromEndianData(entry[4:8], isBigEndian)

			if valueCount == 1:
				if valueType == 1 or valueType == 2:
					return entry[8]
				if valueType == 3:
					return intFromEndianData(entry[8:10], isBigEndian)
				if valueType == 4:
					return intFromEndianData(entry[8:12], isBigEndian)
	return -1
#end def valueForTag()


def printAllIFDEntries(ifd, isBigEndian):
	ifdCount = len(ifd) / 12
	for index in range(0,ifdCount):
		entry = ifd[index*12 : index*12+12]
		ifdTag = intFromEndianData(entry[0:2], isBigEndian)
		ifdType = intFromEndianData(entry[2:4], isBigEndian)
		ifdValueCount = intFromEndianData(entry[4:8], isBigEndian)
		ifdValueOffset = intFromEndianData(entry[8:12], isBigEndian)

		# if value fits in 4 bytes, the value is stored in the ValueOffset field itself
		if ifdValueCount == 1: 
			if ifdType == 1 or ifdType == 2:
				ifdValueOffset = entry[8]
			if ifdType == 3:
				ifdValueOffset = intFromEndianData(entry[8:10], isBigEndian)

		print("IFD[{}]: tag={} type={} count={} offset={}".format(index, ifdTag, ifdType, ifdValueCount, ifdValueOffset) )


#begin script

with open(sys.argv[1], "rb") as file:
	ifh = file.read(8)

	(isBigEndian, isHeaderValid) = validateTIFFHeader(ifh)

	if isHeaderValid:
		if isBigEndian:
			print("TIFF big-endian")
		else:
			print("TIFF little-endian")
	else:
		print("Not a TIFF file.")
		exit()

	ifdOffset = intFromEndianData(ifh[4:8], isBigEndian)
	#print("IFD Offset: " + str(ifdOffset))

	file.seek(ifdOffset, 0)
	ifdCount = intFromEndianData(file.read(2), isBigEndian)
	#print("IFD Count: " + str(ifdCount))

	# read IFDs
	ifd = file.read(12 * ifdCount)

	#printAllIFDEntries(ifd, isBigEndian)

	# get image properties
	imageWidth = valueForTag(256, ifd, isBigEndian)
	imageHeight = valueForTag(257, ifd, isBigEndian)
	compression = valueForTag(259, ifd, isBigEndian)
	photometricInterpretation = valueForTag(262, ifd, isBigEndian)
	stripOffset = valueForTag(273, ifd, isBigEndian)
	samplesPerPixel = valueForTag(277, ifd, isBigEndian)

	# check for specific image properties before continuing
	# file should be uncompressed
	if compression != 1 :
		print("Cannot process compressed file (compression: {}).".format(compression))
		exit()


	# file should be RGB color
	if samplesPerPixel != 3 or photometricInterpretation != 2:
		print("Cannot process file in this format (samplesPerPixel: {} photometricInterpretation: {}).".format(samplesPerPixel, photometricInterpretation))
		exit()

	# full screen images should be 160x72 in size
	print("Image size is {}x{}.".format(imageWidth, imageHeight))

	# read file into buffer as string
	file.seek(stripOffset, 0)
	byteCount = 3 * imageWidth * imageHeight
	pixelData = file.read(byteCount)

	# process pixels into indexed pixel data
	indexedData = []
	for y in range(0, imageHeight):
		for x in range(0, imageWidth):
			pixelOffset = 3 * (x + imageWidth * y)
			# take the simple average of RGB values
			r = pixelData[pixelOffset]
			g = pixelData[pixelOffset+1]
			b = pixelData[pixelOffset+2]
			lum = float(r+g+b) / (255 * 3)
			colorIndex = 1;

			# Apply a custom color mapping. 
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
	for y in range(0, imageHeight):
		packedByte = 0
		for x in range(0, imageWidth):
			packedByte = (packedByte << 2) | indexedData[x + imageWidth * y]
			if x % 4 == 3:
				packedData.append(packedByte)
				packedByte = 0

	# compress with DEFLATE
	compressor = zlib.compressobj(level=9, method=zlib.DEFLATED, strategy=zlib.Z_DEFAULT_STRATEGY)
		# strategy: zlib.Z_DEFAULT_STRATEGY, zlib.Z_FIXED
	zlibData = compressor.compress(packedData)
	zlibData += compressor.flush()
	compData = zlibData[2:-4]

	compLength = len(compData)
	compRatio = 1.0 - (float(compLength) / float(len(packedData)))
	percentStr = "%1.1f%%"%(compRatio*100)

	print ("Length: {}, Compression: {}%.".format(len(compData), percentStr))

	# Print compressed data for C
	i = 0
	for c in bytearray(compData): 
		sys.stdout.write("0x%0.2X,"%c)
		if i % 16 == 15:
			sys.stdout.write("\n")
		i += 1
	sys.stdout.write("\n")

#end script
