from Hlt2Lines.Utilities.Hlt2Filter   import Hlt2ParticleFilter
from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner

class DiMuonFilter(Hlt2ParticleFilter):
    def __init__(self, name, code, nickname):
        
        from Inputs import TrackFittedDiMuon
        inputs = [TrackFittedDiMuon]
        Hlt2ParticleFilter.__init__(self, name, code, inputs, nickname=nickname)
        

class Bc2JpsiMuXCombiner(Hlt2Combiner):
    def __init__(self, name):

        JpsiCuts = ( "%(JpsiCuts)s" )
        DauCuts  = ( "%(DauCuts)s" )
        ComCuts  = ( "%(ComCuts)s" )
        MomCuts  = ( "%(MomCuts)s" ) 
        
        from Inputs import BiKalmanFittedMuons
        inputs = [ DiMuonFilter("JpsiForBc2JpsiMuX", JpsiCuts, nickname=name),
                   BiKalmanFittedMuons ]
        Hlt2Combiner.__init__(self, name, "[ B_c+ -> J/psi(1S) mu+ ]cc", inputs,                             
                              DaughtersCuts  = { 'mu+' : DauCuts },
                              CombinationCut = ComCuts,
                              MotherCut      = MomCuts,
                              Preambulo = [])


class Bc2JpsiHCombiner(Hlt2Combiner):
    def __init__(self, name):

        JpsiCuts = ( "%(JpsiCuts)s" )
        DauCuts  = ( "%(DauCuts)s" )
        ComCuts  = ( "%(ComCuts)s" )
        MomCuts  = ( "%(MomCuts)s" ) 
        
        from Inputs import BiKalmanFittedPions
        inputs = [ DiMuonFilter("JpsiForBc2JpsiH", JpsiCuts, nickname=name),
                   BiKalmanFittedPions ]
        Hlt2Combiner.__init__(self, name, "[ B_c+ -> J/psi(1S) pi+ ]cc", inputs,                             
                              DaughtersCuts  = { 'pi+' : DauCuts },
                              CombinationCut = ComCuts,
                              MotherCut      = MomCuts,
                              Preambulo = [])

        

        
