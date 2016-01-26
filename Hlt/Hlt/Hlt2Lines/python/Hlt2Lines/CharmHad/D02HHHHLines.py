# -*- coding: utf-8 -*-
## @file
#  Set of Hlt2-lines suitable for the study of charm decay channels with
#  four particles in the final state.
#
#  @author Maurizio MARTINELLI maurizio.martinelli@cern.ch
#  @author Simone STRACKA simone.stracka@cern.ch
#  @author Mike Sokoloff  msokolof@cern.ch
#=============================================================================
""" Set of Hlt2-lines suitable for the study of charm decay channels with
    four particles in the final state.
"""
#=============================================================================
__author__  = "Maurizio MARTINELLI maurizio.martinelli@cern.ch ; Simone STRACKA simone.stracka@cern.ch, Mike SOKOLOFF msokolof@cern.ch"

from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser
import copy

class CharmHadD02HHHHLines() :
    def localcuts(self) :
        ## The configuration dictionaries for the lines used to be generated
        ## in a loop in which keys/line names were assembled from components.
        ## This led to very compact code, but it also made it difficult to
        ## find things with simple search tools.
        ## Further, there seem to be only one variant of the dictionary.
        ## Having one configuration dictionary per combiner does make it
        ## possible to apply different configurations for each of the lines
        ## in HltSettings.  However, it is not clear that it would be desirable
        ## to do so.  Until it is clear that we need different configurations
        ## for the combiners of the lines, use just two configuration
        ## dictionaries to configure all of the lines.  (P.S.)

        # The kinematic cuts are in common between the lines
        cutsForD2HHHHComb = {
                'TisTosSpec'               :  [], # Empty line for not applying TisTos
                'Trk_ALL_PT_MIN'           :  250 * MeV,
                'Trk_ALL_MIPCHI2DV_MIN'    :  3,
                'AM_34'                    : (139.5 + 139.5) * MeV,
                'AM_4'                     : (139.5) * MeV,
                'AM_MIN'                   :  1700 * MeV,
                'AM_MAX'                   :  2100 * MeV,
                'ASUMPT_MIN'               :  1800 * MeV,
                'ADOCA_MAX'                :  100.0 * mm,
                'ACHI2DOCA_MAX'            :  10.0,
                'VCHI2PDOF_MAX'            :  12.0,
                'BPVDIRA_MIN'              :  0.9998,
                'BPVLTIME_MIN'             :  0.1*picosecond,
                'Mass_M_MIN'               :  1700 * MeV,
                'Mass_M_MAX'               :  2100 * MeV,
                'AMOM_MIN'                 :  25000 * MeV,
                'DPT_MIN'                  :  2000 * MeV,
                'DMOM_MIN'                 :  30000 * MeV,
                'BPVVDCHI2_MIN'            :  25,
        }

        massForD2HHHH = {
                'Mass_M_MIN'               :  1790 * MeV,
                'Mass_M_MAX'               :  1940 * MeV
        }

        cutsForD2HHHHTag = {
                'TagVCHI2PDOF_MAX'         :  15.0,
                'DeltaM_AM_MIN'            :  0.0,
                'DeltaM_AM_MAX'            :  190.0 * MeV,
                'DeltaM_MIN'               :  0.0,
                'DeltaM_MAX'               :  170.0 * MeV
        }
        cutsForMuTag = {
                                        'DeltaM_AM_MIN'            :  100.0 * MeV,
                                        'DeltaM_MIN'               :  100.0 * MeV,
                                        'DeltaM_AM_MAX'            :  5000. * MeV,
                                        'DeltaM_MAX'               :  5000. * MeV,
                                        'TagVCHI2PDOF_MAX'         :  25.0
        }
        cuts = {   'D02HHHH'        : cutsForD2HHHHComb
                 , 'D02HHHHMass'    : massForD2HHHH
                 , 'DstToD02HHHH'   : cutsForD2HHHHTag
                 , 'DSTP_HHHH_MUTAG_CPV' : cutsForMuTag
        }

        return cuts


    def locallines(self):
        from Stages import MassFilter
        from Stages import SharedDetachedD0ToHHHHChild_pi, SharedDetachedD0ToHHHHChild_K, SharedSoftTagChild_pi
        from Stages import D02HHHH_D02PiPiPiPi, D02HHHH_D02KPiPiPi
        from Stages import D02HHHH_D02KKPiPi, D02HHHH_D02KKKPi
        from Stages import D02HHHHMass_D02PiPiPiPi, D02HHHHMass_D02KPiPiPi
        from Stages import D02HHHHMass_D02KKPiPi, D02HHHHMass_D02KKKPi
        from Stages import DstToD02HHHH_D02CFKPiPiPi
        from Stages import DV4BCombiner, TagDecay
        from Stages import SharedSoftTagChild_mu

        ## D*+ -> D0 pi+ reconstruction on D0 with wide mass windows
        DstToD02HHHH_D02PiPiPiPiWideTag = TagDecay('DstpComb'
                , decay = ["D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-"]
                , inputs = [ D02HHHH_D02PiPiPiPi, SharedSoftTagChild_pi ]
                , nickname = 'DstToD02HHHH')

        DstToD02HHHH_D02KKPiPiWideTag = DstToD02HHHH_D02PiPiPiPiWideTag.clone('DstpComb'
                , inputs = [ D02HHHH_D02KKPiPi, SharedSoftTagChild_pi ] )

        DstToD02HHHH_D02CFKPiPiPiWideTag = TagDecay('DstpComb'
                , decay = ["[D*(2010)+ -> D0 pi+]cc"]
                , inputs = [D02HHHH_D02KPiPiPi, SharedSoftTagChild_pi]
                , nickname = 'DstToD02HHHH')
        DstToD02HHHH_D02DCSKPiPiPiWideTag = DstToD02HHHH_D02CFKPiPiPiWideTag.clone('DstpComb'
                , decay = ["[D*(2010)- -> D0 pi-]cc"])

        DstToD02HHHH_D02CFKKKPiWideTag = DstToD02HHHH_D02CFKPiPiPiWideTag.clone('DstpComb'
                , inputs = [D02HHHH_D02KKKPi, SharedSoftTagChild_pi])
        DstToD02HHHH_D02DCSKKKPiWideTag = DstToD02HHHH_D02CFKKKPiWideTag.clone('DstpComb'
                , decay = ["[D*(2010)- -> D0 pi-]cc"])


        ## D*+ -> D0 pi+ reconstruction
        ## The combiner for the Cabibbo-favored D*+ -> D0(K- pi- pi+ pi+) pi+
        ##   is shared with a spectroscopy line in CharmSpectroscopyLines.py.
        ##   Its definition is moved to Stages.py for sharing.
        DstToD02HHHH_D02PiPiPiPiTag = TagDecay('DstpComb'
                , decay = ["D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-"]
                , inputs = [ D02HHHHMass_D02PiPiPiPi, SharedSoftTagChild_pi ]
                , nickname = 'DstToD02HHHH')

        DstToD02HHHH_D02KKPiPiTag = DstToD02HHHH_D02PiPiPiPiTag.clone('DstpComb'
                , inputs = [ D02HHHHMass_D02KKPiPi, SharedSoftTagChild_pi ] )

        DstToD02HHHH_D02DCSKPiPiPiTag = TagDecay('DstpComb'
                , decay = ["[D*(2010)- -> D0 pi-]cc"]
                , inputs = [D02HHHHMass_D02KPiPiPi, SharedSoftTagChild_pi]
                , nickname = 'DstToD02HHHH')

        DstToD02HHHH_D02CFKKKPiTag = TagDecay('DstpComb'
                , decay = ["[D*(2010)+ -> D0 pi+]cc"]
                , inputs = [D02HHHHMass_D02KKKPi, SharedSoftTagChild_pi]
                , nickname = 'DstToD02HHHH')

        DstToD02HHHH_D02DCSKKKPiTag = DstToD02HHHH_D02CFKKKPiTag.clone('DstpComb'
                , decay = ["[D*(2010)- -> D0 pi-]cc"])


## Add muon-tagged versions of the D*+/- lines following the form used
## for MuTag_DstpMu_Dstp2D0Pip_D02KpPim in D02HHLines.py  [150825, mds]
        MuTag_Dstp_D02PimPimPipPip = TagDecay('DSTP_HHHH_MUTAG_CPV'
                        , ["[B0 -> D*(2010)+ mu-]cc","[Delta(1905)++ -> D*(2010)+ mu+]cc"]
                        , inputs = [ DstToD02HHHH_D02PiPiPiPiTag, SharedSoftTagChild_mu ]
                        , ReFitPVs = True)
        MuTag_Dstp_D02KmKpPimPip = TagDecay('DSTP_HHHH_MUTAG_CPV'
                        , ["[B0 -> D*(2010)+ mu-]cc","[Delta(1905)++ -> D*(2010)+ mu+]cc"]
                        , inputs = [ DstToD02HHHH_D02KKPiPiTag, SharedSoftTagChild_mu ]
                        , ReFitPVs = True)
        MuTag_Dstp_D02KmPimPipPip = TagDecay('DSTP_HHHH_MUTAG_CPV'
                        , ["[B0 -> D*(2010)+ mu-]cc","[Delta(1905)++ -> D*(2010)+ mu+]cc"]
                        , inputs = [ DstToD02HHHH_D02CFKPiPiPi, SharedSoftTagChild_mu ]
                        , ReFitPVs = True)
        MuTag_Dstp_D02KpPimPimPip = TagDecay('DSTP_HHHH_MUTAG_CPV'
                        , ["[B0 -> D*(2010)+ mu-]cc","[Delta(1905)++ -> D*(2010)+ mu+]cc"]
                        , inputs = [ DstToD02HHHH_D02DCSKPiPiPiTag, SharedSoftTagChild_mu ]
                        , ReFitPVs = True)
        MuTag_Dstp_D02KmKmKpPip = TagDecay('DSTP_HHHH_MUTAG_CPV'
                        , ["[B0 -> D*(2010)+ mu-]cc","[Delta(1905)++ -> D*(2010)+ mu+]cc"]
                        , inputs = [ DstToD02HHHH_D02CFKKKPiTag, SharedSoftTagChild_mu ]
                        , ReFitPVs = True)
        MuTag_Dstp_D02KmKpKpPim = TagDecay('DSTP_HHHH_MUTAG_CPV'
                        , ["[B0 -> D*(2010)+ mu-]cc","[Delta(1905)++ -> D*(2010)+ mu+]cc"]
                        , inputs = [ DstToD02HHHH_D02DCSKKKPiTag, SharedSoftTagChild_mu ]
                        , ReFitPVs = True)


        ## The stages dictionary should be a clear two-column list from
        ##   which the lines defined in this module can be directly read.

        stages = {}
        # Create the nominal and wide mass combinations
        stages = {
                ## Main lines (nominal mass window)
                  'D02PimPimPipPipTurbo' : [D02HHHHMass_D02PiPiPiPi]
                , 'D02KmPimPipPipTurbo'  : [D02HHHHMass_D02KPiPiPi]
                , 'D02KmKpPimPipTurbo'   : [D02HHHHMass_D02KKPiPi]
                , 'D02KmKmKpPipTurbo'    : [D02HHHHMass_D02KKKPi]
                , 'Dstp2D0Pip_D02PimPimPipPipTurbo' : [DstToD02HHHH_D02PiPiPiPiTag]
                , 'Dstp2D0Pip_D02KmKpPimPipTurbo'   : [DstToD02HHHH_D02KKPiPiTag]
                , 'Dstp2D0Pip_D02KmPimPipPipTurbo'  : [DstToD02HHHH_D02CFKPiPiPi]
                , 'Dstp2D0Pip_D02KpPimPimPipTurbo'  : [DstToD02HHHH_D02DCSKPiPiPiTag]
                , 'Dstp2D0Pip_D02KmKmKpPipTurbo'    : [DstToD02HHHH_D02CFKKKPiTag]
                , 'Dstp2D0Pip_D02KmKpKpPimTurbo'    : [DstToD02HHHH_D02DCSKKKPiTag]
                , 'MuTag_Dstp_D02PimPimPipPipTurbo' : [MuTag_Dstp_D02PimPimPipPip]
                , 'MuTag_Dstp_D02KmKpPimPipTurbo'   : [MuTag_Dstp_D02KmKpPimPip]
                , 'MuTag_Dstp_D02KmPimPipPipTurbo'  : [MuTag_Dstp_D02KmPimPipPip]
                , 'MuTag_Dstp_D02KpPimPimPipTurbo'  : [MuTag_Dstp_D02KpPimPimPip]
                , 'MuTag_Dstp_D02KmKmKpPipTurbo'    : [MuTag_Dstp_D02KmKmKpPip]
                , 'MuTag_Dstp_D02KmKpKpPimTurbo'    : [MuTag_Dstp_D02KmKpKpPim]

                ## Wide mass lines (plan for obsolescence)
                , 'D02PimPimPipPip_WideTurbo' : [D02HHHH_D02PiPiPiPi]
                , 'D02KmPimPipPip_WideTurbo'  : [D02HHHH_D02KPiPiPi]
                , 'D02KmKpPimPip_WideTurbo'   : [D02HHHH_D02KKPiPi]
                , 'D02KmKmKpPip_WideTurbo'    : [D02HHHH_D02KKKPi]
                , 'Dstp2D0Pip_D02PimPimPipPip_WideTurbo' : [DstToD02HHHH_D02PiPiPiPiWideTag]
                , 'Dstp2D0Pip_D02KmKpPimPip_WideTurbo'   : [DstToD02HHHH_D02KKPiPiWideTag]
                , 'Dstp2D0Pip_D02KmPimPipPip_WideTurbo'  : [DstToD02HHHH_D02CFKPiPiPiWideTag]
                , 'Dstp2D0Pip_D02KpPimPimPip_WideTurbo'  : [DstToD02HHHH_D02DCSKPiPiPiWideTag]
                , 'Dstp2D0Pip_D02KmKmKpPip_WideTurbo'    : [DstToD02HHHH_D02CFKKKPiWideTag]
                , 'Dstp2D0Pip_D02KmKpKpPim_WideTurbo'    : [DstToD02HHHH_D02DCSKKKPiWideTag]
        }

        return stages
