#!/usr/bin/env python
'''
Check some TES locations in a DST and print a summary of the number of events selected by each line.
Useage:
./check_dst_contents.py --input <some file name> --location <TES root of selected particles> --output <output file name>
'''

__author__ = "Juan PALACIOS juan.palacios@cern.ch"

import sys, getopt
from GaudiConf.Configuration import *
from ROOT import TFile, TTree, TObject, TList

class TreeInfo(object) :
    def __init__(self, tree) :
        self.location = tree.GetName().replace('_', '/')
        self.sizeInBytes = tree.GetTotBytes()
        self.sizeInKB = round(self.sizeInBytes/1024., 2)
        self.sizeInMB = round(self.sizeInBytes/(1024.*1024), 2)
        self.entries = tree.GetEntries()

def getTreeInfo( filename, trunk ) :

    file = TFile.Open(filename)
    summary = []
    if file :
        for treeobj in file.GetListOfKeys() :
            tree = file.Get( treeobj.GetName() )
            if tree :
                treeInfo = TreeInfo( tree )
                if trunk == treeInfo.location[:len(location)] :
                    summary.append( TreeInfo( tree ) )
    else :
        print 'ERROR: Could not open file', filename
    return summary

   
if __name__ == '__main__' :
    filename = ''
    location = '/'
    output = 'dst_container_sizes.txt'    
    opts, args = getopt.getopt(sys.argv[1:], "o:i:l", ["input=", "output=", "location="])

    for o, a in opts:
        if o in ("-i", "--input"):
            filename = a
        elif o in ('-o', '--output') :
            output = a
        elif o in ("-l", "--location") :
            location = a
            
    assert(filename != '')

    outputFile = open(output, 'w')

    stats = getTreeInfo(filename, location)

    length = len(sorted([s.location for s in stats], cmp = lambda x,y : cmp(len(y), len(x)))[0])+2
    print '----------------------------------------------------------------------------------'
    for s in stats :
        message = s.location.ljust(length) + str(str(s.sizeInKB)+' KB / '+ str(s.entries)+' Entries.').rjust(10)
        print message
        outputFile.write(message+'\n')
    print '----------------------------------------------------------------------------------'
    print 'Wrote summary to', output
    outputFile.close()
