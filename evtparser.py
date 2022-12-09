import sys
import matplotlib.pyplot as plt

#WARN: this assumes you're using eventbin with ONLY one bin
#WARN: truncates results to 1200KeV
def parse(file):
	values = []
	with open(file) as file:
		expect_value = False
		for line in file:
			if "hit cells" in line:
				n = int(line.split(":")[1])
				if n > 0:
					expect_value = True
					continue

			if expect_value:
				lv = float(line.split("1  ")[1])
				if lv < 1200*1e-6: #1200KeV
					values.append(lv)
				expect_value = False
	return values

vals = []
for i,a in enumerate(sys.argv):
	if i == 0:
		continue

	vals += parse(a)

vals = [v * 1e6 for v in vals]

plt.hist(vals, bins=1024)
plt.xlabel("energia depositata [KeV]")
plt.ylabel("counts")
plt.show()
