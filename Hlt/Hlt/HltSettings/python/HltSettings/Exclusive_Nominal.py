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
        # Hlt2B2DXLines.py
#              'Hlt2B2D2hhBachelorReq3BodySignal'
#            , 'Hlt2B2D2hhBachelorReq4BodySignal'
#            , 'Hlt2B2D2hhhBachelorReq3BodySignal'
#            , 'Hlt2B2D2hhhBachelorReq4BodySignal'
#            , 'Hlt2B2D2hhKstarReq3BodySignal'
#            , 'Hlt2B2D2hhKstarReq4BodySignal'
#            , 'Hlt2B2D2hhBachelorReq3BodyMW'
#            , 'Hlt2B2D2hhBachelorReq4BodyMW'
#            , 'Hlt2B2D2hhhBachelorReq3BodyMW'
#            , 'Hlt2B2D2hhhBachelorReq4BodyMW'
#            , 'Hlt2B2D2hhKstarReq3BodyMW'
#            , 'Hlt2B2D2hhKstarReq4BodyMW'
        # Hlt2B2JpsiXLines.py
              'Bs2JpsiPhiPrescaled'
            , 'Bs2JpsiPhi'
            , 'Bd2JpsiKstar'
            , 'Bu2JpsiK'
            , 'Bd2JpsiMuMuKsLLBiased'
            , 'Bd2JpsiMuMuKsLLUnbiased'
            , 'Bd2JpsiMuMuKsDDBiased'
            , 'Bd2JpsiMuMuKsDDUnbiased'
            , 'B2JpsiX_MuMu'
        # Hlt2B2LLXLines.py
            , 'Hlt2Bu2eeK'
            , 'Hlt2Bu2eeKSignal'
            , 'Hlt2Bu2eeKJpsi'
            , 'Hlt2Bu2eeKHighMass'
            , 'Hlt2Bu2MuMuK'
            , 'Hlt2Bu2MuMuKSignal'
            , 'Hlt2Bu2MuMuKJpsi'
            , 'Hlt2Bu2MuMuKHighMass'
        # Hlt2DisplVerticesLines.py
            , 'Hlt2DisplVertices'
        # Hlt2B2XGammaLines.py
              , 'Hlt2Bs2PhiGamma'
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
        # Hlt2B2PhiXLines
            , 'Hlt2Bs2PhiPhi'
        # Hlt2InclusiveDiMuonLines
            , 'UnbiasedZmm'
             ]
        
        return lines

   
