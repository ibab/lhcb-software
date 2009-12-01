class Physics_320Vis_300L0_10Hlt1_Aug09 :
    """
    Threshold settings for Hlt1 for 2010 conditions
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, M. Gandelman, G. Raven
    @date 2009-08-26
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
        self.verifyType( Physics_320Vis_300L0_10Hlt1_Aug09 )
        return          'Physics_320Vis_300L0_10Hlt1_Aug09'

    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        from Hlt1Lines.Hlt1CommissioningLines  import Hlt1CommissioningLinesConf
        from Hlt1Lines.Hlt1MuonLines           import Hlt1MuonLinesConf
        from Hlt1Lines.Hlt1HadronLines         import Hlt1HadronLinesConf
        from Hlt1Lines.Hlt1ElectronLines       import Hlt1ElectronLinesConf
        from Hlt1Lines.Hlt1PhotonLines         import Hlt1PhotonLinesConf
        from Hlt1Lines.Hlt1ExpressLines        import Hlt1ExpressLinesConf
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
                 , Hlt1PhotonLinesConf : { 'Pho_EtCut'         : 2800.0
                                          , 'Pho_IsPho'        : -0.082 
                                          , 'Track_IPCut'      : 0.15
                                          , 'Track_PtCut'      : 1250.
                                          , 'DiTrack_PtCut'    : 650
                                          }
                 , Hlt1ElectronLinesConf : { 'EleIP_PtCut'             : 2820.0
                                            , 'EleIP_IPCut'            : 0.13 
                                            , 'EleIPCompanion_PtCut'   : 2750
                                            , 'EleCompanion_PtCut'     : 1000
                                            }
                 , Hlt1MuonLinesConf : {  'DiMuon_SumPtCut' : 120.0
                                      , 'Muon_PtCut' : 1000
                                      , 'MuonIP_PtCut' : 1000
                                      , 'Muon_IPMinCut' : 0.05
                                      , 'DiMuon_DOCACut' : 1.0
                                      , 'DiMuon_MassCut' : 2500
                                      , 'DiMuonIP_MassCut' : 300
                                      , 'DiMuon_IPCut' : 0.08
                                      , 'Prescale' : { 'Hlt1MuTrack4JPsi' : 0. }
                                      , 'MuTrackMuPt' : 500
                                      , 'MuTrackMuIP' : 0.07
                                      , 'MuTrackTrPt' : 550
                                      , 'MuTrackTrIP' : 0.1
                                      , 'MuTrackDoca' : 0.2
                                      , 'MuTrackDZ' : 2.0
                                      , 'MuTrackDimuMass' : 1000
                                      , 'MuTrackPoint' : 0.4
                                      , 'MuTrackTrChi2' : 10
                                      }
                 , Hlt1ExpressLinesConf : {  'Prescale' : { '.*' : .000015  } }
                 , Hlt2CommissioningLinesConf : { 'Prescale' : { 'Hlt2PassThrough'  : 1.
                                                               , 'Hlt2Forward'      : 0. # first needs a check that /Hlt/Track/Long actually exists..
                                                               , 'Hlt2DebugEvent'   : 0.0001
                                                }              }

                 }
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        # return [ 'Hlt2PassThrough', 'Hlt2Transparent','Hlt2Forward','Hlt2DebugEvent']
        return [ 'Hlt2PassThrough', 'Hlt2Transparent','Hlt2DebugEvent']

        
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        return [ 'Hlt1SingleHadron',
                 'Hlt1DiHadron',
                 'Hlt1SoftDiHadron',
                 'Hlt1SingleMuonNoIPL0',
                 'Hlt1SingleMuonIPCL0',                    
                 'Hlt1DiMuonNoIPL0Di',
                 'Hlt1DiMuonNoIP2L0',
                 'Hlt1DiMuonNoIPL0Seg',
                 'Hlt1DiMuonIPCL0Di',
                 'Hlt1DiMuonIPC2L0',
                 'Hlt1DiMuonIPCL0Seg',
                 'Hlt1DiMuonNoPVL0Di',
                 'Hlt1DiMuonNoPV2L0',
                 'Hlt1DiMuonNoPVL0Seg',
                 'Hlt1SingleMuonNoIPL0',
                 'Hlt1SingleMuonIPCL0',
                 'Hlt1DiMuonNoIPL0Di',
                 'Hlt1DiMuonNoIP2L0',
                 'Hlt1DiMuonNoIPL0Seg',
                 'Hlt1DiMuonIPCL0Di',
                 'Hlt1DiMuonIPC2L0',
                 'Hlt1DiMuonIPCL0Seg',
                 'Hlt1DiMuonNoPVL0Di',
                 'Hlt1DiMuonNoPV2L0',
                 'Hlt1DiMuonNoPVL0Seg',
                 'Hlt1MuTrack',
                 'Hlt1MuTrackFitMu',
                 'Hlt1MuTrack4JPsi',
                 'Hlt1LumiNoBeam',
                 'Hlt1LumiBeamCrossing',
                 'Hlt1LumiBeam1',
                 'Hlt1LumiBeam2',
                 'Hlt1L0Muon',
                 'Hlt1L0DiMuon',
                 'Hlt1L0Muon,lowMult',
                 'Hlt1L0DiMuon,lowMult',
                 'Hlt1L0Electron',
                 'Hlt1L0Photon',
                 'Hlt1L0Hadron',
                 'Hlt1SingleElectron',
                 'Hlt1ElectronTrackWithIP',
                 'Hlt1ElectronTrackNoIP',
                 'Hlt1PhotonTrack',
                 'Hlt1PhotonFromEleTrack',
                 'Hlt1PhotonDiTrack',
                 'Hlt1PhotonFromEleDiTrack',
                 'Hlt1VeloASide',
                 'Hlt1VeloCSide',
                 'Hlt1AlignVelo',
                 'Hlt1XPress',
                 'Hlt1ODINPhysics',
                 'Hlt1ODINTechnical',
                 'Hlt1Tell1Error' ,
                 'Hlt1Incident']
    
    
