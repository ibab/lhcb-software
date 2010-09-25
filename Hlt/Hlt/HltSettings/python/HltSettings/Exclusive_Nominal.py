class Exclusive_Nominal :
    """
    Threshold settings for Hlt1 and 2 in nominal conditions (list of exclusive)
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, M. Gandelman, G. Raven, K. Vervink
    @date 2009-08-05
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [

        # Hlt2B2JpsiXLines.py
            'Hlt2B2JpsiX_MuMu', 
            'Hlt2Bc2JpsiH', 
            'Hlt2Bc2JpsiHDetached', 
            'Hlt2Bc2JpsiHPrescaled', 
            'Hlt2Bc2JpsiMuX', 
            'Hlt2Bc2JpsiMuXSignal', 
            'Hlt2Bd2JpsiKstarSignal', 
            'Hlt2Bd2JpsiKstarWide', 
            'Hlt2Bd2JpsiMuMuKsDDBiased', 
            'Hlt2Bd2JpsiMuMuKsDDUnbiased', 
            'Hlt2Bd2JpsiMuMuKsLLBiased', 
            'Hlt2Bd2JpsiMuMuKsLLUnbiased', 
            'Hlt2Bs2JpsiPhiDetached', 
            'Hlt2Bs2JpsiPhiPrescaled', 
            'Hlt2Bs2JpsiPhiSignal', 
            'Hlt2Bs2JpsiPhiWide', 
            'Hlt2TFBc2JpsiH', 
            'Hlt2TFBc2JpsiHDetached', 
            'Hlt2TFBc2JpsiHPrescaled', 
            'Hlt2TFBc2JpsiMuX', 
            'Hlt2TFBc2JpsiMuXSignal', 
            # Hlt2B2LLXLines.py
            'Hlt2Bu2eeK', 
            'Hlt2Bu2eeKHighMass', 
            'Hlt2Bu2eeKJpsi', 
            'Hlt2Bu2eeKSignal', 
            'Hlt2Bu2JpsiKSignal', 
            'Hlt2Bu2JpsiKWide', 
            'Hlt2Bu2MuMuK', 
            'Hlt2Bu2MuMuKHighMass', 
            'Hlt2Bu2MuMuKJpsi', 
            'Hlt2Bu2MuMuKSignal', 
            # Hlt2DisplVerticesLines.py
            'Hlt2diphotonDiMuon', 
            'Hlt2DisplVerticesDouble', 
            'Hlt2DisplVerticesSingle', 
            # Hlt2B2XGammaLines.py
                'Hlt2Bs2PhiGamma'
              , 'Hlt2Bs2PhiGammaNoCutsK'
              , 'Hlt2Bs2PhiGammaWideBMass'
              , 'Hlt2Bs2PhiGammaLooseDira'
              , 'Hlt2Bd2KstGamma'
              , 'Hlt2Bd2KstGammaNoCutsKPi'
              , 'Hlt2Bd2KstGammaWideBMass'
              , 'Hlt2Bd2KstGammaWideKMass'
              , 'Hlt2Bd2KstGammaLooseDira'
        # Hlt2B2HHLines
            , 'Hlt2B2HH'
            , 'Hlt2B2HHLTUnbiased' 
        # Hlt2B2PhiXLines
            , 'Hlt2Bs2PhiPhi',
        # Hlt2InclusiveDiMuonLines
#            , 'UnbiasedZmm'
#            , 'Hlt2diphotonDiMuon'
            # Dstar
            'Hlt2Dst2PiD02KMu', 
            'Hlt2Dst2PiD02KPi', 
            'Hlt2Dst2PiD02KPiD0SB', 
            'Hlt2Dst2PiD02KPiDMSB', 
            'Hlt2Dst2PiD02KPiSignal', 
            'Hlt2Dst2PiD02MuMu', 
            'Hlt2Dst2PiD02MuMuDMSB', 
            'Hlt2Dst2PiD02MuMuSignal', 
            'Hlt2Dst2PiD02PiMu', 
            'Hlt2Dst2PiD02PiPi', 
            'Hlt2Dst2PiD02PiPiSignal',
# Di-Proton
            'Hlt2IncDiProton', 
            'Hlt2IncDiProtonPresel', 
            'Hlt2IncDiProtonTrackFitted', 
             ]
        
        return lines

   
