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
        cutsForDpHHHKs_KsLL  = {
            'AM_MIN'                : 1755.0 * MeV,
            'AM_MAX'                : 2055.0 * MeV,
            'ACHI2DOCA_MAX'         : 20.0,
            'ASUMPT_MIN'            : 2.0 * GeV,
            'VCHI2PDOF_MAX'         : 20.0,
            'BPVLTIME_MIN'          : 0.2 * picosecond,
            'BPVDIRA_MIN'           : 0.999,
            'BPVIPCHI2_MAX'         : 20.0,
            'Mass_M_MIN'            : 1795.0 * MeV,
            'Mass_M_MAX'            : 1935.0 * MeV,
        }
        cutsForDpHHHKs_KsDD  = {
            'AM_MIN'                : 1755.0 * MeV,
            'AM_MAX'                : 2055.0 * MeV,
            'ACHI2DOCA_MAX'         : 20.0,
            'ASUMPT_MIN'            : 2.0 * GeV,
            'VCHI2PDOF_MAX'         : 20.0,
            'BPVLTIME_MIN'          : 0.2 * picosecond,
            'BPVDIRA_MIN'           : 0.999,
            'BPVIPCHI2_MAX'         : 20.0,
            'Mass_M_MIN'            : 1795.0 * MeV,
            'Mass_M_MAX'            : 1935.0 * MeV,
        }
        cutsForDsHHHKs_KsLL  = {
            'AM_MIN'                : 1755.0 * MeV,
            'AM_MAX'                : 2055.0 * MeV,
            'ACHI2DOCA_MAX'         : 20.0,
            'ASUMPT_MIN'            : 2.0 * GeV,
            'VCHI2PDOF_MAX'         : 20.0,
            'BPVLTIME_MIN'          : 0.1 * picosecond,
            'BPVDIRA_MIN'           : 0.999,
            'BPVIPCHI2_MAX'         : 20.0,
            'Mass_M_MIN'            : 1895.0 * MeV,
            'Mass_M_MAX'            : 2035.0 * MeV,
        }
        cutsForDsHHHKs_KsDD  = {
            'AM_MIN'                : 1755.0 * MeV,
            'AM_MAX'                : 2055.0 * MeV,
            'ACHI2DOCA_MAX'         : 20.0,
            'ASUMPT_MIN'            : 2.0 * GeV,
            'VCHI2PDOF_MAX'         : 20.0,
            'BPVLTIME_MIN'          : 0.1 * picosecond,
            'BPVDIRA_MIN'           : 0.999,
            'BPVIPCHI2_MAX'         : 20.0,
            'Mass_M_MIN'            : 1895.0 * MeV,
            'Mass_M_MAX'            : 2035.0 * MeV,
        }
        cuts = {}
        for fs in ['PiPiPi', 'KPiPi', 'KKPi']:
            cuts['Dp2'+fs+'Ks_KsLL'] = cutsForDpHHHKs_KsLL
            cuts['Dp2'+fs+'Ks_KsDD'] = cutsForDpHHHKs_KsDD
            cuts['Ds2'+fs+'Ks_KsLL'] = cutsForDsHHHKs_KsLL
            cuts['Ds2'+fs+'Ks_KsDD'] = cutsForDsHHHKs_KsDD
        return cuts
  
    def locallines(self):
        from Stages import MassFilter
        from Stages import SharedKsLL, SharedKsDD
        from Stages import SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi
        from Stages import D2HHHKs_4BCombiner
        decays = { 'PiPiPi': ['[D+ -> pi+ pi+ pi- KS0]cc'],
                   'KPiPi' : ['[D+ -> K+ pi+ pi- KS0]cc','[D+ -> K- pi+ pi+ KS0]cc'],
                   'KKPi'  : ['[D+ -> K+ K+ pi- KS0]cc','[D+ -> K- K+ pi+ KS0]cc'] }
        stages = {}
        for dps in ['Dp','Ds']:
            for fs in ['PiPiPi', 'KPiPi', 'KKPi']:
                stages[dps+'2'+fs+'Ks_KsLL'] = [MassFilter(dps+'2'+fs+'Ks_KsLL', inputs = [D2HHHKs_4BCombiner(dps+'2'+fs+'Ks_KsLL', decays[fs], [SharedKsLL, SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi])])]
                stages[dps+'2'+fs+'Ks_KsDD'] = [MassFilter(dps+'2'+fs+'Ks_KsDD', inputs = [D2HHHKs_4BCombiner(dps+'2'+fs+'Ks_KsDD', decays[fs], [SharedKsDD, SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi])])]
        
        return stages
