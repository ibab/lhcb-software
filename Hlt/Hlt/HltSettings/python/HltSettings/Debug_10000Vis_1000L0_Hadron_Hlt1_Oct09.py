# Note: if you define activeLines, you MUST provide a unique 'HltType' for this 
#       set of activelines

class Debug_10000Vis_1000L0_Hadron_Hlt1_Oct09 :
    """
    Threshold settings for testing the Hlt1 hadron lines, nominal conditions
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author V. Gligorov
    @date 2009-11-2
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
        self.verifyType( Debug_10000Vis_1000L0_Hadron_Hlt1_Oct09 )
        return          'Debug_10000Vis_1000L0_Hadron_Hlt1_Oct09'

    def ActiveHlt2Lines(self) :
        """
        Returns a list of active Hlt2 lines
        """
        # return [ 'Hlt2PassThrough', 'Hlt2Transparent','Hlt2Forward','Hlt2DebugEvent']
        return [ 'Hlt2PassThrough', 'Hlt2Transparent','Hlt2DebugEvent']

    def ActiveHlt1Lines(self) :
        """
        Returns a list of active Hlt1 lines
        """
        return [ 'Hlt1SingleHadron',
                 'Hlt1DiHadron']
    
    

    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        from Hlt1Lines.HltCommissioningLines  import HltCommissioningLinesConf
        from Hlt1Lines.HltMuonLines           import HltMuonLinesConf
        from Hlt1Lines.HltHadronLines         import HltHadronLinesConf
        from Hlt1Lines.HltElectronLines       import HltElectronLinesConf
        from Hlt1Lines.HltPhotonLines         import HltPhotonLinesConf
        from Hlt1Lines.HltExpressLines        import HltExpressLinesConf
        from Hlt1Lines.HltVeloLines           import HltVeloLinesConf
        from Hlt2Lines.Hlt2CommissioningLines import Hlt2CommissioningLinesConf
    
        return { HltHadronLinesConf : { 'SingleHadron_PTCut' : 4450
                                        , 'HadMain_IPCut'      : 0.09
                                        , 'HadMain_PTCut'      : 1600
                                        , 'HadCompanion_DZCut' : 0.05
                                        , 'HadCompanion_IPCut' : 0.13
                                        , 'HadCompanion_PTCut' : 950
                                        , 'HadCompanion_PointingCut' : 1.0 
                                        }
                 , HltPhotonLinesConf : { 'Pho_EtCut'          : 2800.0
                                          , 'Track_PtCut'        : 650
                                          , 'Track_IPCut3D'      : 0.15
                                          , 'Pho_IsPho'          : -0.082 
                                          }
                 , HltElectronLinesConf : { 'SingleEle_PtCut'    : 2820.0
                                            , 'Compan_PtCut'       : 2750
                                            , 'Ele_IPCut'          : 0.13 
                                            }
                 , HltMuonLinesConf : { 'DiMuon_SumPtCut'    : 1480.0
                                        , 'Muon_PtCut'         : 6200
                                        , 'MuonIP_PtCut'       : 1800
                                        , 'Muon_IPMinCut'      : 0.07
                                        , 'DiMuon_DOCACut'     : 1.6
                                        , 'DiMuon_MassCut'     : 2500
                                        , 'DiMuonIP_MassCut'   : 300
                                        , 'DiMuon_IPCut'       : 0.08
                                        , 'Prescale' : { 'Hlt1MuTrack4JPsi' : 0. }
                                        , 'MuTrackMuPt'        : 950
                                        , 'MuTrackMuIP'        : 0.07
                                        , 'MuTrackTrPt'        : 550
                                        , 'MuTrackTrIP'        : 0.1
                                        , 'MuTrackDoca'        : 0.2
                                        , 'MuTrackDZ'          : 2.0
                                        , 'MuTrackDimuMass'    : 1000
                                        , 'MuTrackPoint'       : 0.4
                                        , 'MuTrackTrChi2'      : 10 
                                        }
                 , HltExpressLinesConf : {  'Prescale' : { '.*' : .000015  } }
                 , Hlt2CommissioningLinesConf : { 'Prescale' : { 'Hlt2PassThrough'  : 1.
                                                               , 'Hlt2Forward'      : 0. # first needs a check that /Hlt/Track/Long actually exists..
                                                               , 'Hlt2DebugEvent'   : 0.0001
                                                }              }
                 }
    
