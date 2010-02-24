#!/usr/bin/env gaudirun.py

from Moore.Configuration import Moore

Moore().ThresholdSettings = 'Physics_10000Vis_1000L0_40Hlt1_Apr09'
Moore().UseDBSnapshot     = False
Moore().Simulation        = True
Moore().DDDBtag           = 'MC09-20090602'
Moore().CondDBtag         = 'sim-20090402-vc-md100'
Moore().DataType          = '2009'

# correct data file
Moore().inputFiles = [ 'castor:/castor/cern.ch/user/d/dijkstra/Stripped-L0-MC09/Sim03Reco02-Mbias_%d.mdf'%f for f in range(1,5) ]  
#Moore().inputFiles = [                     '/data/bfys/lhcb/MinBias-L0xHlt1strip/Sim03Reco02-Mbias_%d.mdf'%(i) for i in [ 1, 2 ]  ]


from Configurables import EventSelector
EventSelector().PrintFreq = 100

Moore().EvtMax = 10000

