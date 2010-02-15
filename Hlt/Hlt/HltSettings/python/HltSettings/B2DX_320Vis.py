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
            'Hlt2B2D2hhBachelorReq3BodyWithKsSignal',
            'Hlt2B2D2hhBachelorReq4BodyWithKsSignal',
            'Hlt2B2D2hhBachelorReq3BodyWithoutKsSignal',
            'Hlt2B2D2hhBachelorReq4BodyWithoutKsSignal',
            'Hlt2B2D2hhhBachelorReq3BodyWithKsSignal',
            'Hlt2B2D2hhhBachelorReq4BodyWithKsSignal',
            'Hlt2B2D2hhhBachelorReq3BodyWithoutKsSignal',
            'Hlt2B2D2hhhBachelorReq4BodyWithoutKsSignal',
            'Hlt2B2D2hhKstarReq3BodyWithKsSignal',
            'Hlt2B2D2hhKstarReq3BodyWithoutKsSignal',
            'Hlt2B2D2hhKstarReq4BodyWithKsSignal',
            'Hlt2B2D2hhKstarReq4BodyWithoutKsSignal',
            'Hlt2B2D2hhBachelorReq3BodyWithKsBroadMW',
            'Hlt2B2D2hhBachelorReq4BodyWithKsBroadMW',
            'Hlt2B2D2hhBachelorReq3BodyWithoutKsBroadMW',
            'Hlt2B2D2hhBachelorReq4BodyWithoutKsBroadMW',
            'Hlt2B2D2hhhBachelorReq3BodyWithKsBroadMW',
            'Hlt2B2D2hhhBachelorReq4BodyWithKsBroadMW',
            'Hlt2B2D2hhhBachelorReq3BodyWithoutKsBroadMW',
            'Hlt2B2D2hhhBachelorReq4BodyWithoutKsBroadMW',
            'Hlt2B2D2hhKstarReq3BodyWithKsBroadMW',
            'Hlt2B2D2hhKstarReq3BodyWithoutKsBroadMW',
            'Hlt2B2D2hhKstarReq4BodyWithKsBroadMW',
            'Hlt2B2D2hhKstarReq4BodyWithoutKsBroadMW'
            ]
        
        return lines

   
