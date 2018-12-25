# analyze_trace.py

import sys

with open(sys.argv[1], "r") as file:
	addressDictionary = {}

	# Parse trace file by looking for PC addr lines and adding them to dictionary
	for line in file:
		if len(line) >= 40 and line.startswith(";") == False:
			pc = line[8:12]
			if pc in addressDictionary:
				addressDictionary[pc] += 1
			else:
				addressDictionary[pc] = 1

	# Print out top 20 addresses sorted by value
	for key, value in sorted(addressDictionary.iteritems(), key = lambda (k,v): (v,k), reverse=True)[:20]:
		print "%s: %s" % (key, value)


