# Line config dictionaries for BandQ WG
# https://twiki.cern.ch/twiki/bin/viewauth/LHCbPhysics/BandQStripping20rXp1RequiredLines

# 20r1p1 = 2011
from GaudiKernel.SystemOfUnits import *

Xibc = {
   'BUILDERTYPE'  : 'XibcBuilder',
    'CONFIG'       : {
      'Pions4Lambdac_MINIPCHI2'             : 0.0
      , 'Pion4Lambdac_TRPCHI2'              : 0.015
      , 'Pion4Lambdac_PIDK'                 : 15
      , 'Pion4Lambdac_PT'                   : '250*MeV'
      , 'Protons4Lambdac_TRPCHI2'           : 0.05
      , 'Protons4Lambdac_minP'              : '0*GeV'
      , 'Kaon4Lambdac_TRPCHI2'              : 0.05
      , 'Protons4Lambdac_PIDp'              : -2
      , 'Protons4Lambdac_PIDp-PIDK'         : -5
      , 'Protons4Lambdac_PT'                : '450*MeV'
      , 'Kaon4Lambdac_PT'                   : '450*MeV'
      , 'Kaon4Lambdac_PIDK'                 : 0
      , 'Kaon4Lambdac_minP'                 : '0*GeV'
      , 'Muon4Jpsi_PIDmu'                   : 0
      , 'Muon4Jpsi_TRPCHI2'                 : 0.005
      , 'Lambdac_MassWindowLarge'           : '120*MeV'
      , 'Lambdac_MassWindowTight'           : '30*MeV'		
      , 'Lambdac_MinAPT'                    : '1500*MeV' 
      , 'Lambdac_MaxADOCA'                  : '0.5*mm'
      , 'Lambdac_BPVDIRA'                   : 0.98
      , 'Lambdac_minTAU'                    : -0.0001 	
      , 'Lambdac_ENDVERTEXCHI2'             : 5						
      , 'Jpsi_MassWindowLarge'              : '150*MeV'
      , 'Jpsi_MassWindowTight'              : '50*MeV'
      , 'Muon4Jpsi_PT'                      : '650*MeV'
      , 'Jpsi_ENDVERTEXCHI2'                : 10
      , 'Xibc_ENDVERTEXCHI2'                : 7
      , 'Xibc_MassWindow'                   : '1.5*GeV'
      , 'Xibc_MINPVIP'                      : 0.1			
      , 'Xibc2LcJpsiPrescale'               : 1.0
####### #
      , 'Protons4Lambda0_PIDp'              : -10
      , 'LongProtons4Lambda0_MINIPCHI2'     : 2
      , 'Protons4Lambda0_PT'                : '600*MeV'
      , 'Pions4Lambda0_PIDK'                : +20
      , 'Pions4Lambda0_PT'                  : '0*MeV'
      , 'Pions4Lambda0_MINIPCHI2'           : 0.
      , 'Lambda0_MassWindowLarge'           : '180*MeV'
      , 'Lambda0_MassWindowTight'           : '30*MeV'
      , 'Lambda0_APT'                       : '700*MeV'
      , 'Lambda0_ENDVERTEXCHI2'             : 10	
      , 'Lambda0_DownProtonTrackPvalue'     : 0
      , 'Lambda0_DownPionTrackPvalue'       : 0
      , 'Lambda0_minFD'                     : '1*mm/GeV'
      , 'Pions4Ximinus_PT'                  : '0*MeV'
      , 'Pions4Ximinus_PIDK'                : 20
      , 'Pions4Ximinus_TRPCHI2'             : 0
      , 'Pions4Ximinus_MINIPCHI2'           : 0
      , 'Ximinus_MassWindowLarge'           : '120*MeV'
      , 'Ximinus_MassWindowTight'           : '40*MeV'
      , 'Ximinus_APT'                       : '800*MeV'
      , 'Ximinus_FlightDistance'            : '0.1*mm/GeV'
      , 'Ximinus_ENDVERTEXCHI2'             : 20
      , 'Xic0_MassWindowLarge'              : '600*MeV'
      , 'Xic0_MassWindowTight'              : '100*MeV'
      , 'Pions4Xic0_PT'                     : '200*MeV'
      , 'Pions4Xic0_PIDK'                   : 20
      , 'Pions4Xic0_TRPCHI2'                : 0
      , 'Xic0_APT'                          : '0*MeV'
      , 'Xic0_ENDVERTEXCHI2'                : 20
      , 'Xibc0_MassWindow'                  : '1.5*GeV'
      , 'Xibc0_ENDVERTEXCHI2'               : 20
####### 
      , 'HighMassBaryon_MassLowEdge'        : '4.5*GeV'
      , 'HighMassBaryon_MinAPT'             : '1*GeV'
      , 'ProtonsForHighMassBaryon_TRPCHI2'  : 0.1
      , 'ProtonsForHighMassBaryon_PT'       : '1.5*GeV'
      , 'ProtonsForHighMassBaryon_P'        : '5*GeV'
      , 'ProtonsForHighMassBaryon_PIDp'     : 15
      , 'JpsiForHighMassBaryon_PT'          : '1*GeV'
      , 'JpsiForHighMassBaryon_MassWin'     : '40*MeV'
      , 'JpsiForHighMassBaryon_MuonPIDmu'   : 0
      , 'JpsiProtonForHighMassBaryonCosth'  : 0.992
#######
      , "JpsiKp_CtrlLine_Prescale"          : 1.0
      , "JpsiKp_MinTAU"                     : '0.10*ps'
      , "JpsiKp_MassMin"                    : 5200 #MeV/c2
      , "JpsiKp_MassLbThreshold"            : 5750 #MeV/c2
      , "JpsiKp_MassMax"                    : 8000 #MeV/c2
      , "JpsiKp_MaxVertexChi2"              : 10
      , "JpsiKp_Jpsi_MinPT"                 : 700 #MeV/c
      , "JpsiKp_Jpsi_MassWin"               : 40  #MeV/cc
      , "JpsiKp_mu_MaxTrackGhostProb"       : 0.4
      , "JpsiKp_mu_MinPIDmu"                : 0
      , "JpsiKp_p_MinPt"                    : 300 #MeV
      , "JpsiKp_p_MinPIDp"                  : 10
      , "JpsiKp_p_MaxTrackGhostProb"        : 0.4
      , "JpsiKp_p_MinTrackPvalue"           : 0.1
      , "JpsiKp_p_MinP"                     : 2000 #MeV/c
      , "JpsiKp_K_MinPT"                    : 300 #MeV
      , "JpsiKp_K_MinPIDK"                  : 5
      , "JpsiKp_K_MaxTrackGhostProb"        : 0.4
      , "JpsiKp_K_MinTrackPvalue"           : 0.1
      , "JpsiKp_K_MinP"                     : 2000 #MeV/c
########
      , 'GlobalGhostProb_Max'               : 0.4
      , 'LongTrackGEC'                      : 150 
			} ,
    'STREAMS' : { 'Dimuon' : [ "StrippingXibcXibc2JpsiKp",
                               "StrippingXibcLb2JpsiKp",
                               "StrippingXibcX2JpsiProtonGoodPointing"]},
    'WGs'     : [ 'BandQ' ]
}


Betac2PhiP = {
    'BUILDERTYPE'  : 'StrippingBetac2PhiPConf',
    'CONFIG'       : {
                    'Daug_TRCHI2DOF_MAX'        : 10.     
                  , 'Daug_P_MIN'                : 10000.0 * MeV 
                  , 'Daug_PT_MIN'               : 400.0 * MeV
                  , 'Daug_MIPDV'                : 0.05 * mm
                  , 'Proton_PIDpi_MIN'          : 30.0
                  , 'Proton_PIDK_MIN'           : 5.0 
                  , 'Phi_WIN'                   : 10.0 * MeV
                  , 'Phi_PT'                    : 0.0 * MeV         
                  , 'Betac_AM_MIN'              : 1950.0 * MeV 
                  , 'Betac_AM_MAX'              : 2800.0 * MeV 
                  , 'Betac_BPVDIRA_MIN'         : 0.999
                  , 'Betac_VCHI2VDOF_MAX'       : 10.0   
                  , 'Betac_BPVLTIME_MIN'        : 0.0 * ns
                  , 'Betac_BPVLTIME_MAX'        : 0.006 * ns
                  , 'Hlt2IncPhiDecision'        : "HLT_PASS_RE('Hlt2IncPhiDecision')"
                  , 'PrescaleBetac2PhiP'        : 1.0
                  , 'PostscaleBetac2PhiP'       : 1.0
                 },
    'WGs' : [ 'BandQ' ],
    'STREAMS' :  ['CharmCompleteEvent' ]
}

XicHHH = {
    'BUILDERTYPE' : 'StrippingXic2HHHConf',
    'CONFIG'      : {
                    'Daug_All_PT_MIN'         : "400.0 * MeV"
                  , 'Daug_1of3_PT_MIN'        : "700.0 * MeV"
                  , 'Daug_P_MIN'              : "1200.0 * MeV"
                  , 'Daug_TRCHI2DOF_MAX'      : 10.0
                  , 'Daug_BPVIPCHI2_MIN'      : 0.5
                  , 'Daug_1of3_BPVIPCHI2_MIN' : 5.0
                  , 'Proton_PIDp_MIN'         : 10.0
                  , 'K_IPCHI2_MIN'            : 20.0
                  , 'Pi_PIDK_MAX'             : 0.0
                  , 'K_PIDK_MIN'              : 5.0
                  , 'Comb_MASS_MIN'           : "1950.0 * MeV "
                  , 'Comb_MASS_MAX'           : "2800.0 * MeV "
                  , 'Comb_ADOCAMAX_MAX'       : "0.1 * mm"
                  , 'Xic_PT_MIN'              : "1500.0 * MeV"
                  , 'Xic_VCHI2VDOF_MAX'       : 10.0
                  , 'Xic_BPVVDCHI2_MIN'       : 5.0
                  , 'Xic_BPVDIRA_MIN'         : 0.9999
                  , 'Xic_BPVLTIME_MAX'        : "0.06 * ns"
                  , 'Xic_BPVLTIME_MIN'        : "0.0 * ns"
                  , 'HltFilter'               : "HLT_PASS('Hlt2CharmHadD2HHHDecision')"
                  , 'PrescaleXic2PKPi'        : 0.
                  , 'PostscaleXic2PKPi'       : 0.0
                  , 'PrescaleXic2PKK'         : 0.0
                  , 'PostscaleXic2PKK'        : 0.0
                  , 'PrescaleXic2PV0'         : 1.0
                  , 'PostscaleXic2PV0'        : 1.0
                 },
    'WGs' : [ 'BandQ' ],
    'STREAMS' : [ 'Charm' ] 
#    'STREAMS' : { 'Charm' : 'StrippingXic2HHHTheta2PKS0Line'}
}





