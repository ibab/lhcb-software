#!/usr/bin/env gaudirun.py
#
# Minimal file for running Moore from python prompt
# Syntax is:
#   gaudirun.py ../options/Moore.py
# or just
#   ../options/Moore.py
#
from Gaudi.Configuration import *
from Moore.Configuration import *
from Configurables       import HltSelReportsMaker


# if you want to generate a configuration, uncomment the following lines:
#Moore().generateConfig = True
#Moore().configLabel = 'PA accept=1'

Moore().HltType = 'PA+Hlt1'
Moore().Verbose = True

# FEST specific options...
Moore().ThresholdSettings = 'FEST'
veto = [ 'TES:Trig/L0/FullCalo' ,   'TES:Trig/L0/MuonCtrl'
       , 'TES:Hlt/Vertex/ASidePV3D','TES:Hlt/Vertex/CSidePV3D'
       , 'TES:Hlt/Track/Long', 'TES:Hlt/Track/Forward',   'TES:Hlt/Track/RZVelo',    'TES:Hlt/Track/Velo'
       , 'TES:Hlt/Vertex/PV2D' 
       , 'TES:Hlt/Track/MuonSegmentForL0Single'
       , 'RZVeloBW'
       ]
HltSelReportsMaker().SelectionMaxCandidatesDebug = dict( [ (i,0) for i in veto ] )
HltSelReportsMaker().DebugEventPeriod = 2000
HltConf().LumiBankKillerAcceptFraction = 0.9999
HltConf().HistogrammingLevel = 'Line'

Moore().Simulation = True
Moore().DataType   = '2009'
Moore().inputFiles = [ '/data/bfys/lhcb/test/MDF/00003083/0000/00003083_%08d_1.mdf'%f  for f in range(1,4) ]
#Moore().inputFiles = [ 'castor:/castor/cern.ch/grid/lhcb/test/MDF/00003083/0000/00003083_%08d_1.mdf'%f  for f in range(1,4) ]
#Moore().outputFile = '/tmp/foo.mdf'

EventSelector().PrintFreq = 100
Moore().EvtMax = 100000

Moore().DDDBtag   = "default"
Moore().CondDBtag = "default"

# Moore().RunOnline = True
