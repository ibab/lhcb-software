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
#        self.location = tree.GetName()
        self.sizeInBytes = tree.GetTotBytes()
        self.zippedSizeInBytes = tree.GetZipBytes()
        self.sizeInKB = self.sizeInBytes/1024.
        self.sizeInMB = self.sizeInBytes/(1024.*1024)
        self.zippedSizeInKB = self.zippedSizeInBytes/1024.
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

    locations = [s.location for s in stats]

    zipsize = 0
    size = 0
    
    for s in stats :
        if not s.location.endswith('_') :
            sizeKB = round(s.sizeInKB, 2)
            zipSizeKB = round(s.zippedSizeInKB, 2)
            message = s.location.ljust(length) + str(str(sizeKB)+' ('+ str(zipSizeKB) +') KB /' + str(s.entries)+' Entries.').rjust(10) + '\n'
            zipsize += s.zippedSizeInKB
            size += s.sizeInKB
            _printMessage += message
            outputFile.write(message)

    totzipsize = 'Total zipped size ' + str(round(zipsize,2)) + ' KB'
    outputFile.write(totzipsize)
    totsize = 'Total size        ' + str(round(size,2)) + ' KB'
    outputFile.write(totsize)
    if verbose :
        print '----------------------------------------------------------------------------------'
        print _printMessage
        print totzipsize
        print totsize
        print '----------------------------------------------------------------------------------'
        print 'Wrote summary to', output
        
    outputFile.close()
