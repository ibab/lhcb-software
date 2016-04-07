__authors__ = 'Carlos Vazquez Sierra'
__date__ = '2016/04/07'

'''
Module for slicing joint intersections files.

Run with:

python slicer.py <joint_intersections_file> <stripping_version>

A new file, called 'lines_intersections_<stripping_version>.dat' will be produced.

Example:

python slicer.py joint_streams_21_21r1_lines_intersection.dat 21

will produce a file called 'lines_intersections_21.dat' where only the information
for Stripping21 is dumped.
'''

import sys

inputFile = str(sys.argv[1])
strippingVersion = str(sys.argv[2])

f = open(inputFile,'r')
lines_to_dump = []
for line in f.readlines():
  lenStpV = len(strippingVersion)
  if line[:lenStpV+1]==strippingVersion+'.': lines_to_dump.append(line[:-1])
f.close()
sys.stdout = open('lines_intersections_'+strippingVersion+'.dat','w')

for line in lines_to_dump:
  print line

