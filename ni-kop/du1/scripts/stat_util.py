# počet-iterací, max počet iterací, splněných klauzulí, všech klauzulí,
import os
import csv
from natsort import natsorted
import sys

if len(sys.argv) != 4:
    print('Wrong number of arguments ', len(sys.argv), file=sys.stderr)
    exit(1)

inputDir1 = sys.argv[1]
inputDir2 = sys.argv[2]

outputCsv = sys.argv[3]

# inputDir1 = "output/uf20-91/"
# inputDir2 = "output/uf20-91/"
#
# outputCsv = "ahoj.csv"


def getParts(f):
    line = f.readline().strip()
    return line.split(' ')


def getFileStats(dir, file):
    succ = 0
    stepsSum = 0
    awgFined = 0
    mu = 0
    sig2 = 0
    length = 0
    f = open(dir + file, "r")

    for x in f:
        length += 1
        parts = getParts(f)

        stepsSum += int(parts[0])

        if parts[2] == parts[3]:
            succ += 1
            awgFined += int(parts[0])
        else:
            awgFined += 10 * int(parts[1])


    f.close()

    return {"succ": succ, "length": length, "stepsAvg": stepsSum/length, "awgFined": awgFined/length, "percentageSuccess": succ/length}


statsList = {}


def getStatsDir(inputDir, letter):
    for filename in os.listdir(inputDir):
        if not statsList.get(filename):
            statsList[filename] = {}

        stats = getFileStats(inputDir, filename)
        statsList[filename].update({"succ "+letter: stats['succ'], "of": stats['length'], "steps "+letter: stats['stepsAvg'],
                               "awg fined "+letter: stats['awgFined'], "percent success "+letter: stats['percentageSuccess']})

getStatsDir(inputDir1, 'A')
getStatsDir(inputDir2, 'B')

header = ['inst', 'succ A', 'succ B', 'of', 'steps A', 'steps B', 'awg fined A', 'awg fined B', 'percent success A', 'percent success B']


for row in statsList:
    statsList[row]['inst'] = row
    statsList[row] = sorted(statsList[row].items(), key=lambda pair: header.index(pair[0]))
    statsList[row] = list(map(lambda x: x[1], statsList[row]))


statsList = list(map(lambda x: x[1], natsorted(statsList.items())))

with open(outputCsv, 'w') as myfile:
    wr = csv.writer(myfile)
    wr.writerow(header)

    for row in statsList:
        wr.writerow(row)

print("Success")

