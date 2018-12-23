# charset_convert.py

import sys

with open(sys.argv[1], "r") as file:
	count = 0
	lineIsEmpty = True
	for line in file:
		if len(line) > 8 and line.startswith("//") == False:
			sum = 0
			for x in range(8):
				sum = sum * 2
				if line[x].lower() == 'x':
					sum = sum + 1
			sys.stdout.write("0x%0.2X" % sum + ", ")
			count += 1
			lineIsEmpty = False
		elif lineIsEmpty == False:
			sys.stdout.write("\n")
			lineIsEmpty = True
	if lineIsEmpty == False:
		sys.stdout.write("\n")
	sys.stdout.write("count = " + str(count) + "\n")


