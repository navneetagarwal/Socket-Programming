from random import randint
import sys

x = sys.argv
n = int(x[1]) - 1

text_file = open("Output.txt", "w")
for i in range(0,10):
	range_start = 10**(n-1)
	range_end = (10**n)-1
	y = randint(range_start, range_end)

	text_file.write(str(i) + str(y)+"\n")