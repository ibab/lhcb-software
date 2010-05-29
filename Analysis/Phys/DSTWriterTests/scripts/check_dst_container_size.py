#!/usr/bin/env python
'''
Check some TES locations in a DST and print a summary of the number of events selected by each line.
Useage:
./check_dst_contents.py --input <some file name> --location <TES root of selected particles>
'''

__author__ = "Juan PALACIOS juan.palacios@cern.ch"

import sys, getopt
from GaudiConf.Configuration import *
from ROOT import TFile, TTree, TObject, TList

class TreeInfo(object) :
    def __init__(self, tree) :
        self.location = tree.GetName().replace('_', '/')
        self.sizeInBytes = tree.GetTotBytes()
        self.entries = tree.GetEntries()

def getTreeInfo( filename, outputfilename = 'out.txt'  ) :

    file = TFile.Open(filename)
    summary = []
    if file :
        for treeobj in file.GetListOfKeys() :
            tree = file.Get( treeobj.GetName() )
            if tree :
                summary.append( TreeInfo( tree ) )
    else :
        print 'ERROR: Could not open file', filename
    return summary

   
if __name__ == '__main__' :
    filename = ''
    output = 'out.txt'
    opts, args = getopt.getopt(sys.argv[1:], "l:i", ["input=", "output="])

    for o, a in opts:
        if o in ("-i", "--input"):
            filename = a
        elif o in ('-o', '--output') :
            output = a

    assert(filename != '')

    stats = getTreeInfo(filename, output)

    length = len(sorted([s.location for s in stats], cmp = lambda x,y : cmp(len(y), len(x)))[0])+2
    print '----------------------------------------------------------------------------------'
    for s in stats :
        message = s.location.ljust(length) + str(str(s.sizeInBytes)+' / '+ str(s.entries)+'     Bytes/Entries.').rjust(10)
        print message
    print '----------------------------------------------------------------------------------'

