################################################################################
##                          S T R I P P I N G  2 3                            ##
##                                                                            ##
##  Configuration for B2CC WG                                                 ##
##  Contact person: Carlos Vazquez Sierra (carlos.vazquez@cern.ch)            ##
################################################################################

from GaudiKernel.SystemOfUnits import *

###############################################################################
## StrippingBetaSBu2JpsiKDetachedLine (FullDST)
## StrippingBetaSBd2JpsiKstarDetachedLine (FullDST)
## StrippingBetaSBs2JpsiPhiDetachedLine (FullDST)
## StrippingBetaSBd2JpsiKsDetachedLine (FullDST)
## StrippingBetaSJpsi2MuMuLine (FullDST)
## StrippingBetaSLambdab2JpsiLambdaUnbiasedLine (MicroDST)
## StrippingBetaSBu2JpsiKPrescaledLine (MDST.DST)
## StrippingBetaSBs2JpsiPhiPrescaledLine (MDST.DST)
## StrippingBetaSBd2JpsiKstarPrescaledLine (MDST.DST)
## StrippingBetaSBd2JpsiKsPrescaledLine (MDST.DST)
## StrippingBetaSBd2JpsiKsLDDetachedLine (MDST.DST)
## StrippingBetaSBs2JpsiKstarWideLine (MDST.DST)
## ----------------------------------------------------------------------------
## Lines defined in StrippingB2JpsiXforBeta_s.py
## Authors: Greig Cowan, Juan Palacios, Francesca Dordei, Carlos Vazquez Sierra
###############################################################################

BetaS = {
    'BUILDERTYPE' : 'B2JpsiXforBeta_sConf',
    'CONFIG'      : { 'TRCHI2DOF'            :       5
                    , 'BPVLTIME'             :       0.2
                    , 'JpsiMassWindow'       :       80
                    , 'DaughterPT'           :       1000
                    , 'VCHI2PDOF'            :       10
                    , 'HLTCuts'              :       "HLT_PASS_RE('Hlt2DiMuonJPsiDecision')"
                    , 'Jpsi2MuMuPrescale'    :       0.15
                    , 'Bd2JpsiKstarPrescale' :       1.0
                    , 'Bd2JpsiKsPrescale'    :       1.0
                    , 'Bs2JpsiPhiPrescale'   :       1.0
                    , 'Bu2JpsiKPrescale'     :       1.0
                    },
    'STREAMS'     : { 'Leptonic' : ['StrippingBetaSBu2JpsiKPrescaledLine',
                                    'StrippingBetaSBs2JpsiPhiPrescaledLine',
                                    'StrippingBetaSBd2JpsiKstarPrescaledLine',
                                    'StrippingBetaSBd2JpsiKsPrescaledLine',
                                    'StrippingBetaSBd2JpsiKsLDDetachedLine',
                                    'StrippingBetaSBs2JpsiKstarWideLine',
                                    'StrippingBetaSLambdab2JpsiLambdaUnbiasedLine'],
                      'Dimuon'   : ['StrippingBetaSBu2JpsiKDetachedLine',
                                    'StrippingBetaSBd2JpsiKstarDetachedLine',
                                    'StrippingBetaSBs2JpsiPhiDetachedLine',
                                    'StrippingBetaSJpsi2MuMuLine',
                                    'StrippingBetaSBd2JpsiKsDetachedLine'] },
    'WGs'         : [ 'B2CC' ]
    }

######################################################################
## StrippingB2JpsiHHBs2Jpsif0PrescaledLine (MDST.DST)
## StrippingB2JpsiHHBs2Jpsif0PrescaledLine (MDST.DST)
## StrippingB2JpsiHHBs2Jpsif0Line (FullDST)
## -------------------------------------------------------------------
## Lines defined in StrippingB2JpsiHH.py
## Authors: Liming Zhang
######################################################################

B2JpsiHH = {
    'BUILDERTYPE' : 'B2JpsiHHConf',
    'CONFIG'      : { 'TRCHI2DOF'         :           5
                    , 'JpsiMassWindow'    :           80
                    , 'VCHI2PDOF'         :           10
                    , 'HLTCuts'           :           "HLT_PASS_RE('Hlt2DiMuonJPsiDecision')"
                    , 'Bs2Jpsif0Prescale' :           0.10 # 2011: 0.13, 2012: 0.62
                    },
    'STREAMS'     : { 'Leptonic' : ['StrippingB2JpsiHHBs2JpsiKstarLine',
                                    'StrippingB2JpsiHHBs2Jpsif0PrescaledLine'],
                      'Dimuon'   : ['StrippingB2JpsiHHBs2Jpsif0Line'] },
    'WGs'         : [ 'B2CC' ]
    }

######################################################################
## StrippingBetaSBd2JpsiPi0PrescaledLine (MDST.DST)
## StrippingBetaSBd2JpsiPi0DetachedLine (MDST.DST)
## -------------------------------------------------------------------
## Lines defined in StrippingB2JpsiPi0.py
## Authors: Maximilien Chefdeville
######################################################################

BetaSPi0 = {
    'BUILDERTYPE' : 'B2JpsiPi0Conf',
    'CONFIG'      : { 'BPVLTIME'             :       0.2
                    , 'VCHI2PDOF'            :       10
                    , 'JpsiMassWindow'       :       80
                    , 'HLTCuts'              :       "HLT_PASS_RE('Hlt2DiMuonJPsiDecision')"
                    , 'Bs2JpsiPi0Prescale'   :       1.0
                    },
    'STREAMS'     : [ 'Leptonic' ],
    'WGs'         : [ 'B2CC' ]
    }

######################################################################
## StrippingBetaSBs2JpsieePhiDetachedLine (MDST.DST)
## StrippingBetaSBs2JpsieePhiFromTracksLine (MDST.DST)
## StrippingBetaSBs2JpsieePhiLine (MDST.DST)
## -------------------------------------------------------------------
## Lines defined in StrippingBs2JpsieePhi.py
## Authors: Artur Ukleja, Jibo He, Konrad Klimaszewski
## Last changes made by Konrad Klimaszewski
######################################################################

BetaSBs2JpsieePhi = {
    'BUILDERTYPE' : 'Bs2JpsieePhiConf',
    'CONFIG'      : { 'ElectronPTLoose'            :   500.    # MeV
                    , 'ElectronPIDLoose'           :     0.    # adimensional
                    , 'ElectronTrackCHI2pDOFLoose' :     5.    # adimensional
                    , 'JpsiVertexCHI2pDOFLoose'    :    15.    # adimensional
                    , 'JpsiMassMinLoose'           :  1800.    # MeV
                    , 'JpsiMassMaxLoose'           :  3600.    # MeV
                    , 'KaonTrackCHI2pDOFLoose'     :     5.    # adimensional
                    , 'PhiPTLoose'                 :  1000.    # MeV
                    , 'PhiVertexCHI2pDOFLoose'     :    15.    # adimensional
                    , 'PhiMassMinLoose'            :   990.    # MeV
                    , 'PhiMassMaxLoose'            :  1050.    # MeV
                    , 'BsVertexCHI2pDOFLoose'      :    10.    # adimensional
                    , 'BsMassMinLoose'             :  4000.    # MeV
                    , 'BsMassMaxLoose'             :  6000.    # MeV
                    , 'LifetimeCut'                : " & (BPVLTIME()>0.3*ps)"
                    , 'PrescaleLoose'              :     1.    # adimensional
                    , 'ElectronPT'                 :   500.    # MeV
                    , 'ElectronPID'                :     0.    # adimensional
                    , 'ElectronTrackCHI2pDOF'      :     5.    # adimensional
                    , 'JpsiVertexCHI2pDOF'         :    15.    # adimensional
                    , 'JpsiMassMin'                :  1800.    # MeV
                    , 'JpsiMassMax'                :  3600.    # MeV
                    , 'KaonTrackCHI2pDOF'          :     5.    # adimensional
                    , 'KaonPID'                    :    -2.    # adimensional
                    , 'PhiPT'                      :  1000.    # MeV
                    , 'PhiVertexCHI2pDOF'          :    15.    # adimensional
                    , 'PhiMassMin'                 :   990.    # MeV
                    , 'PhiMassMax'                 :  1050.    # MeV
                    , 'BsVertexCHI2pDOF'           :    10.    # adimensional
                    , 'BsMassMin'                  :  4000.    # MeV
                    , 'BsMassMax'                  :  6000.    # MeV
                    , 'BsDIRA'                     :     0.99  # adimensional
                    , 'Prescale'                   :     0.1   # adimensional
                    },
    'STREAMS'     : [ 'Leptonic' ],
    'WGs'         : [ 'B2CC' ]
    }

######################################################################
## StrippingBs2EtacPhiBDTLine (MDST.DST)
## StrippingBs2EtacPhiBDT_KsKPiLine (MDST.DST)
## -------------------------------------------------------------------
## Lines defined in StrippingBs2EtacPhiBDT.py
## Authors: Morgan Martin, Jibo He
######################################################################

Bs2EtacPhiBDT = {
    'BUILDERTYPE' : 'Bs2EtacPhiBDTConf',
    'CONFIG'      : { 'HLTCuts'              : "(HLT_PASS_RE('Hlt2Topo.*Decision') | HLT_PASS_RE('Hlt2IncPhi.*Decision'))"
                    , 'KaonCuts'             : "(PROBNNk > 0.13) & (PT > 300*MeV) & (TRGHOSTPROB<0.4)"
                    , 'PionCuts'             : "(PROBNNpi > 0.2) & (PT > 250*MeV) & (TRGHOSTPROB<0.4)"
                    , 'EtacComAMCuts'        : "(AM<3.25*GeV)"
                    , 'EtacComN4Cuts'        : """
                                               (in_range(2.75*GeV, AM, 3.25*GeV))
                                               & ( (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3)+ACHILD(PT,4) ) > 2.5 *GeV)
                                               & ( (ACHILD(MIPCHI2DV(), 1) + ACHILD(MIPCHI2DV(), 2) + ACHILD(MIPCHI2DV(), 3) + ACHILD(MIPCHI2DV(), 4))>30)
                                               """
                    , 'EtacMomN4Cuts'        : "(VFASPF(VCHI2/VDOF) < 9.) & (in_range(2.8*GeV, MM, 3.2*GeV)) & (MIPCHI2DV(PRIMARY) > 2.)"
                    , 'EtacComCuts'          : "(in_range(2.75*GeV, AM, 3.25*GeV))"
                    , 'KsCuts'               : "(ADMASS('KS0') < 30.*MeV) & (BPVDLS>5)"
                    , 'PhiCuts'              : """
                                               (MAXTREE(ABSID=='K+',TRGHOSTPROB) < 0.4)
                                               & (MM<1.05*GeV)
                                               & (PT> 800*MeV)
                                               & (MIPCHI2DV(PRIMARY) > 2.)
                                               & (VFASPF(VCHI2) < 9.)
                                               """ 
                    , 'BsComCuts'              : "(ADAMASS('B_s0') < 500 *MeV)"
                    , 'BsMomCuts'              : "(VFASPF(VCHI2/VDOF) < 25.) & (BPVDIRA> 0.99) & (BPVIPCHI2()<25) & (BPVDLS>0)"
                    , 'Bs2EtacPhiMVACut'       :  "0."
                    , 'Bs2EtacPhiXmlFile'      :  "$TMVAWEIGHTSROOT/data/Bs2EtacPhi_BDT_v1r0.xml"
                    , 'Prescale'               : 1.
                    },
    'STREAMS'     : [ 'Bhadron' ],
    'WGs'         : [ 'B2CC' ]
    }

######################################################################
## StrippingBd2JpsieeKSBd2JpsieeKSFromTracksPrescaledLine (MDST.DST)
## StrippingBd2JpsieeKSBd2JpsieeKSFromTracksDetachedLine (MDST.DST)
## StrippingBd2JpsieeKSBd2JpsieeKSDetachedLine (MDST.DST)
## StrippingBd2JpsieeKSBd2JpsieeKstarFromTracksPrescaledLine (MDST.DST)
## StrippingBd2JpsieeKSBd2JpsieeKstarFromTracksDetachedLine (MDST.DST)
## StrippingBd2JpsieeKSBu2JpsieeKFromTracksDetachedLine (MDST.DST)
## -------------------------------------------------------------------
## Lines defined in StrippingBd2JpsieeKS.py
## Author: Ramon Niet
######################################################################

Bd2JpsieeKS = {
    'BUILDERTYPE'       : 'Bd2JpsieeKSConf',
    'CONFIG'    : {
          'BPVLTIME'              :     0.2   # ps
        , 'ElectronPT'            :   500.    # MeV
        , 'ElectronPID'           :     0.    # adimensional
        , 'ElectronTrackCHI2pDOF' :     5.    # adimensional
        , 'KaonPID'               :     0.    # adimensional
        , 'PionForKstarPT'        :   600.    # MeV
        , 'TRCHI2DOF'             :     3.    # adimensional
        , 'JpsiVertexCHI2pDOF'    :    15.    # adimensional
        , 'JpsiMassMin'           :  2300.    # MeV
        , 'JpsiMassMax'           :  3300.    # MeV
        , 'KstarMassMin'          :   826.    # MeV
        , 'KstarMassMax'          :   966.    # MeV
        , 'KstarPT'               :  1500.    # MeV
        , 'KstarVCHI2'            :    20.    # adimensional
        , 'KstarDaughtersIP'      :     9.    # adimensional
        , 'KplusPT'               :   800.    # MeV
        , 'KplusIP'               :     9.    # adimensional
        , 'KSVCHI2'               :    20.    # adimensional
        , 'KSBPVDLS'              :     5.    # adimensional
        , 'BdVertexCHI2pDOF'      :     7.    # adimensional
        , 'BdFromKstarVCHI2pDOF'  :    20.    # adimensional
        , 'BdMassMin'             :  4400.    # MeV
        , 'BdMassMax'             :  6000.    # MeV
        , 'Prescale'              :     0.1   # adamenssional
        , 'Bd2JpsieeKstarPrescale':     0.1   # adimensional
        , 'Bu2JpsieeKPrescale'    :     0.1   # adimensional
        },
    'STREAMS' : [ 'Leptonic' ],
    'WGs'    : [ 'B2CC' ]
    }

