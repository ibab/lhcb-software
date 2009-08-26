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
        
        return       [ 'Hlt2B2HH',
                       'Hlt2Bs2JpsiPhi',          
                       'Hlt2Bs2JpsiPhiSignal',    
                       'Hlt2Bu2eeK',              
                       'Hlt2Bu2eeKSignal',        
                       'Hlt2Bu2eeKJpsi',          
                       'Hlt2Bu2eeKHighMass',         
                       'Hlt2Bu2MuMuK',         
                       'Hlt2Bu2MuMuKSignal',         
                       'Hlt2Bu2MuMuKJpsi',         
                       'Hlt2Bu2MuMuKHighMass',         
                       'Hlt2Bd2JpsiKstar',         
                       'Hlt2Bd2JpsiKstarSignal',
                       'Hlt2Bs2PhiPhi',
                       'Hlt2DisplVertices',
                       'Hlt2Bd2JpsiMuMuKsLLBiased',         
                       'Hlt2Bd2JpsiMuMuKsDDBiased',         
                       'Hlt2Bd2JpsiMuMuKsLLUnbiased',         
                       'Hlt2Bd2JpsiMuMuKsDDUnbiased',         
                       'Hlt2B2JpsiX_MuMu',         
                       'Hlt2Bu2JpsiK',               
                       'Hlt2Bu2JpsiKSignal',         
                       'Hlt2PhiGamma',
                       'Hlt2KstGamma',
                       'Hlt2Bs2JpsiPhiPrescaled',
                       'Hlt2Bs2JpsiPhiDetached' ]

   
