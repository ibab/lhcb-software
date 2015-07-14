##
#  @author G. Ciezarek gregory.max.ciezarek@cern.ch
#          B. Hamilton   
#  @date 2015-14-07
#
#  Please contact the abovementioned responsibles before editing this file
# 
##


from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class XcMuXForTauLines(Hlt2LinesConfigurableUser):
    __slots__ = {'_stages' : {},
                 'Prescale' : {},
                 'Common' : {
                              'Pair_AMINDOCA_MAX'        : 0.10 * mm
                            , 'MIN_D_CHILDPT'          : 800.0 * MeV
                            , 'D0_BPVVDCHI2_MIN'         : 25.0       # neuter
                            , 'D0_BPVDIRA_MIN'           : 0.999    # neuter
                            , 'D0_VCHI2PDOF_MAX'         : 10.0       # neuter
                            , 'D0_PT_MIN'                : 2000.0 * MeV
                            , 'DTrk_ALL_PT_MIN'           : 200.0 * MeV
                            , 'DTrk_ALL_P_MIN'            : 5.0  * GeV
                            , 'DTrk_ALL_MIPCHI2DV_MIN'    : 9.0
                            , 'XcMu_DOCA_MAX'        : 0.50 * mm
                            , 'XcMu_VTXCHI2'        : 15
                            , 'XcMu_DIRA'        : 0.999
                            , 'XcMu_FDCHI2'        : 50
                            , 'Mu_IPCHI2'    : 16
                            , 'D_IPCHI2'    : 5
                            , 'D_SUMPT'    : 2500 * MeV
                            , 'K_PIDK'    : 2 
                            , 'Pi_PIDK'    : 4 
                            , 'P_PIDp'    : 0 
                            , 'D_AM_MIN'              : 1830.0 * MeV
                            , 'D_AM_MAX'              : 1910.0 * MeV
                            , 'Ds_AM_MIN'              : 1920.0 * MeV
                            , 'Ds_AM_MAX'              : 2010.0 * MeV
                            , 'Lc_AM_MIN'              : 2230.0 * MeV
                            , 'Lc_AM_MAX'              : 2330.0 * MeV
                            }
		   
                 }
    
    def stages(self, nickname=""):
        if hasattr(self, '_stages') and self._stages:
            if nickname:
                return self._stages[nickname]
            else:
                return self._stages
        
        from Stages import (
                                               B2D0KPiMuComb,B2D0K3PiMuComb,B2DplusKPiPiMuComb,B2DsKKPiMuComb,B2LcPKPiMuComb,B2D0KPiFakeMuComb,B2D0K3PiFakeMuComb,B2DplusKPiPiFakeMuComb,B2DsKKPiFakeMuComb,B2LcPKPiFakeMuComb)
        self._stages = { 
                   'B2D0KPiMu' : [ B2D0KPiMuComb],
                   'B2D0K3PiMu' : [ B2D0K3PiMuComb],
                   'B2DplusKPiPiMu' : [ B2DplusKPiPiMuComb],
                   'B2DsKKPiMu' : [ B2DsKKPiMuComb],
                   'B2LcPKPiMu' : [ B2LcPKPiMuComb],
                   'B2D0KPiFakeMu' : [ B2D0KPiFakeMuComb],
                   'B2D0K3PiFakeMu' : [ B2D0K3PiFakeMuComb],
                   'B2DplusKPiPiFakeMu' : [ B2DplusKPiPiFakeMuComb],
                   'B2DsKKPiFakeMu' : [ B2DsKKPiFakeMuComb],
                   'B2LcPKPiFakeMu' : [ B2LcPKPiFakeMuComb],
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



