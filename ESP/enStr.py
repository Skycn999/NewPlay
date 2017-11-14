#pragma once
offset = 2

with open("STR.h") as f:
	with open("STR_E.h","w") as output:
		for line in f:
			items = line.split("\"")
			if len(items) != 3:
				continue;
			enStr = ""
			for ch in items[1]:
				enStr += str(chr((ord(ch)+ offset)))


			output.write(items[0] + "\"" +enStr + "\"\n")