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
        cutsForDetachedHHH_KsLL = {
            'Trk_ALL_MIPCHI2DV_MIN'   : 5.0,
            'AM_MIN'                  : 0.0,
            'AM_MAX'                  : 1.5 * GeV,
            'ASUMPT_MIN'              : 1.3 * GeV,
            'Trk_1OF3_MIPCHI2DV_MIN'  : 5.0, # This can be tuned for reducing retention
            'Trk_2OF3_MIPCHI2DV_MIN'  : 5.0, # This can be tuned for reducing retention
            'VCHI2PDOF_MAX'           : 20.0,
            'BPVDIRA_MIN'             : 0.999,
            'BPVVDCHI2_MIN'           : 20.0,
            'BPVLTIME_MIN'            : 0.3 * picosecond,
        }
        cutsForDetachedHHH_KsDD = {
            'Trk_ALL_MIPCHI2DV_MIN'   : 5.0,
            'AM_MIN'                  : 0.0,
            'AM_MAX'                  : 1.5 * GeV,
            'ASUMPT_MIN'              : 1.3 * GeV,
            'Trk_1OF3_MIPCHI2DV_MIN'  : 12.5,
            'Trk_2OF3_MIPCHI2DV_MIN'  : 5.0, # This can be tuned for reducing retention
            'VCHI2PDOF_MAX'           : 20.0,
            'BPVDIRA_MIN'             : 0.999,
            'BPVVDCHI2_MIN'           : 20.0,
            'BPVLTIME_MIN'            : 0.3 * picosecond,
        }
        cutsForKsHHH_KsLL  = {
            'AM_MIN'                : 1755.0 * MeV,
            'AM_MAX'                : 2055.0 * MeV,
            'VCHI2PDOF_MAX'         : 20.0,
            'PT_MIN'                : 2.0 * GeV,
            'BPVLTIME_MIN'          : 0.3 * picosecond,
            'BPVIPCHI2_MAX'         : 20.0,
            'Mass_M_MIN'              : 1795.0 * MeV,
            'Mass_M_MAX'              : 2035.0 * MeV,
            'BPVDIRA_MIN'           : 0.999,
        }
        cutsForKsHHH_KsDD  = {
            'AM_MIN'                : 1755.0 * MeV,
            'AM_MAX'                : 2055.0 * MeV,
            'VCHI2PDOF_MAX'         : 20.0,
            'PT_MIN'                : 2.0 * GeV,
            'BPVLTIME_MIN'          : 0.3 * picosecond,
            'BPVIPCHI2_MAX'         : 20.0,
            'Mass_M_MIN'              : 1795.0 * MeV,
            'Mass_M_MAX'              : 2035.0 * MeV,
            'BPVDIRA_MIN'           : 0.999,
        }
        return {
          'DetachedPiPiPi_forD2HHHKsLL' :  cutsForDetachedHHH_KsLL,
          'DetachedPiPiPi_forD2HHHKsDD' :  cutsForDetachedHHH_KsDD,
          'DetachedKPiPi_forD2HHHKsLL'  :  cutsForDetachedHHH_KsLL,
          'DetachedKPiPi_forD2HHHKsDD'  :  cutsForDetachedHHH_KsDD,
          'DetachedKKPi_forD2HHHKsLL'   :  cutsForDetachedHHH_KsLL,
          'DetachedKKPi_forD2HHHKsDD'   :  cutsForDetachedHHH_KsDD,
          'D2KsPiPiPi_KsLL'             :  cutsForKsHHH_KsLL,
          'D2KsPiPiPi_KsDD'             :  cutsForKsHHH_KsDD,
          'D2KsKPiPi_KsLL'              :  cutsForKsHHH_KsLL,
          'D2KsKPiPi_KsDD'              :  cutsForKsHHH_KsDD,
          'D2KsKKPi_KsLL'               :  cutsForKsHHH_KsLL,
          'D2KsKKPi_KsDD'               :  cutsForKsHHH_KsDD,
        }
  
    def locallines(self):
        from Stages import MassFilter, NoPIDInParticleFilter
        from Stages import DetachedHHHChild, D2KsHHH_KSLL, D2KsHHH_KSDD
        stages = {
            # First the detached HHH lines - KS LL
            'DetachedPiPiPi_forD2HHHKsLL' : [DetachedHHHChild('DetachedPiPiPi_forD2HHHKsLL', decay = ["[K*(892)+ -> pi+ pi+ pi-]cc"])],
            'DetachedKPiPi_forD2HHHKsLL' :  [DetachedHHHChild('DetachedKPiPi_forD2HHHKsLL', decay = ["[K*(892)+ -> K- pi+ pi+]cc","[K*(892)+ -> K+ pi- pi+]cc"])],
            'DetachedKKPi_forD2HHHKsLL'   : [DetachedHHHChild('DetachedKKPi_forD2HHHKsLL',decay = ["[K*(892)+ -> K+ K- pi+]cc","[K*(892)+ -> K+ K+ pi-]cc"]) ],
          # KS DD
            'DetachedPiPiPi_forD2HHHKsDD' : [DetachedHHHChild('DetachedPiPiPi_forD2HHHKsDD',decay = ["[K*(892)+ -> pi+ pi+ pi-]cc"]) ],
            'DetachedKPiPi_forD2HHHKsDD'  : [DetachedHHHChild('DetachedKPiPi_forD2HHHKsDD',decay = ["[K*(892)+ -> K- pi+ pi+]cc", "[K*(892)+ -> K+ pi- pi+]cc"]) ],
            'DetachedKKPi_forD2HHHKsDD'   : [DetachedHHHChild('DetachedKKPi_forD2HHHKsDD',decay = ["[K*(892)+ -> K+ K- pi+]cc","[K*(892)+ -> K+ K+ pi-]cc"]) ],
        }
        stages.update( {
          # First the KS LL lines
            'D2KsPiPiPi_KsLL'   : [MassFilter('D2KsPiPiPi_KsLL', inputs = [D2KsHHH_KSLL('D2KsPiPiPi_KsLL', stages['DetachedPiPiPi_forD2HHHKsLL'][0])])],
            'D2KsKPiPi_KsLL'    : [MassFilter('D2KsKPiPi_KsLL',  inputs = [D2KsHHH_KSLL('D2KsKPiPi_KsLL',  stages['DetachedKPiPi_forD2HHHKsLL' ][0])])],
            'D2KsKKPi_KsLL'     : [MassFilter('D2KsKKPi_KsLL',   inputs = [D2KsHHH_KSLL('D2KsKKPi_KsLL',   stages['DetachedKKPi_forD2HHHKsLL'  ][0])])],
          # Then the KS DD lines
            'D2KsPiPiPi_KsDD'   : [MassFilter('D2KsPiPiPi_KsDD', inputs = [D2KsHHH_KSDD('D2KsPiPiPi_KsDD', stages['DetachedPiPiPi_forD2HHHKsDD'][0])])],
            'D2KsKPiPi_KsDD'    : [MassFilter('D2KsKPiPi_KsDD',  inputs = [D2KsHHH_KSDD('D2KsKPiPi_KsDD',  stages['DetachedKPiPi_forD2HHHKsDD' ][0])])],
            'D2KsKKPi_KsDD'     : [MassFilter('D2KsKKPi_KsDD',   inputs = [D2KsHHH_KSDD('D2KsKKPi_KsDD',   stages['DetachedKKPi_forD2HHHKsDD'  ][0])])],
        } )
        return stages
