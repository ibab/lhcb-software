from ROOT import *

import os.path

#file = TFile('histos_subjob0.root')
#file = TFile('mergedhistos.root')

nfiles = 106
jobnr = 664
nsteps = 110

filelist = []

filledlist = []

root = '/stage/Daan/gangadir/workspace/dvaneijk/LocalXML/' + str(jobnr)


for i in range(0,nfiles):
    filename = root+'/'+str(i)+'/output/histos.root'
    if os.path.isfile(filename):
        filelist.append(filename)

for file in filelist:
    tfile = TFile(file)
    print 'looking in file', file
    for j in range(0,nsteps):
        vars()['set'+str(j)] = tfile.Get('HitEfficiency/threshold_'+str(j))
        
        #check if the file actually contains the histos:
        if vars()['set'+str(j)] != None:
            nentries = vars()['set'+str(j)].GetEntries()
            if nentries != 0:
                filledlist.append(j)
                print 'Filled!', vars()['set'+str(j)]
            #else:
                #print 'Not Filled!', vars()['set'+str(j)]

