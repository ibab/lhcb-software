#!/usr/bin/env python

from StrippingUtils import LineBuilderTests

# The following lines won't work since they have not been updated
# Basem Khanji, Olivier Leroy
#from StrippingSelections.StrippingBiasedBs2JpsiPhi import line
#stream.appendLines( [ line ] )

from StrippingSelections.StrippingB2JpsiXforBeta_s import B2JpsiXforBeta_sConf 
configJpsiX = {      'TRCHI2DOF'             :       4 
        ,       'BPVLTIME'              :       0.25
        ,       'DaughterPT'            :       1000
        ,       'VCHI2PDOF'             :       10
        ,       'Jpsi2MuMuPrescale'     :       0.1
        ,       'Bu2JpsiKPrescale'      :       0.1
        ,       'Bd2JpsiKstarPrescale'  :       0.05
        ,       'Bd2JpsiKsPrescale'     :       0.2
        ,       'Bs2JpsiPhiPrescale'    :       0.2
        ,       'Bs2JpsiEtaPrescale'    :       0.1
        }

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



# Jibo He
from StrippingSelections.StrippingBs2JpsieePhi import Bs2JpsieePhiConf
configJpsieePhi = {
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



# Sebastian Schleich
from StrippingSelections.StrippingBs2PhiPhi import StrippingBs2PhiPhiConf
configBsPhiPhi = {
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

def test_line_builder() :
    LineBuilderTests.test_line_builder(B2JpsiXforBeta_sConf, configJpsiX)
    b = B2JpsiXforBeta_sConf( "", configJpsiX )
    for line in b.lines():
        print line.outputLocation()
   
    LineBuilderTests.test_line_builder(B2Psi2SXConf, configB2Psi2SX)
    b = B2Psi2SXConf( "B2Psi2SX", configB2Psi2SX )
    for line in b.lines():
        print line.outputLocation()
    
    LineBuilderTests.test_line_builder(Bs2Psi2SPhiMuMuConf, configBs2Psi2SPhiMuMu)
    b = Bs2Psi2SPhiMuMuConf( "Bs2Psi2SPhiMuMu", configBs2Psi2SPhiMuMu )
    for line in b.lines():
        print line.outputLocation()
    
    '''
    LineBuilderTests.test_line_builder(StrippingBs2PhiPhiConf, configBsPhiPhi)
    b = StrippingBs2PhiPhiConf( "Bs2PhiPhi", configBsPhiPhi )
    for line in b.lines():
        print line.outputLocation()
    
    LineBuilderTests.test_line_builder(StrippingBs2KstKstConf, configBs2KstKst )
    b = StrippingBs2KstKstConf( "Bs2KstKst", configBs2KstKst )
    for line in b.lines():
        print line.outputLocation()
    
    LineBuilderTests.test_line_builder(B2Quasi2Body4piConf, configQ2B )
    b = B2Quasi2Body4piConf( "Q2B", configQ2B )
    for line in b.lines():
        print line.outputLocation()
   
    LineBuilderTests.test_line_builder(Bs2JpsieePhiConf, configJpsieePhi )
    b = Bs2JpsieePhiConf( "Bs2JpsieePhi", configJpsieePhi )
    for line in b.lines():
        print line.outputLocation()
    '''

    LineBuilderTests.test_line_builder(Bs2ChicPhi_Chic2PiPiPiPiConf, configBs2ChicPhi_Chic2PiPiPiPi )
    b = Bs2ChicPhi_Chic2PiPiPiPiConf( "Bs2ChicPhi_Chic2PiPiPiPi", configBs2ChicPhi_Chic2PiPiPiPi )
    for line in b.lines():
        print line.outputLocation()
    
    '''
    LineBuilderTests.test_line_builder(Bs2ChicPhi_Chic2KKPiPiConf, configBs2ChicPhi_Chic2KKPiPi )
    b = Bs2ChicPhi_Chic2KKPiPiConf( "Bs2ChicPhi_Chic2KKPiPi", configBs2ChicPhi_Chic2KKPiPi )
    for line in b.lines():
        print line.outputLocation()

    LineBuilderTests.test_line_builder(Bs2EtacPhiConf, configBs2EtacPhi )
    b = Bs2EtacPhiConf( "Bs2EtacPhi", configBs2EtacPhi )
    for line in b.lines():
        print line.outputLocation()
    '''

if __name__ == '__main__' :
    test_line_builder()
