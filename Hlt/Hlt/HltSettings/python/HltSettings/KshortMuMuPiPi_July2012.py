class  KshortMuMuPiPi_July2012 :
    """
    add names of line for Kshort -->  mu,mu,pi,pi trigger
 

    @author M. Sokoloff
    @date 2012-07-27
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) : 
        """
        Returns a list of active lines
        """
        
        lines = [ #'Hlt2DimuonForKshortToMuMuPiPi'
                  'Hlt2KshortToMuMuPiPi'
                  , 'Hlt2ChargedHyperon_Omega2Lambda0LLK'
                  , 'Hlt2ChargedHyperon_Omega2Lambda0DDK'
                  , 'Hlt2ChargedHyperon_Xi2Lambda0LLPi'
                  , 'Hlt2ChargedHyperon_Xi2Lambda0DDPi'
                  #, 'Hlt2ChargedHyperon_Xi2Lambda0DDPiD'
             ]
        
        return lines
