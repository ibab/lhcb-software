# Note: if you define activeLines, you MUST provide a unique 'HltType' for this 
#       set of activelines

class Debug_10000Vis_1000L0_Velo_Hlt1_Oct09 :
    """
    Threshold settings for testing the Hlt1 velo lines, nominal conditions
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author V. Gligorov
    @date 2009-11-2
    """

    def __init__(self) :
        self.StripEndSequence = ['']

    def verifyType(self,ref) :
        # verify self.ActiveLines is still consistent with
        # our types self.ActiveHlt2Lines and self.ActiveHlt1Lines...
        # so we can force that classes which inherit from us
        # and overrule either ActiveHlt.Lines also overrule
        # HltType...
        if self.ActiveHlt1Lines() != ref.ActiveHlt1Lines(self)  or self.ActiveHlt2Lines() != ref.ActiveHlt2Lines(self)  :
            raise RuntimeError('Must update HltType when modifying ActiveHlt.Lines()')

    def L0TCK(self) :
        return None

    def HltType(self) :
        self.verifyType( Debug_10000Vis_1000L0_Velo_Hlt1_Oct09 )
        return          'Debug_10000Vis_1000L0_Velo_Hlt1_Oct09'

    def ActiveHlt2Lines(self) :
        """
        Returns a list of active Hlt2 lines
        """
        return [ 'Hlt2Transparent']
        
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active Hlt1 lines
        """
        # Hlt1 + the name of the line
        return [ 'Hlt1VeloASide',
                 'Hlt1VeloCSide']
    

    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        from Hlt1Lines.Hlt1VeloLines           import Hlt1VeloLinesConf
        from Hlt2Lines.Hlt2CommissioningLines import Hlt2CommissioningLinesConf
    
        return {  Hlt1VeloLinesConf : { 'Prescale' : { 'Hlt1Velo(.Side|TrTr)' : 1 }
                                         , 'MinimumNumberOfRClusters'   : 12 # 4 tracks with 3 hits
                                         , 'MinimumNumberOfPhiClusters' : 12 # 4 tracks with 3 hits
                                         , 'MaxNumberOfClusters'        : 450 # 0.5% occupancy
                                         , 'MinTrksPerVtx'              : 4   # 4 is the minumum value you can put here 
                                         , 'ODIN'                       :  None
                                         , 'L0DU'                       : "L0_CHANNEL('CALO')" # warning: this generarates a dependency on the L0 configuration!
                                         }
               ,  Hlt2CommissioningLinesConf : { 'Prescale' : { 'Hlt2PassThrough'  : 1.  }
                                               }
               }
    
