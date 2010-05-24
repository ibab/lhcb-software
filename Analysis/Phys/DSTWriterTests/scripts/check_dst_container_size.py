#!/usr/bin/env python
'''
Check some TES locations in a DST and print a summary of the number of events selected by each line.
Useage:
./check_dst_contents.py --input <some file name> --location <TES root of selected particles>
'''

__author__ = "Juan PALACIOS juan.palacios@cern.ch"

import sys, getopt

from ROOT import TFile, TTree, TObject, TList

def printTree( filename, outputfilename = 'out.txt'  ) :

    file = TFile.Open(filename)

    if file :
        for treeobj in file.GetListOfKeys() :
            tree = file.Get( treeobj.GetName() )
            if tree :  
                print treeobj.GetName().replace('_', '/'), ':', tree.GetTotBytes(), 'Bytes', tree.GetEntries(), 'entries'
                '''
                outfile << treeobj->GetName() 
                << " " <<  1.0*tree->GetTotBytes()/(1024.0*tree->GetEntries()) << " " 
                << 1.0*tree->GetZipBytes()/(1024.0*tree->GetEntries()) <<" " 
                << tree->GetEntries() << "\n";
                '''
    else :
        print 'ERROR: Could not open file', filename
            
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

    printTree(filename, output)
