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

B2DMuForTauMu = {
    'BUILDERTYPE'       : 'B2DMuForTauMuconf',
    'CONFIG'    :{
    "MINIPCHI2"     : 45.0    # adimensiional
    ,"GhostProb"     : 0.5     # adimensional
    ,"KaonPIDK"      : 4.0    # adimensiional
    ,"PionPIDKTight" : 2.0    # adimensiional
    ,"MuonIPCHI2"    : 45.00   # adimensiional
    ,"KPiPT"         : 300.0  # MeV
    ,"DSumPT"        : 1400.0 # MeV
    ,"DsDIRA"        : 0.9998   # adimensiional
    ,"DsFDCHI2"      : 250.0  # adimensiional
    ,"DsMassWin"     : 80.0   # MeV
    ,"DsAMassWin"    : 100.0  # MeV
    ,"DsVCHI2DOF"    : 4.0    # adimensiional
    ,"PIDmu"         : 2.0   # adimensiional
    ,"BDIRA"         : 0.9995  #adimensiional
    ,"BVCHI2DOF"     : 6.0    # adimensiional
    ,"SPDmax"        : 600    # adimensiional
    },
    'STREAMS' : {
    'Semileptonic' : ['Strippingb2D0MuXFakeB2DMuForTauMuLine']
    },
    'WGs'    : ['Semileptonic']
    }

from StrippingSelections import StrippingB2DMuForTauMu
confB2DMuForTauMu = StrippingB2DMuForTauMu.B2DMuForTauMuconf("B2DMuForTauMu",B2DMuForTauMu['CONFIG'])
for l in confB2DMuForTauMu.lines():
    if l.name() in B2DMuForTauMu['STREAMS']['Semileptonic']:
        Semileptonic_stream.appendLines([l])


from StrippingSelections import StrippingB2XTauNu
from StrippingSettings.Stripping20r0p1.LineConfigDictionaries_Semileptonic import B2XTauNu
confB2XTauNu = StrippingB2XTauNu.B2XTauNuAllLinesConf("B2XTauNu",B2XTauNu['CONFIG'])
for l in confB2XTauNu.lines():
    if l.name() in B2XTauNu['STREAMS']['BhadronCompleteEvent']:
        BhadronCompleteEvent_stream.appendLines([l])

from StrippingSelections import StrippingB2DHHHForBXX
#from StrippingSettings.Stripping20r0p1.LineConfigDictionaries_Semileptonic import B2DHHHForBXX
confB2DHHHForBXX =  StrippingB2DHHHForBXX.B2DHHHForBXXLinesConf("B2DHHHForBXX",StrippingB2DHHHForBXX.confdict) #B2DHHHForBXX['CONFIG'])
BhadronCompleteEvent_stream.appendLines( confB2DHHHForBXX.lines() )



# CharmFromBSemi Lines
# Micro-DST, Mika Vesterinen
# Moved to this file in Stripping20
# Lives in StrippingCharmFromBSemi.py
CharmFromBSemi = {
    'BUILDERTYPE' : 'CharmFromBSemiAllLinesConf',
    'CONFIG' :  {
    "GEC_nLongTrk"   : 250    # adimensional
    ,"TrGhostProbMax": 0.5    # adimensional
    ,"MinBMass"      : 2500   # MeV
    ,"MaxBMass"      : 6000   # MeV
    ,"MinBMassTight" : 2800   # MeV
    ,"MaxBMassTight" : 5100   # MeV
    ,"MINIPCHI2"     : 9.0    # adimensiional
    ,"TRCHI2"        : 4.0    # adimensiional
    ,"KaonPIDK"      : 4.0    # adimensiional
    ,"PionPIDK"      : 10.0   # adimensiional
    ,"PionPIDKTight" : 4.0    # adimensiional
    ,"MuonIPCHI2"    : 4.0    # adimensiional
    ,"MuonPT"        : 800.0  # MeV
    ,"KPiPT"         : 300.0  # MeV
    ,"DsDIRA"        : 0.99   # adimensiional
    ,"DsFDCHI2"      : 100.0  # adimensiional
    ,"DsMassWin"     : 80.0   # MeV
    ,"DsAMassWin"    : 100.0  # MeV
    ,"Dto4h_MassWin" : 40.0   # MeV
    ,"Dto4h_AMassWin": 42.0   # MeV
    ,"DsIP"          : 7.4    # mm
    ,"DsVCHI2DOF"    : 6.0    # adimensiional
    ,"PIDmu"         : -0.0   # adimensiional
    ,"BDIRA"         : 0.999  # adimensiional
    ,"BVCHI2DOF"     : 6.0    # adimensiional
    ,"DZ"            : 0      #mm
    ,"DDocaChi2Max"  : 20     #adimensiional
    ,"MINIPCHI2Loose": 4.0   #adimensiional
    ,"KaonPIDKloose" : -5     #adimensiional
    ,'KSCutZFDFromD' :   10.0  #mm
    ,'KSLLPMin'         : 2000  ## MeV
    ,'KSLLPTMin'        : 400   ## MeV
    ,'KSDDPMin'         : 3000  ## MeV
    ,'KSDDPTMin'        : 500   ## MeV
    ,'KSLLCutMass'      : 30    ## MeV
    ,'KSDDCutMass'      : 30    ## MeV
    ,'KSLLCutFDChi2'    : 100   ## unitless
    ,'KSDDCutFDChi2'    : 100   ## unitless
    ,'KSDaugTrackChi2'  : 4     ## max chi2/ndf for Ks daughter tracks
    ,'KSVertexChi2'     : 6     ## max chi2/ndf for Ks vertex
    ,'KSCutDIRA'        : 0.99  ## unitless
    ,'LambdaLLPMin'       : 2000 ## MeV
    ,'LambdaLLPTMin'      : 500  ## MeV
    ,'LambdaLLCutMass'    : 30   ## MeV
    ,'LambdaLLCutFDChi2'  : 100  ## unitless
    ,'LambdaDDPMin'       : 3000 ## MeV
    ,'LambdaDDPTMin'      : 800  ## MeV
    ,'LambdaDDCutMass'    : 30   ## MeV
    ,'LambdaDDCutFDChi2'  : 100  ## unitless
    ,'LambdaCutDIRA'      : 0.99 ## unitless
    ,'LambdaDaugTrackChi2': 4    ## unitless
    ,'LambdaVertexChi2'   : 6    ## max chi2/ndf for Lambda0 vertex
    ,"Pi0PtMinLoose"    : 1000    # Minimum Pt of pi0 (MeV)
    ,"Pi0PtMin"         : 1300   # Minimum Pt of pi0 (MeV)
    ,"Pi0PMin"          : 3000   # Minimum P of pi0 (MeV)
    ,"PhotonCL"         : 0.25   # Confidence level for Pi0 photons
    ,"D02HHPi0AMassWin" : 220  # MeV (mass window for combination)
    ,"D02HHPi0MassWin"  : 210  # MeV (mass window after vertex fit)
    ,"D02HHPi0DocaCut"  : 6.0    # mm
    ,"Dstar_Chi2"       :  8.0 ## unitless
    ,"Dstar_SoftPion_PIDe" : 2. ## unitless
    ,"Dstar_SoftPion_PT" : 180. ## MeV ### 
    ,"Dstar_wideDMCutLower" : 0. ## MeV
    ,"Dstar_wideDMCutUpper" : 170. ## MeV
    ,"PTSUMLoose"  : 1400. ## MeV
    ,"PTSUM"       : 1800. ## MeV
    ,"PTSUM_HHPi0" : 1800. ## MeV
    },
    'STREAMS' : {
    'Charm' : ['Strippingb2DstarMuXKsKPiLLCharmFromBSemiLine',
               'Strippingb2DstarMuXKsKPiDDCharmFromBSemiLine',
               'Strippingb2DstarMuXKsPiPiLLCharmFromBSemiLine',
               'Strippingb2DstarMuXKsPiPiDDCharmFromBSemiLine',
               'Strippingb2DstarMuXKsKKLLCharmFromBSemiLine',
               'Strippingb2DstarMuXKsKKDDCharmFromBSemiLine',
               'Strippingb2D0MuXKsKPiLLCharmFromBSemiLine',
               'Strippingb2D0MuXKsKPiDDCharmFromBSemiLine',
               'Strippingb2D0MuXKsPiPiLLCharmFromBSemiLine',
               'Strippingb2D0MuXKsPiPiDDCharmFromBSemiLine',
               'Strippingb2D0MuXKsKKLLCharmFromBSemiLine',
               'Strippingb2D0MuXKsKKDDCharmFromBSemiLine',
               'Strippingb2MuXLc2pKsLLCharmFromBSemiLine',
               'Strippingb2MuXLc2pKsDDCharmFromBSemiLine']},
    'WGs'    : ['Charm']
    }

from StrippingSelections import StrippingCharmFromBSemi
#from StrippingSettings.Stripping20r0p1.LineConfigDictionaries_Semileptonic import CharmFromBSemi
confCharmFromBSemi = StrippingCharmFromBSemi.CharmFromBSemiAllLinesConf("CharmFromBSemi", CharmFromBSemi['CONFIG'])
for l in confCharmFromBSemi.lines():
    if l.name() in CharmFromBSemi['STREAMS']['Charm']:
        Charm_stream.appendLines([l])

### run from selections
from StrippingSelections import StrippingLb2pMuNu
confLb2pMuNu = StrippingLb2pMuNu.Lb2pMuNuBuilder("Lb2pMuNu", StrippingLb2pMuNu.confdict)
Charm_stream.appendLines(confLb2pMuNu.lines())

######################################
        
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
#DaVinci().DDDBtag  = "head-20120413"
#DaVinci().CondDBtag = "head-20120420"
DaVinci().DDDBtag  = "dddb-20120831"
DaVinci().CondDBtag = "cond-20121008"

#importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco14_2011Data_MagDn.py")
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
