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
SL_stream = StrippingStream("_SL_DST")
Dimuon_stream = StrippingStream("_Dimuon_DST")
BHad_stream = StrippingStream("_BHad_DST")
PID_stream = StrippingStream("_PID_MDST")
Charm_stream = StrippingStream("_Charm_MDST")

######## Semileptonic full DST #########

### flavour tagging calibration
from StrippingSelections import StrippingBd2DstarMuNu
confBd2DstarMuNu = StrippingBd2DstarMuNu.Bd2DstarMuNuAllLinesConf("Bd2DstarMuNu", StrippingBd2DstarMuNu.confdict)
SL_stream.appendLines( confBd2DstarMuNu.lines() )

### delta a_fs
from StrippingSelections import StrippingB0q2DplusMuX 
confStrippingB0q2DplusMuX  = StrippingB0q2DplusMuX.B0q2DplusMuXAllLinesConf("B0q2DplusMuX",StrippingB0q2DplusMuX.confdict)
SL_stream.appendLines( confStrippingB0q2DplusMuX.lines() )

### V_ub
from StrippingSelections import StrippingB2XuMuNu
confB2XuMuNu = StrippingB2XuMuNu.B2XuMuNuBuilder("B2XuMuNu", StrippingB2XuMuNu.confdict)
SL_stream.appendLines( confB2XuMuNu.lines() )

### Delta_ACP, a_sl, B-fractions etc...
from StrippingSelections import StrippingB2DMuNuX
confB2DMuNuX = StrippingB2DMuNuX.B2DMuNuXAllLinesConf("B2DMuNuX", StrippingB2DMuNuX.confdict)
SL_stream.appendLines( confB2DMuNuX.lines() )

##### CHARM MICRO DST STREAM #########

from StrippingSelections import StrippingDForBSemi
confDForBSemi = StrippingDForBSemi.DforBSemiLinesConf("DForBSemi", StrippingDForBSemi.confdict)
Charm_stream.appendLines( confDForBSemi.lines() )

from StrippingSelections import StrippingD0ForBXX
confD0ForBXX = StrippingD0ForBXX.D0forBXXLinesConf("D0ForBXX", StrippingD0ForBXX.confdict)
Charm_stream.appendLines( confD0ForBXX.lines() )

##### DIMUON STREAM FULL DST ###########

## semi-incl J/psi from B
from StrippingSelections import StrippingJPsiForSL
confJPsiForSL = StrippingJPsiForSL.JPsiForSLAllLinesConf("JPsiForSL", StrippingJPsiForSL.confdict)
Dimuon_stream.appendLines( confJPsiForSL.lines() )

##### BHADRON FULL DST ##################

# B->D* TAU NU
from StrippingSelections import StrippingBd2DstarTauNu
confBd2DstarTauNu = StrippingBd2DstarTauNu.Bd2DstarTauNuAllLinesConf("Bd2DstarTauNu", StrippingBd2DstarTauNu.confdict)
BHad_stream.appendLines( confBd2DstarTauNu.lines() )

#### PID MICRO DST STREAM ###############

## MUID calibration
from StrippingSelections import StrippingMuIDCalib
confMuIDCalib = StrippingMuIDCalib.MuIDCalibConf("MuIDCalib",StrippingMuIDCalib.config_params)
PID_stream.appendLines([confMuIDCalib.line_DetachedNoMIP])

from Configurables import  ProcStatusCheck
filterBadEvents =  ProcStatusCheck()

# Configure the stripping using the same options as in Reco06-Stripping10
sc = StrippingConf( Streams = [ SL_stream,Dimuon_stream,PID_stream , Charm_stream],
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

from Configurables import AlgorithmCorrelationsAlg
ac = AlgorithmCorrelationsAlg(Algorithms = sc.selections())

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True

from Configurables import FilterDesktop

MakePionsEtc = FilterDesktop('MakePionsEtc')
MakePionsEtc.Inputs=["Phys/StdNoPIDsKaons","Phys/StdNoPIDsMuons","Phys/StdLooseKaons","Phys/StdLoosePions","Phys/StdLooseMuons","Phys/StdLooseDstarWithD02KPi"]
MakePionsEtc.Code="ALL"

DaVinci().PrintFreq = 5000
DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().EvtMax = 50000
DaVinci().EventPreFilters = [ filterHLT ]
DaVinci().appendToMainSequence( [MakePionsEtc] )
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
#DaVinci().appendToMainSequence( [ ac ] )
DaVinci().DataType = "2012"
DaVinci().InputType = 'SDST'
DaVinci().DDDBtag  = "head-20120413"
DaVinci().CondDBtag = "head-20120420"

importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco13c_Run124134.py")
