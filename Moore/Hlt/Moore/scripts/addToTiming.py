"""
Add /adjust entries in the reference timing table

usage adjust.ref using.ref listofnames
"""

import sys

def usage():
    print __doc__

if len(sys.argv)<4:
    usage()
    raise ValueError("please give a list of alg names")

import os

for f in (sys.argv[1:3]):
    if not os.path.exists(f):
        raise IOError(f+" does not exist")

using=open(sys.argv[2])
usinglines=[l.strip() for l in using.read().split('\n') if len(l.strip())]
using.close()
replaceoraddlines={}
removelines=[]
after=""
replaceafter={}
for line in usinglines:
    for replacename in sys.argv[3:]:
        if replacename in replaceoraddlines:
            continue
        if replacename.strip()==line.split(":")[0].strip():
            replaceoraddlines[replacename]=line
            replaceafter[replacename]=after.split(":")[0].strip()
    after=line

removelines=[f for f in sys.argv[3:] if f not in replaceoraddlines]

adjust=open(sys.argv[1])
adjustlines=[l.strip() for l in adjust.read().split('\n') if len(l.strip())]
adjust.close()

adjusted=[]
replaced=[]
#replace existing lines
for line in adjustlines:
    for replacename in replaceoraddlines:
        if replacename in replaced:
            continue
        if replacename.strip()==line.split(":")[0].strip():
            line=replaceoraddlines[replacename]
            replaced.append(replacename)
    
    adjusted.append(line)

addedlines=[]
addednames=[]
namestoadd=[replacename for replacename in replaceoraddlines if replacename not in replaced]
#add new lines
after=""
linel=0
while linel<len(adjusted):
    line=adjusted[linel]
    for replacename in namestoadd:
        if replacename in addednames:
            continue
        if after.split(":")[0].strip()==replaceafter[replacename]:
            line=replaceoraddlines[replacename]
            linel=linel-1
            addednames.append(replacename)
    addedlines.append(line)
    after=line
    linel=linel+1

addatend=[n for n in namestoadd if n not in addednames]
addedlines=addedlines+[replaceoraddlines[replacename] for replacename in addatend]
#remove lines
shrunklines=[]
shrunknames=[]
for line in addedlines:
    skip=False
    for replacename in removelines:
        if replacename in shrunknames:
            continue
        if replacename.strip()==line.split(":")[0].strip():
            skip=True
            shrunknames.append(replacename)
    if not skip:
        shrunklines.append(line)

print "==================="
print "-Ameliorating", sys.argv[1], "with", sys.argv[2]
print "-Input length", len(adjustlines)
print "-Replaced", replaced
print "-Added", addednames
print "  (added at the end ", addatend,")"
print "-Removed", shrunknames
print "-Output length", len(shrunklines)
print "==================="

#print shrunklines[0]

f=open(sys.argv[1]+".adjusted","w")
f.write("\n".join(shrunklines).strip()+"\n")
f.close()

print "Wrote to", sys.argv[1]+".adjusted"

for i in addatend:
    print "tried to replace ",i,"after",replaceafter[i]
