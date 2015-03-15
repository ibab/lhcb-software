class Hadrons_March2015 :
    """
    Threshold settings for Hlt2 hadronic lines: minimal Sept for 2011
    
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
            'Hlt2TopoRad2BodyBBDT',
            'Hlt2TopoRad2plus1BodyBBDT',
            # XGamma
            'Hlt2Bs2PhiGamma',
            'Hlt2Bs2PhiGammaWideBMass',
            'Hlt2Bd2KstGamma',
            'Hlt2Bd2KstGammaWideBMass',
            'Hlt2Bd2KstGammaWideKMass',
            # B2HH
            'Hlt2B2HH',
            # DiProtons
            'Hlt2DiProton',
            #'Hlt2DiProtonTF',   
            'Hlt2DiProtonLowMult',#,
            #'Hlt2DiProtonLowMultTF'
            #DiPhi
            'Hlt2DiPhi'
            ]

        # For inclusive phi extend using Albert's new setting
        # from Phi_draft2012 import Phi_draft2012 as PhiLines
        # lines.extend ( PhiLines().ActiveHlt2Lines() )

        # from CharmHadronLines_March2012 import CharmHadronLines
        # lines.extend ( CharmHadronLines().ActiveHlt2Lines() )

        return lines

    def Thresholds(self) : 

        d = {}
 
        from TopoLines_March2015 import TopoLines_March2015
        d.update ( TopoLines_March2015().Thresholds() )

        return d
