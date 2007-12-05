#!/usr/bin/env python2.3
# =============================================================================
""" @namespace split
@brief Script to split an option file with data samples produced by the bookkeeping into n option files.

Syntax: ./split.py optionsFileName [number of data files per subsample]
@author Hugo Ruiz hugo.ruiz@cern.ch
@date 2007-11-28
"""
# =============================================================================

import sys

# Get configuration from parameters
def getArguments():
    """ Gets script arguments
    @returns Input file name and number of samples required
    @author Hugo Ruiz hugo.ruiz@cern.ch
    """
    try:
        inputFileName = sys.argv[1]
        nFilesPerSample = int(sys.argv[2])
    except:
        print 'Usage: /split.py optionsFileName [number of subsamples to produce]'
        sys.exit()
    return inputFileName, nFilesPerSample

# Get lines in input files
def doSplit(inputFileName,nFilesPerSample):
    """ Does all the work
    @param inputFileName Name of options file
    @param nFilesPerSample Number of files per option file to produce
    @author Hugo Ruiz hugo.ruiz@cern.ch
    """
    iFileCont = file( inputFileName ).read()
    iHeader = iFileCont.split('{')[0]
    iBody =  iFileCont.split('{')[1].split('}')[0]
    iFooter = iFileCont.split('}')[1]
    iBodyLines = iBody.split(',')
    jOld = -1; j = 0; i = 0; outLines = []
    print 'A total of ',len(iBodyLines),'data files found'

    # Produce outputFiles
    for iLine in iBodyLines:
        j = int( i/nFilesPerSample )
        if j != jOld:
            if j!=0:
                outFileName =  inputFileName.replace('.opts','') + '_' + str(jOld+1) + '.opts'
                print '  Filling',outFileName,'with',len(outLines),'data files'
                outString += reduce(lambda a,b: a+',\n'+b, outLines)
                outString += '\n}\n'+iFooter
                file(outFileName,'w').write( outString )

            outString = iHeader+'{\n'
            outLines = []

        outLines.append(iLine)
        jOld = j
        i +=1

    # And finalize the last one
    outString += reduce(lambda a,b: a+',\n'+b, outLines)
    outString += '\n}\n'+iFooter
    outFileName =  inputFileName + str(jOld+1) + '.opts'
    file(outFileName,'w').write( outString )
    print 'Done'


# Main execution
inputFileName, nFilesPerSample = getArguments()
doSplit(inputFileName, nFilesPerSample)
