#!/usr/bin/env gaudirun.py
#
# Minimal file for running Moore from python prompt
# Syntax is:
#   gaudirun.py ../options/Moore.py
# or just
#   ../options/Moore.py
#
import Gaudi.Configuration
from Moore.Configuration import Moore


# if you want to generate a configuration, uncomment the following lines:
Moore().generateConfig = True
Moore().configLabel = 'Default'

#Moore().ThresholdSettings = 'Commissioning_PassThrough'
#Moore().configLabel = 'ODINRandom acc=0, TELL1Error acc=1'

Moore().ThresholdSettings = 'Physics_320Vis_300L0_10Hlt1_Aug09'
#Moore().ThresholdSettings = 'Physics_10000Vis_1000L0_40Hlt1_LeptonicHlt2_Jul09'
#Moore().ThresholdSettings = 'Physics_10000Vis_1000L0_40Hlt1_CharmingHlt2_Jul09'
#Moore().ThresholdSettings = 'Physics_10000Vis_1000L0_40Hlt1_EffectiveHlt2_Jul09'
#Moore().ThresholdSettings = 'Physics_10000Vis_1000L0_40Hlt1_Apr09'
#Moore().ThresholdSettings = 'Commissioning_PassThrough'

Moore().Verbose = True
Moore().EvtMax = 10000

Moore().DDDBtag   = 'MC09-20090602'
Moore().CondDBtag = 'sim-20090402-vc-md100'
Moore().Simulation = True
Moore().DataType   = '2009'
Moore().inputFiles = [ '/data/bfys/lhcb/MinBias-L0xHlt1strip/Sim03Reco02-Mbias_%d.mdf'%(i) for i in [ 1, 2 ]  ]
#Moore().inputFiles = [ 'castor:/castor/cern.ch/user/d/dijkstra/Stripped-L0-MC09/Sim03Reco02-Mbias_%d.mdf'%f for f in range(1,5) ]  
#Moore().inputFiles = [ '/data/bfys/lhcb/data/2009/RAW/FULL/FEST/FEST/56721/056721_0000000001.raw' ]
#Moore().inputFiles = [ 'castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/FEST/FEST/56721/056721_%010d.raw'%(i) for i in [1,2,3] ]


from Configurables import EventSelector
EventSelector().PrintFreq = 100

