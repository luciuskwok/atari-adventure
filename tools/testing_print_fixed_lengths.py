# print_fixed_lengths.py

import sys

for x in range(0, 144):
	sys.stdout.write("8,")
sys.stdout.write("\n")	

for x in range(144, 256):
	sys.stdout.write("9,")
sys.stdout.write("\n")	

for x in range(256, 280):
	sys.stdout.write("7,")
sys.stdout.write("\n")	

for x in range(280, 288):
	sys.stdout.write("8,")
sys.stdout.write("\n")	



