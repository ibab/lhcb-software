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
            'Hlt2B2HHLTUnbiased',
            'Hlt2Bs2PhiPhi',
            'Hlt2IncDiProton', 
            'Hlt2IncDiProtonPresel', 
            'Hlt2IncDiProtonTrackFitted',
            'Hlt2DisplVerticesDouble', 
            'Hlt2DisplVerticesSingle',            
            # The topological 
            'Hlt2Topo2BodySA',
            'Hlt2Topo3BodySA',
            'Hlt2TopoTF3BodySA',
            'Hlt2TopoTF2BodySA',
            'Hlt2TopoTF3BodyReq3Yes',
            'Hlt2TopoTF3BodyReq2Yes',
            'Hlt2TopoTF2BodyReq3Yes',
            'Hlt2TopoTF2BodyReq2Yes',
            'Hlt2TopoOSTF3Body',
            'Hlt2TopoOSTF2Body',
            'Hlt2TopoRobTF2Body',
            'Hlt2TopoRobTF3Body',
            # The charm lines
            'Hlt2CharmTF2BodySA',
            'Hlt2CharmTF3BodySA',
            'Hlt2CharmTF2BodyWideMassSA',
            'Hlt2CharmTF3BodyWideMassSA',
            'Hlt2Charm2BodySA',
            'Hlt2Charm3BodySA',
            'Hlt2CharmTF2BodySignal',
            'Hlt2CharmTF3BodySignal',
            'Hlt2CharmTF2BodyWideMass',
            'Hlt2CharmTF3BodyWideMass',
            # B2DX lines
            'Hlt2B2D2hhhBachelorWithKsBroadMW',
            'Hlt2B2D2hhBachelorBroadMW',    
            'Hlt2B2D2hhBachelorSignal',    
            'Hlt2B2D2hhhBachelorWithKsSignal',
            'Hlt2B2D2XRobust3BodyMonitor', 
            'Hlt2B2D2hhBachelorSignalMonitor',       
            'Hlt2B2D2hhhBachelorWithKsBroadMWMonitor',
            'Hlt2B2D2hhBachelorBroadMWMonitor'
            ]

        # For inclusive phi extend using Kim's setting
        #from Phi_Reduction5 import PhiCharmLepton_Reduction5 as PhiLines
        from Phi_NominalButPID import Phi_NominalButPID as PhiLines
        lines.extend ( PhiLines().ActiveHlt2Lines() )
       
        return lines

    def Thresholds(self) : 
       
        d = { }
 
        from TopoLines import TopoLines
        d.update ( TopoLines().Thresholds() )

        from CharmLines import CharmLines
        d.update ( CharmLines().Thresholds() )

        #from Phi_Reduction5 import PhiCharmLepton_Reduction5 as PhiLines
        from Phi_NominalButPID import Phi_NominalButPID as PhiLines
        d.update ( PhiLines().Thresholds() )

        return d
