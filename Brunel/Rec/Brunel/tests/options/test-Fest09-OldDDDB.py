from Brunel.Configuration import *
from Configurables import LHCbApp, PatSeedFit

theApp=Brunel()

LHCbApp().DDDBtag="head-20091112"
LHCbApp().CondDBtag="head-20091112"

EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/EXPRESS/FEST/FEST/59512/059512_0000000002.raw' SVC='LHCb::MDFSelector'"]

LHCbApp().EvtMax=20

