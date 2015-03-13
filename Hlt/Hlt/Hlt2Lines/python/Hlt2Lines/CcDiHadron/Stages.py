from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner

class DiProtonCombiner(Hlt2Combiner):
    def __init__(self, name):

        DauCuts  = ( "%(DauCuts)s" )
        ComCuts  = ( "%(ComCuts)s" )
        MomCuts  = ( "%(MomCuts)s" ) 
        
        from Inputs import Hlt2Protons
        inputs = [ Hlt2Protons ]
        Hlt2Combiner.__init__(self, name, "J/psi(1S) -> p+ p~-", inputs,                             
                              DaughtersCuts  = { 'p+' : DauCuts },
                              CombinationCut = ComCuts,
                              MotherCut      = MomCuts,
                              Preambulo = [])


class DiPhiCombiner(Hlt2Combiner):
    def __init__(self, name):

        DauCuts  = ( "%(DauCuts)s" )
        ComCuts  = ( "%(ComCuts)s" )
        MomCuts  = ( "%(MomCuts)s" ) 
        
        from Inputs import Hlt2UnbiasedPhi
        inputs = [ Hlt2UnbiasedPhi ]
        Hlt2Combiner.__init__(self, name, "J/psi(1S) -> phi(1020) phi(1020)", inputs,                             
                              DaughtersCuts  = { 'phi(1020)' : DauCuts },
                              CombinationCut = ComCuts,
                              MotherCut      = MomCuts,
                              Preambulo = [])


        
        
