from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci
from StrippingConf.Configuration import StrippingConf
from StrippingConf.StrippingStream import StrippingStream

SL_stream = StrippingStream("_SL_DST")
Dimuon_stream = StrippingStream("_Dimuon_DST")
BHadronCompleteEvent_stream = StrippingStream("_BHad_DST")
CharmCompleteEvent_stream = StrippingStream("_Charm_DST")
Calib_stream = StrippingStream("_Calib_muDST")
OBSOLETE_stream = StrippingStream("_OBSOLETE")

######## obsolete lines 

from StrippingSelections import StrippingB0q2DplusMuX # Responsible: Rob Lambert (probably obsolete, was SL stream)
OBSOLETE_stream.appendLines(StrippingB0q2DplusMuX.B0q2DplusMuXAllLinesConf("B0q2DplusMuX",StrippingB0q2DplusMuX.confdict).lines())

from StrippingSelections import StrippingDForBSemi # resonsible: Syracuse? 
OBSOLETE_stream.appendLines( StrippingDForBSemi.DforBSemiLinesConf("DForBSemi", StrippingDForBSemi.confdict).lines() )

from StrippingSelections import StrippingBd2DstarTauNu # resonsible: Anne Keune? probably obsolete
OBSOLETE_stream.appendLines( StrippingBd2DstarTauNu.Bd2DstarTauNuAllLinesConf("Bd2DstarTauNu", StrippingBd2DstarTauNu.confdict).lines() )

######## Semileptonic full DST #########

from StrippingSelections import StrippingBd2DstarMuNu # Resonsible: Stefania Vechi
SL_stream.appendLines(StrippingBd2DstarMuNu.Bd2DstarMuNuAllLinesConf("Bd2DstarMuNu", StrippingBd2DstarMuNu.confdict).lines())

from StrippingSelections import StrippingB2DMuNuX # Resonsible: Mika
SL_stream.appendLines( StrippingB2DMuNuX.B2DMuNuXAllLinesConf("B2DMuNuX", StrippingB2DMuNuX.confdict).lines() )

from StrippingSelections import StrippingB2XuMuNu # Resonsible: Alessandra (Need someone new!)
SL_stream.appendLines(StrippingB2XuMuNu.B2XuMuNuBuilder("B2XuMuNu", StrippingB2XuMuNu.confdict).lines() )

from StrippingSelections import StrippingB2DMuForTauMu # Resonsible: Greg
SL_stream.appendLines( StrippingB2DMuForTauMu.B2DMuForTauMuconf("B2DMuForTauMu",StrippingB2DMuForTauMu.confdict).lines())

from StrippingSelections import StrippingCharmFromBSemiForHadronAsy # Resonsible: Mika
SL_stream.appendLines(StrippingCharmFromBSemiForHadronAsy.CharmFromBSemiForHadronAsyAllLinesConf("CharmFromBSemiForHadronAsy",StrippingCharmFromBSemiForHadronAsy.confdict).lines())

from StrippingSelections import StrippingLb2pMuNuVub # Resonsible: William
SL_stream.appendLines(StrippingLb2pMuNuVub.Lb2pMuNuVubBuilder("Lb2pMuNuVub",StrippingLb2pMuNuVub.confdict).lines())

####### CALIB STREAM #################

from StrippingSelections import StrippingPhiToKSKS # resonsible: Mika
Calib_stream.appendLines( StrippingPhiToKSKS.PhiToKSKSAllLinesConf("PhiToKSKS", StrippingPhiToKSKS.confdict).lines())

##### CHARM FULL DST STREAM #########

from StrippingSelections import StrippingD0ForBXX # resonsible: Zhou Xing (comment: should add the kaon line)
CharmCompleteEvent_stream.appendLines(StrippingD0ForBXX.D0forBXXLinesConf("D0ForBXX", StrippingD0ForBXX.confdict).lines() )

##### DIMUON STREAM FULL DST ###########

from StrippingSelections import StrippingJPsiForSL # resonsible: Mika Vesterinen ## should add this one to my tests
Dimuon_stream.appendLines( StrippingJPsiForSL.JPsiForSLAllLinesConf("JPsiForSL", StrippingJPsiForSL.confdict).lines())

##### BHADRON FULL DST ##################

from StrippingSelections import StrippingB2XTauNu # resonsible: Conor/Donal
BHadronCompleteEvent_stream.appendLines( StrippingB2XTauNu.B2XTauNuAllLinesConf("B2XTauNu",StrippingB2XTauNu.confdict).lines() )

from StrippingSelections import StrippingB2DHHHForBXX # resonsible: Paolo Gandini
BHadronCompleteEvent_stream.appendLines(StrippingB2DHHHForBXX.B2DHHHForBXXLinesConf("B2DHHHForBXX",StrippingB2DHHHForBXX.confdict).lines() )

##### CONFIGURE THE STRIPPING
from Configurables import  ProcStatusCheck
ActiveStreams = [SL_stream,BHadronCompleteEvent_stream,Dimuon_stream,CharmCompleteEvent_stream,Calib_stream]
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
sr.ReportFrequency = 1000

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
MakePionsEtc = FilterDesktop('MakePionsEtc')
MakePionsEtc.Inputs=StdParticles
MakePionsEtc.Code="ALL"

DaVinci().PrintFreq = 10000
DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().EvtMax = 100000
DaVinci().EventPreFilters = [ filterHLT ]
DaVinci().appendToMainSequence( [MakePionsEtc] )
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
DaVinci().appendToMainSequence( [ ac ] )
DaVinci().DataType = "2012"
DaVinci().InputType = 'SDST'
DaVinci().DDDBtag  = "dddb-20120831"
DaVinci().CondDBtag = "cond-20121008"
importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco14_Run125113.py")

