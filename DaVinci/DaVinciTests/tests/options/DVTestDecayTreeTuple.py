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
#tuple.Decay = "[B0 -> (^J/psi(1S) -> ^mu+ ^mu-) (^KS0 -> ^pi+ ^pi-)]cc"
# CRJ : Adapt for LoKi decay finder
tuple.Decay = "[B0 -> ^(J/psi(1S) -> ^mu+ ^mu-) ^(KS0 -> ^pi+ ^pi-)]CC"
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
mcTuple.Decay = "{[ [B0]nos -> ^(J/psi(1S) -> ^mu+ ^mu- {,gamma}{,gamma}) ^(KS0 -> ^pi+ ^pi- {,gamma}{,gamma}) {,gamma} ]CC, [ [B0]os -> ^(J/psi(1S) -> ^mu+ ^mu- {,gamma}{,gamma}) ^(KS0 -> ^pi+ ^pi- {,gamma}{,gamma}) {,gamma} ]CC}"
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

#-- GAUDI jobOptions generated on Mon Jan 17 17:06:59 2011
#-- Contains event types : 
#--   11144103 - 1 files - 10000 events - 4.10 GBytes
# MC 2010 Beam3500GeV-VeloClosed-MagDown-Nu3
# B0 -> Jpsi, Ks

from Gaudi.Configuration import * 

EventSelector().Input   = [
"DATAFILE='castor:/castor/cern.ch/user/r/rlambert/testfiles/MC2010_BdJPsiKs_00008414_00000106_1.dst' TYP='POOL_ROOTTREE' OPT='READ'"
#"DATAFILE='PFN:/usera/jonesc/NFS/data/MC/test/MC2010_BdJPsiKs_00008414_00000106_1.dst' TYP='POOL_ROOTTREE' OPT='READ'"
]
#FileCatalog().Catalogs = [ 'xmlcatalog_file:Sim2010_Bd2JpsiKs.xml' ]


