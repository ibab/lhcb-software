# Line config dictionaries for B2CC WG for 2012 restripping

BetaS = {        
    'WGs'               : ['B2CC'],
    'BUILDERTYPE'       : 'B2JpsiXforBeta_sConf',
    'CONFIG'    : {
                         'TRCHI2DOF'                 :       5
                 ,       'BPVLTIME'                  :       0.2
                 ,       'DaughterPT'                :       1000
                 ,       'VCHI2PDOF'                 :       10
                 ,       'Jpsi2MuMuPrescale'         :       0.04   #0.01
                 ,       'Jpsi2MuMuDetachedPrescale' :       1.0
                 ,       'Bu2JpsiKPrescale'          :       0.2  #0.05
                 ,       'Bd2JpsiKstarPrescale'      :       0.2   #0.03
                 ,       'Bd2JpsiKsPrescale'         :       0.5   #0.3
                 ,       'Bs2JpsiPhiPrescale'        :       0.5 #0.15
                 ,       'Bs2Jpsif0Prescale'         :       0.15  
                 ,       'Bs2JpsiEtaPrescale'        :       0.5
                 ,       'Bs2JpsiEtapPrescale'       :       0.15
                 ,       'Bs2JpsiPi0Prescale'        :       0.4
                 ,       'Bs2JpsiRho0Prescale'       :       0.15
                         },
    'STREAMS' : {
        'Leptonic' : [
         'StrippingBetaSBs2JpsiPhiMicroLine'
        ],
        'Dimuon' : [
         'StrippingBetaSJpsi2MuMuLine',
         'StrippingBetaSBs2JpsiPhiDetachedLine',
         'StrippingBetaSBu2JpsiKPrescaledLine',
         'StrippingBetaSBs2JpsiPhiPrescaledLine',
         'StrippingBetaSBd2JpsiKstarPrescaledLine',
         'StrippingBetaSBd2JpsiKsPrescaledLine',
         'StrippingBetaSBs2Jpsif0NoIPPrescaledLine',
         'StrippingBetaSBs2JpsiEtaPrescaledLine',
         'StrippingBetaSBs2JpsiEtapPrescaledLine',
         'StrippingBetaSBd2JpsiPi0PrescaledLine',
         'StrippingBetaSBd2JpsiRho0PrescaledLine'
        ]
    }   
    }


