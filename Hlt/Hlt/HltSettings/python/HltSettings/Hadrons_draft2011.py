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
            'Hlt2TopoE2BodyBBDT',
            'Hlt2TopoE3BodyBBDT',
            'Hlt2TopoE4BodyBBDT',
            # XGamma
            'Hlt2Bs2PhiGamma',
            'Hlt2Bs2PhiGammaWideBMass',
            'Hlt2Bd2KstGamma',
            'Hlt2Bd2KstGammaWideBMass',
            'Hlt2Bd2KstGammaWideKMass',
            # HHPi0
            'Hlt2B2HHPi0_Merged',
            # B2HH
            'Hlt2B2HH',
            'Hlt2B2HHLTUnbiased',
            # DiProtons
            'Hlt2DiProton',
            'Hlt2DiProtonTF',   
            'Hlt2DiProtonLowMult',
            'Hlt2DiProtonLowMultTF'
            ]

        # For inclusive phi extend using Albert's new setting
        from Phi_draft2011 import Phi_draft2011 as PhiLines
        lines.extend ( PhiLines().ActiveHlt2Lines() )

        from CharmHadronLines_draft2011 import CharmHadronLines
        lines.extend ( CharmHadronLines().ActiveHlt2Lines() )

        return lines

    def Thresholds(self) : 

        from Hlt2Lines.Hlt2B2HHLTUnbiasedLines import Hlt2B2HHLTUnbiasedLinesConf 
       
        d = { Hlt2B2HHLTUnbiasedLinesConf : {'NetCut' : (1.0 + (0.85))*0.5 } }
 
        from TopoLines_draft2011 import TopoLines
        d.update ( TopoLines().Thresholds() )

        from Phi_draft2011 import Phi_draft2011 as PhiLines
        d.update ( PhiLines().Thresholds() )

        from CharmHadronLines_draft2011 import CharmHadronLines
        d.update ( CharmHadronLines().Thresholds() )

        return d
