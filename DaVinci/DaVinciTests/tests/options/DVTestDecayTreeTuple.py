## @file DVTestHltDecayTreeTuple.py
 #
 #
 #  See DecayTreeTuple documentation
 #
 #  @author P. Koppenburg
 #  @date 2008-03-07
 #

from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits import *
MessageSvc().Format = "% F%30W%S%7W%R%T %0W%M"
#######################################################################
#
# Selection
#

from StrippingConf.Configuration import StrippingConf
from StrippingConf.StrippingStream import StrippingStream
from StrippingArchive.Utils import lineBuilder
lineBuilder = lineBuilder('Stripping17', 'BetaS')
line = lineBuilder.lineFromName('StrippingBetaSBd2JpsiKsDetachedLine')
stream = StrippingStream("B2JpsiXTest", Lines = [line])
sc = StrippingConf()
sc.appendStream( stream )

########################################################################
#
# The Decay Tuple
#
from Configurables import DecayTreeTuple
tuple = DecayTreeTuple("Tuple")
tuple.ToolList +=  [
      "TupleToolMCTruth"
    , "TupleToolMCBackgroundInfo"
    , "TupleToolGeometry"
    , "TupleToolKinematic"
    , "TupleToolPropertime"
    , "TupleToolPrimaries"
    , "TupleToolEventInfo"
    , "TupleToolTrackInfo"
#    , "TupleToolTISTOS"
     ]
tuple.Inputs = [line.outputLocation()]
tuple.Decay = "[ B0 -> ^( J/psi(1S) -> ^mu+ ^mu- ) ^( KS0 -> ^pi+ ^pi- ) ]CC"
#tuple.OutputLevel = 1 ;
########################################################################
#
# The Event Tuple
#
from Configurables import EventTuple, TupleToolTrigger
evtTuple = EventTuple()
evtTuple.ToolList = [ "TupleToolEventInfo",
                      "TupleToolGeneration" ]
evtTuple.addTool(TupleToolTrigger())
evtTuple.TupleToolTrigger.VerboseHlt1 = True 
evtTuple.TupleToolTrigger.VerboseHlt2 = True
# evtTuple.OutputLevel = 1 
########################################################################
#
# The MC truth Tuple
#
from Configurables import MCDecayTreeTuple
mcTuple = MCDecayTreeTuple("MCTuple")
mcTuple.Decay = "( [ [B0]nos => ^( J/psi(1S) => ^mu+ ^mu- ) ^( KS0 => ^pi+ ^pi- ) ]CC ) || ( [ [B0]os -> ^( J/psi(1S) => ^mu+ ^mu- ) ^( KS0 => ^pi+ ^pi- ) ]CC )"
mcTuple.ToolList = [ "MCTupleToolKinematic", "TupleToolEventInfo", "MCTupleToolReconstructed"  ]
from Configurables import MCTupleToolReconstructed, MCReconstructed
#ToolSvc().addTool(MCReconstructed)
#ToolSvc().MCReconstructed.OutputLevel = 1

#mcTuple.OutputLevel = 1

from Configurables import PrintMCTree
pmc = PrintMCTree()
pmc.ParticleNames = [ "B0", "B~0" ]
########################################################################
#
# DaVinci
#
from Configurables import DaVinci

DaVinci().EvtMax = 100
DaVinci().SkipEvents = 0
DaVinci().DataType = "2010" 
DaVinci().Simulation = True
#DaVinci().CondDBtag = 'MC09-20100827-vc-md100'
DaVinci().TupleFile = "DecayTreeTuple.root"  # Ntuple
DaVinci().MoniSequence = [ tuple, evtTuple, mcTuple ] # , pmc ] 
DaVinci().appendToMainSequence( [ sc.sequence() ] )

DaVinci().CondDBtag="sim-20100715-vc-md100"
DaVinci().DDDBtag="head-20100624"

from PRConfig import TestFileDB
TestFileDB.test_file_db["MC2010_BdJPsiKs"].run(configurable=DaVinci(),withDB=False)
