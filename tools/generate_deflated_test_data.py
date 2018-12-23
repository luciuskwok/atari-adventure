# generate_deflated_test_data.py

import zlib

# Create test data
data = bytearray()
for i in range(0, 40*72):
	data.append((i / 10) % 256)

# Compress test data
zlibFormatData = zlib.compress(buffer(data))

# Remove header (2 bytes) and trailer (4 bytes)
compressedData = zlibFormatData[2:len(zlibFormatData) - 4]

print "Raw length:" + str(len(data))
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

# Decompress data to verify
inflated = zlib.decompress(zlibFormatData)


