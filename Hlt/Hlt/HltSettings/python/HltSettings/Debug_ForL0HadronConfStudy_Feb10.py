class Debug_ForL0HadronConfStudy_Feb10 :
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
        return '0x1309'

    def HltType(self) :
        self.verifyType( Debug_ForL0HadronConfStudy_Feb10 )
        return          'Debug_ForL0HadronConfStudy_Feb10'

    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        from Hlt1Lines.Hlt1CommissioningLines  import Hlt1CommissioningLinesConf
        from Hlt1Lines.Hlt1MuonLines           import Hlt1MuonLinesConf
        from Hlt1Lines.Hlt1HadronViaTLines     import Hlt1HadronViaTLinesConf
	from Hlt1Lines.Hlt1L0Lines             import Hlt1L0LinesConf
        from Hlt1Lines.Hlt1ElectronLines       import Hlt1ElectronLinesConf
        from Hlt1Lines.Hlt1PhotonLines         import Hlt1PhotonLinesConf
        from Hlt1Lines.Hlt1VeloLines           import Hlt1VeloLinesConf
        from Hlt2Lines.Hlt2CommissioningLines  import Hlt2CommissioningLinesConf

        return { Hlt1HadronViaTLinesConf : { 'L0Channel' : "CALO"
					 , 'HadViaT_ETCut' : 100. 
       					 , 'SingleHadronViaT_PTCut' : 4800
                                         , 'HadViaTSingle_IPCut' : 0.06 
                                         , 'HadViaTMain_PTCut' : 250
                                         , 'HadViaTCompanion_DZCut' : 1.83
                                         , 'HadViaTDi_IPCut' : 0.05
                                         , 'HadViaTCompanion_PTCut' : 750
                                         , 'HadViaTCompanion_PointingCut' : 0.39
                                        },
		Hlt1L0LinesConf : {'Postscale' : { 'Hlt1L0(?!Any).*' : 1.00000 } # set new default Postscale for these lines!
               			 , 'Prescale'  : { 'Hlt1L0Any'       : 1.00000 }}

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
        return [ 'Hlt1SingleHadronViaT'
                 ,'Hlt1DiHadronViaT', 'Hlt1L0CALO','Hlt1L0MUON','Hlt1L0Any'
               ]
    
    
