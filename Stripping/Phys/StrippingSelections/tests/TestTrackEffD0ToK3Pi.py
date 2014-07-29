MODE="DATA"
from Configurables import MCTupleToolHierarchy, TupleToolMCTruth, TupleToolMCBackgroundInfo,DaVinciSmartAssociator,P2MCPFromProtoP
from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci
from StrippingConf.Configuration import StrippingConf
from StrippingConf.StrippingStream import StrippingStream
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from PhysSelPython.Wrappers import AutomaticData

SL_stream = StrippingStream("_SL_DST")

from StrippingSelections import StrippingTrackEffD0ToK3Pi # Resonsible: Mika
#SL_stream.appendLines( [StrippingTrackEffD0ToK3Pi.TrackEffD0ToK3PiAllLinesConf("UNPACK", StrippingTrackEffD0ToK3Pi.confdict).b2D0MuXLine] )
SL_stream.appendLines( StrippingTrackEffD0ToK3Pi.TrackEffD0ToK3PiAllLinesConf("TrackEffD0ToK3Pi", StrippingTrackEffD0ToK3Pi.confdict).lines() )

##### CONFIGURE THE STRIPPING
from Configurables import  ProcStatusCheck
ActiveStreams = [SL_stream]
sc = StrippingConf( Streams = ActiveStreams,
                    MaxCandidates = 2000,
                    AcceptBadEvents = False,
                    BadEventSelection = ProcStatusCheck() )

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True

# Remove the microbias and beam gas etc events before doing the tagging step
regexp = "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!NZS)(?!Velo)(?!BeamGas)(?!Incident).*Decision')"
from Configurables import LoKi__HDRFilter
filterHLT = LoKi__HDRFilter("FilterHLT",Code = regexp )

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

from Configurables import AuditorSvc, ChronoAuditor
AuditorSvc().Auditors.append( ChronoAuditor("Chrono") )

from Configurables import StrippingReport
sr = StrippingReport(Selections = sc.selections())
sr.OnlyPositive = False
sr.ReportFrequency = 100

from Configurables import AlgorithmCorrelationsAlg
ac = AlgorithmCorrelationsAlg(Algorithms = sc.selections())

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True

from Configurables import FilterDesktop

StdParticles = []

StdParticles.append("Phys/StdNoPIDsProtons")
StdParticles.append("Phys/StdNoPIDsPions")
StdParticles.append("Phys/StdNoPIDsKaons")
StdParticles.append("Phys/StdNoPIDsMuons")
StdParticles.append("Phys/StdAllLoosePions")
StdParticles.append("Phys/StdLoosePions")
StdParticles.append("Phys/StdLooseKaons")
StdParticles.append("Phys/StdLooseProtons")
StdParticles.append("Phys/StdLooseKsLL")
StdParticles.append("Phys/StdLooseKsDD")
StdParticles.append("Phys/StdLooseLambdaLL")
StdParticles.append("Phys/StdLooseLambdaDD")
StdParticles.append("Phys/StdLooseResolvedPi0")
StdParticles.append("Phys/StdLooseMergedPi0")
StdParticles.append("Phys/StdLooseDstarWithD02KPi")
StdParticles.append("Phys/StdLooseDplus2KPiPi")
StdParticles.append("Phys/StdLooseD02KPi")
StdParticles.append("Phys/StdLooseDsplus2KKPi")
StdParticles.append('Phys/StdLooseJpsi2MuMu')
StdParticles.append('Phys/StdLooseDetachedTau')
StdParticles.append('Phys/StdLooseDetachedTau3pi')
StdParticles.append('Phys/StdLooseDetachedTau3piNonPhys')
## reset
StdParticles = ["StdAllNoPIDsPions","StdLoosePions","StdLooseMuons","StdLooseKaons","StdLooseProtons","StdNoPIDsPions"]
MakePionsEtc = FilterDesktop('MakePionsEtc')
MakePionsEtc.Inputs=StdParticles
MakePionsEtc.Code="ALL"


####################### FAST VELO TRACKING #########################################
from Configurables import DecodeVeloRawBuffer, FastVeloTracking, TrackPrepareVelo, NoPIDsParticleMaker, DataOnDemandSvc, ChargedProtoParticleMaker,  ChargedProtoParticleAddRichInfo,  ChargedPP2MC, ChargedProtoCombineDLLsAlg, PrTrackAssociator, PrLHCbID2MCParticle, UnpackMCParticle, UnpackMCVertex, P2MCPFromProtoP, DelegatingTrackSelector, TrackContainerCopy, TrackAssociator
from TrackFitter.ConfiguredFitters import ConfiguredFit, ConfiguredFitSeed, ConfiguredMasterFitter

preve = TrackPrepareVelo("preve")
preve.inputLocation = "Rec/Track/Velo"
preve.outputLocation = "Rec/Track/UnFittedVelo"
preve.bestLocation = ""
VeloDecoding = DecodeVeloRawBuffer("VeloDecoding",DecodeToVeloLiteClusters=True,DecodeToVeloClusters=True)
MyFastVeloTracking = FastVeloTracking("FastVelo",OutputTracksName="Rec/Track/Velo")
alg = GaudiSequencer("VeloMuonTrackingFor")
alg.Members = [ VeloDecoding,MyFastVeloTracking,preve]

copyVelo = TrackContainerCopy( "CopyVelo" )
copyVelo.inputLocation = "Rec/Track/UnFittedVelo";
copyVelo.outputLocation = "Rec/Track/PreparedVelo";
fitter = GaudiSequencer("VeloMuonTrackingFor")
fitter.Members += [ copyVelo , ConfiguredFit("FitVelo","Rec/Track/PreparedVelo") ]

VeloProtoOutputLocation = 'Rec/ProtoP/VeloProtos'
VeloProtos = ChargedProtoParticleMaker('VeloProtos')
VeloProtos.Inputs=['Rec/Track/PreparedVelo']
VeloProtos.Output = VeloProtoOutputLocation
VeloProtos.addTool( DelegatingTrackSelector, name="TrackSelector" )
VeloProtos.TrackSelector.TrackTypes = [ "Velo" ]

### enable MC truth matching for the pions
assoctr = TrackAssociator("AssocTr")
assoctr.TracksInContainer = "Rec/Track/PreparedVelo"
AssocProtos=ChargedPP2MC("AssocProtos")                                                                                                                                          
AssocProtos.TrackLocations = [ 'Rec/Track/PreparedVelo' ]                                                                                                             
AssocProtos.InputData = [ VeloProtoOutputLocation ]                                                                                                                     
AssocProtos.OutputTable = "Relations/"+VeloProtoOutputLocation                                                                                                          

VeloAllPions = NoPIDsParticleMaker("VeloAllPions"
                                   , Particle             = "pion"
                                   , Input                 =  VeloProtoOutputLocation
                                   , Output                = 'Phys/VeloAllPions/Particles'
                                   , WriteP2PVRelations        =  False)

makeparts = GaudiSequencer('makeparts')
if MODE == "MC":
    makeparts.Members = [VeloProtos,assoctr,AssocProtos,VeloAllPions]
else:
    makeparts.Members = [VeloProtos,VeloAllPions]

################ tuples

from Configurables import LoKi__Hybrid__TupleTool
from Configurables import TupleToolDecay
LoKi_KIN=LoKi__Hybrid__TupleTool("LoKi_KIN")
LoKi_KIN.Variables =  {
    "DTF_D0_M" : "DTF_FUN ( CHILD(1,M) ,True)",
    "DTFD0_D0_M" : "DTF_FUN ( CHILD(1,M) ,True, 'D0')",
    "DTFD0_Dst_M" : "DTF_FUN ( M ,True, 'D0')",
    "DTFD0_CHI2NDF" : "DTF_CHI2NDOF(True,'D0')"}

tupletools = []
#tupletools.append("TupleToolPrimaries")                                                                                                                                        
tupletools.append("TupleToolKinematic")                                                                                                                                 
#tupletools.append("TupleToolGeometry")                                                                                                                                     
#Tupletools.append("TupleToolTrackInfo")                                                                                                                                        
tupletools.append("TupleToolPid")                                                                                                                                                
#tupletools.append("TupleToolRecoStats") 

triglist=['Hlt1TrackAllL0Decision',
          'Hlt2CharmHadD02HH_D02KPiDecision']
from Configurables import TupleToolTISTOS
TISTOSTool = TupleToolTISTOS('TISTOSTool')
TISTOSTool.VerboseL0 = False
TISTOSTool.VerboseHlt1 = True
TISTOSTool.VerboseHlt2 = True
TISTOSTool.TriggerList = triglist[:]

from Configurables import DecayTreeTuple
DecayDescr = '[D*(2010)+ ->  ^(D0 ->  ^(K*(892)+ -> ^K- ^pi+ ^pi+) ^[pi-]CC)  ^pi+]CC'
Branches = {'Dst'   : '[D*(2010)+ ->   (D0 ->   (K*(892)+ ->  K-  pi+  pi+)  [pi-]CC)   pi+]CC',
            'D0'    : '[D*(2010)+ ->  ^(D0 ->   (K*(892)+ ->  K-  pi+  pi+)  [pi-]CC)   pi+]CC',
            'Kst'   : '[D*(2010)+ ->   (D0 ->  ^(K*(892)+ ->  K-  pi+  pi+)  [pi-]CC)   pi+]CC',
            'K'     : '[D*(2010)+ ->   (D0 ->   (K*(892)+ -> ^K-  pi+  pi+)  [pi-]CC)   pi+]CC',
            'Pi1'   : '[D*(2010)+ ->   (D0 ->   (K*(892)+ ->  K- ^pi+  pi+)  [pi-]CC)   pi+]CC',
            'Pi2'   : '[D*(2010)+ ->   (D0 ->   (K*(892)+ ->  K-  pi+ ^pi+)  [pi-]CC)   pi+]CC',
            'Pi'    : '[D*(2010)+ ->   (D0 ->   (K*(892)+ ->  K-  pi+  pi+) ^[pi-]CC)   pi+]CC',
            'Slowpi': '[D*(2010)+ ->   (D0 ->   (K*(892)+ ->  K-  pi+  pi+)  [pi-]CC)  ^pi+]CC'}
TupleDst = DecayTreeTuple("Dst")
TupleDst.Decay = DecayDescr
TupleDst.Inputs = ['Phys/TrackEffD0ToK3PiDstarLine/Particles']

TupleDst.ToolList =  tupletools[:]
TupleDst.Branches = Branches
TupleDst.addTool(TupleToolDecay, name="Dst")
TupleDst.Dst.addTool(LoKi_KIN)
TupleDst.Dst.ToolList+=["LoKi::Hybrid::TupleTool/LoKi_KIN"]
TupleDst.addTool(TISTOSTool, name="TISTOSTool")
TupleDst.ToolList += [ "TupleToolTISTOS/TISTOSTool" ]
TupleDst.addTool(TupleToolMCTruth,name="TruthTool")
TupleDst.addTool(TupleToolMCBackgroundInfo,name="BackgroundInfo")
TupleDst.ToolList+=["TupleToolMCTruth/TruthTool"]
TupleDst.ToolList+=["TupleToolMCBackgroundInfo/BackgroundInfo"]
TupleDst.addTool(MCTupleToolHierarchy,name="Hierarchy")
TupleDst.ToolList+=["MCTupleToolHierarchy/Hierarchy"]
TupleDst.TruthTool.addTool(MCTupleToolHierarchy())
TupleDst.TruthTool.ToolList+=["MCTupleToolHierarchy/Hierarchy"]
rellocations= [ "Relations/Rec/ProtoP/Charged",        
                "Relations/Rec/ProtoP/Upstream",   
                "Relations/Rec/ProtoP/Neutrals",       
                "Relations/"+VeloProtoOutputLocation]
TupleDst.TruthTool.addTool(DaVinciSmartAssociator)
TupleDst.TruthTool.DaVinciSmartAssociator.addTool(P2MCPFromProtoP) 
TupleDst.TruthTool.DaVinciSmartAssociator.P2MCPFromProtoP.Locations=rellocations  
TupleDst.Hierarchy.addTool(DaVinciSmartAssociator)
TupleDst.Hierarchy.DaVinciSmartAssociator.addTool(P2MCPFromProtoP) 
TupleDst.Hierarchy.DaVinciSmartAssociator.P2MCPFromProtoP.Locations=rellocations  
    
#from Configurables import MicroDSTWriter
#conf = MicroDSTWriter("MicroDST0")
#conf.OutputFileSuffix = "Test"
#conf.SelectionSequences = sc.activeStreams()

DaVinci().PrintFreq = 1000
DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().EvtMax = 10000
DaVinci().TupleFile = 'DVTuples.root'
DaVinci().EventPreFilters = [ filterHLT ]
DaVinci().appendToMainSequence( [MakePionsEtc] )
DaVinci().appendToMainSequence( [alg] )
DaVinci().appendToMainSequence( [ makeparts ] )
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
#DaVinci().appendToMainSequence( [ TupleDst ] )
if MODE == "MC":
    DaVinci().DataType = "2011"
    DaVinci().Simulation = True
    DaVinci().InputType = 'DST'
    #DaVinci().DDDBtag  = "dddb-20120831"
    #DaVinci().CondDBtag = "sim-20111111-vc-md100" 
    from GaudiConf import IOHelper
    #evt://MC/MC11a/21163020 ( D+_K-pi+pi+=phsp,DecProdCut )/Beam3500GeV-2011-MagDown-Nu2-EmNoCuts/Sim05/Trig0x40760037Flagged/Reco12a/Stripping17NoPrescalingFlagged/ALLSTREAMS.DST
    IOHelper().inputFiles(['PFN:root://eoslhcb.cern.ch//eos/lhcb/grid/prod/lhcb/MC/MC11a/ALLSTREAMS.DST/00014704/0000/00014704_00000001_1.allstreams.dst'])
else:
    DaVinci().DataType = "2012"
    DaVinci().Simulation = False
    DaVinci().InputType = 'SDST'
    importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco14_Run125113.py")
    DaVinci().DDDBtag  = "dddb-20120831"
    DaVinci().CondDBtag = "cond-20121008"
