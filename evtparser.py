import sys
import matplotlib.pyplot as plt

#WARN: this assumes you're using eventbin with ONLY one bin
#WARN: truncates results to 1200KeV

def parse(file):
	f = open(file)
	vals = []
	lines = f.readlines()

	for i,l in enumerate(lines):
		if "hit cells" in l:
			vl = l.split(":")
			nc = int(vl[1])
			if nc > 0:
				lv = lines[i+1].split("1  ")[1]
				lvn = float(lv)
				if lvn < 1200*1e-6: #1200KeV
					vals.append(lvn)
	return vals

vals = []
for i,a in enumerate(sys.argv):
	if i == 0:
		continue

	vals += parse(a)

plt.hist(vals, bins=1024)
plt.show()
