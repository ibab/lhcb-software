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
            # XGamma
            'Hlt2Bs2PhiGamma',
            #'Hlt2Bs2PhiGammaWideBMass',
            'Hlt2Bd2KstGamma',
            #'Hlt2Bd2KstGammaWideBMass',
            #'Hlt2Bd2KstGammaWideKMass',
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
        from TopoLines_April2015 import TopoLines_April2015
        lines.extend( TopoLines_April2015().ActiveHlt2Lines() )
        

        # For inclusive phi extend using Albert's new setting
        # from Phi_draft2012 import Phi_draft2012 as PhiLines
        # lines.extend ( PhiLines().ActiveHlt2Lines() )

        # from CharmHadronLines_March2012 import CharmHadronLines
        # lines.extend ( CharmHadronLines().ActiveHlt2Lines() )

        return lines

    def Thresholds(self) : 

        d = {}
 
        from TopoLines_April2015 import TopoLines_April2015
        d.update ( TopoLines_April2015().Thresholds() )

        return d
