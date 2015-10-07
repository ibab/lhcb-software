##
#  @author F. Dettori Francesco.Dettori@cern.ch
#  @date 2015-03-07
#
#  Please contact the abovementioned responsibles before editing this file
# 
##


from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class RareStrangeLines(Hlt2LinesConfigurableUser):
    __slots__ = {'_stages' : {},
                 'Prescale' : {},
                 'Common' : {'TrChi2' : 3,
                             'TrGP' : 0.3
                             },
                 'SigmaPMuMu' : { 'muonMinIpChi2' :  25. ,
                                  'pPIDp' : 5. ,
                                  'pMinIpChi2' :  25. ,
                                  'SigmaMassWin' : 500 * MeV ,
                                  'SigmaMaxDOCA' : 2. * mm ,
                                  'SigmaVtxChi2' : 25 ,  # adimensional
                                  'SigmaMinPt' : 500 * MeV ,
                                  'SigmaMinDIRA' : 0.9 ,   # adimensional
                                  'SigmaMaxIpChi2' : 36 ,  # adimensional
                                  'SigmaMinTauPs'  : 6 * picosecond
                                  },
                 
                 'KPiMuMu' : { 'muonMinIpChi2' :  25. ,
                               'piMinIpChi2' :  25. ,
                               'KMassWin' : 500 * MeV ,
                              'KMaxDOCA' : 2. * mm ,
                              'KVtxChi2' : 25. ,  # adimensional
                              'KMinPt' : 500 * MeV ,
                              'KMinDIRA' : 0.9 ,   # adimensional
                              'KMaxIpChi2' : 36 ,  # adimensional
                              'KMinTauPs'  : 10 * picosecond
                              }
                 
                 }
                 
                 
    def stages(self, nickname=""):
        if hasattr(self, '_stages') and self._stages:
            if nickname:
                return self._stages[nickname]
            else:
                return self._stages
        
        from Stages import SigmaPMuMuCombiner, KPiMuMuCombiner 
        self._stages = { 'SigmaPMuMu'   : [ SigmaPMuMuCombiner('SigmaPMuMu') ],
                         'KPiMuMu'      : [ KPiMuMuCombiner('KPiMuMu')]
                  }
        
        if nickname:
            return self._stages[nickname]
        else:
            return self._stages
    
    def __apply_configuration__(self):
        from HltLine.HltLine import Hlt2Line

        stages = self.stages()

        for (nickname, algos) in self.algorithms(stages).iteritems():
            linename ='RareStrange'+nickname
            Hlt2Line(linename,
                     prescale = self.prescale,
                     algos = algos,
                     postscale = self.postscale)



