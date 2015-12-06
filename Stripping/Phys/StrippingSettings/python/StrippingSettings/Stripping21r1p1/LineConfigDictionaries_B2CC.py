################################################################################
##                     S T R I P P I N G  2 1 r 1 p 1                         ##
##                                                                            ##
##  Configuration for B2CC WG                                                 ##
##  Contact person: Carlos Vazquez Sierra (carlos.vazquez@cern.ch)            ##
################################################################################

######################################################################
## StrippingBetaSBd2JpsiPi0DetachedLine (MDST.DST)
## -------------------------------------------------------------------
## Lines defined in StrippingB2JpsiPi0_Run1.py
## Authors: Maximilien Chefdeville
######################################################################

BetaSPi0_RI = {
    'BUILDERTYPE' : 'B2JpsiPi0Conf_RI',
    'CONFIG'      : { 'BPVLTIME'             :       0.2
                    , 'VCHI2PDOF'            :       10
                    , 'JpsiMassWindow'       :       80
                    },
    'STREAMS'     : [ 'Leptonic' ],
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
        , 'KplusPT'               :   500.    # MeV
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
