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
            'D02HHPi0_HH' : {
              'AM_MAX'         : 1900 * MeV,
              'ASUMPT_MIN'     : 1.0 * GeV,
              'ADOCA_MAX'      : 0.08 * mm,
              'VCHI2_MAX'      : 20.0,
              'BPVVD_MIN'      : -999.,
              'BPVCORRM_MAX'   : 3.5 * GeV,
              'BPVVDCHI2_MIN'  : 25.0,                                 
            },
            'D2HHPi0R' : {
              'AM_MIN'            : 1700 * MeV, # was 1735 
              'AM_MAX'            : 2020 * MeV, # was 1995
              'APT_MIN'           : 1400.0 * MeV,
              'ADAU1PT_MIN'       : 1700,
              'ADAU1BPVIPCHI2_MIN': 36,
              'ADAU2PT_MIN'       : 1700,
              'ADAU2BPVIPCHI2_MIN': 36,
              'DMASS_MIN'         : 1745 * MeV,
              'DMASS_MAX'         : 1985 * MeV,
              'BPVIPCHI2_MAX'     : 50.0,
              'BPVDIRA_MIN'       : 0.9995,
              'BPVLTIME_MIN'      : 0.2 * picosecond, 
              'VCHI2PDOF_MAX'     : 20,
            },
            'D2HHPi0M' : {
              'AM_MIN'            : 1700 * MeV, # was 1735
              'AM_MAX'            : 2020 * MeV, # was 1995
              'APT_MIN'           : 1400.0 * MeV,
              'ADAU1PT_MIN'       : 1700,
              'ADAU1BPVIPCHI2_MIN': 36,
              'ADAU2PT_MIN'       : 1700,
              'ADAU2BPVIPCHI2_MIN': 36,
              'DMASS_MIN'         : 1745 * MeV,
              'DMASS_MAX'         : 1985 * MeV,
              'BPVIPCHI2_MAX'     : 50.0,
              'BPVDIRA_MIN'       : 0.9995,
              'BPVLTIME_MIN'      : 0.2 * picosecond, 
              'VCHI2PDOF_MAX'     : 20,
              },
            'DstD02HHPi0R' : {
              'DeltaM_AM_MIN'    : -999 * MeV,
              'DeltaM_AM_MAX'    : 195 * MeV,
              'DeltaM_MIN'       : -999 * MeV,
              'DeltaM_MAX'       : 185 * MeV,
              'TagVCHI2PDOF_MAX' : 10.0,
              'TisTosSpec'       : "Hlt1.*Track.*Decision%TOS",  # was MVA
              },                
            'DstD02HHPi0M' : {
              'DeltaM_AM_MIN'    : -999 * MeV,
              'DeltaM_AM_MAX'    : 195 * MeV,
              'DeltaM_MIN'       : -999 * MeV,
              'DeltaM_MAX'       : 185 * MeV,
              'TagVCHI2PDOF_MAX' : 10.0,
              'TisTosSpec'       : "Hlt1.*Track.*Decision%TOS",  # was MVA
              }
            }

    def locallines(self):
        from Stages import AttachParticle, TagDecay
        from Stages import SharedNeutralChild_pi0R, SharedNeutralChild_pi0M
        from Stages import SharedSoftTagChild_pi
        from Stages import D2HHPi0_PiPi, D2HHPi0_KPi, D2HHPi0_KK


        D2PiPiPi0R = AttachParticle('D2PiPiPi0R'
                        , decay = "D0 -> K*(892)0 pi0"
                        , inputs = [ D2HHPi0_PiPi , SharedNeutralChild_pi0R ]
                        , nickname = 'D2HHPi0R')

        DstD02PiPiPi0R = TagDecay('DstD02PiPiPi0R'
                , decay = ["D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-"]
                , inputs = [ D2PiPiPi0R, SharedSoftTagChild_pi]
                , nickname = 'DstD02HHPi0R' )


        D2PiPiPi0M = AttachParticle('D2PiPiPi0M'
                        , decay = "D0 -> K*(892)0 pi0"
                        , inputs = [ D2HHPi0_PiPi , SharedNeutralChild_pi0M ]
                        , nickname = 'D2HHPi0M')

        DstD02PiPiPi0M = TagDecay('DstD02PiPiPi0M'
                , decay = ["D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-"]
                , inputs = [D2PiPiPi0M, SharedSoftTagChild_pi]
                , nickname = 'DstD02HHPi0M')


        D2KKPi0R = AttachParticle('D2KKPi0R'
                        , decay = "D0 -> K*(892)0 pi0"
                        , inputs = [ D2HHPi0_KK , SharedNeutralChild_pi0R ]
                        , nickname = 'D2HHPi0R')

        DstD02KKPi0R = TagDecay('DstD02KKPi0R'
                , decay = ["D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-"]
                , inputs = [ D2KKPi0R, SharedSoftTagChild_pi]
                , nickname = 'DstD02HHPi0R' )


        D2KKPi0M = AttachParticle('D2KKPi0M'
                        , decay = "D0 -> K*(892)0 pi0"
                        , inputs = [ D2HHPi0_KK, SharedNeutralChild_pi0M ]
                        , nickname = 'D2HHPi0M')

        DstD02KKPi0M = TagDecay('DstD02KKPi0M'
                , decay = ["D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-"]
                , inputs = [D2KKPi0M, SharedSoftTagChild_pi]
                , nickname = 'DstD02HHPi0M')


        D2KPiPi0R = AttachParticle('D2KPiPi0R'
                        , decay = "[D0 -> K*(892)0 pi0]cc"
                        , inputs = [ D2HHPi0_KPi , SharedNeutralChild_pi0R ]
                        , nickname = 'D2HHPi0R')

        DstD02KmPipPi0R = TagDecay('DstD02KPiPi0R'
                , decay = ["[D*(2010)+ -> D0 pi+]cc"]
                , inputs = [ D2KPiPi0R, SharedSoftTagChild_pi]
                , nickname = 'DstD02HHPi0R' )
        DstD02KpPimPi0R = TagDecay('DstD02KPiPi0R'
                , decay = ["[D*(2010)- -> D0 pi-]cc"]
                , inputs = [ D2KPiPi0R, SharedSoftTagChild_pi]
                , nickname = 'DstD02HHPi0R' )


        D2KPiPi0M = AttachParticle('D2KPiPi0M'
                        , decay = "[D0 -> K*(892)0 pi0]cc"
                        , inputs = [ D2HHPi0_KPi , SharedNeutralChild_pi0M ]
                        , nickname = 'D2HHPi0M')

        DstD02KmPipPi0M = TagDecay('DstD02KPiPi0M'
                , decay = ["[D*(2010)+ -> D0 pi+]cc"]
                , inputs = [D2KPiPi0M, SharedSoftTagChild_pi]
                , nickname = 'DstD02HHPi0M')
        DstD02KpPimPi0M = TagDecay('DstD02KPiPi0M'
                , decay = ["[D*(2010)- -> D0 pi-]cc"]
                , inputs = [D2KPiPi0M, SharedSoftTagChild_pi]
                , nickname = 'DstD02HHPi0M')


        ## The stages dictionary should be a clear two-column list from
        ##   which the lines defined in this module can be directly read.
        stages = {
            'Dstp2D0Pip_D02PimPipPi0_Pi0R' : [DstD02PiPiPi0R],
            'Dstp2D0Pip_D02PimPipPi0_Pi0M' : [DstD02PiPiPi0M],
            'Dstp2D0Pip_D02KmKpPi0_Pi0R'   : [DstD02KKPi0R],
            'Dstp2D0Pip_D02KmKpPi0_Pi0M'   : [DstD02KKPi0M],
            'Dstp2D0Pip_D02KmPipPi0_Pi0R'  : [DstD02KmPipPi0R],
            'Dstp2D0Pip_D02KpPimPi0_Pi0R'  : [DstD02KpPimPi0R],
            'Dstp2D0Pip_D02KmPipPi0_Pi0M'  : [DstD02KmPipPi0M],
            'Dstp2D0Pip_D02KpPimPi0_Pi0M'  : [DstD02KpPimPi0M],
            }
        return stages
    
