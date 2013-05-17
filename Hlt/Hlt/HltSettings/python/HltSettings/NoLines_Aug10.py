class NoLines_Aug10 :
    """
    Threshold settings with no Hlt1 lines included for debugging of individual lines
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author R. Aaij
    @date 2010-08-06
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
    
    def L0TCK(self) :
        return '0x001F'

    def HltType(self) :
        self.verifyType( NoLines_Aug10 )
        return          'NoLines_Aug10'

    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        from Hlt2Lines.Hlt2CommissioningLines  import Hlt2CommissioningLinesConf
        
        return { Hlt2CommissioningLinesConf : { 'Prescale' : { 'Hlt2PassThrough'  : 1.
                                                               }
                                                }
                 }
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        return [ 'Hlt2PassThrough' ]
    
    
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        return []
