#!/usr/bin/env python
'''
Check some TES locations in a DST and print a summary of the number of events selected by each line.
Useage:
./check_dst_contents.py --help
'''

__author__ = "Juan PALACIOS juan.palacios@cern.ch"

class TreeInfo(object) :
    def __init__(self, tree) :
        self.location = tree.GetName().replace('_', '/')
        self.sizeInBytes = tree.GetTotBytes()
        self.zippedSizeInBytes = tree.GetZipBytes()
        self.sizeInKB = round(self.sizeInBytes/1024., 2)
        self.sizeInMB = round(self.sizeInBytes/(1024.*1024), 2)
        self.zippedSizeInKB = round(self.zippedSizeInBytes/1024., 2)
        self.entries = tree.GetEntries()

def getTreeInfo( filename, trunk ) :
    from ROOT import TFile
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
    file.Close()
    return summary

   
if __name__ == '__main__' :

    from DSTWriterTests.default_args import parser
    parser.set_defaults(output='dst_sizes.txt')
    parser.set_defaults(location='/')
    parser.remove_option('--nevents')
    parser.remove_option('--branch')
    
    (options, args) = parser.parse_args()

    if len(args) != 1 :
        parser.error('expected one positional argument')        

    filename = args[0]
    location = options.root
    output = options.output
    verbose = options.verbose

    outputFile = open(output, 'w')

    stats = getTreeInfo(filename, location)

    length = len(sorted([s.location for s in stats], cmp = lambda x,y : cmp(len(y), len(x)))[0])+2
    _printMessage = ''

    for s in stats :
        message = s.location.ljust(length) + str(str(s.sizeInKB)+' ('+ str(s.zippedSizeInKB) +') KB /' + str(s.entries)+' Entries.').rjust(10) + '\n'
        _printMessage += message
        outputFile.write(message)

    if verbose :
        print '----------------------------------------------------------------------------------'
        print _printMessage
        print '----------------------------------------------------------------------------------'
        print 'Wrote summary to', output
        
    outputFile.close()
