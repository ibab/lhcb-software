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
import copy

class CharmHadD02HHHHLines() :
    def localcuts(self) :
        # The kinematic cuts are in common between the lines
        cutsForD2HHHH = {
                'AM_MAX'                   :  2100 * MeV,
                'ASUMPT_MIN'               :  0.0 * MeV,
                'ACHI2DOCA_MAX'            :  16.0,
                'VCHI2PDOF_MAX'            :  15.0,
                'BPVDIRA_MIN'              :  0.9999,
                'BPVLTIME_MIN'             :  0.1*picosecond,
                'Mass_M_MIN'               :  1790 * MeV,
                'Mass_M_MAX'               :  1940 * MeV,
                }
        # modify the mass cuts to enlarge the mass window for prescaled control lines (are these really needed?)
        cutsForD2HHHHWide = copy.copy(cutsForD2HHHH)
        cutsForD2HHHHWide.update({
                'Mass_M_MIN'               :  1700 * MeV,
                'Mass_M_MAX'               :  2100 * MeV,
                })
        # Specific cuts for tagging
        cutsForD2HHHHTag = {
                'TagVCHI2PDOF_MAX'         :  15.0,
                'DeltaM_AM_MIN'            :  0.0,
                'DeltaM_AM_MAX'            :  190.0 * MeV,
                'DeltaM_MIN'               :  0.0,
                'DeltaM_MAX'               :  170.0 * MeV
                }
        cuts = {}
        for fs in ['PiPiPiPi', 'KPiPiPi', 'KKPiPi', 'KKKPi']:
            cuts.update( {
            # Now the combiner for the CPV lines
            'D02'+fs            : cutsForD2HHHH,
            'D02'+fs+'Wide'     : cutsForD2HHHHWide,
            'D02'+fs+'Tag'      : cutsForD2HHHHTag,
            'D02'+fs+'WideTag'  : cutsForD2HHHHTag,
            } )
        # No PID line
        cuts.update( {
            'D02KPiPiPi_NoPid'     : cutsForD2HHHH,
            'D02KPiPiPi_NoPidTag'  : cutsForD2HHHHTag
            } )
        return cuts

  
    def locallines(self):
        from Stages import MassFilter
        from Stages import SharedNoPIDDetachedChild_pi, SharedNoPIDDetachedChild_K
        from Stages import SharedDetachedD0ToHHHHChild_pi, SharedDetachedD0ToHHHHChild_K
        from Stages import DV4BCombiner, Dst2D0pi
        
        stages = {}
        # Create the nominal and wide mass combinations
        for des in ['','Wide']:
            # First for the D0
            stages.update( {
              'D02PiPiPiPi'+des : [MassFilter('D02PiPiPiPi'+des,inputs=[DV4BCombiner('D02PiPiPiPi'+des, inputs=[SharedDetachedD0ToHHHHChild_pi], decay=['D0 -> pi+ pi+ pi- pi-'])])],
              'D02KPiPiPi'+des  : [MassFilter('D02KPiPiPi'+des,inputs=[DV4BCombiner('D02KPiPiPi'+des, inputs=[SharedDetachedD0ToHHHHChild_pi,SharedDetachedD0ToHHHHChild_K], decay=['[D0 -> K- pi+ pi+ pi-]cc'])])],
              'D02KKPiPi'+des   : [MassFilter('D02KKPiPi'+des,inputs=[DV4BCombiner('D02KKPiPi'+des, inputs=[SharedDetachedD0ToHHHHChild_pi,SharedDetachedD0ToHHHHChild_K], decay=['D0 -> K+ K- pi+ pi-'])])],
              'D02KKKPi'+des    : [MassFilter('D02KKKPi'+des,inputs=[DV4BCombiner('D02KKKPi'+des, inputs=[SharedDetachedD0ToHHHHChild_pi,SharedDetachedD0ToHHHHChild_K], decay=['[D0 -> K- K- K+ pi+]cc'])])],
                          } )
            # Then add the soft pion for the D*
            for fs in ['PiPiPiPi', 'KPiPiPi', 'KKPiPi', 'KKKPi']:
                stages['D02'+fs+des+'Tag'] = [Dst2D0pi('D02'+fs+des+'Tag', d0=stages['D02'+fs+des][0])]
    
        # NO PID LINE
        stages['D02KPiPiPi_NoPid'] = [MassFilter('D02KPiPiPi_NoPid',inputs=[DV4BCombiner('D02KPiPiPi_NoPid', inputs=[SharedNoPIDDetachedChild_pi,SharedNoPIDDetachedChild_K], decay=['[D0 -> K- pi+ pi+ pi-]cc'])])]
        stages['D02KPiPiPi_NoPidTag'] = [Dst2D0pi('D02KPiPiPi_NoPidTag', d0=stages['D02KPiPiPi_NoPid'][0])]
        return stages
