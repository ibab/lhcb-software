class B2DX_320Vis :
    """
    Threshold settings for Hlt1 and 2 in low lumi conditions (B->DX)
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg
    @date 2009-10-27
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [
             'Hlt2B2D2hhBachelorReq3BodySignal'
            , 'Hlt2B2D2hhBachelorReq4BodySignal'
            , 'Hlt2B2D2hhhBachelorReq3BodySignal'
            , 'Hlt2B2D2hhhBachelorReq4BodySignal'
            , 'Hlt2B2D2hhKstarReq3BodySignal'
            , 'Hlt2B2D2hhKstarReq4BodySignal'
            , 'Hlt2B2D2hhBachelorReq3BodyMW'
            , 'Hlt2B2D2hhBachelorReq4BodyMW'
            , 'Hlt2B2D2hhhBachelorReq3BodyMW'
            , 'Hlt2B2D2hhhBachelorReq4BodyMW'
            , 'Hlt2B2D2hhKstarReq3BodyMW'
            , 'Hlt2B2D2hhKstarReq4BodyMW'
             ]
        
        return lines

   
