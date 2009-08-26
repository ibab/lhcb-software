
import os
import dotcreater
from TCKUtils.utils import *

Path = '/afs/cern.ch/user/k/kneumann/tcktestpage/'

TCKs = getTCKList()

print "number of TCKs: ", len(TCKs)

for TCK in TCKs:
    if not os.path.exists(Path + TCK + '/'):
        print 'create TCK: ' + TCK
        #dotcreater.create(TCK, Path)
        os.system('python TCKPresenter.py ' + TCK + ' ' + Path)
    else:
        print 'TCK exist already: ' + TCK
        


