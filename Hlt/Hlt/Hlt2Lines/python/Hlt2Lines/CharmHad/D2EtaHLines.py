# -*- coding: utf-8 -*-
## @file
#  Set of Hlt2-lines for D(s) -> eta(') h, with eta(') -> pi+ pi- X (X = pi0, eta)
#
#  @author Simone STRACKA simone.stracka@cern.ch
#

## eta' channels still missing

from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CharmHadD2EtaHLines() :
    def localcuts(self) :
                return {
                    'DiPion_forD2EtaH' : {
                    'AM_MAX'                   : 1150 * MeV,     
                    'ASUMPT_MIN'               :  0.0 * MeV,
                    'ADOCA_MAX'                :  0.1 * mm,
                    'VCHI2_MAX'                :  20.0,
                    'BPVVD_MIN'                :  0.0,
                    'BPVCORRM_MAX'             : 9999 * GeV, #3.5 * GeV
                    'BPVVDCHI2_MIN'            :  0.0,
                                              },
                    'D2EtaPi_3h' : {
                    'AM_MIN'            : 1550 * MeV, 
                    'AM_MAX'            : 2550 * MeV,
                    'DMASS_MIN'         : 1600 * MeV,
                    'DMASS_MAX'         : 2500 * MeV,
                    'APT_MIN'           : 1500.0, 
                    'BPVLTIME_MIN'      : 0.1,   # ps 
                    'VCHI2PDOF_MAX'     : 5,
                    'AMRhoH_MIN'        : 0 * MeV,
                    'AMRhoH_MAX'        : 2550 * MeV, 
                    'AMEta_MIN'        : 450 * MeV,
                    'AMEta_MAX'        : 1150 * MeV,
                                                    },
                    'D2EtaK_3h' : {
                    'AM_MIN'            : 1550 * MeV, 
                    'AM_MAX'            : 2550 * MeV,
                    'DMASS_MIN'         : 1600 * MeV,
                    'DMASS_MAX'         : 2500 * MeV,
                    'APT_MIN'           : 1500.0,  
                    'BPVLTIME_MIN'      : 0.1,   # ps 
                    'VCHI2PDOF_MAX'     : 5,
                    'AMRhoH_MIN'        : 0 * MeV,
                    'AMRhoH_MAX'        : 2550 * MeV, 
                    'AMEta_MIN'        : 450 * MeV,
                    'AMEta_MAX'        : 1150 * MeV,
                                                    },
                    'D2EtaPrimePi_3h' : {
                    'AM_MIN'            : 1550 * MeV, 
                    'AM_MAX'            : 2550 * MeV,
                    'DMASS_MIN'         : 1600 * MeV,
                    'DMASS_MAX'         : 2500 * MeV,
                    'APT_MIN'           : 1500.0, 
                    'BPVLTIME_MIN'      : 0.1,   # ps 
                    'VCHI2PDOF_MAX'     : 5,
                    'AMRhoH_MIN'        : 0 * MeV,
                    'AMRhoH_MAX'        : 2550 * MeV, 
                    'AMEta_MIN'        : 750 * MeV,
                    'AMEta_MAX'        : 1150 * MeV,
                                                    },
                    'D2EtaPrimeK_3h' : {
                    'AM_MIN'            : 1550 * MeV, 
                    'AM_MAX'            : 2550 * MeV,
                    'DMASS_MIN'         : 1600 * MeV,
                    'DMASS_MAX'         : 2500 * MeV,
                    'APT_MIN'           : 1500.0,  
                    'BPVLTIME_MIN'      : 0.1,   # ps 
                    'VCHI2PDOF_MAX'     : 5,
                    'AMRhoH_MIN'        : 0 * MeV,
                    'AMRhoH_MAX'        : 2550 * MeV, 
                    'AMEta_MIN'        : 750 * MeV,
                    'AMEta_MAX'        : 1150 * MeV,
                                                    },
                }

    def locallines(self):
            from Stages import D2RhoHG_3Body_Combiner
            from Stages import DetachedRhoPIDChild
            from Stages import SharedNoPIDDetachedChild_pi
            from Stages import SharedNoPIDDetachedChild_K
            from Stages import SharedNeutralLowPtChild_gamma
            from Stages import SharedNeutralLowPtChild_pi0
            from Stages import SharedNeutralLowPtChild_eta

            stages = {
                'DiPion_forD2EtaH'  : [ DetachedRhoPIDChild('DiPion_forD2EtaH') ],
                'D2EtaPi_3h' : [D2RhoHG_3Body_Combiner('D2EtaPi_3h',
                                                    decay = ["D+ -> rho(770)0 pi+ pi0","D- -> rho(770)0 pi- pi0"],
                                                    inputs = [
                DetachedRhoPIDChild('DiPion_forD2EtaH'),
                SharedNoPIDDetachedChild_pi,
                SharedNeutralLowPtChild_pi0 ])
                             ],
                'D2EtaK_3h' : [D2RhoHG_3Body_Combiner('D2EtaK_3h',
                                                    decay = ["D+ -> rho(770)0 K+ pi0","D- -> rho(770)0 K- pi0"],
                                                    inputs = [
                DetachedRhoPIDChild('DiPion_forD2EtaH'),
                SharedNoPIDDetachedChild_K,
                SharedNeutralLowPtChild_pi0 ])
                             ],
                'D2EtaPrimePi_3h' : [D2RhoHG_3Body_Combiner('D2EtaPrimePi_3h',
                                                    decay = ["D+ -> rho(770)0 pi+ eta","D- -> rho(770)0 pi- eta"],
                                                    inputs = [
                DetachedRhoPIDChild('DiPion_forD2EtaH'),
                SharedNoPIDDetachedChild_pi,
                SharedNeutralLowPtChild_eta ])
                             ],
                'D2EtaPrimeK_3h' : [D2RhoHG_3Body_Combiner('D2EtaPrimeK_3h',
                                                    decay = ["D+ -> rho(770)0 K+ eta","D- -> rho(770)0 K- eta"],
                                                    inputs = [
                DetachedRhoPIDChild('DiPion_forD2EtaH'),
                SharedNoPIDDetachedChild_K,
                SharedNeutralLowPtChild_eta ])
                             ],
                }

            return stages

        
