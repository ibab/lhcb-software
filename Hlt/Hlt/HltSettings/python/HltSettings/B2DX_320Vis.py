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
            'Hlt2B2D2hhhBachelorWithKsBroadMW'
            'Hlt2B2D2hhBachelorBroadMW'    
            , 'Hlt2B2D2hhhBachelorBroadMW'
            , 'Hlt2B2D2hhKstarBroadMW'
            , 'Hlt2B2D2hhhBachelorWithKsSignal'
            , 'Hlt2B2D2hhBachelorSignal'    
            , 'Hlt2B2D2hhhBachelorSignal'
            , 'Hlt2B2D2hhKstarSignal'
            , 'Hlt2B2D2XRobust3BodyMonitor' 
            , 'Hlt2B2D2hhhBachelorWithKsSignalMonitor'
            , 'Hlt2B2D2hhBachelorSignalMonitor'       
            , 'Hlt2B2D2hhhBachelorSignalMonitor'       
            , 'Hlt2B2D2hhKstarSignalMonitor'           
            , 'Hlt2B2D2hhhBachelorWithKsBroadMWMonitor'
            , 'Hlt2B2D2hhBachelorBroadMWMonitor' 
            , 'Hlt2B2D2hhhBachelorBroadMWMonitor'
            , 'Hlt2B2D2hhKstarBroadMWMonitor'
            ]
        
        return lines

   
