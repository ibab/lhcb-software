# -*- coding: utf-8 -*-
## @file
#  Set of Hlt2-lines suitable for the study of B-meson decay channels with
#  four particles in the final state.
#
#  This is specifically intended for B --> p,p,pbar,pbar where the
#  topo trigger lines may be less efficient due to the relatively
#  small Q-value; the combinatorics for this channel should be
#  acceptably small as the PID requirements are relatively tight.
#
#  @author Mike Sokoloff  msokolof@cern.ch
#=============================================================================
""" Set of Hlt2-lines suitable for the study of B-meson decay channels with
    four particles in the final state.
"""
#=============================================================================
__author__  = " Mike SOKOLOFF msokolof@cern.ch"

from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser
import copy

class BHadB02HHHHLines() :
    def localcuts(self) :
      return {

         # The kinematic cuts
         'B02HHHH':  {
                'TisTosSpec'               :  "Hlt1.*Track.*Decision%TOS",
                'Trk_ALL_PT_MIN'           :  250 * MeV,
                'Trk_ALL_MIPCHI2DV_MIN'    :  4,
                'AM_34'                    : (938.272 + 938.272) * MeV,
                'AM_4'                     : (938.272) * MeV,
                'AM_MIN'                   :  5150 * MeV,
                'AM_MAX'                   :  5500 * MeV,
                'ASUMPT_MIN'               :  2200 * MeV,
                'ADOCA_MAX'                :  100.0 * mm,
                'ACHI2DOCA_MAX'            :  10.0,
                'VCHI2PDOF_MAX'            :  12.0,
                'BPVDIRA_MIN'              :  0.99995,
                'BPVLTIME_MIN'             :  0.3*picosecond,
                'AMOM_MIN'                 :  45000 * MeV,
                'BPT_MIN'                  :  2500 * MeV,
                'BMOM_MIN'                 :  50000 * MeV,
                'BPVVDCHI2_MIN'            :  75,
         }

        ,'B2PPPbarPbarMass':  {
                'Mass_M_MIN'               :  5180 * MeV,
                'Mass_M_MAX'               :  5470 * MeV
         }

      }

    def locallines(self):
        from Stages import B02HHHHMass_B02PPPbarPbar

        ## The stages dictionary should be a clear two-column list from
        ##   which the lines defined in this module can be directly read.

        stages = {
                  'B02PpPpPmPm'          : [B02HHHHMass_B02PPPbarPbar]
        }

        return stages
