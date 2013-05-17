class CharmCEP_September2012 :
    """
    Threshold settings for Hlt1 and 2 in nominal conditions (list of exclusive)
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, M. Gandelman, G. Raven, V. Gligorov, P. Spradlin
    @date 2009-08-05
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) : 
        """
        Returns a list of active lines
        """
        
        lines = [ 'Hlt2LowMultD2KPi'       
                  , 'Hlt2LowMultD2KPiPi'   
                  , 'Hlt2LowMultD2K3Pi'    
                  , 'Hlt2LowMultChiC2HH'
                  , 'Hlt2LowMultChiC2HHHH'      
                  , 'Hlt2LowMultD2KPiWS'   
                  , 'Hlt2LowMultD2KPiPiWS' 
                  , 'Hlt2LowMultD2K3PiWS'  
                  , 'Hlt2LowMultChiC2HHWS'
                  , 'Hlt2LowMultChiC2HHHHWS'    
                  , 'Hlt2LowMultDDIncCP'
                  , 'Hlt2LowMultDDIncVF'
                  , 'Hlt2LowMultChiC2PP'
                  , 'Hlt2LowMultLMR2HH'
             ]
        
        return lines
