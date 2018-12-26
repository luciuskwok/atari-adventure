# analyze_trace.py

import sys

def parseSymbol(string):
	name = string[0:26].strip()
	addr = string[28:32]
	return {"name":name, "addr":addr}

# == Begin Script ==
codeStartAddr = "0000"
codeEndAddr = "0000"
symbols = []

# Read the linker_map.txt file to symbolicate addresses
with open("../linker_map.txt", "r") as mapFile:
	foundSegmentList = False
	foundCodeSegment = False
	foundExportsList = False
	skipHorizontalRule = False
	completedExportsList = False

	for line in mapFile:		

		# Find "Segment List:"
		if foundSegmentList == False:
			if line.startswith("Segment list:"):
				foundSegmentList = True
				print "Found segment list."
			continue

		# Find "CODE" segment
		if foundCodeSegment == False:
			if line.startswith("CODE"):
				codeStartAddr = line[24:28]
				codeEndAddr = line[32:36]
				foundCodeSegment = True
				print "Found code segment."
			continue

		# Find "Exports list by value:"
		if foundExportsList == False:
			if line.startswith("Exports list by value:"):
				foundExportsList = True
				skipHorizontalRule = True
				print "Found exports list."
			continue

		if skipHorizontalRule == True:
			skipHorizontalRule = False
			continue

		# Parse 2-column line of symbols
		if completedExportsList == False:
			if len(line) >= 40:
				#print "Found symbol line."
				symbols.append(parseSymbol(line[0:40]))
				if len(line) >= 80:
					symbols.append(parseSymbol(line[40:80]))
			else:
				completedExportsList = True
			continue

	print "CODE %s to %s, %d symbols" % (codeStartAddr, codeEndAddr, len(symbols))


# Read the trace file
with open(sys.argv[1], "r") as traceFile:
	addressDictionary = {}

	# Parse trace file by looking for PC addr lines and adding them to dictionary
	for line in traceFile:
		if len(line) >= 40 and line.startswith(";") == False:
			pc = line[8:12]
			foundSymbol = "Unknown"

			# Find matching symbol
			if pc > codeEndAddr:
				foundSymbol = "OS"

			elif pc >= codeStartAddr:
				for symbol in reversed(symbols):
					if symbol["addr"] <= pc:
						foundSymbol = symbol["name"]
						break

			if foundSymbol in addressDictionary:
				addressDictionary[foundSymbol] += 1
			else:
				addressDictionary[foundSymbol] = 1

	# Print out top 20 addresses sorted by value
	for key, value in sorted(addressDictionary.iteritems(), key = lambda (k,v): (v,k), reverse=True)[:48]:
		print "{1:>10,} {0}".format(key, value)
		#print "%s: %s" % (key, value)

# == End Script ==
