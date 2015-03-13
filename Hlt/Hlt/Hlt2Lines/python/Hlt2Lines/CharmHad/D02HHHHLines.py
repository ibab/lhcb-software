# -*- coding: utf-8 -*-
## @file
#  Set of Hlt2-lines suitable for the study of charm decay channels with
#  four particles in the final state.
#
#  @author Maurizio MARTINELLI maurizio.martinelli@cern.ch
#  @author Simone STRACKA simone.stracka@cern.ch
#=============================================================================
""" Set of Hlt2-lines suitable for the study of charm decay channels with
    four particles in the final state.
"""
#=============================================================================
__author__  = "Maurizio MARTINELLI maurizio.martinelli@cern.ch ; Simone STRACKA simone.stracka@cern.ch"

from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CharmHadD02HHHHLines() :
    def localcuts(self) :
        cutsForDetachedHH = {
                'AM_MAX'                   :  2.1 * GeV,
                'ASUMPT_MIN'               :  0.0 * MeV,
                'ADOCA_MAX'                :  0.1 * mm,
                'VCHI2_MAX'                :  20.0,
                'BPVVD_MIN'                :  0.0,
                'BPVCORRM_MAX'             :  3.5 * GeV,
                'BPVVDCHI2_MIN'            :  20.0,
                }
        cutsForD2HHHH = {
                'AM_MAX'                   :  2.1 * GeV,
                'ASUMPT_MIN'               :  1.5 * GeV,
                'AMINDOCA_MAX'             :  0.1 * mm,
                'AMAXDOCA_MAX'             :  0.25 * mm,
                'Mass_M_MIN'               :  1790 * MeV,
                'Mass_M_MAX'               :  1940 * MeV,
                'VCHI2PDOF_MAX'            :  15.0,
                'BPVCORRM_MAX'             :  3.5 * GeV,
                'BPVVDCHI2_MIN'            :  36.0,
                'BPVVDR_MIN'               :  0.0,
                'BPVDIRA_MIN'              :  0.9999,
                'BPVIPCHI2_MAX'            :  42.0
                }
        cutsForD2HHHHWide = {
                'AM_MAX'                   :  2.1 * GeV,
                'ASUMPT_MIN'               :  1.5 * GeV,
                'AMINDOCA_MAX'             :  0.1 * mm,
                'AMAXDOCA_MAX'             :  0.25 * mm,
                'Mass_M_MIN'               :  1700 * MeV,
                'Mass_M_MAX'               :  2100 * MeV,
                'VCHI2PDOF_MAX'            :  15.0,
                'BPVCORRM_MAX'             :  3.5 * GeV,
                'BPVVDCHI2_MIN'            :  36.0,
                'BPVVDR_MIN'               :  0.0,
                'BPVDIRA_MIN'              :  0.9999,
                'BPVIPCHI2_MAX'            :  42.0
                }
        cutsForD2HHHHTag = {
                'TagVCHI2PDOF_MAX'         :  15.0,
                'DeltaM_AM_MIN'            :  0.0,
                'DeltaM_AM_MAX'            :  190.0 * MeV,
                'DeltaM_MIN'               :  0.0,
                'DeltaM_MAX'               :  170.0 * MeV
                }
        cuts = { # First the cuts for the 2-body combination
            'DetachedPiPi_forD2HHHH' : cutsForDetachedHH,
            'DetachedKPi_forD2HHHH'  : cutsForDetachedHH,
            'DetachedKK_forD2HHHH'   : cutsForDetachedHH,
            }
        for fs in ['PiPiPiPi', 'KPiPiPi', 'KKPiPi', 'KKKPi']:
            cuts.update( {
            # Now the combiner for the CPV lines
            'D02'+fs            : cutsForD2HHHH,
            'D02'+fs+'Tag'      : cutsForD2HHHHTag,
            'D02'+fs+'Wide'     : cutsForD2HHHHWide,
            'D02'+fs+'WideTag'  : cutsForD2HHHHTag,
            } )
        cuts.update( {
            # Now the combiner for the CPV lines
            'D02KPiPiPiFromPiPi'    : cutsForD2HHHH,
            'D02KPiPiPiFromPiPiWide': cutsForD2HHHHWide,
            'D02KKPiPiFromKPi'      : cutsForD2HHHH,
            'D02KKPiPiFromKPiWide'  : cutsForD2HHHHWide,
            'D02KKPiPiFromKK'       : cutsForD2HHHH,
            'D02KKPiPiFromKKWide'   : cutsForD2HHHHWide,
            'D02KKKPiFromKK'        : cutsForD2HHHH,
            'D02KKKPiFromKKWide'    : cutsForD2HHHHWide,
                    } )
        return cuts

  
    def locallines(self):
        from Stages import MassFilter, SoftTagInParticleFilter, NoPIDInParticleFilter
        from Stages import DetachedHHChild, D2HHHHCombiner, Dst2D0pi
        stages = {
            # First the 2-body line
            'DetachedPiPi_forD2HHHH' : [DetachedHHChild('DetachedPiPi_forD2HHHH',["[K*(892)+ -> pi+ pi+]cc","K*(892)0 -> pi+ pi-"])],
            'DetachedKPi_forD2HHHH'  : [DetachedHHChild('DetachedKPi_forD2HHHH', ["[K*(892)0 -> K+ pi-]cc"])],
            'DetachedKK_forD2HHHH'   : [DetachedHHChild('DetachedKK_forD2HHHH',  ["[K*(892)+ -> K+ K+]cc","K*(892)0 -> K+ K-"])],
            }
        for des in ['','Wide']:
            stages.update( {
                # Then the D2HHHH lines
                'D02PiPiPiPi'+des    : [MassFilter('D02PiPiPiPi'+des,
                                                   inputs=[D2HHHHCombiner('D02PiPiPiPi'+des,
                                                                        inputs=[DetachedHHChild('DetachedPiPi_forD2HHHH'),
                                                                                NoPIDInParticleFilter("SharedNoPIDDetachedD0Child_pi")],
                                                                        decay=['D0 -> K*(892)0 pi+ pi-',
                                                                               'D0 -> K*(892)+ pi- pi-', 'D0 -> K*(892)- pi+ pi+'])])],
                'D02KPiPiPi'+des     : [MassFilter('D02KPiPiPi'+des,
                                                   inputs=[D2HHHHCombiner('D02KPiPiPi'+des,
                                                                        inputs=[DetachedHHChild('DetachedKPi_forD2HHHH'),
                                                                                NoPIDInParticleFilter("SharedNoPIDDetachedD0Child_pi")],
                                                                        decay=['D0 -> K*(892)0 pi+ K-', 'D0 -> K*(892)~0 pi- K+']),
                                                           D2HHHHCombiner('D02KPiPiPiFromPiPi'+des,
                                                                        inputs=[DetachedHHChild('DetachedPiPi_forD2HHHH'),
                                                                                NoPIDInParticleFilter("SharedNoPIDDetachedD0Child_pi"),
                                                                                NoPIDInParticleFilter("SharedNoPIDDetachedD0Child_K")],
                                                                        decay=['D0 -> K*(892)+ pi- pi-', 'D0 -> K*(892)- pi+ pi+'])])],
                'D02KKPiPi'+des     : [MassFilter('D02KKPiPi'+des,
                                                   inputs=[D2HHHHCombiner('D02KKPiPi'+des,
                                                                        inputs=[DetachedHHChild('DetachedPiPi_forD2HHHH'),
                                                                                NoPIDInParticleFilter("SharedNoPIDDetachedD0Child_K")],
                                                                        decay=['D0 -> K*(892)0 K+ K-',
                                                                               'D0 -> K*(892)+ K- K-', 'D0 -> K*(892)- K+ K+']),
                                                           D2HHHHCombiner('D02KKPiPiFromKPi'+des,
                                                                        inputs=[DetachedHHChild('DetachedKPi_forD2HHHH'),
                                                                                NoPIDInParticleFilter("SharedNoPIDDetachedD0Child_pi"),
                                                                                NoPIDInParticleFilter("SharedNoPIDDetachedD0Child_K")],
                                                                        decay=['D0 -> K*(892)0 K- pi+', 'D0 -> K*(892)~0 pi- K+']),
                                                           D2HHHHCombiner('D02KKPiPiFromKK'+des,
                                                                        inputs=[DetachedHHChild('DetachedKK_forD2HHHH'),
                                                                                NoPIDInParticleFilter("SharedNoPIDDetachedD0Child_pi")],
                                                                        decay=['D0 -> K*(892)0 pi+ pi-',
                                                                               'D0 -> K*(892)+ pi- pi-', 'D0 -> K*(892)- pi+ pi+'])])],
                'D02KKKPi'+des     : [MassFilter('D02KKKPi'+des,
                                                   inputs=[D2HHHHCombiner('D02KKKPi'+des,
                                                                        inputs=[DetachedHHChild('DetachedKPi_forD2HHHH'),
                                                                                NoPIDInParticleFilter("SharedNoPIDDetachedD0Child_K")],
                                                                        decay=['D0 -> K*(892)0 K- K+', 'D0 -> K*(892)~0 K- K+']),
                                                           D2HHHHCombiner('D02KKKPiFromKK'+des,
                                                                        inputs=[DetachedHHChild('DetachedKK_forD2HHHH'),
                                                                                NoPIDInParticleFilter("SharedNoPIDDetachedD0Child_pi"),
                                                                                NoPIDInParticleFilter("SharedNoPIDDetachedD0Child_K")],
                                                                        decay=['D0 -> K*(892)0 K+ pi-', 'D0 -> K*(892)0 K- pi+',
                                                                               'D0 -> K*(892)+ K- pi-', 'D0 -> K*(892)- K+ pi+'])])],
                } )
            for fs in ['PiPiPiPi', 'KPiPiPi', 'KKPiPi', 'KKKPi']:
                stages.update( {
                              'D02'+fs+des+'Tag' : [Dst2D0pi('D02'+fs+des+'Tag', d0=stages['D02'+fs+des][0])],
                          } )
        return stages