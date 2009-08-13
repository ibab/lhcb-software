class Commissioning_Nominal :
    """
    Threshold settings for Hlt1 and 2 in nominal conditions (list of commissioning)
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, G. Raven
    @date 2009-08-11
    """
    
    __all__ = ( 'ActiveLines' )
    
    
    def ActiveLines(self) :
        """
        Returns a list of active lines
        """
        
        return       [ 'Hlt2Transparant', 'Hlt2PassThrough' ]

   
