class Hadrons_draft2011 :
    """
    Threshold settings for Hlt2 hadronic lines: minimal draft for 2011
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author V. Gligorov 
    @date 2010-12-02
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [
            # The topological 
            'Hlt2Topo2BodySimple',
            'Hlt2Topo3BodySimple',
            'Hlt2Topo4BodySimple',
            'Hlt2Topo2BodyBBDT',
            'Hlt2Topo3BodyBBDT',
            'Hlt2Topo4BodyBBDT',
            'Hlt2TopoMu2BodyBBDT',
            'Hlt2TopoMu3BodyBBDT',
            'Hlt2TopoMu4BodyBBDT',
            # XGamma
            'Hlt2Bs2PhiGamma',
            'Hlt2Bs2PhiGammaWideBMass',
            'Hlt2Bd2KstGamma',
            'Hlt2Bd2KstGammaWideBMass',
            'Hlt2Bd2KstGammaWideKMass'
            ]

        # For inclusive phi extend using Albert's new setting
        from Phi_draft2011 import Phi_draft2011 as PhiLines
        lines.extend ( PhiLines().ActiveHlt2Lines() )

        from CharmHadronLines_draft2011 import CharmHadronLines
        lines.extend ( CharmHadronLines().ActiveHlt2Lines() )

        return lines

    def Thresholds(self) : 
       
        d = { }
 
        from TopoLines_draft2011 import TopoLines
        d.update ( TopoLines().Thresholds() )

        from Phi_draft2011 import Phi_draft2011 as PhiLines
        d.update ( PhiLines().Thresholds() )

        from CharmHadronLines_draft2011 import CharmHadronLines
        d.update ( CharmHadronLines().Thresholds() )

        return d
