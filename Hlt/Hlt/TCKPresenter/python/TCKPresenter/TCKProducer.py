
import os
import sys
from TCKUtils.utils import *

Path = '/afs/cern.ch/user/k/kneumann/tcktestpage/'

TCKs = getTCKList()

if len(sys.argv) == 3:
    Path = sys.argv[1]
    Overwrite = sys.argv[2]
else:
    print 'try: python TCKProducer.py /var/www/vhosts/tck_presenter/httpdocs/TCKs/ 0'
    sys.exit()

print "number of TCKs: ", len(TCKs)

for TCK in TCKs:
    if not os.path.exists(Path + TCK + '/') or Overwrite=='1':
        print 'create TCK: ' + TCK
        os.system('python TCKPresenter.py ' + TCK + ' ' + Path)
    else:
        print 'TCK exist already: ' + TCK
        


