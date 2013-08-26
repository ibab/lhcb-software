# $Id: $
# Test your line(s) of the stripping

### Setup for Stripping20rXp2 (incremental restripping starting August 2013)

from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci
from StrippingConf.Configuration import StrippingConf

# Tighten Trk Chi2 to <3
from CommonParticles.Utils import DefaultTrackingCuts
DefaultTrackingCuts().Cuts  = { "Chi2Cut" : [ 0, 3 ],
                                "CloneDistCut" : [5000, 9e+99 ] }
# Now build the stream
from StrippingConf.StrippingStream import StrippingStream

Streams = {
    "Semileptonic" : StrippingStream("_Semileptonic"),
    "Dimuon" : StrippingStream("_Dimuon"),
    "Bhadron" : StrippingStream("_Bhadron"),
    "BhadronCompleteEvent" : StrippingStream("_BhadronCompleteEvent"),
    "Leptonic" : StrippingStream("_Leptonic"),
    "PID" : StrippingStream("_PID"),
    "Radiative" : StrippingStream("_Radiative"),
    "Charm" : StrippingStream("_Charm"),
    "CharmCompleteEvent" : StrippingStream("_CharmCompleteEvent"),
    "EW" : StrippingStream("_EW")
    }


def AddLines(_Streams,_conf,_settings):
    print _settings['STREAMS']
    print '%s' %len(_settings['STREAMS'])
    if len(_settings['STREAMS']) == 1:
        print 'adding just to %s' %_settings['STREAMS']
        _Streams[ _settings['STREAMS'][0]].appendLines(_conf.lines())
    else:
        for STREAM in _settings['STREAMS']:
            for line in _settings['STREAMS'][STREAM]:
                _Streams[STREAM].appendLines([ l for l in _conf.lines() if l.name() == line])

### modules that will be run
### specific lines may have been switched off in the settings

from StrippingSelections import StrippingB2DMuNuX
from StrippingSettings.Stripping20r0p2.LineConfigDictionaries_Semileptonic import B2DMuNuX
confB2DMuNuX = StrippingB2DMuNuX.B2DMuNuXAllLinesConf("B2DMuNuX", B2DMuNuX['CONFIG'])
AddLines(Streams,confB2DMuNuX,B2DMuNuX)

from StrippingSelections import StrippingPhiToKSKS
from StrippingSettings.Stripping20r0p2.LineConfigDictionaries_Semileptonic import PhiToKSKS
confPhiToKSKS = StrippingPhiToKSKS.PhiToKSKSAllLinesConf("PhiToKSKS", PhiToKSKS['CONFIG'])
AddLines(Streams,confPhiToKSKS,PhiToKSKS)

from StrippingSelections import StrippingB2XuMuNu
from StrippingSettings.Stripping20r0p2.LineConfigDictionaries_Semileptonic import B2XuMuNu
confB2XuMuNu = StrippingB2XuMuNu.B2XuMuNuBuilder("B2XuMuNu", B2XuMuNu['CONFIG'])
AddLines(Streams,confB2XuMuNu,B2XuMuNu)

#from StrippingSelections import StrippingB2XuMuNu
#from StrippingSettings.Stripping20r0p2.LineConfigDictionaries_Semileptonic import B2XuMuNu
#confB2XuMuNu1 = StrippingB2XuMuNu.B2XuMuNuBuilder("B2XuMuNu1", StrippingB2XuMuNu.confdict)
#Streams["Semileptonic"].appendLines(confB2XuMuNu1.lines())
#AddLines(Streams,confB2XuMuNu,B2XuMuNu)
#confB2XuMuNu2 = StrippingB2XuMuNu.B2XuMuNuBuilder("B2XuMuNu2", StrippingB2XuMuNu.confdict)
#Streams["Semileptonic"].appendLines(confB2XuMuNu2.lines())

######################################
        
from Configurables import  ProcStatusCheck
filterBadEvents =  ProcStatusCheck()

ActiveStreams = []
for STREAM in Streams:
    if len(Streams[STREAM].lines) > 0:
        ActiveStreams.append(Streams[STREAM])
sc = StrippingConf( Streams = ActiveStreams,
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
#DaVinci().appendToMainSequence( [ ac ] )
DaVinci().DataType = "2012"
DaVinci().InputType = 'SDST'
DaVinci().DDDBtag  = "dddb-20120831"
DaVinci().CondDBtag = "cond-20121008"

importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco14_Run125113.py")

########################################################################

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

#for l in confCharmFromBSemi.lines():
#    if l.name() in CharmFromBSemi['STREAMS']['Charm']:
#        Charm_stream.appendLines([l])


##### For the Stripping 20r0p1 and 20r1p1 incremental restrippings.
#from StrippingSelections import StrippingCharmFromBSemiForHadronAsy
#from StrippingSettings.Stripping20r0p1.LineConfigDictionaries_Semileptonic import CharmFromBSemiForHadronAsy
#confCharmFromBSemiForHadronAsy = StrippingCharmFromBSemiForHadronAsy.CharmFromBSemiForHadronAsyAllLinesConf("CharmFromBSemiForHadronAsy", CharmFromBSemiForHadronAsy['CONFIG']#)
#Semileptonic_stream.appendLines( confCharmFromBSemiForHadronAsy.lines() )

#from StrippingSelections import StrippingB2DMuForTauMu
#confB2DMuForTauMu = StrippingB2DMuForTauMu.B2DMuForTauMuconf("B2DMuForTauMu",B2DMuForTauMu['CONFIG'])
#for l in confB2DMuForTauMu.lines():
#    if l.name() in B2DMuForTauMu['STREAMS']['Semileptonic']:
#        Semileptonic_stream.appendLines([l])

#from StrippingSelections import StrippingB2XTauNu
#from StrippingSettings.Stripping20r0p1.LineConfigDictionaries_Semileptonic import B2XTauNu
#confB2XTauNu = StrippingB2XTauNu.B2XTauNuAllLinesConf("B2XTauNu",B2XTauNu['CONFIG'])
#for l in confB2XTauNu.lines():
#    if l.name() in B2XTauNu['STREAMS']['BhadronCompleteEvent']:
#        BhadronCompleteEvent_stream.appendLines([l])

#from StrippingSelections import StrippingB2DHHHForBXX
#from StrippingSettings.Stripping20r0p1.LineConfigDictionaries_Semileptonic import B2DHHHForBXX
#confB2DHHHForBXX =  StrippingB2DHHHForBXX.B2DHHHForBXXLinesConf("B2DHHHForBXX",StrippingB2DHHHForBXX.confdict) #B2DHHHForBXX['CONFIG'])
#BhadronCompleteEvent_stream.appendLines( confB2DHHHForBXX.lines() )

#from StrippingSelections import StrippingCharmFromBSemi
##from StrippingSettings.Stripping20r0p1.LineConfigDictionaries_Semileptonic import CharmFromBSemi
#confCharmFromBSemi = StrippingCharmFromBSemi.CharmFromBSemiAllLinesConf("CharmFromBSemi", CharmFromBSemi['CONFIG'])
#for l in confCharmFromBSemi.lines():
#    if l.name() in CharmFromBSemi['STREAMS']['Charm']:
#        Charm_stream.appendLines([l])

### run from selections
#from StrippingSelections import StrippingLb2pMuNu
#confLb2pMuNu = StrippingLb2pMuNu.Lb2pMuNuBuilder("Lb2pMuNu", StrippingLb2pMuNu.confdict)
#Charm_stream.appendLines(confLb2pMuNu.lines())
