#import commands

import os
#os.path, os.system

from ROOT import *

nfiles = 106
jobnr = 664

list = []

root = '/stage/Daan/gangadir/workspace/dvaneijk/LocalXML/' + str(jobnr)

mergestring = 'hadd -f mergedhistos.root'

for i in range(0,nfiles):
    filename = root+'/'+str(i)+'/output/histos.root'
    if os.path.isfile(filename):
        list.append(filename)
        mergestring = mergestring + ' ' + filename
    else:
        print 'file ', i, 'not found'

print 'start merging, please wait....'
#merge = commands.getoutput(mergestring)
os.system(mergestring)
