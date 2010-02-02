class Debug_320Vis_300L0_Hadron_Hlt1_Oct09 :
    """
    Threshold settings for testing the Hlt1 hadron lines, 2010 conditions
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author V. Gligorov
    @date 2009-10-30
    """
    
    __all__ = ( 'ActiveHlt1Lines', 'ActiveHlt2Lines', 'Thresholds','L0TCK' )

    def verifyType(self,ref) :
        # verify self.ActiveLines is still consistent with
        # our types self.ActiveHlt2Lines and self.ActiveHlt1Lines...
        # so we can force that classes which inherit from us
        # and overrule either ActiveHlt.Lines also overrule
        # HltType...
        if self.ActiveHlt1Lines() != ref.ActiveHlt1Lines(self)  or self.ActiveHlt2Lines() != ref.ActiveHlt2Lines(self)  :
            raise RuntimeError('Must update HltType when modifying ActiveHlt.Lines()')

    
    def L0TCK(self) :
        return '0xFF68'

    def HltType(self) :
        self.verifyType( Debug_320Vis_300L0_Hadron_Hlt1_Oct09 )
        return          'Debug_320Vis_300L0_Hadron_Hlt1_Oct09'

    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        from Hlt1Lines.Hlt1CommissioningLines  import Hlt1CommissioningLinesConf
        from Hlt1Lines.Hlt1MuonLines           import Hlt1MuonLinesConf
        from Hlt1Lines.Hlt1HadronLines         import Hlt1HadronLinesConf
        from Hlt1Lines.Hlt1ElectronLines       import Hlt1ElectronLinesConf
        from Hlt1Lines.Hlt1PhotonLines         import Hlt1PhotonLinesConf
       from Hlt1Lines.Hlt1VeloLines           import Hlt1VeloLinesConf
        from Hlt2Lines.Hlt2CommissioningLines  import Hlt2CommissioningLinesConf

        return { Hlt1HadronLinesConf : {  'SingleHadron_PTCut' : 4800
                                         , 'HadSingle_IPCut' : 0.06 
                                         , 'HadMain_PTCut' : 250
                                         , 'HadCompanion_DZCut' : 1.83
                                         , 'HadDi_IPCut' : 0.05
                                         , 'HadCompanion_PTCut' : 750
                                         , 'HadCompanion_PointingCut' : 0.39
                                        }

                 }
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        return [ ]

        
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        return [ 'Hlt1SingleHadron'
                 ,'Hlt1DiHadron'
               ]
    
    
