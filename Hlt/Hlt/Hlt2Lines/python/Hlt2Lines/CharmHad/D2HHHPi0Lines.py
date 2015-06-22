# -*- coding: utf-8 -*-
## @file
#  Set of Hlt2-lines suitable for the study of four-body charm decay channels 
#  with neutrals (pi0,gamma) in the final state.
#
#  @author Maurizio MARTINELLI maurizio.martinelli@cern.ch
#  @author Simone STRACKA simone.stracka@cern.ch
#=============================================================================
""" Set of Hlt2-lines suitable for the study of four-body charm decay channels 
    with neutrals (pi0,gamma) in the final state.
"""
#=============================================================================
__author__  = "Maurizio MARTINELLI maurizio.martinelli@cern.ch ; Simone STRACKA simone.stracka@cern.ch"

from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CharmHadD2HHHPi0Lines() :
    def localcuts(self) :
        ## The configuration dictionaries for the lines used to be generated
        ## in a loop in which keys/line names were assembled from components.
        ## This led to very compact code, but it also made it difficult to
        ## find things with simple search tools.
        ## Further, there seem to be only one variant of the dictionaries.
        ## Having one configuration dictionary per combiner does make it
        ## possible to apply different configurations for each of the lines
        ## in HltSettings.  However, it is not clear that it would be desirable
        ## to do so.  Until it is clear that we need different configurations
        ## for the combiners of the lines, use just one set of configuration
        ## dictionaries to configure all of the lines.  (P.S.)

        cutsForDetachedHHH_Pi0 = {
            'Trk_ALL_MIPCHI2DV_MIN'   : 5.0,
            'AM_MIN'                  : 0.0,
            'AM_MAX'                  : 1.5 * GeV,
            'ASUMPT_MIN'              : 1.9 * GeV,
            'Trk_1OF3_MIPCHI2DV_MIN'  : 5.0, # This can be tuned for reducing retention
            'Trk_2OF3_MIPCHI2DV_MIN'  : 5.0, # This can be tuned for reducing retention
            'VCHI2PDOF_MAX'           : 10.0,
            'BPVCORRM_MAX'            : 3500 * MeV,
            'BPVDIRA_MIN'             : 0.999,
            'BPVVDCHI2_MIN'           : 20.0,
            'BPVLTIME_MIN'            : 0.3 * picosecond,
            'TisTosSpec'              : "Hlt1.*Track.*Decision%TOS"
        }
        cutsForHHHPi0  = {
            'AM_MIN'                : 1755.0 * MeV,
            'AM_MAX'                : 2055.0 * MeV,
            'APT_MIN'               : 2500.0,
            'ADAU1PT_MIN'           : 1900,
            'ADAU1BPVIPCHI2_MIN'    : 36,
            'ADAU2PT_MIN'           : 1700,
            'ADAU2BPVIPCHI2_MIN'    : 36,
            'BPVLTIME_MIN'          : 0.3 * picosecond,
            'BPVIPCHI2_MAX'         : 20.0,
            'DMASS_MIN'             : 1795.0 * MeV,
            'DMASS_MAX'             : 2035.0 * MeV,
            'BPVDIRA_MIN'           : 0.999,
            'VCHI2PDOF_MAX'         : 20.0,
            'TisTosSpec'              : "Hlt1.*Track.*Decision%TOS"
        }
        cuts = {
                'DetachedHHH_forD2HHHPi0' :  cutsForDetachedHHH_Pi0,
                'D2HHHPi0'                :  cutsForHHHPi0
        }

        return cuts
  
    def locallines(self):
        from Stages import SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi
        from Stages import SharedNeutralChild_pi0
        from Stages import DetachedHHHChildCombiner, AttachParticle

        ## Detached HHH combinations
        ## Make the combiner names compact since these are not shared.
        DetachedPiPiPi = DetachedHHHChildCombiner('HHHComb'
                , decay = ["[K*(892)+ -> pi+ pi+ pi-]cc"]
                , inputs = [SharedDetachedDpmChild_pi]
                , nickname = 'DetachedHHH_forD2HHHPi0')

        DetachedKmPipPip = DetachedHHHChildCombiner('HHHComb'
                , decay = ["[K*(892)+ -> K- pi+ pi+]cc"]
                , inputs = [SharedDetachedDpmChild_pi, SharedDetachedDpmChild_K]
                , nickname = 'DetachedHHH_forD2HHHPi0')

        DetachedKpPimPip = DetachedKmPipPip.clone('HHHComb'
                , decay = ["[K*(892)+ -> K+ pi- pi+]cc"] )

        DetachedKmKpPip = DetachedKmPipPip.clone('HHHComb'
                , decay = ["[K*(892)+ -> K+ K- pi+]cc"] )

        DetachedKpKpPim = DetachedKmPipPip.clone('HHHComb'
                , decay = ["[K*(892)+ -> K+ K+ pi-]cc"] )

        DetachedKKK = DetachedHHHChildCombiner('HHHComb'
                , decay = ["[K*(892)+ -> K+ K- K+]cc"]
                , inputs = [SharedDetachedDpmChild_K]
                , nickname = 'DetachedHHH_forD2HHHPi0')


        ## Add Pi0s to the combinations to complete the reconstruction
        ## Make the combiner names compact since these are not shared.
        D2PiPiPiPi0 = AttachParticle('AddPi0'
                , decay = "[D+ -> K*(892)+ pi0]cc"
                , inputs = [ DetachedPiPiPi, SharedNeutralChild_pi0 ]
                , nickname = 'D2HHHPi0')

        D2KmPipPipPi0 = D2PiPiPiPi0.clone('AddPi0'
                , inputs = [ DetachedKmPipPip, SharedNeutralChild_pi0 ] )

        D2KpPimPipPi0 = D2PiPiPiPi0.clone('AddPi0'
                , inputs = [ DetachedKpPimPip, SharedNeutralChild_pi0 ] )

        D2KmKpPipPi0 = D2PiPiPiPi0.clone('AddPi0'
                , inputs = [ DetachedKmKpPip, SharedNeutralChild_pi0 ] )

        D2KpKpPimPi0 = D2PiPiPiPi0.clone('AddPi0'
                , inputs = [ DetachedKpKpPim, SharedNeutralChild_pi0 ] )

        D2KKKPi0 = D2PiPiPiPi0.clone('AddPi0'
                , inputs = [ DetachedKKK, SharedNeutralChild_pi0 ] )



        ## The stages dictionary should be a clear two-column list from
        ##   which the lines defined in this module can be directly read.
        stages = {
                  'DpDsp2PimPipPipPi0' : [ D2PiPiPiPi0 ]
                , 'DpDsp2KmPipPipPi0'  : [ D2KmPipPipPi0 ]
                , 'DpDsp2KpPimPipPi0'  : [ D2KpPimPipPi0 ]
                , 'DpDsp2KmKpPipPi0'   : [ D2KmKpPipPi0 ]
                , 'DpDsp2KpKpPimPi0'   : [ D2KpKpPimPi0 ]
                , 'DpDsp2KmKpKpPi0'    : [ D2KKKPi0 ]
        }
        return stages
