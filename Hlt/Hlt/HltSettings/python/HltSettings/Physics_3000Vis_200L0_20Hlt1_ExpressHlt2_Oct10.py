class Physics_3000Vis_200L0_20Hlt1_ExpressHlt2_Oct10 :
    """
    Threshold settings for Hlt1 for 2010 conditions with pileup
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, M. Gandelman, G. Raven, J. Albrecht
    @date 2010-06-23
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
        # orig -> lower photon threshold
        #  29 -> 2A
        #  30 -> 2B
        #  31 -> 2C
        return '0x002b'

    def HltType(self) :
        self.verifyType( Physics_3000Vis_200L0_20Hlt1_ExpressHlt2_Oct10 )
        return          'Physics_3000Vis_200L0_20Hlt1_ExpressHlt2_Oct10'
    
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        from Hlt1Lines.Hlt1CommissioningLines  import Hlt1CommissioningLinesConf
        from Hlt1Lines.Hlt1MuonLines           import Hlt1MuonLinesConf
        from Hlt1Lines.Hlt1TrackLines          import Hlt1TrackLinesConf
        from Hlt1Lines.Hlt1HadronLines         import Hlt1HadronLinesConf
        from Hlt1Lines.Hlt1ElectronLines       import Hlt1ElectronLinesConf
        from Hlt1Lines.Hlt1PhotonLines         import Hlt1PhotonLinesConf
        from Hlt1Lines.Hlt1VeloLines           import Hlt1VeloLinesConf
        from Hlt1Lines.Hlt1MBLines             import Hlt1MBLinesConf
        from Hlt1Lines.Hlt1L0Lines             import Hlt1L0LinesConf
        from Hlt1Lines.Hlt1LumiLines           import Hlt1LumiLinesConf
        from Hlt1Lines.Hlt1BeamGasLines        import Hlt1BeamGasLinesConf
        from Hlt2Lines.Hlt2CommissioningLines  import Hlt2CommissioningLinesConf
        from Configurables import Hlt2ExpressLinesConf

        thresholds = { Hlt1TrackLinesConf : {   'AllL0_PT'      : 1450
                                            ,   'AllL0_P'       : 13300
                                            ,   'AllL0_IP'      : 0.110
                                            ,   'AllL0_IPChi2'  : 34
                                            ,   'AllL0_TrChi2'  : 3
                                            ,   'Muon_PT'       : 800 
                                            ,   'Muon_P'        : 8000 
                                            ,   'Muon_IP'       : 0.110
                                            ,   'Muon_IPChi2'   : 25
                                            ,   'Muon_TrChi2'   : 10                    
                                            ,   'Photon_PT'     : 800
                                            ,   'Photon_P'      : 8000
                                            ,   'Photon_IP'     : 0.13
                                            ,   'Photon_IPChi2' : 50
                                            ,   'Photon_TrChi2' : 5
                                            ,   'Velo_NHits'    : 9 # Minimum number of hits on a Velo track - 1
                                            ,   'Velo_Qcut'     : 3 # This - 1 is the maximum allowed difference between
                                         # the number of hits on a velo track and the
                                         # expected number of hits from the track's angle
                                         # and first measured point on the track                      
                                         } 
                     , Hlt1HadronLinesConf : { 'SingleHadron_PTCut'      : 2150
                                             , 'HadSingle_IPChi2Cut'     : 29.0
                                             , 'HadMain_PTCut'           : 1200
                                             , 'HadCompanion_DZCut'      : 2.0
                                             , 'HadDi_VDChi2Cut'         : 55.0
                                             , 'HadCompanion_PTCut'      : 500
                                             , 'HadCompanion_PointingCut': 0.5
                                             , 'HadMain_TrackFitChi2Cut' : 5. # 16.
                                             , 'HadDi_IP' : 0.07
                                             , 'Prescale' : { 'Hlt1SingleHadron' : 0
                                                            , 'Hlt1DiHadron'     : 0
                                                            }
                                             }
                       , Hlt1PhotonLinesConf : { 'Pho_EtCut'        : 4400.0
                                               , 'Pho_IsPho'        : -0.1
                                               , 'Track_IPCut'      : 0.19
                                               , 'Track_PtCut'      : 900
                                               , 'DiTrack_PtCut'    : 500
                                               , 'Prescale' : { 'Hlt1PhotonDiTrackFromEle' : 0
                                                              , 'Hlt1PhotonTrackFromEle'   : 0
                                                              , 'Hlt1PhotonTrack'          : 0
                                                              , 'Hlt1PhotonDiTrack'        : 0
                                                              }
                                               }
                       , Hlt1ElectronLinesConf : { 'Ele_EtCut'              : 3000
                                                 , 'Ele_EtCutSingleNoIP'    : 5000
                                                 , 'EleIP_PtCut'            : 1400
                                                 , 'EleIP_IPCut'            : 0.09
                                                 , 'EleIPCompanion_PtCut'   : 1000
                                                 , 'EleCompanion_PtCut'     : 1000
                                                 , 'DiEle_HighMassCut'      : -1 # only applied if >0
                                                 , 'Prescale' : { 'Hlt1SingleElectronWithIP' : 0
                                                                , 'Hlt1ElectronTrackWithIP'  : 0
                                                                }
                                                 }
                       , Hlt1MuonLinesConf :  { 'DiMuon_SumPtCut' :  560.0
                                              , 'Muon_DeltaPCut'  :    0  
                                              , 'Muon_PtCut' : 1800
                                              , 'Muon_highPtCut':5000#new line, cut sensible?
                                              , 'MuonIP_PtCut' : 850  
                                              , 'Muon_IPMinCut' : 0.09       
                                              , 'DiMuon_DOCACut' : 0.2
                                              , 'DiMuon_MassCut' : 2500     
                                              , 'DiMuonIP_MassCut' : 750    
                                              , 'DiMuon_IPCut' : 0.0        
                                              , 'Muon_FitChiCut' :  10.
                                              , 'Muon_FitMuChi2Cut' : 16.
                                              , 'MuTrackMuPt' : 800
                                              , 'MuTrackMuIP' : 0.05
                                              , 'MuTrackTrPt' : 600
                                              , 'MuTrackTrIP' : 0.05
                                              , 'MuTrackDoca' : 0.2
                                              , 'MuTrackDZ' : 1.0
                                              , 'MuTrackDimuMass' : 1000
                                              , 'MuTrackPoint' : 0.4
                                              , 'MuTrackTrChi2' : 16
                                              , 'MuTrackMuChi2' : 16
                                              , 'bmm_IP'        : 0.2
                                              , 'bmm_MuChi2'    : 16
                                              , 'bmm_fitChi2'   : 5
                                              , 'bmm_IPS'       : 81
                                              , 'bmm_pt'        :3000
                                              , 'Prescale' : { 'Hlt1DiMuonIPCL0Di'   : 0
                                                             , 'Hlt1DiMuonIPCL0Seg'  : 0
                                                             , 'Hlt1DiMuonNoIPL0Seg' : 0
                                                             , 'Hlt1DiMuonIPC2L0'    : 0
                                                             , 'Hlt1SingleMuonIPCL0' : 0
                                                             , 'Hlt1MuTrack'         : 0
                                                             }
                                              }

                       , Hlt1L0LinesConf :{ 'Prescale'  : { 'Hlt1L0Any'            : 0.000001
                                                          , 'Hlt1L0(?!Any).*'      : 0.000001
                                                          , 'Hlt1L0AnyRateLimited' : 1 }
                                            , 'Postscale' : { 'Hlt1L0(?!Any).*'    : 1
                                                            , 'Hlt1L0Any'          : 1
                                                            , 'Hlt1L0.*RateLimited': 'RATE(10)' }
                                            # WARNING: keys MUST match the ones in Hlt1L0LinesConf
                                            
                                           }
                       , Hlt1MBLinesConf : { 'Prescale' : {  'Hlt1MBMicroBias.*(?<!RateLimited)$' : 0.0001 
                                                            , 'Hlt1MBMicroBias.*RateLimited'       : 0.01} 
                                             , 'Postscale': { 'Hlt1MBMicroBias.*' : 1.
                                                              ,'Hlt1MBMicroBias.*(?<!RateLimited)$' : 1
                                                              # must veto 'RateLimited' at the end...
                                                              , 'Hlt1MBMicroBias.*RateLimited'       : 'RATE(50)'
                                                              # key MUST match the one in Hlt1MBLinesConf...,
                                                              }
                                             }
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
                       , Hlt1LumiLinesConf :     {  'Prescale'  : { 'Hlt1LumiLow.*(?<!RateLimited)$'   : 0.01  # assume remainder done in L0 -- above L0 does 1/1000
                                                                  , 'Hlt1LumiMid.*(?<!RateLimited)$' : 0.1 # assume remainder done in L0 -- above L0 does 1/1000
                                                                  }
                                                    , 'Postscale' : { 'Hlt1LumiLowNoBeamRateLimited'   : 'RATE(5)' 
                                                                      , 'Hlt1LumiLowBeam2RateLimited'  : 'RATE(10)' 
                                                                      , 'Hlt1LumiLowBeam1RateLimited'  : 'RATE(15)' 
                                                                      , 'Hlt1LumiLowBeamCrossingRateLimited': 'RATE(70)' 
                                                                      }
                                                    }
                       
                       , Hlt1BeamGasLinesConf : { 'ForcedInputRateLimit'  :  5000
                                                  , 'Prescale' :  { 'Hlt1BeamGasBeam1' :                1.0
                                                                    , 'Hlt1BeamGasBeam2' :                1.0
                                                                    , 'Hlt1BeamGasCrossing' :             1.0
                                                                    , 'Hlt1BeamGasCrossingForcedRZReco' : 1.0
                                                                    }
                                                  }
                       
                       , Hlt2CommissioningLinesConf : { 'Prescale' : { 'Hlt2PassThrough'    : 1.
                                                                       , 'Hlt2Forward'      : 0.
                                                                       , 'Hlt2DebugEvent'   : 0.0001
                                                                       }
                                                        }
                       
                       
                       , Hlt2ExpressLinesConf : { 'ExKSMassWin'           :  70
                                                  , 'ExLambdaMassWin'     :  40 
                                                  , 'ExLambdaMassWinWide' :  60
                                                  # to save CPU time, prescale lines to double of needed input
                                                  # Ks takes 1/4 of HLT1 --> 10Hz
                                                  # Lambda takes 1/20 --> 50Hz
                                                  , 'Prescale' :  { 'Hlt2ExpressKS' : 'RATE(400)'
                                                                    ,'Hlt2ExpressLambda' : 'RATE(400)'
                                                                    ,'Hlt2ExpressBeamHalo': 'RATE(200)'
                                                                    ,'Hlt2ExpressJPsiTagProbe': 'RATE(200)'
                                                                    }

                                                  }
                       }
        
##         # Hlt2
##         from DefaultHlt2Lines import DefaultHlt2Lines         
##         thresholds.update( DefaultHlt2Lines().Thresholds() )
        
        return thresholds




                       
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        hlt2 = [ 'Hlt2PassThrough', 'Hlt2Transparent','Hlt2Forward','Hlt2DebugEvent','Hlt2ErrorEvent','Hlt2BeamGasNonBBCrossing','Hlt2BeamGasBBCrossing']
        from Express_Hlt2_Sep10 import Express_Hlt2_Sep10
        hlt2.extend( Express_Hlt2_Sep10().ActiveHlt2Lines() )


##         from DefaultHlt2Lines import DefaultHlt2Lines 
##         list.extend( DefaultHlt2Lines().ActiveHlt2Lines() )
##         return list

        return hlt2
      

       
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        lines = [ 'Hlt1ODINTechnical', 'Hlt1Tell1Error' , 'Hlt1Incident' , 'Hlt1ErrorEvent' # , 'Hlt1NZSVelo'
                , 'Hlt1L0Any'
                , 'Hlt1Lumi' , 'Hlt1LumiMidBeamCrossing', 'Hlt1LumiLowBeam1' , 'Hlt1LumiLowBeam2' , 'Hlt1LumiLowBeamCrossing' , 'Hlt1LumiLowNoBeam'
                , 'Hlt1BeamGasBeam1', 'Hlt1BeamGasBeam2', 'Hlt1BeamGasCrossing', 'Hlt1BeamGasCrossingForcedRZReco' , 'Hlt1BeamGasCrossingLonelyBeam1' , 'Hlt1BeamGasCrossingLonelyBeam2'
                , 'Hlt1MBNoBias' , 'Hlt1MBMicroBiasRZVelo', 'Hlt1MBMicroBiasTStation'
                , 'Hlt1TrackAllL0', 'Hlt1TrackMuon', 'Hlt1TrackPhoton' # Add one track lines!
                , 'Hlt1SingleHadron'
                , 'Hlt1DiHadron' # , 'Hlt1DiHadronLTUnbiased'
                , 'Hlt1SingleMuonNoIPL0', 'Hlt1SingleMuonIPCL0'
                , 'Hlt1SingleMuonNoIPL0HighPT'  
                , 'Hlt1SingleMuon4BsMuMu', 'Hlt1DiMuon4BsMuMu'
                , 'Hlt1DiMuonNoIPL0Di',  'Hlt1DiMuonNoPVL0Di', 'Hlt1DiMuonIPCL0Di'
                , 'Hlt1DiMuonNoIP2L0',   'Hlt1DiMuonNoPV2L0',  'Hlt1DiMuonIPC2L0'
                , 'Hlt1DiMuonNoIPL0Seg' , 'Hlt1DiMuonNoPVL0Seg', 'Hlt1DiMuonIPCL0Seg'
                , 'Hlt1MuTrack'
                , 'Hlt1SingleElectronNoIP', 'Hlt1SingleElectronWithIP'
                , 'Hlt1ElectronTrackNoIP', 'Hlt1ElectronTrackWithIP'
                , 'Hlt1PhotonTrack', 'Hlt1PhotonTrackFromEle' , 'Hlt1PhotonDiTrack', 'Hlt1PhotonDiTrackFromEle'
                ]

        lines += [ i + 'RateLimited' for i in lines 
                                     if i.startswith('Hlt1MBMicroBias') 
                                     or  i.startswith('Hlt1L0Any') ]

        return lines 






