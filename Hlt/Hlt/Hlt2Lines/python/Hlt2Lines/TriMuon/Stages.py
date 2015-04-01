###
#  @author J. Harrison 	jonathan.harrison@manchester.ac.uk
#  @date 2015-03-16
#
#  Please contact the above author before editing this file
#
##

from Hlt2Lines.Utilities.Hlt2Filter import Hlt2VoidFilter
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner

class MultiMuonFilter(Hlt2VoidFilter):
    def __init__(self,name):
        from Inputs import Hlt2Muons
        cut = ("CONTAINS('%s')" % Hlt2Muons.outputSelection()) + " > 2.9"
        Hlt2VoidFilter.__init__(self, name, cut, [Hlt2Muons], shared = True)

class GoodMuonFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        from Inputs import Hlt2Muons
        cut = ("(TRCHI2DOF < %(TrackChi2)g)" +
               "& ( BPVIPCHI2() > %(Chi2IP)g)")
        Hlt2ParticleFilter.__init__(self, name, cut, [Hlt2Muons], shared = True)

goodMuons = GoodMuonFilter('GoodMuonsForTriMuonLines')

class Tau23MuCombiner(Hlt2Combiner):
    def __init__(self,name):
        dc =     {'mu+' : "ALL"} 
        cc =     ("(ADAMASS('tau+') < %(MassWin)g)" +
		 "& (AM12 > 2 * PDGM('mu+') + %(DiMuMass)g)")
        mc =     ("(VFASPF(VCHI2) < %(VertexChi2)g)" +
		 "& (BPVLTIME() * c_light > %(ctau)g)")
        from HltTracking.HltPVs import PV3D 
        inputs = [goodMuons]
        Hlt2Combiner.__init__(self, name, "[tau+ -> mu+ mu+ mu-]cc", inputs,
                              dependencies = [MultiMuonFilter('TriMuonMultiMuonFilter'), 
			      PV3D('Hlt2')],
			      DaughtersCuts = dc, CombinationCut = cc,
                              MotherCut = mc) 

class TriMuonDetachedCombiner(Hlt2Combiner):
    def __init__(self,name):
        tightMu = ("(PT > %(MuonPT)g)" +
                  "& ( BPVIPCHI2() > %(Chi2IP_Tight)g)")
        dc =     {'mu+' : tightMu} 
        cc =     "AALL"
        mc =     "ALL"
        from HltTracking.HltPVs import PV3D 
        inputs = [goodMuons]
        Hlt2Combiner.__init__(self, name, "[B_c+ -> mu+ mu+ mu-]cc", inputs,
                              dependencies = [MultiMuonFilter('TriMuonMultiMuonFilter'), 
			      PV3D('Hlt2')],
			      DaughtersCuts = dc, CombinationCut = cc,
                              MotherCut = mc) 
