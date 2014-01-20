# Line config dictionaries for BandQ WG
# 

from GaudiKernel.SystemOfUnits import *

#########################################################
### StrippingCC2DD
### -----------------------------------------------------
### Defined in:                 StrippingCC2DD.py
### Proponent:                  Andrea.Bizzeti@fi.infn.it
### Motivation:                 Low PT 2x charm, psi(3770)
### Documentation:              https://indico.cern.ch/conferenceDisplay.py?confId=269979
#########################################################

CC2DD = {        
    'WGs'               : ['BandQ'],
    'BUILDERTYPE'       : 'CC2DDConf',
    'CONFIG'   :  { ####### D0 / D~0 -> K pi cuts
                    'D0MassWin'     : "60*MeV",
                    'D0PT'          :  "0*MeV",
                    'D0VtxChi2Ndof' :  10.,
                    'D0Bpvdira'     : -10.,
                    'D0Bpvdls'      :   4.,
                    'D0daughterBpvIpChi2'    : 6.,
                    'D0daughterPT'           : "600*MeV",
                    'D0daughterP'            : "5*GeV",
                    'D0daughterTrkChi2'      : 3.,
                    'D0daughterTrkGhostProb' : 0.3,
                    ### ProbNN conditions
                    'D0daughterKaonProbNNk'  : 0.1,
                    'D0daughterPionProbNNpi' : 0.1,
                    ######## Dplus/Dminus -> Kpipi cuts, used also for D_s+/D_s- ->KKpi
                    'DpmMassWin'     : "60*MeV",
                    'DpmPT'          :  "0*MeV",
                    'DpmVtxChi2Ndof' :  10.,
                    'DpmBpvdira'     : -10.,
                    'DpmBpvdls'      :   4.,
                    'DpmdaughterBpvIpChi2'    : 6.,
                    'DpmdaughterPT'           : "500*MeV",  ## lower than for D0
                    'DpmdaughterP'            : "5*GeV",
                    'DpmdaughterTrkChi2'      : 3.,
                    'DpmdaughterTrkGhostProb' : 0.3,
                    ### ProbNN conditions
                    'DpmdaughterKaonProbNNk'  : 0.1,
                    'DpmdaughterPionProbNNpi' : 0.1,
                    ######## psi(3779) -> D D  cuts
                    #'CCMassCut'     : "(AM<5000*MeV)",
                    # no mass constraint
                    'CCMassCut'      : "(AM>0)",
                    'CCVtxChi2Ndof'  : 10,
                    'CCMaxD0ChildPT' : "1500*MeV",
                    'CCMaxD0TreePT'  : "1200*MeV",
                    'CCMaxD0MinTreeIpChi2'   : "0."    ## unused for the moment
                 },
    'STREAMS' : [
        'CharmCompleteEvent' 
        ],
}   
 

####################################################
### StrippingXic2HHH
###-------------------------------------------------
### Defined in: StrippingXic2HHH.py
### Proponent:  Yury.Shcheglov@cern.ch
### Motivation: Xic studies and pentaquark searches
### Documentation:
###  (Yury Shcheglov 2014-01-14 wrote:)  
###  1) Measurements ratios of the production  rates
###    R(pT)= N(Xic->p+\phi(KK)) / N( Lc->p+\phi(KK)) -
###  new doubly Cabbibo supressed decay Xic->p+phi to the  
###  known Lc->p+phi decay and  
###  R (pT) = N(Xic->p + Kstar(Kpi)) / N( Lc->p +  Kstar(Kpi)) 
###  - used to cross-check the pT distribution  shape of the first ratio;
###  
###  2) pentaquark search (both, long-lived charmed pentaquark Beta+ and
###  pentaquark Theta+ (recently JLab reported peak at M = 1540 MeV);
###  
###  The preliminary result for the measurement of the  rates ratios
###  shows the some difference  in the pT shape of
###  the ratios    N(Xic->p+\phi(KK)) / N( Lc->p+\phi(KK)) and
###  N(Xic->p + Kstar(Kpi)) / N( Lc->p +  Kstar(Kpi))  at the range  pT = 4-7 GeV. 
###  But experimental errors for the first ratio are large enough 
###  and it will be good to increase statistic for  the Xic->p+\phi(KK).
###  
###  For this goal from the stripping lines were rejected 
###  too tight cuts connected with kaons IPCHI2, which were 
###  found non-effective to suppress the background. 
###  Besides we refused from the tight DIRA cut to avoid 
###  correlation between pT and lifetime particle. 
###  Additionally, to keep  a reasonable retention 
###  factor in the  stripping lines, results of ANN for 
###  the particle identification ( PROBNNp(pi,K)) 
###  were used in the event selections. 
###  Besides, all  pT restrictions were 
###  choosen  more loose for all stripping lines.
###  
###  All efforts above give us a hope to reduce in 1.5-2 times our
###  experimental errors in the ratio N(Xic->p+\phi(KK)) / N( Lc->p+\phi(KK)) and 
###  to increase the sensitivity to the possible pentaquarks contributions due 
###  to more large statistics  for the signal.  
###  The common idea of all our modifications in the streeping lines is 
###  to keep signal efficiencies as high as it is a possible and 
###  simultaneously to keep possible systematic errors of 
###  measurements at the  some reasonable level. 
###
##################################################


Xic2HHH = {
    'WGs'           : ['BandQ'],
    'BUILDERTYPE'   : 'StrippingXic2HHHConf',
    'CONFIG'        : {  'Daug_All_PT_MIN'          : 300.0 * MeV
                        , 'Daug_P_MIN'              : 3000.0 * MeV
                        , 'Daug_TRCHI2DOF_MAX'      : 10.0
                        , 'Daug_1of3_BPVIPCHI2_MIN' : 0.5
                        , 'Proton_PIDp_MIN'         : 10.0 
                        , 'K_IPCHI2_MIN'            : 0.0 
                        , 'Comb_MASS_MIN'           : 1950.0 * MeV 
                        , 'Comb_MASS_MAX'           : 2800.0 * MeV 
                        , 'Comb_ADOCAMAX_MAX'       : 0.3 * mm   
                        , 'Xic_PT_MIN'              : 1500.0 * MeV
                        , 'Xic_VCHI2VDOF_MAX'       : 8.0
                        , 'Xic_BPVVDCHI2_MIN'       : 0.0
                        , 'Xic_BPVDIRA_MIN'         : 0.9
                        , 'Xic_BPVIPCHI2_MAX'       : 10.
                        , 'Xic_BPVLTIME_MAX'        : 0.0025 * ns
                        , 'Xic_BPVLTIME_MIN'        : 0.0002 * ns
                        , 'HltFilter'               : "HLT_PASS('Hlt2CharmHadD2HHHDecision')"
                        , 'PrescaleXic2PKPi'        : 1.0
                        , 'PostscaleXic2PKPi'       : 1.0
                        , 'PrescaleXic2PKK'         : 1.0
                        , 'PostscaleXic2PKK'        : 1.0
                        , 'PrescaleXic2PV0'         : 1.0
                        , 'PostscaleXic2PV0'        : 1.0
                        , 'PrescaleXic2KLam'        : 0.0 
                        , 'PostscaleXic2KLam'       : 0.0 
                        , 'ExtraInfoTools' : [
                            { "Type" : "ConeVariables", 
                              "ConeAngle" : 1.5, 
                              "ConeNumber" : 1, 
                              "Variables" : ['angle', 'mult', 'ptasy']
                            }, 
                            { "Type" : "ConeVariables", 
                              "ConeAngle" : 15., 
                              "ConeNumber" : 2, 
                              "Variables" : ['angle', 'mult', 'ptasy']
                            }, 
                            { "Type" : "VertexIsolation"}
                        ] 
                       },
    'STREAMS' : [
        'Charm' 
        ],
}   


###############################################################
### StrippingXibStarToXibZero
###------------------------------------------------------------
### Defined in: StrippingXibStarToXibZero.py
### Proponent:  matthew.john.charles@cern.ch
### Motivation: FCNC searches, Xib* searches, selection tuning
### Documentation:  
###   https://indico.cern.ch/conferenceDisplay.py?confId=269979
###############################################################

XibStarToXibZero = {
    'WGs'           : ['BandQ'],
    'BUILDERTYPE'   : 'XibStarBuilder',
    'CONFIG'        : {  
                        'LongTrackGEC'              : 300
                        , 'prescaleSignalDefault'   : 1.0
                        , 'prescaleSignalJpsi'      : 1.0
                        , 'prescaleControlHadronic' : 0.1
                        , 'prescaleControlMuonic'   : 1.0
                        , 'XibStar_PT_Min'          : 2500.0*MeV
                      },
    'STREAMS' : [
        'Bhadron' 
        ],
}


#########################################################
### StrippingPromptCharm
### -----------------------------------------------------
### Defined in:                 StrippingPromptCharm.py
### Proponent:                  Ivan.Belyaev@cern.ch
### Motivation:                 coherent stripping of stable charm hadrons
### Documentation: https://indico.cern.ch/conferenceDisplay.py?confId=270130
#########################################################

PromptCharm = {
    'WGs'           : ['BandQ'],
    'BUILDERTYPE'   : 'StrippingPromptCharmConf',
    'CONFIG'        : { 'D0Prescale'       : 0.05 ,
                        'D+Prescale'       : 0.05 ,
                        'D*Prescale'       : 0.1  ,
                        'DsPrescale'       : 0.5  ,
                      },
    'STREAMS' : [
        'Charm' 
        ],
}

