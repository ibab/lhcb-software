class Debug_320Vis_300L0_NoLines_Hlt1_Aug09 :
    """
    Threshold settings for Hlt1 for 2010 conditions, no Hlt1 lines included for debugging of individual lines
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author A. Perez-Calero
    @date 2009-11-26
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
        return '0xFF68'

    def HltType(self) :
        self.verifyType( Debug_320Vis_300L0_NoLines_Hlt1_Aug09 )
        return          'Debug_320Vis_300L0_NoLines_Hlt1_Aug09'

    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        from HLt2Lines.HLt2CommissioningLines  import HLt2CommissioningLinesConf
        
        return {   Hlt2CommissioningLinesConf : { 'Prescale' : { 'Hlt2PassThrough'  : 1.
                                                                 , 'Hlt2Forward'      : 0. # first needs a check that /Hlt/Track/Long actually exists..
                                                                 , 'Hlt2DebugEvent'   : 0.0001
                                                                 }              }
                 
                 }
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        return [ 'Hlt2PassThrough', 'Hlt2Transparent','Hlt2DebugEvent']
    
    
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        return []
    
    
