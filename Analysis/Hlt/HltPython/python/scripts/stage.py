#!/usr/bin/env python
# =============================================================================
""" @namespace stage
@brief Script to stage all files refered in an option file. Allows to check their integrity by briefly opening them with ROOT.

Syntax: ./stage.py fileName [check]
@author Hugo Ruiz hugo.ruiz@cern.ch
@date 2007-11-28
"""
# =============================================================================

import sys
import os
import commands
from ROOT import TFile

DEBUG = False

def getArguments():
    """ Gets script arguments
    @returns Input file name and number of samples required
    @author Hugo Ruiz hugo.ruiz@cern.ch
    """

    usageInfo = "Usage: Stage.py filename [check]"
    
    # Option file name
    try:
        optFileName=sys.argv[1]
    except:
        print usageInfo
        sys.exit()    

    # Is 'check' the second argument?
    try:
        opt=sys.argv[2]
        if opt=='check':
            wantCheck=True
        else:
            print usageInfo
            sys.exit()    
    except:
        wantCheck = False
    return optFileName, wantCheck


def doStage( optFileName, wantCheck ):
    """ Does all the work
    @param optFileName Option file containing all the files to stage
    @param wantCheck If set to true, stage.py will try to open all files with ROOT to check for 
    @author Hugo Ruiz hugo.ruiz@cern.ch
    """

    # Check that option file is there, open and read it
    if not os.access(optFileName,os.F_OK):
        print "File not found: "+optFileName;sys.exit()
    optFile=open(optFileName)
    fileContent=optFile.read()

    # Check which kind of files it includes
    if fileContent.find('.oodst')!=-1:
        breakWord='.oodst'
    elif fileContent.find('.dst')!=-1:
        breakWord='.dst'
    elif fileContent.find('.rdst')!=-1:
        breakWord='.rdst'
    elif fileContent.find('.sim')!=-1:
        breakWord='.sim'
    else:
        print "No file found with extension .oodst .rdst or .dst in "+optFileName
        sys.exit()

    # Start parsing and get the list of files to be staged
    fragments=fileContent.split("'")
    fileList = []
    for iFrag in fragments:
        if DEBUG: print iFrag
        if iFrag.find(breakWord)!=-1 and (iFrag.find('castor')!=-1 or iFrag.find('rfio')!=-1) :
            try:
                iFile=iFrag.split('castor:')[1]
            except:
                try:
                    iFile=iFrag.split('rfio:')[1]
                except:
                    print 'Could not find castor: or rfio: keywords in file'

            notThere = commands.getoutput('nsls '+iFile).find('No such file or directory')!=-1
            if notThere:
                print 'IGNORING REQUESTED FILE THAT IS NOT IN CASTOR: '+iFile
            else:
                fileList.append( iFile )


    # Split list of files in sets of 5000 maximum (more can cause problems!)
    dictList = {}
    i = 0
    j = 0
    dictList[0] = []
    for iFile in fileList:
        i = i+1
        dictList[j].append(iFile)
        if i==5000:
            if DEBUG: print 'A set of  5000 files completed'
            i = 0
            j = j+1
            dictList[j] = []

    # Send the stage command
    for iKey in dictList.keys():
        commandLine='stager_get'
        if DEBUG: print iKey, len(dictList[ iKey ])
        for iFile in dictList[ iKey ]:
            if DEBUG: print iFile
            commandLine+=' -M'+iFile


        print '\nCOMMAND SENT: '+commandLine+'\n'
        print commands.getoutput(commandLine)


    # Check files by opening them with ROOT, to avoid bad files
    if wantCheck:
        print
        print 'Checking files by opening them with ROOT:'
        print '========================================='
        for iKey in dictList.keys():
            if DEBUG: print iKey, len(dictList[ iKey ])
            for iFile in dictList[ iKey ]:

                print iFile
                iRootFile = TFile.Open('rfio:'+iFile)
                iRootFile.Close()
                print

# Main execution
optFileName, wantCheck = getArguments()
doStage( optFileName, wantCheck )
