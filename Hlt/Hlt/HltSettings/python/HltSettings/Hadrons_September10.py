class Hadrons_Minimal :
    """
    Threshold settings for a minimal Hlt2 in summer 2010 conditions
    This file is for the hadron channels, I have taken the liberty of also
    including the Xgamma selections as they fit more naturally into the hadron
    label than the muon one. 
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author V. Gligorov 
    @date 2010-05-27
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [
            'Hlt2Bs2PhiGamma',
            'Hlt2Bs2PhiGammaNoCutsK',
            'Hlt2Bs2PhiGammaWideBMass',
            'Hlt2Bs2PhiGammaLooseDira',
            'Hlt2Bd2KstGamma',
            'Hlt2Bd2KstGammaNoCutsKPi',
            'Hlt2Bd2KstGammaWideBMass',
            'Hlt2Bd2KstGammaWideKMass',
            'Hlt2Bd2KstGammaLooseDira',
            'Hlt2B2HH',
            # 'Hlt2B2HHLTUnbiased',
            'Hlt2Bs2PhiPhi',
            'Hlt2IncDiProton', 
            'Hlt2IncDiProtonPresel', 
            'Hlt2IncDiProtonTrackFitted',
            'Hlt2DisplVerticesDouble', 
            'Hlt2DisplVerticesSingle',
            'Hlt2DisplVerticesSinglePostScaled',            
            'Hlt2B2HHPi0_Merged',
            'Hlt2B2HHPi0_Resolved',
            'Hlt2CharmD2KS0PiLL',        
            # The topological 
            'Hlt2TopoOSTF2Body',
            'Hlt2TopoOSTF3Body',
            'Hlt2TopoOSTF4Body',
            # The charm lines
            'Hlt2CharmOSTF2Body',
            'Hlt2CharmOSTF3Body',
            #'Hlt2CharmOSTF2BodyInc',    
            'Hlt2CharmOSTF2BodyWideMass',
            'Hlt2CharmOSTF3BodyWideMass',
            ]

        # For inclusive phi extend using Kim's setting
        from Phi_September10 import Phi_September10 as PhiLines
        lines.extend ( PhiLines().ActiveHlt2Lines() )
       
        return lines

    def Thresholds(self) : 
       
        d = { }
 
        from TopoLines_September10 import TopoLines
        d.update ( TopoLines().Thresholds() )

        from CharmLines_September10 import CharmLines
        d.update ( CharmLines().Thresholds() )

        from Phi_September10 import Phi_September10 as PhiLines
        d.update ( PhiLines().Thresholds() )

        return d
