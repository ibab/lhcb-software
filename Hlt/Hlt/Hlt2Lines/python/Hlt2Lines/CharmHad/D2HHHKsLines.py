# -*- coding: utf-8 -*-
## @file
#  Set of Hlt2-lines suitable for the study of four-body charm decay channels 
#  with KS (DD,LL) in the final state.
#
#  @author Maurizio MARTINELLI maurizio.martinelli@cern.ch
#  @author Simone STRACKA simone.stracka@cern.ch
#=============================================================================
""" Set of Hlt2-lines suitable for the study of four-body charm decay channels 
    with KS (DD,LL) in the final state.
"""
#=============================================================================
__author__  = "Maurizio MARTINELLI maurizio.martinelli@cern.ch ; Simone STRACKA simone.stracka@cern.ch"

import copy

from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CharmHadD2HHHKsLines() :
    def localcuts(self) :
        ## The configuration dictionaries for the lines used to be generated
        ## in a loop in which keys/line names were assembled from components.
        ## This led to very compact code, but it also made it difficult to
        ## find things with simple search tools.
        ## Further, there seem to be only two variants of the dictionaries.
        ## Having one configuration dictionary per combiner does make it
        ## possible to apply different configurations for each of the lines
        ## in HltSettings.  However, it is not clear that it would be desirable
        ## to do so.  Until it is clear that we need different configurations
        ## for the combiners of the lines, use just two configuration
        ## dictionaries to configure all of the lines.  (P.S.)

        cutsForDp2HHHKs = {
            'Trk_ALL_PT_MIN'        :  300 * MeV,
            'Trk_ALL_MIPCHI2DV_MIN' :  3,
            'AM_34'                 : (139.5 + 139.5) * MeV,
            'AM_4'                  : (139.5) * MeV,
            'AM_MIN'                :  1755.0 * MeV,
            'AM_MAX'                :  2055.0 * MeV,
            'ADOCA_MAX'             :  100.0 * mm,
            'ACHI2DOCA_MAX'         :  10.0,
            'ASUMPT_MIN'            :  2.0 * GeV,
            'VCHI2PDOF_MAX'         :  12.0,
            'BPVLTIME_MIN'          :  0.2 * picosecond,
            'BPVDIRA_MIN'           :  0.999,
            'BPVIPCHI2_MAX'         :  20.0,
            'Mass_M_MIN'            :  1795.0 * MeV,
            'Mass_M_MAX'            :  1935.0 * MeV,
            'TisTosSpec'            :  "Hlt1.*Track.*Decision%TOS"
        }
        cutsForDsp2HHHKs = copy.deepcopy(cutsForDp2HHHKs)
        cutsForDsp2HHHKs.update({
            'Mass_M_MIN'            : 1895.0 * MeV,
            'Mass_M_MAX'            : 2035.0 * MeV,
        })

        cutsForLcpToLambda0KmKpPip = {
                'Trk_ALL_PT_MIN'           :  250 * MeV,
                'Trk_ALL_MIPCHI2DV_MIN'    :  3,
                'AM_34'                    : (493.677 + 139.5) * MeV,
                'AM_4'                     : (139.5) * MeV,
                'AM_MIN'                   :  2186 * MeV,
                'AM_MAX'                   :  2586 * MeV,
                'ASUMPT_MIN'               :  2000.0 * MeV,
                'ADOCA_MAX'                :  100.0 * mm,
                'ACHI2DOCA_MAX'            :  10.0,
                'VCHI2PDOF_MAX'            :  12.0,
                'BPVDIRA_MIN'              :  0.9998,
                'BPVLTIME_MIN'             :  0.1*picosecond,
                'Mass_M_MIN'               :  2206 * MeV,
                'Mass_M_MAX'               :  2348 * MeV,
                'TisTosSpec'               :  "Hlt1.*Track.*Decision%TOS"
                }
        cutsForLcpToLambda0KmPipPip = {
                'Trk_ALL_PT_MIN'           :  250 * MeV,
                'Trk_ALL_MIPCHI2DV_MIN'    :  3,
                'AM_34'                    : (139.5 + 139.5) * MeV,
                'AM_4'                     : (139.5) * MeV,
                'AM_MIN'                   :  2186 * MeV,
                'AM_MAX'                   :  2586 * MeV,
                'ASUMPT_MIN'               :  2000.0 * MeV,
                'ADOCA_MAX'                :  100.0 * mm,
                'ACHI2DOCA_MAX'            :  10.0,
                'VCHI2PDOF_MAX'            :  12.0,
                'BPVDIRA_MIN'              :  0.9998,
                'BPVLTIME_MIN'             :  0.1*picosecond,
                'Mass_M_MIN'               :  2206 * MeV,
                'Mass_M_MAX'               :  2348 * MeV,
                'TisTosSpec'               :  "Hlt1.*Track.*Decision%TOS"
                }

        cuts = {   'Dp2HHHKs'  : cutsForDp2HHHKs
                 , 'Dsp2HHHKs' : cutsForDsp2HHHKs
                 , 'LcpToLamKmKpPip'  : cutsForLcpToLambda0KmKpPip
                 , 'LcpToLamKmPipPip' : cutsForLcpToLambda0KmPipPip
               }

        return cuts
  
    def locallines(self):
        from Stages import MassFilter
        from Stages import CharmHadSharedKsLL, CharmHadSharedKsDD
        from Stages import SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi
        from Stages import LcpToLambda0KmKpPip, LcpToLambda0KmPipPip
        from Stages import DV4BCombiner, D2HHHKs_4BCombiner

        ## For tracibility, i want the combiners explictly made.
        ## Not made as shared instances, so line names will be inserted
        ## into the combiner names automatically.
        DpToKS0KmPipPip_KS0LL = DV4BCombiner( 'Comb'
                , decay = [ '[D+ -> K- pi+ pi+ KS0]cc' ]
                , inputs = [ SharedDetachedDpmChild_pi
                             ,  SharedDetachedDpmChild_K
                             , CharmHadSharedKsLL ]
                , shared = False
                , nickname = 'Dp2HHHKs' )

        DpToKS0KpPimPip_KS0LL = DpToKS0KmPipPip_KS0LL.clone( 'Comb'
                , decay = [ '[D+ -> K+ pi+ pi- KS0]cc' ] )

        DpToKS0KmKpPip_KS0LL = DpToKS0KmPipPip_KS0LL.clone( 'Comb'
                , decay =  [ '[D+ -> K- K+ pi+ KS0]cc' ] )

        DpToKS0KpKpPim_KS0LL = DpToKS0KmPipPip_KS0LL.clone( 'Comb'
                , decay =  [ '[D+ -> K+ K+ pi- KS0]cc' ] )

        DpToKS0PimPipPip_KS0LL = DpToKS0KmPipPip_KS0LL.clone( 'Comb'
                , decay = [ '[D+ -> pi+ pi+ pi- KS0]cc' ]
                , inputs = [ SharedDetachedDpmChild_pi, CharmHadSharedKsLL ])

        ## Combiners with DD KS rather than LL KS.
        DpToKS0KmPipPip_KS0DD = DpToKS0KmPipPip_KS0LL.clone( 'Comb'
                , inputs = [ SharedDetachedDpmChild_pi
                             ,  SharedDetachedDpmChild_K
                             , CharmHadSharedKsDD ] )
        
        DpToKS0KpPimPip_KS0DD = DpToKS0KmPipPip_KS0DD.clone( 'Comb'
                , decay = [ '[D+ -> K+ pi+ pi- KS0]cc' ] )

        DpToKS0KmKpPip_KS0DD = DpToKS0KmPipPip_KS0DD.clone( 'Comb'
                , decay =  [ '[D+ -> K- K+ pi+ KS0]cc' ] )

        DpToKS0KpKpPim_KS0DD = DpToKS0KmPipPip_KS0DD.clone( 'Comb'
                , decay =  [ '[D+ -> K+ K+ pi- KS0]cc' ] )

        DpToKS0PimPipPip_KS0DD = DpToKS0KmPipPip_KS0DD.clone( 'Comb'
                , decay = [ '[D+ -> pi+ pi+ pi- KS0]cc' ]
                , inputs = [ SharedDetachedDpmChild_pi, CharmHadSharedKsDD ])


        ## Combiners for D_s+ with LL KS.
        DspToKS0KmPipPip_KS0LL = DV4BCombiner( 'Comb'
                , decay = [ '[D_s+ -> K- pi+ pi+ KS0]cc' ]
                , inputs = [ SharedDetachedDpmChild_pi
                             ,  SharedDetachedDpmChild_K
                             , CharmHadSharedKsLL ]
                , shared = False
                , nickname = 'Dsp2HHHKs' )

        DspToKS0KpPimPip_KS0LL = DspToKS0KmPipPip_KS0LL.clone( 'Comb'
                , decay = [ '[D_s+ -> K+ pi+ pi- KS0]cc' ] )

        DspToKS0KmKpPip_KS0LL = DspToKS0KmPipPip_KS0LL.clone( 'Comb'
                , decay =  [ '[D_s+ -> K- K+ pi+ KS0]cc' ] )

        DspToKS0KpKpPim_KS0LL = DspToKS0KmPipPip_KS0LL.clone( 'Comb'
                , decay =  [ '[D_s+ -> K+ K+ pi- KS0]cc' ] )

        DspToKS0PimPipPip_KS0LL = DspToKS0KmPipPip_KS0LL.clone( 'Comb'
                , decay = [ '[D_s+ -> pi+ pi+ pi- KS0]cc' ]
                , inputs = [ SharedDetachedDpmChild_pi, CharmHadSharedKsLL ])

        ## Combiners for D_s+ with DD KS.
        DspToKS0KmPipPip_KS0DD = DspToKS0KmPipPip_KS0LL.clone( 'Comb'
                , inputs = [ SharedDetachedDpmChild_pi
                             ,  SharedDetachedDpmChild_K
                             , CharmHadSharedKsDD ] )
        
        DspToKS0KpPimPip_KS0DD = DspToKS0KmPipPip_KS0DD.clone( 'Comb'
                , decay = [ '[D_s+ -> K+ pi+ pi- KS0]cc' ] )

        DspToKS0KmKpPip_KS0DD = DspToKS0KmPipPip_KS0DD.clone( 'Comb'
                , decay =  [ '[D_s+ -> K- K+ pi+ KS0]cc' ] )

        DspToKS0KpKpPim_KS0DD = DspToKS0KmPipPip_KS0DD.clone( 'Comb'
                , decay =  [ '[D_s+ -> K+ K+ pi- KS0]cc' ] )

        DspToKS0PimPipPip_KS0DD = DspToKS0KmPipPip_KS0DD.clone( 'Comb'
                , decay = [ '[D_s+ -> pi+ pi+ pi- KS0]cc' ]
                , inputs = [ SharedDetachedDpmChild_pi, CharmHadSharedKsDD ])


        ## The stages dictionary should be a clear two-column list from
        ##   which the lines defined in this module can be directly read.
        stages = {
                  'Dp2KS0KmPipPip_KS0LLTurbo'   : [ DpToKS0KmPipPip_KS0LL ]
                , 'Dp2KS0KpPimPip_KS0LLTurbo'   : [ DpToKS0KpPimPip_KS0LL ]
                , 'Dp2KS0KmKpPip_KS0LLTurbo'    : [ DpToKS0KmKpPip_KS0LL ]
                , 'Dp2KS0KpKpPim_KS0LLTurbo'    : [ DpToKS0KpKpPim_KS0LL ]
                , 'Dp2KS0PimPipPip_KS0LLTurbo'  : [ DpToKS0PimPipPip_KS0LL ]

                , 'Dp2KS0KmPipPip_KS0DDTurbo'   : [ DpToKS0KmPipPip_KS0DD ]
                , 'Dp2KS0KpPimPip_KS0DDTurbo'   : [ DpToKS0KpPimPip_KS0DD ]
                , 'Dp2KS0KmKpPip_KS0DDTurbo'    : [ DpToKS0KmKpPip_KS0DD ]
                , 'Dp2KS0KpKpPim_KS0DDTurbo'    : [ DpToKS0KpKpPim_KS0DD ]
                , 'Dp2KS0PimPipPip_KS0DDTurbo'  : [ DpToKS0PimPipPip_KS0DD ]

                , 'Dsp2KS0KmPipPip_KS0LLTurbo'  : [ DspToKS0KmPipPip_KS0LL ]
                , 'Dsp2KS0KpPimPip_KS0LLTurbo'  : [ DspToKS0KpPimPip_KS0LL ]
                , 'Dsp2KS0KmKpPip_KS0LLTurbo'   : [ DspToKS0KmKpPip_KS0LL ]
                , 'Dsp2KS0KpKpPim_KS0LLTurbo'   : [ DspToKS0KpKpPim_KS0LL ]
                , 'Dsp2KS0PimPipPip_KS0LLTurbo' : [ DspToKS0PimPipPip_KS0LL ]

                , 'Dsp2KS0KmPipPip_KS0DDTurbo'  : [ DspToKS0KmPipPip_KS0DD ]
                , 'Dsp2KS0KpPimPip_KS0DDTurbo'  : [ DspToKS0KpPimPip_KS0DD ]
                , 'Dsp2KS0KmKpPip_KS0DDTurbo'   : [ DspToKS0KmKpPip_KS0DD ]
                , 'Dsp2KS0KpKpPim_KS0DDTurbo'   : [ DspToKS0KpKpPim_KS0DD ]
                , 'Dsp2KS0PimPipPip_KS0DDTurbo' : [ DspToKS0PimPipPip_KS0DD ]

                , 'Lcp2LamKmKpPip_Lam2PpPimTurbo'  : [ LcpToLambda0KmKpPip ]
                , 'Lcp2LamKmPipPip_Lam2PpPimTurbo' : [ LcpToLambda0KmPipPip ]
        }

        return stages
