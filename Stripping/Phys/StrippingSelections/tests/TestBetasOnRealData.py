# $Id: $
# Test your line(s) of the stripping

from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci
from StrippingConf.Configuration import StrippingConf

# Now build the stream
from StrippingConf.StrippingStream import StrippingStream
stream = StrippingStream("Betas")

from StrippingSelections.StrippingB2JpsiXforBeta_s import B2JpsiXforBeta_sConf 
configJpsiX = {      'TRCHI2DOF'             :       4 
        ,       'BPVLTIME'              :       0.2
        ,       'DaughterPT'            :       1000
        ,       'VCHI2PDOF'             :       10
        ,       'Jpsi2MuMuPrescale'     :       0.1
        ,       'Bu2JpsiKPrescale'      :       0.1
        ,       'Bd2JpsiKstarPrescale'  :       0.05
        ,       'Bd2JpsiKsPrescale'     :       0.5
        ,       'Bs2JpsiPhiPrescale'    :       0.5
        ,       'Bs2JpsiEtaPrescale'    :       0.2
        }
B2JpsiXforBeta_s = B2JpsiXforBeta_sConf("", configJpsiX)
stream.appendLines( B2JpsiXforBeta_s.lines() )

# Fred Blanc, Neus Lopez March
from StrippingSelections.StrippingB2Psi2SX import B2Psi2SXConf
configB2Psi2SX = {'PionsTRCHI2DOF': 5,
                 'Psi2SJpsiMIPCHI2DV' : 4 ,
                 'Psi2SPiMIPCHI2DV':4 ,
                 'Psi2SAM23down': 400,
                 'Psi2SAM23up': 800,
                 'Psi2SAPT': 500,
                 'Psi2SADAMASS': 30,
                 'Psi2SVFASPF': 16,
                 'ChKTRCHI2DOF':5,
                 'ChKPID':-2,#
                 'PhiWin':20,
                 'PhiPT':500,
                 'PhiVFASPF':16,
                 'PhiMAXTRCHI2DOF':5,
                 'PhiMINTRCHI2DOF':-2,#
                 'KstPipTRCHI2DOF':5,
                 'KstAPT' :500,
                 'KstADAMASS' : 90,
                 'KstVFASPF':16, #
                 'KsVFASPF':20,
                 'KsBPVDLS':5,
                 'incl_LinePrescale':0.5,
                 'incl_LinePostscale':1,
                 'BsMassCutDownPre':5000,
                 'BsMassCutUpPre':5650,
                 'BsMassCutDownPost':5100,
                 'BsMassCutUpPost':5550,
                 'BsVCHI2PDOF':10,
                 'sig_LinePrescale': 1,
                 'sig_LinePostscale': 1,
                 'ChKPT': 500 ,
                 'K_LinePrescale': 1,
                 'K_LinePostscale': 1,
                 'Kstar_LinePrescale': 1,
                 'Kstar_LinePostscale': 1,
                 'BKsVCHI2PDOF': 10,
                 'Ks_LinePrescale': 1,
                 'Ks_LinePostscale':1
                 }
#B2Psi2SX = B2Psi2SXConf("B2Psi2SX", configB2Psi2SX)
#stream.appendLines( B2Psi2SX.lines() )

from StrippingSelections.StrippingB2Psi2SXMuMu import Bs2Psi2SPhiMuMuConf
configBs2Psi2SPhiMuMu = {'muPID':0.,
                 'Psi2SMassWin':60.,
                 'Psi2SADOCACHI2CUT':30.,
                 'Psi2SVFASPF':16,#
                 'ChKTRCHI2DOF':5,
                 'ChKPID':-2,#
                 'PhiWin':20,
                 'PhiPT':500,
                 'PhiVFASPF':16,
                 'PhiMAXTRCHI2DOF':5,
                 'PhiMINTRCHI2DOF':-2,#
                 'KstPipTRCHI2DOF':5,
                 'KstAPT' :500,
                 'KstADAMASS' : 90,
                 'KstVFASPF':16, #
                 'KsVFASPF':20,
                 'KsBPVDLS':5,
                 'incl_LinePrescale':0.5,
                 'incl_LinePostscale':1,
                 'BPVLTIME_detatched':0.15,
                 'incl_DetatchedLinePrescale':1,
                 'incl_DetatchedLinePostscale':1,
                 'BsMassCutDownPre':5000,
                 'BsMassCutUpPre':5650,
                 'BsMassCutDownPost':5100,
                 'BsMassCutUpPost':5550,
                 'BsVCHI2PDOF':10,
                 'sig_PrescaledLinePrescale':1,
                 'sig_PrescaledLinePostscale':1,
                 'sig_DetatchedLinePrescale':1,
                 'sig_DetatchedLinePostscale':1,
                 'BPVLTIME_unbiased': None,
                 'MINTREEPT':1000,
                 'sig_UnbiasedLinePrescale':1,
                 'sig_UnbiasedLinePostscale':1,
                 'ChKPT':500,
                 'K_PrescaledLinePrescale':1,
                 'K_PrescaledLinePostscale':1,
                 'K_DetatchedLinePrescale':1,
                 'K_DetatchedLinePostscale':1,
                 'K_UnbiasedLinePrescale':1,
                 'K_UnbiasedLinePostscale':1,
                 'KstarPT': 2,
                 'Kstar_PrescaledLinePrescale':0.5,
                 'Kstar_PrescaledLinePostscale':1,
                 'Kstar_DetatchedLinePrescale':1,
                 'Kstar_DetatchedLinePostscale':1,
                 'Kstar_UnbiasedLinePrescale':1,
                 'Kstar_UnbiasedLinePostscale':1,
                 'BKsVCHI2PDOF':10,
                 'Ks_PrescaledLinePrescale':1,
                 'Ks_PrescaledLinePostscale':1,
                 'Ks_DetatchedLinePrescale':1,
                 'Ks_DetatchedLinePostscale':1,
                 'Ks_UnbiasedLinePrescale':1,
                 'Ks_UnbiasedLinePostscale':1
                 }
#Bs2Psi2SPhiMuMu = Bs2Psi2SPhiMuMuConf("Bs2Psi2SPhiMuMu", configBs2Psi2SPhiMuMu)
#stream.appendLines( Bs2Psi2SPhiMuMu.lines() )

# Jibo He
from StrippingSelections.StrippingBs2JpsieePhi import Bs2JpsieePhiConf
configBs2JpsieePhi = {
                  'ElectronPTLoose'            :   800.    # MeV
                , 'ElectronTrackCHI2pDOFLoose' :    10.    # adimensional
                , 'JpsiVertexCHI2pDOFLoose'    :    15.    # adimensional
                , 'JpsiMassMinLoose'           :  2700.    # MeV
                , 'JpsiMassMaxLoose'           :  3300.    # MeV
                , 'KaonTrackCHI2pDOFLoose'     :    10.    # adimensional
                , 'PhiPTLoose'                 :  1000.    # MeV
                , 'PhiVertexCHI2pDOFLoose'     :    15.    # adimensional
                , 'PhiMassMinLoose'            :   990.    # MeV
                , 'PhiMassMaxLoose'            :  1050.    # MeV
                , 'BsVertexCHI2pDOFLoose'      :    10.    # adimensional
                , 'BsMassMinLoose'             :  4500.    # MeV
                , 'BsMassMaxLoose'             :  6000.    # MeV
                , 'BsDIRALoose'                :     0.99  # adimensional
                , 'ElectronPT'            :   800.    # MeV
                , 'ElectronPID'           :     2.    # adimensional
                , 'ElectronTrackCHI2pDOF' :     5.    # adimensional
                , 'JpsiVertexCHI2pDOF'    :    15.    # adimensional
                , 'JpsiMassMin'           :  2700.    # MeV
                , 'JpsiMassMax'           :  3300.    # MeV
                , 'KaonTrackCHI2pDOF'     :     5.    # adimensional
                , 'KaonPID'               :     0.    # adimensional
                , 'PhiPT'                 :  1000.    # MeV
                , 'PhiVertexCHI2pDOF'     :    15.    # adimensional
                , 'PhiMassMin'            :  1005.    # MeV
                , 'PhiMassMax'            :  1035.    # MeV
                , 'BsVertexCHI2pDOF'      :     6.    # adimensional
                , 'BsMassMin'             :  4500.    # MeV
                , 'BsMassMax'             :  6000.    # MeV
                , 'BsDIRA'                :     0.99  # adimensional
                }
Bs2JpsieePhi = Bs2JpsieePhiConf("Bs2JpsieePhi", configBs2JpsieePhi)
stream.appendLines( Bs2JpsieePhi.lines() )


# Fred Blanc
from StrippingSelections.StrippingB2CharmlessQuasi2Body import B2Quasi2Body4piConf
configQ2B = {'Q2BPrescale'     : 1.,
                 'Q2BResMinPT'     : 1500.,
                 'Q2BResMinP'      : 10.,
                 'Q2BResMaxMass'   : 900.,
                 'Q2BResVtxChiDOF' : 4.,
                 'Q2BBMinM'        : 2800.,
                 'Q2BBMaxM'        : 5700.,
                 'Q2BBVtxChi2DOF'  : 4.,
                 'Q2BBMinTrkPT'    : 1500.,
                 'Q2BBPVVDZ'       : 2. }
B2Quasi2Body4pi = B2Quasi2Body4piConf("B2Quasi2Body4pi", configQ2B)
stream.appendLines( B2Quasi2Body4pi.lines() )

# Sebastian Schleich
from StrippingSelections.StrippingBs2PhiPhi import StrippingBs2PhiPhiConf
configBs2PhiPhi = {
                  'KaonPT'              : 500      # MeV/c
                , 'KaonIPCHI2'          : 3.5      # adimensional
                , 'PhiPT'               : 900      # MeV/c
                , 'PhiPTsq'             : 2.0      # GeV*GeV/cc
                , 'PhiVertexCHI2pDOF'   : 25       # adimensional
                , 'PhiMassWindow'       : 25       # MeV/cc
                , 'PhiMassMax'          : 1090     # MeV/cc
                , 'BsVertexCHI2pDOF'    : 25       # adimensional
                , 'BsMassWindow'        : 300      # MeV/cc
                , 'WidePrescale'        : 0.05     # adimensional
                }
Bs2PhiPhi = StrippingBs2PhiPhiConf("Bs2PhiPhi", configBs2PhiPhi)
stream.appendLines( Bs2PhiPhi.lines() )


# Paula Alvarez Cartelle
from StrippingSelections.StrippingBs2Kst0Kst0 import StrippingBs2KstKstConf
configBs2KstKst = {
          "KaonPT"              : 500.0 # MeV
          ,     "KaonIPCHI2"            : 9.    # adimensional
          ,  "PionPT"                : 500.0 # MeV
          ,  "PionIPCHI2"            : 9.       # adimensional
          ,     "KstarVCHI2"            : 9.0   # adimensional
          ,     "KstarPT"               : 900.0 # MeV
          ,     "KaonPIDK"              : -5.0  # adimensional
          ,     "KstarMassWin"          : 150.0 # MeV
          ,     "BMassWin"              : 500.0 # MeV
          ,     "BVCHI2"                : 15.0  # adimensional
          ,     "BDOCA"                 : 0.3   # mm
          ,  "BIPCHI2"               : 25    # adimensional
          }
Bs2KstKst = StrippingBs2KstKstConf("Bs2KstKst", configBs2KstKst)
stream.appendLines( Bs2KstKst.lines() )

# Katarzyna Senderowska
from StrippingSelections.StrippingBs2EtacPhi import Bs2EtacPhiConf
configBs2EtacPhi = {
    'Prescale'                : 1.0 ,
    'Postscale'               : 1.0 ,
    #K parameters
    'K_PT'                    : 500.,
    'K_TRCHI2'                : 5.,
    'K_IPCHI2'                : 6.,
    #pi parameters
    'pi_PT'                   : 500.,
    'pi_TRCHI2'               : 5.,
    'pi_IPCHI2'               : 6.,
    #phi parameters
    'phi_VDZ'                 : 0.,
    'phi_PT'                  : 1000.,
    'phi_IPCHI2'              : 6.,
    'phi_VCHI2_VDOF'          : 16.,
    #rho (for eta_c decay) parameters
    'rho_etac_VDZ'            : 0.,
    'rho_etac_VCHI2_VDOF'     : 16.,
    #phi (for eta_c decay) parameters
    'phi_etac_VDZ'            : 0.,
    'phi_etac_VCHI2_VDOF'     : 16.,
    #eta_c parameters         
    'etac_VDZ'                : 0.,   
    'etac_PT'                 : 1000.,
    'etac_IPCHI2'             : 6., 
    'etac_VCHI2_VDOF'         : 16.,
    #Bs parameters            
    'Bs_VDZ'                  : 0.,
    'Bs_DIRA'                 : 0.999,
    'Bs_IPCHI2'               : 9.,
    'Bs_VCHI2_VDOF'           : 16.
    }
Bs2EtacPhi = Bs2EtacPhiConf("Bs2EtacPhi", configBs2EtacPhi)
stream.appendLines( Bs2EtacPhi.lines() )
 
from StrippingSelections.StrippingBs2ChicPhi_Chic2KKPiPi import Bs2ChicPhi_Chic2KKPiPiConf
configBs2ChicPhi_Chic2KKPiPi = {
    'Prescale'               : 1.0 ,
    'Postscale'              : 1.0 ,
    #K parameters
    'K_PT'                   : 500.,
    'K_TRCHI2'               : 5.,
    'K_IPCHI2'               : 6.,
    #pi parameters
    'pi_PT'                  : 500.,
    'pi_TRCHI2'              : 5.,
    'pi_IPCHI2'              : 6.,
    #phi parameters
    'phi_VDZ'                : 0.,
    'phi_PT'                 : 1000.,
    'phi_IPCHI2'             : 6.,
    'phi_VCHI2_VDOF'         : 16.,
    #rho (for chi_c decay) parameters
    'rho_chic_VDZ'           : 0.,
    'rho_chic_VCHI2_VDOF'    : 16.,
    #phi (for chi_c decay) parameters
    'phi_chic_VDZ'           : 0.,
    'phi_chic_VCHI2_VDOF'    : 16.,
    #chi_c parameters        
    'chic_VDZ'               : 0.,   
    'chic_PT'                : 1000.,
    'chic_IPCHI2'            : 6.,
    'chic_VCHI2_VDOF'        : 16.,
    #Bs parameters           
    'Bs_VDZ'                 : 0.,
    'Bs_DIRA'                : 0.999,
    'Bs_IPCHI2'              : 9.,
    'Bs_VCHI2_VDOF'          : 16.
    }
Bs2ChicPhi_Chic2KKPiPi = Bs2ChicPhi_Chic2KKPiPiConf("Bs2ChicPhi_Chic2KKPiPi", configBs2ChicPhi_Chic2KKPiPi)
stream.appendLines( Bs2ChicPhi_Chic2KKPiPi.lines() )

from StrippingSelections.StrippingBs2ChicPhi_Chic2PiPiPiPi import Bs2ChicPhi_Chic2PiPiPiPiConf
configBs2ChicPhi_Chic2PiPiPiPi = {
    'Prescale'               : 1.0 ,
    'Postscale'              : 1.0 ,
    #phi parameters
    'K_PT'                   : 500.,
    'K_TRCHI2'               : 5.,
    'K_IPCHI2'               : 6.,
    #pi parameters
    'pi_PT'                  : 500.,
    'pi_TRCHI2'              : 5.,
    'pi_IPCHI2'              : 6.,
    #phi parameters
    'phi_VDZ'                : 0.,
    'phi_PT'                 : 1000.,
    'phi_IPCHI2'             : 9.,
    'phi_VCHI2_VDOF'         : 16.,
    #rho (for chi_c decay) parameters
    'rho_chic_VDZ'           : 0.,
    'rho_chic_VCHI2_VDOF'    : 16.,
    #chi_c parameters
    'chic_VDZ'               : 0.,
    'chic_PT'                : 1000.,
    'chic_IPCHI2'            : 6.,
    'chic_VCHI2_VDOF'        : 16.,
    #Bs parameters
    'Bs_VDZ'                  : 0.,
    'Bs_DIRA'                 : 0.999,
    'Bs_IPCHI2'               : 9.,
    'Bs_VCHI2_VDOF'           : 16.
    }
Bs2ChicPhi_Chic2PiPiPiPi = Bs2ChicPhi_Chic2PiPiPiPiConf("Bs2ChicPhi_Chic2PiPiPiPi", configBs2ChicPhi_Chic2PiPiPiPi)
stream.appendLines( Bs2ChicPhi_Chic2PiPiPiPi.lines() )


config_strip12 = {      'TRCHI2DOF'             :   5 
        ,       'BPVLTIME'              :       0.15
        ,       'DaughterPT'            :       1000
        ,       'VCHI2PDOF'             :       10
        ,       'Jpsi2MuMuPrescale'     :       0.5
        ,       'Bu2JpsiKPrescale'      :       0.125
        ,       'Bd2JpsiKstarPrescale'  :       0.125
        }

from Configurables import  ProcStatusCheck
filterBadEvents =  ProcStatusCheck()

# Configure the stripping using the same options as in Reco06-Stripping10
sc = StrippingConf( Streams = [ stream ],
                    MaxCandidates = 2000,
                    AcceptBadEvents = False,
                    BadEventSelection = filterBadEvents )
sc.OutputType = "ETC"                    # Can be either "ETC" or "DST"

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True

# Configure the ETC writing step
from Configurables import EventTuple, TupleToolSelResults
from Configurables import TupleToolStripping

tag = EventTuple("TagCreator")
tag.EvtColsProduce = True
tag.ToolList = [ "TupleToolEventInfo", "TupleToolRecoStats", "TupleToolStripping"  ]
tag.addTool( TupleToolStripping )
tag.TupleToolStripping.StrippigReportsLocations = "/Event/Strip/Phys/DecReports"

# Remove the microbias and beam gas etc events before doing the tagging step
regexp = "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!NZS)(?!Velo)(?!BeamGas)(?!Incident).*Decision')"
from Configurables import LoKi__HDRFilter
filterHLT = LoKi__HDRFilter("FilterHLT",Code = regexp )

seq = GaudiSequencer("TagSeq")
seq.Members = [tag]

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

from Configurables import AuditorSvc, ChronoAuditor
AuditorSvc().Auditors.append( ChronoAuditor("Chrono") )

from Configurables import StrippingReport
sr = StrippingReport(Selections = sc.selections())

from Configurables import AlgorithmCorrelationsAlg
ac = AlgorithmCorrelationsAlg(Algorithms = sc.selections())

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True

DaVinci().PrintFreq = 2000
DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().ETCFile = "etc.root"
DaVinci().EvtMax = 10000
DaVinci().EventPreFilters = [ filterHLT ]
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
DaVinci().appendToMainSequence( [ ac ] )
DaVinci().MoniSequence += [ seq ]            # Append the TagCreator to DaVinci
DaVinci().DataType = "2010"
DaVinci().InputType = 'SDST'
importOptions("$STRIPPINGSELECTIONSROOT/tests/data/RUN_81430_RealData+Reco08-Stripping12_90000000_SDST.py")
