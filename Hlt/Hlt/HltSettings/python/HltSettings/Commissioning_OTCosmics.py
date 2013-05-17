class Commissioning_OTCosmics :
    """
    Threshold settings for Hlt1 and 2 for OuterTracker Cosmic Runs 
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author S.Nies, J.Merkel 
    @date 2009-10-22
    """
    __all__ = ( 'ActiveHlt1Lines', 'ActiveHlt2Lines', 'Thresholds','L0TCK' )

    def verifyType(self,ref) :
        # verify self.ActiveLines is still consistent with
        # our types self.ActiveHlt2Lines and self.ActiveHlt1Lines...
        # so we can force that classes which inherit from us
        # and overrule either ActiveHlt.Lines also overrule
        # HltType...
        if ( self.ActiveHlt1Lines() != ref.ActiveHlt1Lines(self)  or
             self.ActiveHlt2Lines() != ref.ActiveHlt2Lines(self) ) :
            raise RuntimeError('Must update HltType when modifying ActiveHlt.Lines()')

    def HltType(self) :  
        self.verifyType( Commissioning_OTCosmics )
        return          'Commissioning_OTCosmics'
    
    def __init__(self) :
        self.StripEndSequence = ['']


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
        from Hlt1Lines.Hlt1CosmicLines  import Hlt1CosmicLinesConf
        return { Hlt1CosmicLinesConf : { 'Prescale' : { 'Hlt1CosmicOT'  : 1.
                                      }              } 
               }

def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        return []
