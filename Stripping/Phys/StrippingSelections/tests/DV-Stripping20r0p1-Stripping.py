"""
Options for building Stripping20r1. 
"""

DecayDescriptorDictionary = {
  "Strippingbb2D0MuXBB2DMuNuXLine" : "[Z0 -> (^B+ -> (^D~0 -> ^K+ ^pi-) ^mu+) (^B- -> (^D0 -> ^K- ^pi+) ^mu-)]cc",
  "StrippingFullDSTDiMuonJpsi2MuMuDetachedMinusLine" : "[J/psi(1S) -> ^mu+ ^mu-]cc",
  "StrippingFullDSTDiMuonPsi2MuMuDetachedMinusLine" : "[J/psi(1S) -> ^mu+ ^mu-]cc",
  "StrippingFullDSTDiMuonDiMuonNoPVLine" : "[J/psi(1S) -> ^mu+ ^mu-]cc",
  "StrippingChiCJPsiGammaConvChibLine" : "[chi_c1(1P) -> (^J/psi(1S) -> ^mu+ ^mu-) ^gamma]cc",
  "StrippingChiCJPsiGammaConvChicLine" : "[chi_c1(1P) -> (^J/psi(1S) -> ^mu+ ^mu-) ^gamma]cc",
  "StrippingHeavyBaryonsXib2JpsiXi" : "[Xi_b- -> (^Xi- -> ^Lambda0 ^pi-) (^J/psi(1S) -> ^mu+ ^mu-)]cc",
  "StrippingHeavyBaryonsXibzero2JpsiXistar" : "[Xi_b0 -> (^Xi- -> ^Lambda0 ^pi-) ^pi+ (^J/psi(1S) -> ^mu+ ^mu-)]cc",
  "StrippingHeavyBaryonsOmegab2JpsiOmega" : "[Omega_b- -> (^Omega- -> ^Lambda0 K-) (^J/psi(1S) -> ^mu+ ^mu-)]cc",
  "StrippingXicHHHXic2PKPiLine" : "[Lambda_c+ -> ^K- ^pi+ ^p+]cc",
  "StrippingXicHHHXic2PKKLine" : "[Lambda_c+ -> ^K- ^K+ ^p+]cc",
  "StrippingXicHHHTheta2PKS0Line" : "[Lambda_c+ -> ^p+ (^KS0 -> ^pi+ ^pi-)]cc ",
  "StrippingXicHHHXic2KLamLine" : "[Lambda_c+ -> ^K+ (^Lambda0 -> ^p+ ^pi-)]cc",
  "StrippingXicc+ForPromptCharm" : ["[ Xi_cc+ -> (^Lambda_c+ -> ^K- ^pi+ ^p+) ^K- ^pi+]cc", 
                                    "[ Xi_cc+ -> (^Lambda_c+ -> ^K- ^pi+ ^p+) ^K+ ^pi-]cc"],
  "StrippingXicc++ForPromptCharm" : ["[ Xi_cc++ -> (^Lambda_c+ -> ^K- ^pi+ ^p+) ^K- ^pi+ ^pi+]cc",
                                     "[ Xi_cc++ -> (^Lambda_c+ -> ^K- ^pi+ ^p+) ^K+ ^pi- ^pi-]cc"],
  "StrippingDsLamCForPromptCharm" : ["[Lambda_b0 -> ^Lambda_c+ ^D_s-]cc", 
                                     "[Lambda_b0 -> ^Lambda_c+ ^D_s+]cc" ],
  "StrippingMicroDSTDiMuonDiMuonIncLowPTLine" : "[J/psi(1S) -> ^mu+ ^mu-]cc",
  "StrippingBetac2PhiPBetac2PhiPLine" : "[Lambda_c+ -> ^p+ (^phi(1020) -> ^K+ ^K-)]cc",
  "StrippingCharmAndWForPromptCharm" : [
            " [ chi_b0(2P) -> (^D0 -> ^K- ^pi+)  ^mu+ ]cc " ,
            " [ chi_b0(2P) -> (^D0 -> ^K- ^pi+)  ^mu- ]cc " ,
            " [ chi_b0(2P) -> (^D*(2010)+ -> (^D0 -> ^K- ^pi+) ^pi+) mu+ ]cc " ,
            " [ chi_b0(2P) -> (^D*(2010)+ -> (^D0 -> ^K- ^pi+) ^pi+) mu- ]cc " ,
            " [ chi_b0(2P) -> (^D+ -> ^K- ^pi+ ^pi+)  ^mu+ ]cc " ,
            " [ chi_b0(2P) -> (^D+ -> ^K- ^pi+ ^pi+)  ^mu- ]cc " ,
            " [ chi_b0(2P) -> (^D_s+ -> ^K- ^K+ ^pi+) ^mu+ ]cc " ,
            " [ chi_b0(2P) -> (^D_s+ -> ^K- ^K+ ^pi+) ^mu- ]cc " ,
            " [ chi_b0(2P) -> (^Lambda_c+ -> ^p+ ^K- ^pi+) ^mu+ ]cc " ,
            " [ chi_b0(2P) -> (^Lambda_c+ -> ^p+ ^K- ^pi+) ^mu- ]cc " ,
            ] ,
  "StrippingDiMuonAndWForPromptCharm" : [
            "[ Upsilon(1S) -> (^J/psi(1S) -> ^mu+ ^mu-) (^D0 -> ^K- ^pi+) ]cc" ,
            "[ Upsilon(1S) -> (^J/psi(1S) -> ^mu+ ^mu-) (^D*(2010)+ -> ^K- ^pi+ ^pi+) ]cc" ,
            "[ Upsilon(1S) -> (^J/psi(1S) -> ^mu+ ^mu-) (^D+ -> ^K- ^pi+ ^pi+) ]cc" ,
            "[ Upsilon(1S) -> (^J/psi(1S) -> ^mu+ ^mu-) (^D_s+ -> ^K- ^K+ ^pi+) ]cc" ,
            "[ Upsilon(1S) -> (^J/psi(1S) -> ^mu+ ^mu-) (^Lambda_c+ -> ^p+ ^pi+ ^K-) ]cc"
            ] ,
}






from Configurables import DecayTreeTuple
from Configurables import DecayTreeTuple
from DecayTreeTuple import *
from Configurables import FilterDesktop, DecayTreeTuple, TupleToolTISTOS
from Configurables import TupleToolTrigger, TupleToolEventInfo, TupleToolDecay
from Configurables import TupleToolRecoStats, TupleToolGeometry, TupleToolPid
from DecayTreeTuple.Configuration import *
from Configurables import TupleToolMuonPid
from Configurables import ReadHltReport

def simpleTupleTool (Stream, StrippingLineName, DecayDescriptor):
  shortName = StrippingLineName.replace("Stripping", "",1)
  tuple = DecayTreeTuple("Tuple"+shortName)


  isMicroDst = False;
  if (Stream == 'Leptonic' or
      Stream == 'Charm' or
      Stream == 'PID'   or
      Stream == 'Bhadron' ):
    isMicroDst = True;

#  if isMicroDst:
#    tuple.RootInTES = '/Event/' + Stream
#    StrippingPath   = "Phys/" + shortName + "/Particles"
#  else :
  StrippingPath   = "Phys/" + shortName + "/Particles"

  print StrippingPath

  tuple.Inputs = [ StrippingPath ]
  tuple.Decay = DecayDescriptor
  

  tuple.ToolList = [
  #    "TupleToolAngles",
      "TupleToolEventInfo",
#      "TupleToolGeometry",
      "TupleToolKinematic",
      "TupleToolPid",
      "TupleToolMuonPid",
      "TupleToolRICHPid",
      "TupleToolPrimaries",     
      "TupleToolPropertime",
      "TupleToolRecoStats",
      "TupleToolTrackInfo",
      "TupleToolTrackIsolation"
      ]

  tuple.addTool(TupleToolRecoStats, name="TupleToolRecoStats")
  tuple.TupleToolRecoStats.Verbose=True

  tuple.addTool(TupleToolGeometry, name="TupleToolGeometry")
  tuple.TupleToolGeometry.Verbose=True

  eta = tuple.addTupleTool("LoKi::Hybrid::TupleTool")
  eta.Variables = { "ETA"   : "ETA" } 

  tlist = [
      "L0HadronDecision",
      "L0MuonDecision",
      "L0DiMuonDecision"
      "L0HadronDecision"
      "Hlt1DiMuonHighMassDecision",
      "Hlt1DiMuonLowMassDecision",
      "Hlt1DiProtonDecision",
      "Hlt1SingleMuonHighPTDecision",
      "Hlt1TrackAllL0Decision",
      "Hlt1TrackMuonDecision",
      "Hlt2CharmHadD02HH_D02KPiDecision",
      "Hlt2CharmHad2HHHDecision",
      "Hlt2DiMuonDetachedDecision",
      "Hlt2DiMuonDetachedHeavyDecision",
      "Hlt2DiMuonDetachedJPsiDecision",
      "Hlt2DiMuonJPsiDecision",
      "Hlt2DiMuonJPsiHighPTDecision",
  ]

  TTT = tuple.addTupleTool("TupleToolTrigger")
  TTT.VerboseHlt1 = True
  TTT.VerboseHlt2 = True
  TTT.VerboseStripping  = True
  TTT.TriggerList = tlist

  TisTosTT = tuple.addTupleTool("TupleToolTISTOS")
  TisTosTT.VerboseL0   = True
  TisTosTT.VerboseHlt1 = True
  TisTosTT.VerboseHlt2 = True
  TisTosTT.VerboseStripping  = True
  TisTosTT.TriggerList = tlist


  hltSequence = GaudiSequencer("hltSeq"+shortName)
  hltSequence.Members = [tuple, ReadHltReport()]
  #DaVinci().UserAlgorithms += [ tuple ]  # The algorithms
  return hltSequence;
  


####### MAIN #########

from Gaudi.Configuration import *
MessageSvc().Format = "% F%30W%S%7W%R%T %0W%M"

# Tighten Trk Chi2 to <3
from CommonParticles.Utils import DefaultTrackingCuts
DefaultTrackingCuts().Cuts  = { "Chi2Cut" : [ 0, 3 ],
                                "CloneDistCut" : [5000, 9e+99 ] }

#
# Build the streams and stripping object
#
from StrippingConf.Configuration import StrippingConf, StrippingStream
from StrippingSettings.Utils import strippingConfiguration
from StrippingSelections.Utils import buildStreams
from StrippingArchive import strippingArchive

stripping='stripping20r0p1'
config  = strippingConfiguration(stripping)

streams = buildStreams(stripping = config, WGs = [ 'BandQ' ] )

from Configurables import ProcStatusCheck
filterBadEvents = ProcStatusCheck()

sc = StrippingConf( Streams = streams,
                    MaxCandidates = 2000,
                    AcceptBadEvents = False,
                    BadEventSelection = filterBadEvents, 
                    TESPrefix = 'Strip' )


tupletools = []
for stream in streams:
  for line in stream.lines:
    print stream.name(), ": ", line.name()
    if not isinstance(DecayDescriptorDictionary[line.name()], list):
      ddlist = [DecayDescriptorDictionary[line.name()]]
    else:
      ddlist = DecayDescriptorDictionary[line.name()]

    for decayDesc in ddlist:
      tupletools += [simpleTupleTool (stream.name(), line.name(), decayDesc)]
    
enablePacking = True

from DSTWriters.microdstelements import *
from DSTWriters.Configuration import (SelDSTWriter,
                                      stripDSTStreamConf,
                                      stripDSTElements,
                                      stripMicroDSTStreamConf,
                                      stripMicroDSTElements,
                                      stripCalibMicroDSTStreamConf
                                      )

#
# Configuration of MicroDST
#
mdstStreamConf = stripMicroDSTStreamConf(pack=enablePacking)
mdstElements   = stripMicroDSTElements(pack=enablePacking)

leptonicMicroDSTname   = 'Leptonic'
charmMicroDSTname      = 'Charm'
pidMicroDSTname        = 'PID' 
bhadronMicroDSTname    = 'Bhadron' 

#
# Configuration of SelDSTWriter
#
SelDSTWriterElements = {
    'default'               : stripDSTElements(pack=enablePacking),
    charmMicroDSTname       : mdstElements,
    leptonicMicroDSTname    : mdstElements,
    pidMicroDSTname         : mdstElements,
    bhadronMicroDSTname     : mdstElements
    }


SelDSTWriterConf = {
    'default'                : stripDSTStreamConf(pack=enablePacking),
    charmMicroDSTname        : mdstStreamConf,
    leptonicMicroDSTname     : mdstStreamConf,
    bhadronMicroDSTname      : mdstStreamConf,
    pidMicroDSTname          : stripCalibMicroDSTStreamConf(pack=enablePacking)
    }


dstWriter = SelDSTWriter( "MyDSTWriter",
                          StreamConf = SelDSTWriterConf,
                          MicroDSTElements = SelDSTWriterElements,
                          OutputFileSuffix ='000000',
                          SelectionSequences = sc.activeStreams()
                          )


# Add cone variables to Bhadron and Charm streams
from Configurables import AddExtraInfo, ConeVariables

bhadronOutputLocations = []
charmOutputLocations = []

for stream in streams :
    if stream.name() == 'Bhadron' : bhadronOutputLocations = [ "/Event/" + x for x in stream.outputLocations() ]
    if stream.name() == 'Charm' : charmOutputLocations = [ "/Event/" + x for x in stream.outputLocations() ]

bhadron_extra = AddExtraInfo('BhadronExtraInfo')
bhadron_extra.Inputs = bhadronOutputLocations

cv1   = ConeVariables('BhadronExtraInfo.ConeVariables1', ConeAngle = 1.5, ConeNumber = 1,
                       Variables = ['angle', 'mult', 'ptasy'] )
cv2   = ConeVariables('BhadronExtraInfo.ConeVariables2', ConeAngle = 1.7, ConeNumber = 2,
                       Variables = ['angle', 'mult', 'ptasy'] )
cv3   = ConeVariables('BhadronExtraInfo.ConeVariables3', ConeAngle = 1.0, ConeNumber = 3,
                       Variables = ['angle', 'mult', 'ptasy'] )

bhadron_extra.addTool( cv1 , 'BhadronExtraInfo.ConeVariables1')
bhadron_extra.addTool( cv2 , 'BhadronExtraInfo.ConeVariables2')
bhadron_extra.addTool( cv3 , 'BhadronExtraInfo.ConeVariables3')
bhadron_extra.Tools = [ 'ConeVariables/BhadronExtraInfo.ConeVariables1',
                        'ConeVariables/BhadronExtraInfo.ConeVariables2', 
                        'ConeVariables/BhadronExtraInfo.ConeVariables3', 
                         ]

charm_extra = AddExtraInfo('CharmExtraInfo')
charm_extra.Inputs = charmOutputLocations
#charm_extra.MaxLevel = 1

charm_cv1   = ConeVariables('CharmExtraInfo.ConeVariables1', ConeAngle = 1.5, ConeNumber = 1,
                            Variables = ['angle', 'mult', 'ptasy'] )

charm_extra.addTool( charm_cv1 , 'CharmExtraInfo.ConeVariables1')
charm_extra.Tools = [ 'ConeVariables/CharmExtraInfo.ConeVariables1' ]


# Add stripping TCK 
from Configurables import StrippingTCK
stck = StrippingTCK(HDRLocation = '/Event/Strip/Phys/DecReports', TCK=0x32232001)

#
# DaVinci Configuration
#
from Configurables import DaVinci
DaVinci().EvtMax = -1                        # Number of events
DaVinci().HistogramFile = "DVHistos.root"
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ bhadron_extra, charm_extra ] )
DaVinci().appendToMainSequence( [ stck ] )
DaVinci().appendToMainSequence( [ dstWriter.sequence() ] )
DaVinci().ProductionType = "Stripping"
DaVinci().TupleFile = "/afs/cern.ch/work/l/landerli/public/TupleTool.root"


# Change the column size of Timing table
from Configurables import TimingAuditor, SequencerTimerTool
TimingAuditor().addTool(SequencerTimerTool,name="TIMER")
TimingAuditor().TIMER.NameSize = 60

# For tests
from Configurables import ApplicationMgr
ApplicationMgr().ExtSvc += [ 'AuditorSvc' ]
#AuditorSvc().Auditors += [ 'MemoryAuditor' ]
from Configurables import AuditorSvc, ChronoAuditor
AuditorSvc().Auditors.append( ChronoAuditor("Chrono") )
from Configurables import StrippingReport
sr = StrippingReport(Selections = sc.selections(), OnlyPositive = False, EveryEvent = False, ReportFrequency=2000)
DaVinci().appendToMainSequence( [ sr ] )
DaVinci().appendToMainSequence( tupletools )


DaVinci().Simulation = False
DaVinci().DataType  = "2012"
DaVinci().InputType = "DST"

# change the column size of timing table
from Configurables import TimingAuditor, SequencerTimerTool
TimingAuditor().addTool(SequencerTimerTool,name="TIMER")
TimingAuditor().TIMER.NameSize = 60

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

# database
DaVinci().DDDBtag  = "dddb-20120831"
DaVinci().CondDBtag = "cond-20121008"

# input file
importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco14_Run125113.py")
