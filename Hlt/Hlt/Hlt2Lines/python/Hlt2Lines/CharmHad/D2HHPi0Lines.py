# -*- coding: utf-8 -*-
## @file
#  Set of Hlt2-lines suitable for the study of charm decay channels with
#  neutrals (pi0, gamma) in the final state.
#
#  @author Simone STRACKA simone.stracka@cern.ch
#  @author Maurizio MARTINELLI maurizio.martinelli@cern.ch
#=============================================================================
""" Set of Hlt2-lines suitable for the study of charm decay channels with
    neutrals (pi0, gamma) in the final state.
"""
#=============================================================================
__author__  = "Simone STRACKA simone.stracka@cern.ch ; Maurizio MARTINELLI maurizio.martinelli@cern.ch"

from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser
from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm

class CharmHadD2HHPi0Lines() :
    def localcuts(self) : 
        return { 
                 'DetachedHH_forD2HHPi0' : { 
                                 'AM_MAX'         : 1865 * MeV,
                                 'ASUMPT_MIN'     : 1.0 * GeV,
                                 'ADOCA_MAX'      : 0.08 * mm,
                                 'VCHI2_MAX'      : 20.0,
                                 'BPVVD_MIN'      : -999.,
                                 'BPVCORRM_MAX'   : 1000 * GeV,
                                 'BPVVDCHI2_MIN'  : 25.0,
                                 },
                 'D2HHPi0R' : {
                                 'AM_MIN'            : 1735 * MeV,
                                 'AM_MAX'            : 1995 * MeV,
                                 'APT_MIN'           : 2500.0,
                                 'ADAU1PT_MIN'       : 1700,
                                 'ADAU1BPVIPCHI2_MIN': 36,
                                 'ADAU2PT_MIN'       : 1700,
                                 'ADAU2BPVIPCHI2_MIN': 36,
                                 'DMASS_MIN'         : 1745 * MeV,
                                 'DMASS_MAX'         : 1985 * MeV,
                                 'BPVIPCHI2_MAX'     : 50.0,
                                 'BPVDIRA_MIN'       : 0.9995,
                                 'BPVLTIME_MIN'      : 0.2, 
                                 'VCHI2PDOF_MAX'     : 99999,
                                },
                 'D2HHPi0M' : {
                                 'AM_MIN'            : 1735 * MeV,
                                 'AM_MAX'            : 1995 * MeV,
                                 'APT_MIN'           : 2500.0,
                                 'ADAU1PT_MIN'       : 1700,
                                 'ADAU1BPVIPCHI2_MIN': 36,
                                 'ADAU2PT_MIN'       : 1700,
                                 'ADAU2BPVIPCHI2_MIN': 36,
                                 'DMASS_MIN'         : 1745 * MeV,
                                 'DMASS_MAX'         : 1985 * MeV,
                                 'BPVIPCHI2_MAX'     : 50.0,
                                 'BPVDIRA_MIN'       : 0.9995,
                                 'BPVLTIME_MIN'      : 0.2, 
                                 'VCHI2PDOF_MAX'     : 99999,
                                },
                }

    def locallines(self):
        from Stages import AttachParticle, NeutralInParticleFilter, DetachedHHChild
        
        stages = {
                'D2HHPi0R' : [AttachParticle('D2HHPi0R', "D0 -> K*(892)0 pi0",#"D0 -> (K*(892)0 -> K+ pi-) pi0",
                                             inputs = [ DetachedHHChild('DetachedHH_forD2HHPi0'),
                                                        NeutralInParticleFilter("SharedNeutralChild_pi0R")]
                                             )],
                'D2HHPi0M' : [AttachParticle('D2HHPi0M', "D0 -> K*(892)0 pi0",#"D0 -> (K*(892)0 -> K+ pi-) pi0",
                                           inputs = [ DetachedHHChild('DetachedHH_forD2HHPi0'),
                                                      NeutralInParticleFilter("SharedNeutralChild_pi0M")]
                                           )],
                  }
        return stages

