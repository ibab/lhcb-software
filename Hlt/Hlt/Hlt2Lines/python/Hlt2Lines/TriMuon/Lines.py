from GaudiKernel.SystemOfUnits import MeV, GeV, micrometer
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class TriMuonLines(Hlt2LinesConfigurableUser):
    __slots__ = {'Prescale'  		    : {},
		 'Common'    		    : {},
	         'GoodMuonsForTriMuonLines' : {'TrackChi2': 4,
                                	       'Chi2IP'	 : 9},
	         'TriMuonTau23Mu'           : {'MassWin'	  : 225 * MeV,
			       		       'DiMuMass'  : 14 * MeV,
                               		       'VertexChi2': 25,
			       		       'ctau'	  : 45 * micrometer},
	         'TriMuonDetached'  	    : {'Chi2IP_Tight': 36,
                               		       'MuonPT'	    : 1.4 * GeV},
                }

    def __apply_configuration__(self) :
        from Stages import Tau23MuCombiner, TriMuonDetachedCombiner
        stages = {'TriMuonTau23Mu'  : [Tau23MuCombiner('TriMuonTau23Mu')],
        	  'TriMuonDetached' : [TriMuonDetachedCombiner('TriMuonDetached')]}

        from HltLine.HltLine import Hlt2Line
        for nickname, algos in self.algorithms(stages):
            Hlt2Line(nickname, prescale = self.prescale,
                     algos = algos, postscale = self.postscale)
