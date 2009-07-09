 ## @file DVTestHltDecayTreeTuple-TisTos.py
 #
 #  See DecayTreeTuple documentation
 #
 #  @author P. Koppenburg
 #  @date 2008-03-07
 #

from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits import *
########################################################################
#
# The Decay Tuple
#
from Configurables import DecayTreeTuple
tuple = DecayTreeTuple("Tuple")
tuple.ToolList +=  [
      "TupleToolTrigger"
    , "TupleToolGeometry"
    , "TupleToolKinematic"
    , "TupleToolPrimaries"
    , "TupleToolEventInfo"
    , "TupleToolTISTOS"
     ]
tuple.InputLocations = ["StdLooseJpsi2MuMu"]
tuple.Decay = "J/psi(1S) -> ^mu+ ^mu-"
#tuple.OutputLevel = 1 ;
########################################################################
#
# DaVinci
#
from Configurables import DaVinci
DaVinci().EvtMax = 100
DaVinci().SkipEvents = 0
DaVinci().DataType = "MC09" 
DaVinci().Simulation   = True
DaVinci().TupleFile = "DecayTreeTuple_TisTos.root"  # Ntuple
DaVinci().MoniSequence = [ tuple ]
DaVinci().ReplaceL0BanksWithEmulated = True
DaVinci().HltType = "Hlt1+Hlt2"

#-- GAUDI jobOptions generated on Mon Jun 22 17:05:57 2009
#-- Contains event types : 
#--   24142000 - 8 files - 123545 events - 27.24 GBytes

EventSelector().Input   = [
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004831/0000/00004831_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004831/0000/00004831_00000002_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004831/0000/00004831_00000003_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004831/0000/00004831_00000004_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004831/0000/00004831_00000006_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004831/0000/00004831_00000007_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004831/0000/00004831_00000008_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004831/0000/00004831_00000009_1.dst' TYP='POOL_ROOTTREE' OPT='READ'"]
