# screen_image_deflate.py

# takes base64 encoded screen memory and uses zlib compression, which is output as C code


import zlib, sys, base64

# Read base64 data from terminal
input = sys.stdin.read()

# Convert from base64 to binary data (as string)
packedData = base64.b64decode(input)

# Compress the data
zlibData = zlib.compress(buffer(packedData))

# Remove header (2 bytes) and trailer (4 bytes)
compressedData = zlibData[2:len(zlibData) - 4]

print "Raw length:" + str(len(packedData))
print "Deflated length:" + str(len(compressedData))

# Print compressed data for C
i = 0
s = ""
for c in bytearray(compressedData): 
	if i % 12 == 0:
		print s
		s = ""
	s = s + "0x%0.2X, " % c
	i += 1

print s


