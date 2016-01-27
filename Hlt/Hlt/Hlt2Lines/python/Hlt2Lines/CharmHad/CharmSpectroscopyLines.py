from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

## Temporary local definition of particle masses.  Used to adapt TagDecay cut
## variables from from Delta M to Q.
_local_m_pip =  139.57018 * MeV
_local_m_KS0 =  497.61400 * MeV
_local_m_pp  =  938.27205 * MeV
_local_m_Lam = 1115.68300 * MeV
_local_m_eta =  547.86200 * MeV

class CharmSpectroscopyLines :
    def localcuts(self) :
        return {
##  D0,pi is considered separately from the others because the inclusive D0 rate is
##  expected to be very high due to the large number of real D0 signal events
                'Spec_D0Pi' : {
                                        'Q_AM_MIN'         : 135 * MeV - _local_m_pip,
                                        'Q_AM_MAX'         : 1025 * MeV - _local_m_pip,
                                        'Q_M_MIN'          : 135 * MeV - _local_m_pip,
                                        'Q_M_MAX'          : 995 * MeV - _local_m_pip,
                                        'TagVCHI2PDOF_MAX' : 10.0,
                                        'TisTosSpec'       : "Hlt1.*MVA.*Decision%TOS",
                                       }
##  Dpm,pi is considered separately from the others because the inclusive Dpm rate is
##  expected to be very high due to the large number of real D+ signal events
               ,'Spec_DpmPi' : {
                                        'Q_AM_MIN'         : 135 * MeV - _local_m_pip,
                                        'Q_AM_MAX'         : 1025 * MeV - _local_m_pip,
                                        'Q_M_MIN'          : 135 * MeV - _local_m_pip,
                                        'Q_M_MAX'          : 995 * MeV - _local_m_pip,
                                        'TagVCHI2PDOF_MAX' : 10.0,
                                       }
               ,'Spec_DPi' : {
                                        'Q_AM_MIN'         : 135 * MeV - _local_m_pip,
                                        'Q_AM_MAX'         : 1025 * MeV - _local_m_pip,
                                        'Q_M_MIN'          : 135 * MeV - _local_m_pip,
                                        'Q_M_MAX'          : 995 * MeV - _local_m_pip,
                                        'TagVCHI2PDOF_MAX' : 10.0,
                                       }
               ,'Spec_DK' : {
                                        'Q_AM_MIN'         : 490 * MeV - _local_m_KS0,
                                        'Q_AM_MAX'         : 1380 * MeV - _local_m_KS0,
                                        'Q_M_MIN'          : 490 * MeV - _local_m_KS0,
                                        'Q_M_MAX'          : 1350 * MeV - _local_m_KS0,
                                        'TagVCHI2PDOF_MAX' : 10.0,
                                       }
               ,'Spec_DPr' : {
                                        'Q_AM_MIN'         : 930 * MeV - _local_m_pp,
                                        'Q_AM_MAX'         : 1820 * MeV - _local_m_pp,
                                        'Q_M_MIN'          : 930 * MeV - _local_m_pp,
                                        'Q_M_MAX'          : 1790 * MeV - _local_m_pp,
                                        'TagVCHI2PDOF_MAX' : 10.0,
                                       }
               ,'Spec_DLambda' : {
                                        'Q_AM_MIN'         : 1110 * MeV - _local_m_Lam,
                                        'Q_AM_MAX'         : 2000 * MeV - _local_m_Lam,
                                        'Q_M_MIN'          : 1110 * MeV - _local_m_Lam,
                                        'Q_M_MAX'          : 1970 * MeV - _local_m_Lam,
                                        'TagVCHI2PDOF_MAX' : 10.0,
                                       }
               ,'Spec_DEta' : {
                                        'Q_AM_MIN'         :  540 * MeV - _local_m_eta,
                                        'Q_AM_MAX'         : 1440 * MeV - _local_m_eta,
                                        'Q_M_MIN'          :  540 * MeV - _local_m_eta,
                                        'Q_M_MAX'          : 1410 * MeV - _local_m_eta,
                                       }
               ,'Spec_DGamma' : {
                                        'Q_AM_MIN'         :  0  * MeV,
                                        'Q_AM_MAX'         : 900 * MeV,
                                        'Q_M_MIN'          :  0  * MeV,
                                        'Q_M_MAX'          : 855 * MeV,
                                       }
                ,'Dst2D0pi_D02KPiPiPi' : {
                                        'Q_AM_MIN'         : 135 * MeV - _local_m_pip,
                                        'Q_AM_MAX'         : 175 * MeV - _local_m_pip,
                                        'Q_M_MIN'          : 135 * MeV - _local_m_pip,
                                        'Q_M_MAX'          : 165 * MeV - _local_m_pip,
                                        'TagVCHI2PDOF_MAX' : 10.0,
                                       }
                 ,'ChargedSpectroscopyBachelors' : {
                                        'IPCHI2_MAX'       :  15,
                                       }

                 # these are cuts for the PromptSpectroscopyFilter
                 # the default mass window, 2211 - 2362 correspond to the values in D2HHHLines.py
                 ,'LcSpec' : {
                                'IPCHI2_MAX'       :  15,
                                'D_BPVLTIME_MIN'   :  0.3 * picosecond,
                                'DMASS_MIN'        :  2211 * MeV,
                                'DMASS_MAX'        :  2362 * MeV,
                                'DPT_MIN'          :  2000 * MeV,
                                'DMom_MIN'         :  30000 * MeV
                               }

                 , 'D02KPiPiPiSpec' : {
                                'IPCHI2_MAX'       :  15,
                                'D_BPVLTIME_MIN'   :  0.3 * picosecond,
                                'DMASS_MIN'        :  1790 * MeV,
                                'DMASS_MAX'        :  1940 * MeV,
                                'DPT_MIN'          :  2000 * MeV,
                                'DMom_MIN'         :  30000 * MeV
                   }
        }

    def locallines(self):

##  these are  combiners instantiated in Stages.py
##  as indicated below, there is a different grammar to access
##  candidates passing trigger selections specified in  other files
        from Stages import D02HH_D0ToKmPip
        from Stages import TagDecay, TagDecayWithNeutral
        from Stages import SharedSoftTagChild_pi, SharedPromptChild_K, SharedPromptChild_p
        from Stages import CharmHadSharedKsLL, CharmHadSharedKsDD
        from Stages import CharmHadSharedSecondaryLambdaLL, CharmHadSharedSecondaryLambdaDD
        from Stages import D02KsKK_LL, D02KsKK_DD, D02KsPiPi_LL, D02KsPiPi_DD
        from Stages import D02KsKPi_LL, D02KsKPi_DD
        from Stages import D2HHH_DspToKmKpPip, D2HHH_DpToKmPipPip
        from Stages import D0_TAG_CPV_Dstp2D0Pip_D02KmPip
        from Stages import DstToD02HHHH_D02CFKPiPiPi
        from Stages import D02HHHHMass_D02KPiPiPi
        from Stages import SharedNeutralLowPtChild_pi0, SharedNeutralLowPtChild_eta
        from Stages import SharedNeutralLowPtChild_gamma
        from Stages import PromptBachelorFilter, PromptSpectroscopyFilter
        from Stages import SharedTighterPromptChild_p
        from Stages import SharedSoftTagChild_pi
##

 
## these come from D02HHHHLines.py
        D02KPiPiPiForSpectroscopy = PromptSpectroscopyFilter('D02KPiPiPiSpec'
                , inputs=[D02HHHHMass_D02KPiPiPi]
                , shared = True)

        from Stages import Lc2HHH_LcpToKmPpPip




        from Hlt2Lines.Utilities.Hlt2Stage import Hlt2ExternalStage
        from Hlt2Lines.DPS.Stages import MergeCharm

##  change from MergedD0 to separate trigger lines as D0 --> K,pi and
##  D0 --> K,pi,pi,pi have very different S:B circa July 15, 2015
##  mds        d0 = MergeCharm('MergedD0', inputs = [D02HH_D0ToKmPip, D02KsKK_LL, D02KsKK_DD, 
##  mds                D02KsPiPi_LL, D02KsPiPi_DD,
##  mds                D02KsKPi_LL, D02KsKPi_DD,
##  mds                D02KPiPiPiForSpectroscopy
##  mds                ])

##        dstar = MergeCharm('MergedDstar', inputs = [D0_TAG_CPV_Dstp2D0Pip_D02KmPip, DstToD02HHHH_D02CFKPiPiPi])
##        dstar = MergeCharm('MergedDstar', inputs = [D0_TAG_CPV_Dstp2D0Pip_D02KmPip])
        dstarKPi  = D0_TAG_CPV_Dstp2D0Pip_D02KmPip
        dstarK3Pi = TagDecay('Dst2D0pi_D02KPiPiPi'
                , decay = ["[D*(2010)+ -> D0 pi+]cc"]
                , inputs = [D02KPiPiPiForSpectroscopy, SharedSoftTagChild_pi])


##  
##  
##  start by filtering the standard "prompt" track lists to add IPChi2 cuts
##
        ## shared instances should usually be defined in Stages.py.
        ## Consider moving them there.
        BachelorPi = PromptBachelorFilter('CharmHadBachelorPi',
                                       inputs = [SharedSoftTagChild_pi],
                                       nickname = 'ChargedSpectroscopyBachelors',
                                       shared = True)

        BachelorK = PromptBachelorFilter('CharmHadBachelorK',
                                       inputs = [SharedPromptChild_K],
                                       nickname = 'ChargedSpectroscopyBachelors',
                                       shared = True)

        BachelorPr = PromptBachelorFilter('CharmHadBachelorPr',
                                       inputs = [SharedTighterPromptChild_p],
                                       nickname = 'ChargedSpectroscopyBachelors',
                                       shared = True)


## LcpForSpectroscopy is meant to be used as input for spectroscopy studies. It has
## a tighter set of selections on  decay time, on IPChi2 (nonexistent for 
## default CPV lines), and (potentially) Lambda_c mass to create candidates for spectroscopy studies
## It should be sent to *neither* Full or Turbo.
        ## shared instances should usually be defined in Stages.py.
        ## Consider moving them there.
        LcpForSpectroscopy = PromptSpectroscopyFilter('LcForSpec', nickname = 'LcSpec',
                                            inputs=[Lc2HHH_LcpToKmPpPip], shared = True)



        stages = {}
##  recall that the "nicknake" in TagDecay is the "name" used in the dictionary of cuts.
##  
##
        stages.update ( {
                  'Spec_D0ToKPi_PiTurbo' : [TagDecay('Spec_D0Pi',
                     decay = ["[D*(2010)+ -> D0 pi+]cc", "[D*(2010)- -> D0 pi-]cc"],
                     nickname = 'Spec_D0Pi',
                     inputs = [D02HH_D0ToKmPip,BachelorPi])],
                  'Spec_D0ToKPi_KsTurbo' : [TagDecay('Spec_D0Ks',
                     decay = ["[D*(2007)0 -> D0 KS0]cc"],
                     nickname = 'Spec_DK',
                     inputs = [D02HH_D0ToKmPip,CharmHadSharedKsLL,CharmHadSharedKsDD])],
                  'Spec_D0ToKPi_KpmTurbo' : [TagDecay('Spec_D0K',
                     decay = ["[D*(2010)+ -> D0 K+]cc", "[D*(2010)- -> D0 K-]cc"],
                     nickname = 'Spec_DK',
                     inputs = [D02HH_D0ToKmPip,BachelorK])],
                  'Spec_D0ToKPi_PrTurbo' : [TagDecay('Spec_D0Pr',
                     decay = ["[D*(2010)+ -> D0 p+]cc", "[D*(2010)- -> D0 p~-]cc"],
                     nickname = 'Spec_DPr',
                     inputs = [D02HH_D0ToKmPip,BachelorPr])],
                  'Spec_D0ToKPi_LambdaTurbo' : [TagDecay('Spec_D0Lambda',
                     decay = ["[D*(2007)0 -> D0 Lambda0]cc", "[D*(2007)0 -> D~0 Lambda0]cc"],
                     nickname = 'Spec_DLambda',
                     inputs = [D02HH_D0ToKmPip,CharmHadSharedSecondaryLambdaLL,CharmHadSharedSecondaryLambdaDD])],
                  'Spec_D0ToKPi_Pi0Turbo' : [TagDecayWithNeutral('Spec_D0Pi0',
                     decay = ["[D*(2007)0 -> D0 pi0]cc"],
                     nickname = 'Spec_DPi',
                     inputs = [D02HH_D0ToKmPip,SharedNeutralLowPtChild_pi0])],
                  'Spec_D0ToKPi_EtaTurbo' : [TagDecayWithNeutral('Spec_D0Eta',
                     decay = ["[D*(2007)0 -> D0 eta]cc"],
                     nickname = 'Spec_DEta',
                     inputs = [D02HH_D0ToKmPip,SharedNeutralLowPtChild_eta])],
                  'Spec_D0ToKPi_GammaTurbo' : [TagDecayWithNeutral('Spec_D0Gamma',
                     decay = ["[D*(2007)0 -> D0 gamma]cc"],
                     nickname = 'Spec_DGamma',
                     inputs = [D02HH_D0ToKmPip,SharedNeutralLowPtChild_gamma])],

                  'Spec_D0ToK3Pi_PiTurbo' : [TagDecay('Spec_D0Pi',
                     decay = ["[D*(2010)+ -> D0 pi+]cc", "[D*(2010)- -> D0 pi-]cc"],
                     nickname = 'Spec_D0Pi',
                     inputs = [D02KPiPiPiForSpectroscopy,BachelorPi])],
                  'Spec_D0ToK3Pi_KsTurbo' : [TagDecay('Spec_D0Ks',
                     decay = ["[D*(2007)0 -> D0 KS0]cc"],
                     nickname = 'Spec_DK',
                     inputs = [D02KPiPiPiForSpectroscopy,CharmHadSharedKsLL,CharmHadSharedKsDD])],
                  'Spec_D0ToK3Pi_KpmTurbo' : [TagDecay('Spec_D0K',
                     decay = ["[D*(2010)+ -> D0 K+]cc", "[D*(2010)- -> D0 K-]cc"],
                     nickname = 'Spec_DK',
                     inputs = [D02KPiPiPiForSpectroscopy,BachelorK])],
                  'Spec_D0ToK3Pi_PrTurbo' : [TagDecay('Spec_D0Pr',
                     decay = ["[D*(2010)+ -> D0 p+]cc", "[D*(2010)- -> D0 p~-]cc"],
                     nickname = 'Spec_DPr',
                     inputs = [D02KPiPiPiForSpectroscopy,BachelorPr])],
                  'Spec_D0ToK3Pi_LambdaTurbo' : [TagDecay('Spec_D0Lambda',
                     decay = ["[D*(2007)0 -> D0 Lambda0]cc", "[D*(2007)0 -> D~0 Lambda0]cc"],
                     nickname = 'Spec_DLambda',
                     inputs = [D02KPiPiPiForSpectroscopy,CharmHadSharedSecondaryLambdaLL,CharmHadSharedSecondaryLambdaDD])],
                  'Spec_D0ToK3Pi_Pi0Turbo' : [TagDecayWithNeutral('Spec_D0Pi0',
                     decay = ["[D*(2007)0 -> D0 pi0]cc"],
                     nickname = 'Spec_DPi',
                     inputs = [D02KPiPiPiForSpectroscopy,SharedNeutralLowPtChild_pi0])],
                  'Spec_D0ToK3Pi_EtaTurbo' : [TagDecayWithNeutral('Spec_D0Eta',
                     decay = ["[D*(2007)0 -> D0 eta]cc"],
                     nickname = 'Spec_DEta',
                     inputs = [D02KPiPiPiForSpectroscopy,SharedNeutralLowPtChild_eta])],
                  'Spec_D0ToK3Pi_GammaTurbo' : [TagDecayWithNeutral('Spec_D0Gamma',
                     decay = ["[D*(2007)0 -> D0 gamma]cc"],
                     nickname = 'Spec_DGamma',
                     inputs = [D02KPiPiPiForSpectroscopy,SharedNeutralLowPtChild_gamma])],

                  'Spec_DsPiTurbo' : [TagDecay('Spec_D0Pi',
                     decay = ["[Delta(1905)++ -> D_s+ pi+]cc", "[D*(2007)0 -> D_s+ pi-]cc"],
                     nickname = 'Spec_DPi',
                     inputs = [D2HHH_DspToKmKpPip,BachelorPi])],
                  'Spec_DsKsTurbo' : [TagDecay('Spec_D0K',
                     decay = ["[D*(2010)+ -> D_s+ KS0]cc"],
                     nickname = 'Spec_DK',
                     inputs = [D2HHH_DspToKmKpPip,CharmHadSharedKsLL,CharmHadSharedKsDD])],
                  'Spec_DsKpmTurbo' : [TagDecay('Spec_D0K',
                     decay = ["[Delta(1905)++ -> D_s+ K+]cc", "[D*(2007)0 -> D_s+ K-]cc"],
                     nickname = 'Spec_DK',
                     inputs = [D2HHH_DspToKmKpPip,BachelorK])],
                  'Spec_DsPrTurbo' : [TagDecay('Spec_D0Pr',
                     decay = ["[Delta(1905)++ -> D_s+ p+]cc", "[D*(2007)0 -> D_s+ p~-]cc"],
                     nickname = 'Spec_DPr',
                     inputs = [D2HHH_DspToKmKpPip,BachelorPr])],
                  'Spec_DsLambdaTurbo' : [TagDecay('Spec_D0Lambda',
                     decay = ["[D*(2010)+ -> D_s+ Lambda0]cc", "[D*(2010)- -> D_s- Lambda0]cc"],
                     nickname = 'Spec_DLambda',
                     inputs = [D2HHH_DspToKmKpPip,CharmHadSharedSecondaryLambdaLL,CharmHadSharedSecondaryLambdaDD])],
                  'Spec_DsPi0Turbo' : [TagDecayWithNeutral('Spec_DsPi0',
                     decay = ["[D*(2010)+ -> D_s+ pi0]cc"],
                     nickname = 'Spec_DPi',
                     inputs = [D2HHH_DspToKmKpPip,SharedNeutralLowPtChild_pi0])],
                  'Spec_DsEtaTurbo' : [TagDecayWithNeutral('Spec_DsEta',
                     decay = ["[D*(2010)+ -> D_s+ eta]cc"],
                     nickname = 'Spec_DEta',
                     inputs = [D2HHH_DspToKmKpPip,SharedNeutralLowPtChild_eta])],
                  'Spec_DsGammaTurbo' : [TagDecayWithNeutral('Spec_DsGamma',
                     decay = ["[D*(2010)+ -> D_s+ gamma]cc"],
                     nickname = 'Spec_DGamma',
                     inputs = [D2HHH_DspToKmKpPip,SharedNeutralLowPtChild_gamma])],


                  'Spec_DpPiTurbo' : [TagDecay('Spec_DpPi',
                     decay = ["[Delta(1905)++ -> D+ pi+]cc", "[D*(2007)0 -> D+ pi-]cc"],
                     nickname = 'Spec_DpmPi',
                     inputs = [D2HHH_DpToKmPipPip,BachelorPi])],
                  'Spec_DpKsTurbo' : [TagDecay('Spec_DpK',
                     decay = ["[D*(2010)+ -> D+ KS0]cc"],
                     nickname = 'Spec_DK',
                     inputs = [D2HHH_DpToKmPipPip,CharmHadSharedKsLL,CharmHadSharedKsDD])],
                  'Spec_DpKpmTurbo' : [TagDecay('Spec_DpK',
                     decay = ["[Delta(1905)++ -> D+ K+]cc", "[D*(2007)0 -> D+ K-]cc"],
                     nickname = 'Spec_DK',
                     inputs = [D2HHH_DpToKmPipPip,BachelorK])],
                  'Spec_DpPrTurbo' : [TagDecay('Spec_DpPr',
                     decay = ["[Delta(1905)++ -> D+ p+]cc", "[D*(2007)0 -> D+ p~-]cc"],
                     nickname = 'Spec_DPr',
                     inputs = [D2HHH_DpToKmPipPip,BachelorPr])],
                  'Spec_DpLambdaTurbo' : [TagDecay('Spec_DpLambda',
                     decay = ["[D*(2010)+ -> D+ Lambda0]cc", "[D*(2010)- -> D- Lambda0]cc"],
                     nickname = 'Spec_DLambda',
                     inputs = [D2HHH_DpToKmPipPip,CharmHadSharedSecondaryLambdaLL,CharmHadSharedSecondaryLambdaDD])],
                  'Spec_DpPi0Turbo' : [TagDecayWithNeutral('Spec_DpPi0',
                     decay = ["[D*(2010)+ -> D+ pi0]cc"],
                     nickname = 'Spec_DPi',
                     inputs = [D2HHH_DpToKmPipPip,SharedNeutralLowPtChild_pi0])],
                  'Spec_DpEtaTurbo' : [TagDecayWithNeutral('Spec_DpEta',
                     decay = ["[D*(2010)+ -> D+ eta]cc"],
                     nickname = 'Spec_DEta',
                     inputs = [D2HHH_DpToKmPipPip,SharedNeutralLowPtChild_eta])],
                  'Spec_DpGammaTurbo' : [TagDecayWithNeutral('Spec_DpGamma',
                     decay = ["[D*(2010)+ -> D+ gamma]cc"],
                     nickname = 'Spec_DGamma',
                     inputs = [D2HHH_DpToKmPipPip,SharedNeutralLowPtChild_gamma])],

                  'Spec_LcPiTurbo' : [TagDecay('Spec_LcPi',
                     decay = ["[Delta(1905)++ -> Lambda_c+ pi+]cc", "[D*(2007)0 -> Lambda_c+ pi-]cc"],
                     nickname = 'Spec_DPi',
                     inputs = [LcpForSpectroscopy,BachelorPi])],
                  'Spec_LcKsTurbo' : [TagDecay('Spec_LcK',
                     decay = ["[D*(2010)+ -> Lambda_c+ KS0]cc"],
                     nickname = 'Spec_DK',
                     inputs = [LcpForSpectroscopy,CharmHadSharedKsLL,CharmHadSharedKsDD])],
                  'Spec_LcKpmTurbo' : [TagDecay('Spec_LcK',
                     decay = ["[Delta(1905)++ -> Lambda_c+ K+]cc", "[D*(2007)0 -> Lambda_c+ K-]cc"],
                     nickname = 'Spec_DK',
                     inputs = [LcpForSpectroscopy,BachelorK])],
                  'Spec_LcPrTurbo' : [TagDecay('Spec_LcPr',
                     decay = ["[Delta(1905)++ -> Lambda_c+ p+]cc", "[D*(2007)0 -> Lambda_c+ p~-]cc"],
                     nickname = 'Spec_DPr',
                     inputs = [LcpForSpectroscopy,BachelorPr])],
                  'Spec_LcLambdaTurbo' : [TagDecay('Spec_LcLambda',
                     decay = ["[D*(2010)+ -> Lambda_c+ Lambda0]cc", "[D*(2010)- -> Lambda_c~- Lambda0]cc"],
                     nickname = 'Spec_DLambda',
                     inputs = [LcpForSpectroscopy,CharmHadSharedSecondaryLambdaLL,
                               CharmHadSharedSecondaryLambdaDD])],
                  'Spec_LcPi0Turbo' : [TagDecayWithNeutral('Spec_LcPi0',
                     decay = ["[D*(2010)+ -> Lambda_c+ pi0]cc"],
                     nickname = 'Spec_DPi',
                     inputs = [LcpForSpectroscopy,SharedNeutralLowPtChild_pi0])],
                  'Spec_LcEtaTurbo' : [TagDecayWithNeutral('Spec_LcEta',
                     decay = ["[D*(2010)+ -> Lambda_c+ eta]cc"],
                     nickname = 'Spec_DEta',
                     inputs = [LcpForSpectroscopy,SharedNeutralLowPtChild_eta])],
                  'Spec_LcGammaTurbo' : [TagDecayWithNeutral('Spec_LcGamma',
                     decay = ["[D*(2010)+ -> Lambda_c+ gamma]cc"],
                     nickname = 'Spec_DGamma',
                     inputs = [LcpForSpectroscopy,SharedNeutralLowPtChild_gamma])],

                  'Spec_Dst_KPi_PiTurbo' : [TagDecay('Spec_D0Pi',
                     decay = ["[Delta(1905)++ -> D*(2010)+ pi+]cc", "[D*(2007)0 -> D*(2010)+ pi-]cc"],
                     nickname = 'Spec_DPi',
                     inputs = [dstarKPi, BachelorPi])],
                  'Spec_Dst_KPi_KsTurbo' : [TagDecay('Spec_D0K',
                     decay = ["[Delta(1905)~+ -> D*(2010)+ KS0]cc"],
                     nickname = 'Spec_DK',
                     inputs = [dstarKPi, CharmHadSharedKsLL,CharmHadSharedKsDD])],
                  'Spec_Dst_KPi_KpmTurbo' : [TagDecay('Spec_D0K',
                     decay = ["[Delta(1905)++ -> D*(2010)+ K+]cc", "[D*(2007)0 -> D*(2010)+ K-]cc"],
                     nickname = 'Spec_DK',
                     inputs = [dstarKPi, BachelorK])],
                  'Spec_Dst_KPi_PrTurbo' : [TagDecay('Spec_D0Pr',
                     decay = ["[Delta(1905)++ -> D*(2010)+ p+]cc", "[D*(2007)0 -> D*(2010)+ p~-]cc"],
                     nickname = 'Spec_DPr',
                     inputs = [dstarKPi, BachelorPr])],
                  'Spec_Dst_KPi_LambdaTurbo' : [TagDecay('Spec_D0Lambda',
                     decay = ["[Delta(1905)~+ -> D*(2010)+ Lambda0]cc", "[D*(2010)- -> D*(2010)- Lambda0]cc"],
                     nickname = 'Spec_DLambda',
                     inputs = [dstarKPi, CharmHadSharedSecondaryLambdaLL,CharmHadSharedSecondaryLambdaDD])],
                  'Spec_Dst_KPi_Pi0Turbo' : [TagDecayWithNeutral('Spec_DstPi0',
                     decay = ["[Delta(1905)~+ -> D*(2010)+  pi0]cc"],
                     nickname = 'Spec_DPi',
                     inputs = [dstarKPi,SharedNeutralLowPtChild_pi0])],
                  'Spec_Dst_KPi_EtaTurbo' : [TagDecayWithNeutral('Spec_DstEta',
                     decay = ["[Delta(1905)~+ -> D*(2010)+ eta]cc"],
                     nickname = 'Spec_DEta',
                     inputs = [dstarKPi,SharedNeutralLowPtChild_eta])],
                  'Spec_Dst_KPi_GammaTurbo' : [TagDecayWithNeutral('Spec_DstGamma',
                     decay = ["[Delta(1905)~+ -> D*(2010)+ gamma]cc"],
                     nickname = 'Spec_DGamma',
                     inputs = [dstarKPi,SharedNeutralLowPtChild_gamma])],

                  'Spec_Dst_K3Pi_PiTurbo' : [TagDecay('Spec_D0Pi',
                     decay = ["[Delta(1905)++ -> D*(2010)+ pi+]cc", "[D*(2007)0 -> D*(2010)+ pi-]cc"],
                     nickname = 'Spec_DPi',
                     inputs = [dstarK3Pi, BachelorPi])],
                  'Spec_Dst_K3Pi_KsTurbo' : [TagDecay('Spec_D0K',
                     decay = ["[Delta(1905)~+ -> D*(2010)+ KS0]cc"],
                     nickname = 'Spec_DK',
                     inputs = [dstarK3Pi, CharmHadSharedKsLL,CharmHadSharedKsDD])],
                  'Spec_Dst_K3Pi_KpmTurbo' : [TagDecay('Spec_D0K',
                     decay = ["[Delta(1905)++ -> D*(2010)+ K+]cc", "[D*(2007)0 -> D*(2010)+ K-]cc"],
                     nickname = 'Spec_DK',
                     inputs = [dstarK3Pi, BachelorK])],
                  'Spec_Dst_K3Pi_PrTurbo' : [TagDecay('Spec_D0Pr',
                     decay = ["[Delta(1905)++ -> D*(2010)+ p+]cc", "[D*(2007)0 -> D*(2010)+ p~-]cc"],
                     nickname = 'Spec_DPr',
                     inputs = [dstarK3Pi, BachelorPr])],
                  'Spec_Dst_K3Pi_LambdaTurbo' : [TagDecay('Spec_D0Lambda',
                     decay = ["[Delta(1905)~+ -> D*(2010)+ Lambda0]cc", "[D*(2010)- -> D*(2010)- Lambda0]cc"],
                     nickname = 'Spec_DLambda',
                     inputs = [dstarK3Pi, CharmHadSharedSecondaryLambdaLL,CharmHadSharedSecondaryLambdaDD])],
                  'Spec_Dst_K3Pi_Pi0Turbo' : [TagDecayWithNeutral('Spec_DstPi0',
                     decay = ["[Delta(1905)~+ -> D*(2010)+ pi0]cc"],
                     nickname = 'Spec_DPi',
                     inputs = [dstarK3Pi,SharedNeutralLowPtChild_pi0])],
                  'Spec_Dst_K3Pi_EtaTurbo' : [TagDecayWithNeutral('Spec_DstEta',
                     decay = ["[Delta(1905)~+ -> D*(2010)+  eta]cc"],
                     nickname = 'Spec_DEta',
                     inputs = [dstarK3Pi,SharedNeutralLowPtChild_eta])],
                  'Spec_Dst_K3Pi_GammaTurbo' : [TagDecayWithNeutral('Spec_DstGamma',
                     decay = ["[Delta(1905)~+ -> D*(2010)+  gamma]cc"],
                     nickname = 'Spec_DGamma',
                     inputs = [dstarK3Pi,SharedNeutralLowPtChild_gamma])],


                 } )
        

        return stages
