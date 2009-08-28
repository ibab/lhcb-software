
################################################
## basic script for the TCK-Presenter Project ##
## Klaus Neumann                              ##
## 17.07.09                                   ##
## CERN-PH-LHCb                               ##
################################################

## how to start the script with abstract parameters:
## python TCKPresenter.py TCK Path
## e.g. here with concrete paramters:
## python TCKPresenter.py 0x803b0000 /afs/cern.ch/user/k/kneumann/tcktestpage/

## imported scripts. the main scipt is the dotcreater file,
## which creates the graphes.
import dotcreater
import sys

## read in the TCK as a parameter and look if enough parameters
## are given. the programm will use the path to write the html pages in
## this directory.
if len(sys.argv) == 3:
    TCK  = sys.argv[1]
    Path = sys.argv[2]
    
    ## create the html pages for the given TCK in the given directory
    dotcreater.create(TCK, Path)

else:
    print 'the parameter signature is wrong. should be like "python TCKPresenter.py 0x803b0000 /afs/cern.ch/user/k/kneumann/tcktestpage/ 0"'

