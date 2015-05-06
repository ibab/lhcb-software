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

from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CharmHadD2HHHKsLines() :
    def localcuts(self) :
        cutsForD2HHHKs = {
            'Trk_ALL_PT_MIN'        :  300 * MeV,
            'Trk_ALL_MIPCHI2DV_MIN' :  3,
            'AM_34'                 : (139.5 + 139.5) * MeV,
            'AM_4'                  : (139.5) * MeV,
            'AM_MIN'                :  1755.0 * MeV,
            'AM_MAX'                :  2055.0 * MeV,
            'ACHI2DOCA_MAX'         :  10.0,
            'ASUMPT_MIN'            :  2.0 * GeV,
            'VCHI2PDOF_MAX'         :  12.0,
            'BPVLTIME_MIN'          :  0.2 * picosecond,
            'BPVDIRA_MIN'           :  0.999,
            'BPVIPCHI2_MAX'         :  20.0,
            'Mass_M_MIN'            :  1795.0 * MeV,
            'Mass_M_MAX'            :  1935.0 * MeV,
        }
        cuts = {}
        for ks in ['LL','DD']:
            for fs in ['PiPiPi', 'KPiPi', 'KKPi']:
                cuts['D2'+fs+'Ks_Ks'+ks] = cutsForD2HHHKs
                for dps in ['Dp','Ds']:
                    cuts[dps+'2'+fs+'Ks_Ks'+ks] = cutsForD2HHHKs
                    if dps == 'Ds':
                        cuts[dps+'2'+fs+'Ks_Ks'+ks].update({
                                'Mass_M_MIN'            : 1895.0 * MeV,
                                'Mass_M_MAX'            : 2035.0 * MeV,
                        })
        return cuts
  
    def locallines(self):
        from Stages import MassFilter
        from Stages import CharmHadSharedKsLL, CharmHadSharedKsDD
        from Stages import SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi
        from Stages import DV4BCombiner, D2HHHKs_4BCombiner
        decays = { 'PiPiPi': ['[D+ -> pi+ pi+ pi- KS0]cc'],
                   'KPiPi' : ['[D+ -> K+ pi+ pi- KS0]cc','[D+ -> K- pi+ pi+ KS0]cc'],
                   'KKPi'  : ['[D+ -> K+ K+ pi- KS0]cc','[D+ -> K- K+ pi+ KS0]cc'] }
        stages = {}
        sharedKs = {'LL': CharmHadSharedKsLL, 'DD': CharmHadSharedKsLL}
        for fs in ['PiPiPi', 'KPiPi', 'KKPi']:
            for ks in ['LL','DD']:
                stages['D2'+fs+'Ks_Ks'+ks] = [DV4BCombiner('D2'+fs+'Ks_Ks'+ks, decays[fs], [sharedKs[ks], SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi])]
                for dps in ['Dp','Ds']:
                    stages[dps+'2'+fs+'Ks_Ks'+ks] = [MassFilter(dps+'2'+fs+'Ks_Ks'+ks, inputs = [stages['D2'+fs+'Ks_Ks'+ks][0]])]
        return stages
