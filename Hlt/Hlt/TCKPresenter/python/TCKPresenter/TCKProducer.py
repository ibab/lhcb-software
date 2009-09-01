
################################################
## basic script for the TCK-Presenter Project ##
## Klaus Neumann                              ##
## 17.07.09                                   ##
## CERN-PH-LHCb                               ##
################################################

## this script is used to read in all the tcks and produce one after the other.
## there is a possibility to overwrite existing tcks or not depends on the choosen option

import os
import sys
from TCKUtils.utils import *

## the path where to put the tcks is mentioned here 
Path = '/afs/cern.ch/user/k/kneumann/tcktestpage/'

TCKs = getTCKList()

## read in the parameters
if len(sys.argv) == 3:
    Path = sys.argv[1]
    Overwrite = sys.argv[2]
else:
    print 'try: python TCKProducer.py /var/www/vhosts/tck_presenter/httpdocs/TCKs/ 0'
    sys.exit()

print "number of TCKs: ", len(TCKs)

for TCK in TCKs:
    ## investigate if a tcks should be overwritten or not
    if not os.path.exists(Path + TCK + '/') or Overwrite=='1':
        print 'create TCK: ' + TCK
        ##TCKPresenter script is the main script to produce tcks
        os.system('python TCKPresenter.py ' + TCK + ' ' + Path)
    else:
        print 'TCK exist already: ' + TCK
        


