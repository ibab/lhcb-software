# $Id: $
# Test your line(s) of the stripping

from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci
from StrippingConf.Configuration import StrippingConf

# Tighten Trk Chi2 to <3
from CommonParticles.Utils import DefaultTrackingCuts
DefaultTrackingCuts().Cuts  = { "Chi2Cut" : [ 0, 3 ],
                                "CloneDistCut" : [5000, 9e+99 ] }
# Now build the stream
from StrippingConf.StrippingStream import StrippingStream
Semileptonic_stream = StrippingStream("_Semileptonic")
Dimuon_stream = StrippingStream("_Dimuon")
BhadronCompleteEvent_stream = StrippingStream("_BHad")
PID_stream = StrippingStream("_PID")
Charm_stream = StrippingStream("_Charm")
CharmComplete_stream = StrippingStream("_CharmComplete")

##### For the Stripping 20r0p1 and 20r1p1 incremental restrippings.

from StrippingSelections import StrippingCharmFromBSemiForHadronAsy
from StrippingSettings.Stripping20r0p1.LineConfigDictionaries_Semileptonic import CharmFromBSemiForHadronAsy
confCharmFromBSemiForHadronAsy = StrippingCharmFromBSemiForHadronAsy.CharmFromBSemiForHadronAsyAllLinesConf("CharmFromBSemiForHadronAsy", CharmFromBSemiForHadronAsy['CONFIG'])
Semileptonic_stream.appendLines( confCharmFromBSemiForHadronAsy.lines() )

#from StrippingSelections import StrippingB2DMuForTauMu
#from StrippingSettings.Stripping20r0p1.LineConfigDictionaries_Semileptonic import B2DMuForTauMu
#confB2DMuForTauMu = StrippingB2DMuForTauMu.B2DMuForTauMuconf("B2DMuForTauMu",B2DMuForTauMu['CONFIG'])
#Semileptonic_stream.appendLines(confB2DMuForTauMu.lines())

#from StrippingSelections import StrippingB2XTauNu
#from StrippingSettings.Stripping20r0p1.LineConfigDictionaries_Semileptonic import B2XTauNu
#confB2XTauNu = StrippingB2XTauNu.B2XTauNuAllLinesConf("B2XTauNu",B2XTauNu['CONFIG'])
#for l in confB2XTauNu.lines():
#    if l.name() in B2XTauNu['STREAMS']['BhadronCompleteEvent']:
#        BhadronCompleteEvent_stream.append(l)

#from StrippingSelections import StrippingB2DHHHForBXX
#from StrippingSettings.Stripping20r0p1.LineConfigDictionaries_Semileptonic import B2DHHHForBXX
#confB2DHHHForBXX =  StrippingB2DHHHForBXX.B2DHHHForBXXLinesConf("B2DHHHForBXX",B2DHHHForBXX['CONFIG'])
#BhadronCompleteEvent_stream.appendLines( confB2DHHHForBXX.lines() )

### flavour tagging calibration
#from StrippingSelections import StrippingBd2DstarMuNu
#confBd2DstarMuNu = StrippingBd2DstarMuNu.Bd2DstarMuNuAllLinesConf("Bd2DstarMuNu", StrippingBd2DstarMuNu.confdict)
#Semileptonic_stream.appendLines( confBd2DstarMuNu.lines() )

### delta a_fs
#from StrippingSelections import StrippingB0q2DplusMuX 
#confStrippingB0q2DplusMuX  = StrippingB0q2DplusMuX.B0q2DplusMuXAllLinesConf("B0q2DplusMuX",StrippingB0q2DplusMuX.confdict)
#Semileptonic_stream.appendLines( confStrippingB0q2DplusMuX.lines() )

### V_ub
#from StrippingSelections import StrippingB2XuMuNu
#confB2XuMuNu = StrippingB2XuMuNu.B2XuMuNuBuilder("B2XuMuNu", StrippingB2XuMuNu.confdict)
#Semileptonic_stream.appendLines( confB2XuMuNu.lines() )

### Delta_ACP, a_sl, B-fractions etc...
#from StrippingSelections import StrippingB2DMuNuX
#confB2DMuNuX = StrippingB2DMuNuX.B2DMuNuXAllLinesConf("B2DMuNuX", StrippingB2DMuNuX.confdict)
#Semileptonic_stream.appendLines( confB2DMuNuX.lines() )

### B->D tau(->mu) nu 

                    
#from StrippingSettings.Stripping20r0p1.LineConfigDictionaries_Semileptonic import B2DMuForTauMu
#confB2KSh = Bu2KShConf("Bu2KSh",B2KSh['CONFIG'])
#for l in confB2KSh.lines():
#    if l.name() in B2KSh['STREAMS']['Bhadron']:
#            bhadronLines.append(l)
#            if l.name() in B2KSh['STREAMS']['BhadronCompleteEvent']:
 #                   bhadroncompleteeventLines.append(l)
 #                   

#confB2DMuForTauMu = StrippingB2DMuForTauMu.B2DMuForTauMuconf("B2DMuForTauMu",StrippingB2DMuForTauMu.confdict)
#Semileptonic_stream.appendLines( confB2DMuForTauMu.lines())

##### CHARM MICRO DST STREAM #########

#from StrippingSelections import StrippingDForBSemi
#confDForBSemi = StrippingDForBSemi.DforBSemiLinesConf("DForBSemi", StrippingDForBSemi.confdict)
#Charm_stream.appendLines( confDForBSemi.lines() )

#from StrippingSelections import StrippingD0ForBXX
#confD0ForBXX = StrippingD0ForBXX.D0forBXXLinesConf("D0ForBXX", StrippingD0ForBXX.confdict)
#Charm_stream.appendLines( confD0ForBXX.lines() )

##### DIMUON STREAM FULL DST ###########

## semi-incl J/psi from B
#from StrippingSelections import StrippingJPsiForSL
#confJPsiForSL = StrippingJPsiForSL.JPsiForSLAllLinesConf("JPsiForSL", StrippingJPsiForSL.confdict)
#Dimuon_stream.appendLines( confJPsiForSL.lines() )

##### BHADRON FULL DST ##################


#### PID MICRO DST STREAM ###############

## MUID calibration
#from StrippingSelections import StrippingMuIDCalib
#confMuIDCalib = StrippingMuIDCalib.MuIDCalibConf("MuIDCalib",StrippingMuIDCalib.config_params)
#PID_stream.appendLines([confMuIDCalib.line_DetachedNoMIP])

from Configurables import  ProcStatusCheck
filterBadEvents =  ProcStatusCheck()

# Configure the stripping using the same options as in Reco06-Stripping10
#sc = StrippingConf( Streams = [ Semileptonic_stream,Dimuon_stream,PID_stream , Charm_stream],
sc = StrippingConf( Streams = [ Semileptonic_stream,Dimuon_stream,Charm_stream,BhadronCompleteEvent_stream],
                    MaxCandidates = 2000,
                    AcceptBadEvents = False,
                    BadEventSelection = filterBadEvents )

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
MakePionsEtc = FilterDesktop('MakePionsEtc')
MakePionsEtc.Inputs=StdParticles
MakePionsEtc.Code="ALL"

DaVinci().PrintFreq = 10000
DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().EvtMax = 20000
DaVinci().EventPreFilters = [ filterHLT ]
DaVinci().appendToMainSequence( [MakePionsEtc] )
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
#DaVinci().appendToMainSequence( [ ac ] )
DaVinci().DataType = "2012"
DaVinci().InputType = 'SDST'
#DaVinci().DDDBtag  = "head-20120413"
#DaVinci().CondDBtag = "head-20120420"
DaVinci().DDDBtag  = "dddb-20120831"
DaVinci().CondDBtag = "cond-20121008"


#importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco13c_Run124134.py")
#importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco14_2011Data_MagDn.py")
importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco14_Run125113.py")
#importOptions("$STRIPPINGSELECTIONSROOT/Reco13e.py")
