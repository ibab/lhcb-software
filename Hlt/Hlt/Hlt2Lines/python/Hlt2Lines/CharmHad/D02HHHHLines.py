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
        stages.update( {
              'D02PiPiPiPiWide' : [MassFilter('D02PiPiPiPiWide',inputs=[DV4BCombiner('D02PiPiPiPiWide', inputs=[SharedDetachedD0ToHHHHChild_pi], decay=['D0 -> pi+ pi+ pi- pi-'], shared=True)])],
              'D02KPiPiPiWide'  : [MassFilter('D02KPiPiPiWide',inputs=[DV4BCombiner('D02KPiPiPiWide', inputs=[SharedDetachedD0ToHHHHChild_pi,SharedDetachedD0ToHHHHChild_K], decay=['[D0 -> K- pi+ pi+ pi-]cc'], shared=True)])],
              'D02KKPiPiWide'   : [MassFilter('D02KKPiPiWide',inputs=[DV4BCombiner('D02KKPiPiWide', inputs=[SharedDetachedD0ToHHHHChild_pi,SharedDetachedD0ToHHHHChild_K], decay=['D0 -> K+ K- pi+ pi-'], shared=True)])],
              'D02KKKPiWide'    : [MassFilter('D02KKKPiWide',inputs=[DV4BCombiner('D02KKKPiWide', inputs=[SharedDetachedD0ToHHHHChild_pi,SharedDetachedD0ToHHHHChild_K], decay=['[D0 -> K- K- K+ pi+]cc'], shared=True)])],
            } )
        # Restrict to the nominal mass
        for fs in ['PiPiPiPi', 'KPiPiPi', 'KKPiPi', 'KKKPi']:
            stages.update( { 'D02'+fs : [MassFilter('D02'+fs,inputs=[stages['D02'+fs+'Wide'][0]])] } )
        # Then add the soft pion for the D*
        for des in ['','Wide']:
            for fs in ['PiPiPiPi', 'KPiPiPi', 'KKPiPi', 'KKKPi']:
                stages['D02'+fs+des+'Tag'] = [Dst2D0pi('D02'+fs+des+'Tag', d0=stages['D02'+fs+des][0])]
    
        # NO PID LINE
        stages['D02KPiPiPi_NoPid'] = [MassFilter('D02KPiPiPi_NoPid',inputs=[DV4BCombiner('D02KPiPiPi_NoPid', inputs=[SharedNoPIDDetachedChild_pi,SharedNoPIDDetachedChild_K], decay=['[D0 -> K- pi+ pi+ pi-]cc'])])]
        stages['D02KPiPiPi_NoPidTag'] = [Dst2D0pi('D02KPiPiPi_NoPidTag', d0=stages['D02KPiPiPi_NoPid'][0])]
        return stages
