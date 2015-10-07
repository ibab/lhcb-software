##
#  @author G. Ciezarek gregory.max.ciezarek@cern.ch
#          B. Hamilton   
#  @date 2015-28-08
#
#  Please contact the abovementioned responsibles before editing this file
# 
##


from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class XcMuXForTauLines(Hlt2LinesConfigurableUser):
    __slots__ = {'_stages' : {},
                 'Prescale' : {'Hlt2XcMuXForTauB2XcFakeMu' :   0.1},
                 'XcDaughter'  : {
                             'DTrk_ALL_PT_MIN'           : 200.0 * MeV
                            , 'DTrk_ALL_P_MIN'            : 5.0  * GeV
                            , 'DTrk_ALL_MIPCHI2DV_MIN'    : 9.0
                            , 'K_PIDK'    : 2 
                            , 'Pi_PIDK'    : 4 
                            , 'P_PIDp'    : 0 
                 
                            },
                 'Xc' : {
                              'Pair_AMINDOCA_MAX'        : 0.10 * mm
                            , 'MIN_D_CHILDPT'          : 800.0 * MeV
                            , 'D0_BPVVDCHI2_MIN'         : 25.0       # neuter
                            , 'D0_BPVDIRA_MIN'           : 0.999    # neuter
                            , 'D0_VCHI2PDOF_MAX'         : 10.0       # neuter
                            , 'D0_PT_MIN'                : 2000.0 * MeV
                            , 'D_SUMPT'    : 2500 * MeV
                            , 'D_AM_MIN'              : 1830.0 * MeV
                            , 'D_AM_MAX'              : 1910.0 * MeV
                            , 'Ds_AM_MIN'              : 1920.0 * MeV
                            , 'Ds_AM_MAX'              : 2010.0 * MeV
                            , 'Lc_AM_MIN'              : 2230.0 * MeV
                            , 'Lc_AM_MAX'              : 2330.0 * MeV
                            },
                 'Muon' : {
                             'Mu_IPCHI2'    : 16
                            },
                 'B' : {
                
                             'XcMu_VTXCHI2'        : 15
                            , 'XcMu_DOCA_MAX'        : 0.50 * mm
                            , 'XcMu_DIRA'        : 0.999
                            , 'XcMu_FDCHI2'        : 50
                            }
		   
                 }
    
    def stages(self, nickname=""):
        if hasattr(self, '_stages') and self._stages:
            if nickname:
                return self._stages[nickname]
            else:
                return self._stages


        from Stages import (B2XcMuComb,B2XcFakeMuComb)
        self._stages = { 
                   'B2XcMu' : [ B2XcMuComb],
                   'B2XcFakeMu' : [ B2XcFakeMuComb],
                  }
                  
        if nickname:
            return self._stages[nickname]
        else:
            return self._stages
    
    def __apply_configuration__(self):
        
        from HltLine.HltLine import Hlt2Line

        stages = self.stages()
        
        for (nickname, algos) in self.algorithms(stages).iteritems():
            Hlt2Line('XcMuXForTau' + nickname,
                     prescale = self.prescale,
                     algos = algos,
                     postscale = self.postscale)



