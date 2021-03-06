##
#  @author F. Dettori Francesco.Dettori@cern.ch
#  @date 2015-03-07
#
#  Please contact the abovementioned responsibles before editing this file
#
##
""" Lines for the study of the physics of strange hadrons. 
"""

__version__ = "$Revision: $"
__author__  = "Francesco Dettori francesco.dettori@cern.ch"


from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class StrangeLines(Hlt2LinesConfigurableUser):
    __slots__ = {'_stages' : {},
                 'Prescale' : {},
                 'Common' : {'TrChi2' : 3,
                             'TrGP' : 0.3
                             },
                 'KPiPiPiTurbo' : { 'piMinIpChi2' :  25. ,
                               'KMassWin' : 100 * MeV ,
                              'KMaxDOCA' : 2. * mm ,
                              'KVtxChi2' : 25. ,  # adimensional
                              'KMinPt' : 300 * MeV ,
                              'KMinDIRA' : 0.999 ,   # adimensional
                              'KMaxIpChi2' : 25 ,  # adimensional
                              'KMinTauPs'  : 10 * picosecond
                              }

                 }


    def stages(self, nickname=""):
        if hasattr(self, '_stages') and self._stages:
            if nickname:
                return self._stages[nickname]
            else:
                return self._stages
        
        from Stages import KPiPiPiCombiner

        self._stages = {
                         'KPiPiPiTurbo'      : [ KPiPiPiCombiner('KPiPiPiTurbo')]
                  }

        if nickname:
            return self._stages[nickname]
        else:
            return self._stages

    def __apply_configuration__(self):
        from HltLine.HltLine import Hlt2Line

        stages = self.stages()

        for (nickname, algos) in self.algorithms(stages):
            linename ='Strange'+nickname
            if "Turbo" in nickname : 
                Hlt2Line(linename,
                         prescale = self.prescale,
                         algos = algos,
                         postscale = self.postscale, Turbo = True)
            else: 
                Hlt2Line(linename,
                         prescale = self.prescale,
                         algos = algos,
                         postscale = self.postscale)
