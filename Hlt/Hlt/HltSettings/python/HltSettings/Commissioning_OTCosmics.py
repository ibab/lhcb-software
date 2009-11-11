class Commissioning_OTCosmics :
    """
    Threshold settings for Hlt1 and 2 for OuterTracker Cosmic Runs 
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author S.Nies, J.Merkel 
    @date 2009-10-22
    """
    
    
    def HltType(self) :
        return 'CosmicOT'

    def L0TCK(self) :
        return None

    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        return  [ ]
   
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        return [ 'Hlt1CosmicOT' ]

    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        from Hlt1Lines.HltCosmicLines  import HltCosmicLinesConf
        return { HltCosmicLinesConf : { 'Prescale' : { 'Hlt1CosmicOT'  : 1.
                                      }              } 
               }
