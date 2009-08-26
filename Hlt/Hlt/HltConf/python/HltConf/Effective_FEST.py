class Effective_FEST :
    """
    Threshold settings for Hlt1 and 2 in nominal conditions (Effective/FEST)
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, G. Raven
    @date 2009-08-06
    """
    
    __all__ = ( 'ActiveHlt1Lines', 'ActiveHlt2Lines', 'Thresholds' )
    
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        
        from Hlt1Lines.HltVeloLines import HltVeloLinesConf
        from Hlt1Lines.HltCommissioningLines import HltCommissioningLinesConf
        from Hlt1Lines.HltExpressLines import HltExpressLinesConf
        
        from Effective_Nominal import Effective_Nominal
        d = Effective_Nominal().Thresholds()
        
        d.update(  { HltVeloLinesConf : { 'Prescale' : { '.*'            : 1 } }
                   , HltExpressLinesConf : { 'Prescale' : { 'Hlt1XPress'    : 0.0025
                                                          , 'Hlt1AlignVelo' : 1 } }
                   , HltCommissioningLinesConf : { 'Prescale' : { 'Hlt1NonRandomODIN'    : 1 
                                                                , 'Hlt1RandomODIN'     : 1 # @OnlineEnv.AcceptRate
                                                                , 'Hlt1Tell1Error' : 0
                                                                  }
                                                     }
                     }
                   )
        
        return d
    
######################################################################################################
    #
    # Active Lines
    #
    
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines 
        """
        
        from Effective_Nominal import Effective_Nominal
        list = Effective_Nominal().ActiveHlt1Lines()
        
        return list
    
######################################################################################################
    #
    # Active Lines
    #
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines 
        """
        
        from Effective_Nominal import Effective_Nominal
        list = Effective_Nominal().ActiveHlt2Lines()
        
        return list
    
