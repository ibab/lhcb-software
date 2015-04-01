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
        cutsForDetachedHHH_Pi0 = {
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
        cutsForHHHPi0  = {
            'AM_MIN'                : 1755.0 * MeV,
            'AM_MAX'                : 2055.0 * MeV,
            'APT_MIN'               : 2500.0,
            'ADAU1PT_MIN'           : 1700,
            'ADAU1BPVIPCHI2_MIN'    : 36,
            'ADAU2PT_MIN'           : 1700,
            'ADAU2BPVIPCHI2_MIN'    : 36,
            #'VCHI2PDOF_MAX'         : 20.0,
            #            'PT_MIN'                : 2.0 * GeV,
            'BPVLTIME_MIN'          : 0.3 * picosecond,
            'BPVIPCHI2_MAX'         : 20.0,
            'DMASS_MIN'             : 1795.0 * MeV,
            'DMASS_MAX'             : 2035.0 * MeV,
            'BPVDIRA_MIN'           : 0.999,
        }
        return {
          'DetachedPiPiPi_forD2HHHPi0' :  cutsForDetachedHHH_Pi0,
          'DetachedKPiPi_forD2HHHPi0'  :  cutsForDetachedHHH_Pi0,
          'DetachedKKPi_forD2HHHPi0'   :  cutsForDetachedHHH_Pi0,
          'DetachedKKK_forD2HHHPi0'    :  cutsForDetachedHHH_Pi0,
          'D2PiPiPiPi0'                :  cutsForHHHPi0,
          'D2KPiPiPi0'                 :  cutsForHHHPi0,
          'D2KKPiPi0'                  :  cutsForHHHPi0,
          'D2KKKPi0'                   :  cutsForHHHPi0,
        }
  
    def locallines(self):
        from Stages import DetachedHHHChild, AttachParticle, NeutralInParticleFilter
        inputs = {
            # First the detached HHH lines - KS LL
            'DetachedPiPiPi_forD2HHHPi0' : [DetachedHHHChild('DetachedPiPiPi_forD2HHHPi0', decay = ["[K*(892)+ -> pi+ pi+ pi-]cc"])],
            'DetachedKPiPi_forD2HHHPi0'  : [DetachedHHHChild('DetachedKPiPi_forD2HHHPi0', decay = ["[K*(892)+ -> K- pi+ pi+]cc","[K*(892)+ -> K+ pi- pi+]cc"])],
            'DetachedKKPi_forD2HHHPi0'   : [DetachedHHHChild('DetachedKKPi_forD2HHHPi0',decay = ["[K*(892)+ -> K+ K- pi+]cc","[K*(892)+ -> K+ K+ pi-]cc"]) ],
            'DetachedKKK_forD2HHHPi0'    : [DetachedHHHChild('DetachedKKK_forD2HHHPi0',decay = ["[K*(892)+ -> K+ K- K+]cc"]) ],
        }
        stages ={
          # Then we attach the neutral
            'D2PiPiPiPi0'   : [AttachParticle('D2PiPiPiPi0', "[D+ -> K*(892)+ pi0]cc",
                                             inputs = [ inputs['DetachedPiPiPi_forD2HHHPi0'][0],
                                                        NeutralInParticleFilter("SharedNeutralChild_pi0R")] )],
            'D2KPiPiPi0'    : [AttachParticle('D2KPiPiPi0', "[D+ -> K*(892)+ pi0]cc",
                                             inputs = [ inputs['DetachedKPiPi_forD2HHHPi0'][0],
                                                        NeutralInParticleFilter("SharedNeutralChild_pi0R")] )],
            'D2KKPiPi0'     : [AttachParticle('D2KKPiPi0', "[D+ -> K*(892)+ pi0]cc",
                                             inputs = [ inputs['DetachedKKPi_forD2HHHPi0'][0],
                                                        NeutralInParticleFilter("SharedNeutralChild_pi0R")] )],
            'D2KKKPi0'      : [AttachParticle('D2KKKPi0', "[D+ -> K*(892)+ pi0]cc",
                                             inputs = [ inputs['DetachedKKK_forD2HHHPi0'][0],
                                                        NeutralInParticleFilter("SharedNeutralChild_pi0R")] )],
        }
        return stages
