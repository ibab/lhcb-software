from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner

class DiPhiWideCombiner(Hlt2Combiner):
    def __init__(self, name):

        DauCuts  = ( "%(DauCuts)s" )
        ComCuts  = ( "%(ComCuts)s" )
        MomCuts  = ( "%(MomCuts)s" ) 
        
        from Inputs import Hlt2WidePhi
        inputs = [ Hlt2WidePhi ]
        Hlt2Combiner.__init__(self, name, "J/psi(1S) -> phi(1020) phi(1020)", inputs,                             
                              DaughtersCuts  = { 'phi(1020)' : DauCuts },
                              CombinationCut = ComCuts,
                              MotherCut      = MomCuts,
                              Preambulo = [])


class DiKstarCombiner(Hlt2Combiner):
    def __init__(self, name):

        DauCuts  = ( "%(DauCuts)s" )
        ComCuts  = ( "%(ComCuts)s" )
        MomCuts  = ( "%(MomCuts)s" ) 
        
        from Inputs import Hlt2TightKstar
        inputs = [ Hlt2TightKstar ]
        Hlt2Combiner.__init__(self, name, "J/psi(1S) -> K*(892)0 K*(892)~0", inputs,                             
                              DaughtersCuts  = { 'K*(892)0' : DauCuts ,
                                                 'K*(892)~0': DauCuts },
                              CombinationCut = ComCuts,
                              MotherCut      = MomCuts,
                              Preambulo = [])


        
        
