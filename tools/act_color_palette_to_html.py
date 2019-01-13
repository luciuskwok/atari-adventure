# act_color_palette_to_html.py

import sys

with open(sys.argv[1], "rb") as file:
	data = file.read(3)
	while len(data) == 3:
		byAr = bytearray(data)
		print("\"#%0.2X%0.2X%0.2X\"," % (byAr[0], byAr[1], byAr[2]))
		data = file.read(3)
