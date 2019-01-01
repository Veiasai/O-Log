import threading
import time
import argparse
import sys
import string
import random
import re
import copy
import os

LogDir = "DebugLog/"
file = open(LogDir + "Ori.log", "r")
data = file.read()
file.close()
lines = data.split("\n")
newLines = []
for line in lines:
    if line != "":
        templist = line.split("\t")
        newLine = templist[0]
        templist = templist[1].split(",")
        newLine = newLine + " " + templist[3][11:17] + " " + templist[len(templist)-1][0:19]+"\n"
        newLines.append(newLine)
outputFile = open(LogDir + "temp.log", "w")
for line in newLines:
    outputFile.write(line)
outputFile.close()

