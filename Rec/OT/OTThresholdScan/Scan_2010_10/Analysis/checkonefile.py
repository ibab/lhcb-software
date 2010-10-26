from ROOT import *

import os.path


file = '/stage/Daan/OTEfficiency/ThresholdScans/Scan_2010_10/histos.root'

tfile = TFile(file)

nsteps = 33

filelist = []

filledlist = []

print 'looking in file', file
for j in range(0,nsteps):
    vars()['set'+str(j)] = tfile.Get('HitEfficiency/threshold_'+str(j))

    #check if the file actually contains the histos:
    if vars()['set'+str(j)] != None:
        nentries = vars()['set'+str(j)].GetEntries()
        if nentries != 0:
            filledlist.append(j)
            print 'Filled!', vars()['set'+str(j)]
        else:
            print 'Not Filled!', vars()['set'+str(j)]

