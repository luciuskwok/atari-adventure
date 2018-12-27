# tiff_to_deflate.py

# Given a filename in command line, opens TIFF image, converts to 2-bpp screen data, and uses DEFLATE, which is output as C code

# Python 2.7 returns a string for file.read() calls, but the strings are actually 8-bit binary data. This seems to be the native data type for Python 2, so it's easier to handle data in that format, and then call ord() to get the value of each byte.

import zlib, sys, base64


def validateTIFFHeader(ifh):
	isBigEndian = False
	isHeaderValid = False

	if ifh[0:2] == "II":
		isHeaderValid = True
		isBigEndian = False
	elif ifh[0:2] == "MM":
		isHeaderValid = True
		isBigEndian = True

	if isHeaderValid:
		magic = intFromEndianData(ifh[2:4], isBigEndian)
		if magic != 42:
			isHeaderValid = False

	return (isHeaderValid, isBigEndian)
#end def validateTIFFHeader()


def intFromEndianData(dataString, isBigEndian):
	bytes = []
	for c in dataString:
		bytes.append(ord(c))

	result = 0;
	while len(bytes) > 0:
		c = 0;
		result = result << 8;
		if isBigEndian:
			result = result | bytes[0] 
			bytes.pop(0)
		else:
			result = result | bytes[-1]
			bytes.pop()
	return result
#end def intFromEndianData()


def valueForTag(tag, ifd, isBigEndian):
	ifdCount = len(ifd) / 12
	for index in range(0,ifdCount):
		ifdOffset = index*12
		entry = ifd[ifdOffset : ifdOffset + 12]
		if tag == intFromEndianData(entry[0:2], isBigEndian):
			valueType = intFromEndianData(entry[2:4], isBigEndian)
			valueCount = intFromEndianData(entry[4:8], isBigEndian)

			if valueCount == 1:
				if valueType == 1 or valueType == 2:
					return ord(entry[8])
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
				ifdValueOffset = ord(entry[8])
			if ifdType == 3:
				ifdValueOffset = intFromEndianData(entry[8:10], isBigEndian)

		print("IFD["+str(index)+"]: tag="+str(ifdTag)+" type="+str(ifdType)+" count="+str(ifdValueCount)+" offset="+str(ifdValueOffset) )


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
		print("Cannot process compressed file (compression: "+str(compression)+").")
		exit()


	# file should be RGB color
	if samplesPerPixel != 3 or photometricInterpretation != 2:
		print("Cannot process file in this format (samplesPerPixel: "+str(samplesPerPixel)+" photometricInterpretation: "+str(photometricInterpretation)+").")
		exit()

	#file should be 160x72 in size
	if imageWidth != 160 or imageHeight != 72:
		print("Image size is not 160x72. This file is "+str(imageWidth)+"x"+str(imageHeight))
		exit()

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
			r = ord(pixelData[pixelOffset])
			g = ord(pixelData[pixelOffset+1])
			b = ord(pixelData[pixelOffset+2])
			lum = float(r+g+b) / (255 * 3)
			colorIndex = 1;

			# Apply a custom color mapping. 
			if lum > 0.9:
				colorIndex = 2 # white
			elif lum > 0.5:
				colorIndex = 0 # light background
			elif lum > 0.1:
				colorIndex = 1 # dark background
			else:
				colorIndex = 3 # black

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

	# integrity check
	if len(packedData) != 2880:
		print "Packed data length should be 2880 but is "+str(len(packedData))

	# compress with DEFLATE
	zlibData = zlib.compress(buffer(packedData))
	compData = zlibData[2:-4]

	compLength = len(compData)
	compRatio = 1.0 - (float(compLength) / float(len(packedData)))
	percentStr = "%1.1f%%"%(compRatio*100)

	print "Length: "+str(len(compData))+", Compression: "+percentStr

	# Print compressed data for C
	i = 0
	for c in bytearray(compData): 
		sys.stdout.write("0x%0.2X,"%c)
		if i % 16 == 15:
			sys.stdout.write("\n")
		i += 1
	sys.stdout.write("\n")

#end script
