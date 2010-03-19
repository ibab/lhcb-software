class Physics_MinBiasL0_PassThroughHlt_Feb10 :
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
        return '0x1810'

    def HltType(self) :
        self.verifyType( Physics_MinBiasL0_PassThroughHlt_Feb10 )
        return          'Physics_MinBiasL0_PassThroughHlt_Feb10'
    
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
        from Hlt1Lines.Hlt1MBLines             import Hlt1MBLinesConf
        from Hlt1Lines.Hlt1L0Lines             import Hlt1L0LinesConf
        from Hlt2Lines.Hlt2CommissioningLines  import Hlt2CommissioningLinesConf

        return { Hlt1HadronLinesConf : {  'SingleHadron_PTCut' : 1400
                                         , 'HadSingle_IPCut' : 0.14
                                         , 'HadMain_PTCut' : 1200
                                         , 'HadCompanion_DZCut' : 0.80
                                         , 'HadDi_IPCut' : 0.07
                                         , 'HadCompanion_PTCut' : 750
                                         , 'HadCompanion_PointingCut' : 0.5
                                         , 'HadMain_TrackFitChi2Cut' :  999.
                                        }
                 , Hlt1PhotonLinesConf : { 'Pho_EtCut'         : 2780.0
                                          , 'Pho_IsPho'        : -0.1
                                          , 'Track_IPCut'      : 0.18
                                          , 'Track_PtCut'      : 900
                                          , 'DiTrack_PtCut'    : 250
                                          }
                 , Hlt1ElectronLinesConf : { 'Ele_EtCut'               : 700.0
                                            , 'EleIP_PtCut'            : 1250
                                            , 'EleIP_IPCut'            : 0.12
                                            , 'EleIPCompanion_PtCut'   : 2300
                                            , 'EleCompanion_PtCut'     : 4750
                                            , 'DiEle_HighMassCut'      : -1 # only applied if >0
                                            }
                 , Hlt1MuonLinesConf : {  'DiMuon_SumPtCut' : 320.0
                                          , 'Muon_DeltaPCut'               :   -1 
                                          , 'Muon_TConfNSigmaX'            :   10 
                                          , 'Muon_TConfNSigmaY'            :   10 
                                          , 'Muon_TConfNSigmaTx'           :   10 
                                          , 'Muon_TConfNSigmaTy'           :   10 
                                          , 'Muon_PtCut' : 1300
                                          , 'MuonIP_PtCut' : 800
                                          , 'Muon_IPMinCut' : 0.08
                                          , 'DiMuon_DOCACut' : 2.0
                                          , 'DiMuon_MassCut' : 2500
                                          , 'DiMuonIP_MassCut' : 750
                                          , 'DiMuon_IPCut' : 0.0
                                          , 'Muon_FitChiCut' :  999.
                                          , 'Muon_FitMuChi2Cut' :  999.
                                          , 'MuTrackMuPt' : 800
                                          , 'MuTrackMuIP' : 0.03
                                          , 'MuTrackTrPt' : 750
                                          , 'MuTrackTrIP' : 0.01
                                          , 'MuTrackDoca' : 0.2
                                          , 'MuTrackDZ' : 1.00
                                          , 'MuTrackDimuMass' : 1000
                                          , 'MuTrackPoint' : 0.4
                                          , 'MuTrackTrChi2' : 999.
                                          , 'MuTrackMuChi2' :  999.
                                          , 'MuTrackMuPt4JPsi' : 1600
                                          , 'MuTrackTrPt4JPsi' : 400
                                          , 'MuTrackDoca4JPsi' : 0.1
                                          , 'MuTrackAngle4JPsiLow' : 0.02
                                          , 'MuTrackAngle4JPsiHigh' : 0.3
                                          , 'MuTrackDimuMass4JPsiLow' : 2900
                                          , 'MuTrackDimuMass4JPsiHigh' : -1 # <= 0 implies 'remove cut'
                                          , 'MuTrackMuChi24JPsi' : 999.
                                          , 'MuTrackTrChi24JPsi' : 999.
                                          }
                 , Hlt1L0LinesConf :      { 'Prescale'  : { 'Hlt1L0Any' : 1 }
                                          , 'Postscale' : { 'Hlt1L0(?!Any).*' : 0.000001
                                                          , 'Hlt1L0Any' : 1 
                                                          }
                                          }

                 , Hlt1MBLinesConf :      { 'Prescale' : { 'Hlt1MBMicroBias.*' : 1. } } # To be modified during clone
                 , Hlt1VeloLinesConf :    { 'ODIN'     : '( ODIN_BXTYP == LHCb.ODIN.BeamCrossing )' 
                                          , 'L0DU'     : "( L0_CHANNEL('CALO') )"
                                          , 'Prescale' : { 'Hlt1Velo.*' : 0. }       # To be modified during clone
                                          }
                 , Hlt1CommissioningLinesConf : { 'Prescale' :   { 'Hlt1ODINPhysics'    : 1
                                                                 , 'Hlt1ODINTechnical'  : 0.000001 # @OnlineEnv.AcceptRate
                                                                 , 'Hlt1Tell1Error'     : 1
                                                                 } 
                                                , 'Postscale' : { 'Hlt1Tell1Error' : 'RATE(1)' }
                                                }
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
        return [ 'Hlt1ODINTechnical', 'Hlt1Tell1Error'
               , 'Hlt1L0Any'
               , 'Hlt1L0SPD', 'Hlt1L0CALO', 'Hlt1L0MUON,minbias', 'Hlt1L0PU', 'Hlt1L0SPD40', 'Hlt1L0PU20'
               , 'Hlt1L0Electron', 'Hlt1L0Photon', 'Hlt1L0Hadron'
               , 'Hlt1L0Muon', 'Hlt1L0DiMuon', 'Hlt1L0Muon,lowMult', 'Hlt1L0DiMuon,lowMult'
               , 'Hlt1L0B1gas', 'Hlt1L0B2gas'
               , 'Hlt1VeloASide', 'Hlt1VeloCSide'
               , 'Hlt1Lumi'
               , 'Hlt1LumiLowBeam1' , 'Hlt1LumiLowBeam2' , 'Hlt1LumiLowBeamCrossing' , 'Hlt1LumiLowNoBeam'
               , 'Hlt1BeamGasBeam1', 'Hlt1BeamGasBeam2', 'Hlt1BeamGasCrossing', 'Hlt1BeamGasCrossingForcedRZReco'
               , 'Hlt1MBNoBias' # , 'Hlt1MBMiniBias'
               , 'Hlt1MBMicroBiasRZVelo', 'Hlt1MBMicroBiasTStation'
               , 'Hlt1SingleHadron',     'Hlt1SingleHadronViaT'
               , 'Hlt1DiHadron',         'Hlt1DiHadronViaT',    'Hlt1DiHadronSoft'
               , 'Hlt1SingleMuonNoIPL0', 'Hlt1SingleMuonIPCL0'
               , 'Hlt1DiMuonNoIPL0Di',  'Hlt1DiMuonNoPVL0Di', 'Hlt1DiMuonIPCL0Di'
               , 'Hlt1DiMuonNoIP2L0',   'Hlt1DiMuonNoPV2L0',  'Hlt1DiMuonIPC2L0'
               , 'Hlt1DiMuonNoIPL0Seg' , 'Hlt1DiMuonNoPVL0Seg', 'Hlt1DiMuonIPCL0Seg'
               , 'Hlt1MuTrack', 'Hlt1MuTrack4JPsi'
               #'Hlt1MuTrackFitMu',
               , 'Hlt1SingleElectronNoIP', 'Hlt1SingleElectronWithIP'
               , 'Hlt1ElectronTrackNoIP', 'Hlt1ElectronTrackWithIP'
               , 'Hlt1PhotonTrack', 'Hlt1PhotonTrackFromEle'
               , 'Hlt1PhotonDiTrack', 'Hlt1PhotonDiTrackFromEle'
               , 'Hlt1Incident'
               ]
