
i = 1
sum_all = 0
with open("text.txt") as f:
	for line in f:
		if (i == 1):
			content = line
			i = 0
		else:
			i = 1
			content1 = line
			str2_ind = content1.find("seconds")
			content1 = content1[0:str2_ind]
			sum_all = float(content1) + sum_all

print sum_all/10


