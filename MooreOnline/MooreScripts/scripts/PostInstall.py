#!/usr/bin/env python
import sys, os

#sys.argv[1] must be *where* to put the output file
if len(sys.argv)<2:
    raise AttributeError("Not enough inputs..., give me the directory I should write into")

setupDir=sys.argv[1]

from MooreScripts.CreateRunEFF import CreateRunMooreOnline_EFF

for split in [ '','Hlt1','Hlt2'] :
    f=setupDir+"runMoore%sOnline_EFF.sh"%(split)
    if os.path.exists(f):
        print "# WARNING: EFF File exists! Remove if you want me to recreate it! "+f
    else:
        CreateRunMooreOnline_EFF( f, setup,split )
