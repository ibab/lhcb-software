from ROOT import *

#filename = '/stage/Daan/OTEfficiency/NTuple/NTuplesSep/tuples0.root'
#filename = '/stage/Daan/OTEfficiency/NTuple/NTuplesSep/tuples527.root'

#filename = '/stage/Daan/OTEfficiency/NTuple/AnalysisDaanSep/tuples5.root'
#filename = '/stage/Daan/OTEfficiency/NTuple/AnalysisDaanSep/tuples15.root'
filename = '/stage/Daan/OTEfficiency/NTuple/AnalysisDaanSep/tuples22.root'

file = TFile(filename)
data = file.Get('HitEfficiency/Efficiency')

nentries = data.GetEntries()

eventnumberlist = []

for i in range(0,nentries):
    if i%100000==0:
        print "entry %s out of %s entries in tree"%(i, nentries)
    data.GetEntry(i)
    eventnumberlist.append(data.eventNumber)

setje = set(eventnumberlist)

print 'number of events in ntuple =', len(setje)
    




