# Note: if you define activeLines, you MUST provide a unique 'HltType' for this 
#       set of activelines

class Debug_10000Vis_1000L0_NoLines_Hlt1_Oct09 :
    """
    Thresholds and settings for testing the Hlt1 lines for nominal conditions. No lines included by default
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author A. Perez-Calero (aperez@ecm.ub.es)
    @date 2009-11-4
    """
    
    def verifyType(self,ref) :
        # verify self.ActiveLines is still consistent with
        # our types self.ActiveHlt2Lines and self.ActiveHlt1Lines...
        # so we can force that classes which inherit from us
        # and overrule either ActiveHlt.Lines also overrule
        # HltType...
        if self.ActiveHlt1Lines() != ref.ActiveHlt1Lines(self)  or self.ActiveHlt2Lines() != ref.ActiveHlt2Lines(self)  :
            raise RuntimeError('Must update HltType when modifying ActiveHlt.Lines()')

    def L0TCK(self) :
        return '0xDC09'

    def HltType(self) :
        self.verifyType( Debug_10000Vis_1000L0_NoLines_Hlt1_Oct09 )
        return          'Debug_10000Vis_1000L0_NoLines_Hlt1_Oct09'

    def ActiveHlt2Lines(self) :
        """
        Returns a list of active Hlt2 lines
        """
        return [ 'Hlt2PassThrough', 'Hlt2Transparent','Hlt2DebugEvent']

    def ActiveHlt1Lines(self) :
        """
        Returns a list of active Hlt1 lines
        """
        return []

    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        from Hlt2Lines.Hlt2CommissioningLines import Hlt2CommissioningLinesConf
    
        return { Hlt2CommissioningLinesConf : { 'Prescale' : { 'Hlt2PassThrough'  : 1.
                                                               , 'Hlt2Forward'      : 0. # first needs a check that /Hlt/Track/Long actually exists..
                                                               , 'Hlt2DebugEvent'   : 0.0001
                                                }              }
                 }
    
