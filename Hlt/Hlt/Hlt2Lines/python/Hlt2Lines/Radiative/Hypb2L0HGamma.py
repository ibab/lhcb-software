# -*- coding: utf-8 -*-
# =============================================================================
# @file   Hypb2L0HGamma.py
#
# 
# =============================================================================
"""Creation of the Xi_b-(Omega_b-) -> Xi_-(Omega_-) gamma lines."""

from RadiativeLineBuilder import RadiativeLineBuilder
from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm


class Hypb2L0HGammaLines(RadiativeLineBuilder):
    @staticmethod
    def get_stages(_):
        from Stages import TrackGEC, MassFilter, B2XGammaCombiner, HyperonAll, SecondaryLambdaFilter, ChargedHyperonL0HCombiner, Hypb2L0HGammaCombiner
        from HltTracking.HltPVs import PV3D
        from Inputs import Hlt2LoosePions, Hlt2DownPions, Hlt2LooseKaons, Hlt2DownKaons

        ## instantiate  combiners for LL and DD
        from Inputs import LambdaDD, LambdaLL
        SharedSecondaryLambdaLL = SecondaryLambdaFilter("SharedSecondaryLambdaLL",[LambdaLL])
        SharedSecondaryLambdaDD = SecondaryLambdaFilter("SharedSecondaryLambdaDD",[LambdaDD]) 

        Xi2L0Pi_LLL =   ChargedHyperonL0HCombiner('Ximinus2L0Pi_LLL',
                                                  decay="[Xi- -> Lambda0 pi-]cc",
                                                  inputs=[SharedSecondaryLambdaLL, Hlt2LoosePions])

        Xi2L0Pi_DDL =   ChargedHyperonL0HCombiner('Ximinus2L0Pi_DDL',
                                                  decay="[Xi- -> Lambda0 pi-]cc",
                                                  inputs=[SharedSecondaryLambdaDD, Hlt2LoosePions])

        Xi2L0Pi_DDD =   ChargedHyperonL0HCombiner('Ximinus2L0Pi_DDD',
                                                  decay="[Xi- -> Lambda0 pi-]cc",
                                                  inputs=[SharedSecondaryLambdaDD, Hlt2DownPions])

        Omega2L0K_LLL = ChargedHyperonL0HCombiner('Omegaminus2L0K_LLL',
                                                  decay="[Omega- -> Lambda0 K-]cc",
                                                  inputs=[SharedSecondaryLambdaLL, Hlt2LooseKaons])

        Omega2L0K_DDL = ChargedHyperonL0HCombiner('Omegaminus2L0K_DDL',
                                                  decay="[Omega- -> Lambda0 K-]cc",
                                                  inputs=[SharedSecondaryLambdaDD, Hlt2LooseKaons])

        Omega2L0K_DDD = ChargedHyperonL0HCombiner('Omegaminus2L0K_DDD',
                                                  decay="[Omega- -> Lambda0 K-]cc",
                                                  inputs=[SharedSecondaryLambdaDD, Hlt2DownKaons])

        Xim2LamPim_LLL = MassFilter('Ximinus2L0Pi_LLL', [Xi2L0Pi_LLL])
        Xim2LamPim_DDL = MassFilter('Ximinus2L0Pi_DDL', [Xi2L0Pi_DDL])
        Xim2LamPim_DDD = MassFilter('Ximinus2L0Pi_DDD', [Xi2L0Pi_DDD])
        Omm2LamKm_LLL  = MassFilter('Omegaminus2L0K_LLL', [Omega2L0K_LLL])
        Omm2LamKm_DDL  = MassFilter('Omegaminus2L0K_DDL', [Omega2L0K_DDL])
        Omm2LamKm_DDD  = MassFilter('Omegaminus2L0K_DDD', [Omega2L0K_DDD])

        Xi_minus = HyperonAll('Xi_minus',
                              [Xim2LamPim_LLL, Xim2LamPim_DDL, Xim2LamPim_DDD])
        Omega_minus = HyperonAll('Omega_minus',
                                 [Omm2LamKm_LLL, Omm2LamKm_DDL, Omm2LamKm_DDD])

        #Build stages
        stages = {'RadiativeHypb2L0HGammaXi': [TrackGEC(),
                                               PV3D('Hlt2'),
                                               Hypb2L0HGammaCombiner('Xibm2XimGamma',
                                                                     '[Xi_b- -> Xi- gamma]cc',
                                                                     Xi_minus)],
                  
                  'RadiativeHypb2L0HGammaOm': [TrackGEC(),
                                               PV3D('Hlt2'),
                                               Hypb2L0HGammaCombiner('Ombm2OmmGamma',
                                                                     '[Omega_b- -> Omega- gamma]cc',
                                                                     Omega_minus)],
                   
                  'RadiativeHypb2L0HGammaXiEE': [TrackGEC(),
                                                 PV3D('Hlt2'),
                                                 B2XGammaCombiner('Xibm2XimGammaEE',
                                                                  '[Xi_b- -> Xi- gamma]cc',
                                                                  Xi_minus,
                                                                  True)],
                  

                  'RadiativeHypb2L0HGammaOmEE': [TrackGEC(),
                                                 PV3D('Hlt2'),
                                                 B2XGammaCombiner('Ombm2OmmGammaEE',
                                                                  '[Omega_b- -> Omega- gamma]cc',
                                                                  Omega_minus,
                                                                  True)]
                }
        return stages
        

    @staticmethod
    def get_cuts():
        
        cuts = {'Ximinus2L0Pi_LLL':{
                                    'VCHI2PDOF_MAX'            :  10.0,
                                    'Trk_ALL_MIPCHI2DV_MIN'    :  16.0,
                                    'TRPT_MIN'                 :  200 * MeV,
                                    'TRP_MIN'                  :  1000 * MeV,
                                    'TRCHI2DOF_MAX'            :  4,
                                    'TRACK_IPCHI2_MIN'         :  20,
                                    'BPVVDCHI2_MIN'            :  10.0,
                                    'BPVLTIME_MIN'             :  2.0 * picosecond,
                                    'BPVDIRA_MIN'              :  0.95,
                                    'PMIN'                     :  10000 * MeV,
                                    'PTMIN'                    :  1000 * MeV,
                                    'ASUMPT_MIN'               :  500 * MeV,
                                    'AM_MIN'                   :  1270 * MeV,
                                    'AM_MAX'                   :  1370 * MeV,
                                    'M_MIN'                    :  1290.0 * MeV,
                                    'M_MAX'                    :  1355.0 * MeV,
                                   },
                'Ximinus2L0Pi_DDL':{
                                    'VCHI2PDOF_MAX'            :  10.0,
                                    'Trk_ALL_MIPCHI2DV_MIN'    :  16.0,
                                    'TRPT_MIN'                 :  200 * MeV,
                                    'TRP_MIN'                  :  1000 * MeV,
                                    'TRCHI2DOF_MAX'            :  4,
                                    'TRACK_IPCHI2_MIN'         :  20,
                                    'BPVVDCHI2_MIN'            :  10.0,
                                    'BPVLTIME_MIN'             :  2.0 * picosecond,
                                    'BPVDIRA_MIN'              :  0.95,
                                    'PMIN'                     :  10000 * MeV,
                                    'PTMIN'                    :  1000 * MeV,
                                    'ASUMPT_MIN'               :  500 * MeV,
                                    'AM_MIN'                   :  1230 * MeV,
                                    'AM_MAX'                   :  1400 * MeV,
                                    'M_MIN'                    :  1270.0 * MeV,
                                    'M_MAX'                    :  1370.0 * MeV,
                                   },
                 'Ximinus2L0Pi_DDD':{
                                    'VCHI2PDOF_MAX'            :  10.0,
                                    'Trk_ALL_MIPCHI2DV_MIN'    :  16.0,
                                    'TRPT_MIN'                 :  200 * MeV,
                                    'TRP_MIN'                  :  1000 * MeV,
                                    'TRCHI2DOF_MAX'            :  4,
                                    'TRACK_IPCHI2_MIN'         :  20,
                                    'BPVVDCHI2_MIN'            :  10.0,
                                    'BPVLTIME_MIN'             :  2.0 * picosecond,
                                    'BPVDIRA_MIN'              :  0.95,
                                    'PMIN'                     :  10000 * MeV,
                                    'PTMIN'                    :  1000 * MeV,
                                    'ASUMPT_MIN'               :  500 * MeV,
                                    'AM_MIN'                   :  1230 * MeV,
                                    'AM_MAX'                   :  1400 * MeV,
                                    'M_MIN'                    :  1270.0 * MeV,
                                    'M_MAX'                    :  1370.0 * MeV,
                                   },
                'Omegaminus2L0K_LLL':{
                                    'VCHI2PDOF_MAX'            :  10.0,
                                    'Trk_ALL_MIPCHI2DV_MIN'    :  16.0,
                                    'TRPT_MIN'                 :  200 * MeV,
                                    'TRP_MIN'                  :  1000 * MeV,
                                    'TRCHI2DOF_MAX'            :  4,
                                    'TRACK_IPCHI2_MIN'         :  20,
                                    'BPVVDCHI2_MIN'            :  10.0,
                                    'BPVLTIME_MIN'             :  2.0 * picosecond,
                                    'BPVDIRA_MIN'              :  0.95,
                                    'PMIN'                     :  10000 * MeV,
                                    'PTMIN'                    :  1000 * MeV,
                                    'ASUMPT_MIN'               :  500 * MeV,
                                    'AM_MIN'                   :  1620 * MeV,
                                    'AM_MAX'                   :  1720 * MeV,
                                    'M_MIN'                    :  1640.0 * MeV,
                                    'M_MAX'                    :  1705.0 * MeV,
                                   },
                'Omegaminus2L0K_DDL':{
                                    'VCHI2PDOF_MAX'            :  10.0,
                                    'Trk_ALL_MIPCHI2DV_MIN'    :  16.0,
                                    'TRPT_MIN'                 :  200 * MeV,
                                    'TRP_MIN'                  :  1000 * MeV,
                                    'TRCHI2DOF_MAX'            :  4,
                                    'TRACK_IPCHI2_MIN'         :  20,
                                    'BPVVDCHI2_MIN'            :  10.0,
                                    'BPVLTIME_MIN'             :  2.0 * picosecond,
                                    'BPVDIRA_MIN'              :  0.95,
                                    'PMIN'                     :  10000 * MeV,
                                    'PTMIN'                    :  1000 * MeV,
                                    'ASUMPT_MIN'               :  500 * MeV,
                                    'AM_MIN'                   :  1590 * MeV,
                                    'AM_MAX'                   :  1750 * MeV,
                                    'M_MIN'                    :  1620.0 * MeV,
                                    'M_MAX'                    :  1720.0 * MeV,
                                   },
                 'Omegaminus2L0K_DDD':{
                                    'VCHI2PDOF_MAX'            :  10.0,
                                    'Trk_ALL_MIPCHI2DV_MIN'    :  16.0,
                                    'TRPT_MIN'                 :  200 * MeV,
                                    'TRP_MIN'                  :  1000 * MeV,
                                    'TRCHI2DOF_MAX'            :  4,
                                    'TRACK_IPCHI2_MIN'         :  20,
                                    'BPVVDCHI2_MIN'            :  10.0,
                                    'BPVLTIME_MIN'             :  2.0 * picosecond,
                                    'BPVDIRA_MIN'              :  0.95,
                                    'PMIN'                     :  10000 * MeV,
                                    'PTMIN'                    :  1000 * MeV,
                                    'ASUMPT_MIN'               :  500 * MeV,
                                    'AM_MIN'                   :  1590 * MeV,
                                    'AM_MAX'                   :  1750 * MeV,
                                    'M_MIN'                    :  1620.0 * MeV,
                                    'M_MAX'                    :  1720.0 * MeV,
                                   },
                'Xibm2XimGamma': {'SUM_PT_MIN'    : 5000*MeV,
                                 'PT_MIN'        : 1000*MeV,
                                 'MASS_WIN'      : 1000*MeV,
                                 'P_MIN'      : 20000*MeV,
                                 'PARTICLE'      :  'Xi_b-',
                                 'MTDOCACHI2_MAX': 9},
                
                'Ombm2OmmGamma': {'SUM_PT_MIN'    : 5000*MeV,
                                 'PT_MIN'        : 1000*MeV,
                                 'MASS_WIN'      : 1000*MeV,
                                 'P_MIN'      : 20000*MeV,
                                 'PARTICLE'     : 'Omega_b-',
                                 'MTDOCACHI2_MAX': 9},
                
                'Xibm2XimGammaEE': {'PARTICLE'     : 'Xi_b-',
                                   'VCHI2PDOF_MAX': 25,
                                   'PT_MIN'       : 1000*MeV,
                                   'MASS_WIN'     : 1000*MeV,
                                   'BPVDIRA_MIN'  : 0.1,
                                   'BPVIPCHI2_MAX': 16 },
                
                'Ombm2OmmGammaEE': {'PARTICLE'     : 'Omega_b-',
                                   'VCHI2PDOF_MAX': 25,
                                   'PT_MIN'       : 1000*MeV,
                                   'MASS_WIN'     : 1000*MeV,
                                   'BPVDIRA_MIN'  : 0.1,
                                   'BPVIPCHI2_MAX': 16 },

                'SharedSecondaryLambdaLL' : {
                                 'DecayTime_MIN'           : 4.5 * picosecond,
                                 'VZ_MIN'                  : -100. * mm,
                                 'VZ_MAX'                  :  500. * mm,
                                 'MASSW'                  :  20*MeV},


                'SharedSecondaryLambdaDD' : {
                                 'DecayTime_MIN'	   : 4.5 * picosecond,
                                 'VZ_MIN'                  : 300. * mm,
                                 'VZ_MAX'                  : 2500. * mm,
                                 'MASSW'                  :  20*MeV}
                }
        return cuts

    @staticmethod
    def get_l0():
        l0 = {}
        for line in ['RadiativeHypb2L0HGammaXi', 'RadiativeHypb2L0HGammaXiEE', 'RadiativeHypb2L0HGammaOm', 'RadiativeHypb2L0HGammaOmEE']:
            l0[line] = "L0_CHANNEL('Electron') | L0_CHANNEL('Photon')"
        return l0

# EOF
