class DiMuonLines :
    """
    Threshold settings for Hlt1 and 2 in nominal conditions (list of exclusive)
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, M. Gandelman, G. Raven
    @date 2009-08-05
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [
            'Hlt2SingleMuonLow',
            'Hlt2IncMuTrack',
            'Hlt2IncMuTrack4Jpsi', 
            'Hlt2IncMuTrackLow', 
            'Hlt2IncMuTrackLowMass', 
            'Hlt2IncMuTrackLowMassLow', 
            'Hlt2IncMuTrackLowMassMid', 
            'Hlt2IncMuTrackMid', 
            'Hlt2DiMuonUnbiasedJPsi',
            'Hlt2DiMuonUnbiasedPsi2S',
            'Hlt2DiMuonUnbiasedBmm',
            'Hlt2DiMuonUnbiasedZmm',
            'Hlt2DiMuonDY1',
            'Hlt2DiMuonDY2',
            'Hlt2DiMuonDY3',
            'Hlt2DiMuonDY4',
            'Hlt2BiasedDiMuonSimple',
            'Hlt2BiasedDiMuonRefined',
            'Hlt2BiasedDiMuonMass',
            'Hlt2BiasedDiMuonIP',
            'Hlt2DiMuonUnbiasedJPsiLow',
            'Hlt2UnbiasedDiMuon',  
            'Hlt2UnbiasedDiMuonLowMass', 
            ]
            
        return lines

   
