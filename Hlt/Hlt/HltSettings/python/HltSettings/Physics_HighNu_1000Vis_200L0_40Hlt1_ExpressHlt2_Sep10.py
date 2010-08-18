class Physics_HighNu_1000Vis_200L0_40Hlt1_ExpressHlt2_Sep10 :
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
        return '0x001F'

    def HltType(self) :
        self.verifyType( Physics_HighNu_1000Vis_200L0_40Hlt1_ExpressHlt2_Sep10 )
        return          'Physics_HighNu_1000Vis_200L0_40Hlt1_ExpressHlt2_Sep10'
    
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
        from Hlt1Lines.Hlt1LumiLines           import Hlt1LumiLinesConf
        from Hlt1Lines.Hlt1BeamGasLines        import Hlt1BeamGasLinesConf
        from Hlt2Lines.Hlt2CommissioningLines  import Hlt2CommissioningLinesConf
        from Configurables import Hlt2ExpressLinesConf

        thresholds = { Hlt1HadronLinesConf : {  'SingleHadron_PTCut'        : 2150
                                                , 'HadSingle_IPChi2Cut'     : 29.0
                                                , 'HadMain_PTCut'           : 1200
                                                , 'HadCompanion_DZCut'      : 2.0
                                                , 'HadDi_VDChi2Cut'         : 55.0
                                                , 'HadCompanion_PTCut'      : 500
                                                , 'HadCompanion_PointingCut': 0.5
                                                , 'HadMain_TrackFitChi2Cut' : 16.
                                        }
                       , Hlt1PhotonLinesConf : { 'Pho_EtCut'         : 2400.0
                                                 , 'Pho_IsPho'        : -0.1
                                                 , 'Track_IPCut'      : 0.22
                                                 , 'Track_PtCut'      : 900
                                                 , 'DiTrack_PtCut'    : 350
                                          }
                       , Hlt1ElectronLinesConf : { 'Ele_EtCut'                : 1320
                                                   , 'EleIP_PtCut'            : 1300
                                                   , 'EleIP_IPCut'            : 0.12
                                                   , 'EleIPCompanion_PtCut'   : 2400
                                                   , 'EleCompanion_PtCut'     : 4750
                                                   , 'DiEle_HighMassCut'      : -1 # only applied if >0
                                                   }
                       , Hlt1MuonLinesConf : {  'DiMuon_SumPtCut'                : 1000.
                                                , 'Muon_DeltaPCut'               :   -1 
                                                , 'Muon_TConfNSigmaX'            :   10 
                                                , 'Muon_TConfNSigmaY'            :   10 
                                                , 'Muon_TConfNSigmaTx'           :   10 
                                                , 'Muon_TConfNSigmaTy'           :   10 
                                                , 'Muon_PtCut'                   : 1350
                                                , 'MuonIP_PtCut'                 : 850
                                                , 'Muon_IPMinCut'                : 0.09
                                                , 'DiMuon_DOCACut'               : 1.5
                                                , 'DiMuon_MassCut'               : 2500
                                                , 'DiMuonIP_MassCut'             : 750
                                                , 'DiMuon_IPCut'                 : 0.0
                                                , 'Muon_FitChiCut'               : 50.
                                                , 'Muon_FitMuChi2Cut'            : 100.
                                                , 'MuTrackMuPt'                  : 800
                                                , 'MuTrackMuIP'                  : 0.05
                                                , 'MuTrackTrPt'                  : 600
                                                , 'MuTrackTrIP'                  : 0.05
                                                , 'MuTrackDoca'                  : 0.2
                                                , 'MuTrackDZ'                    : 1.00
                                                , 'MuTrackDimuMass'              : 1000
                                                , 'MuTrackPoint'                 : 0.4
                                                , 'MuTrackTrChi2'                : 16.
                                                , 'MuTrackMuChi2'                : 50. #this is the track fit chi2!
                                                , 'MuTrackMuPt4JPsi'             : 1300
                                                , 'MuTrackTrPt4JPsi'             : 400
                                          , 'MuTrackDoca4JPsi'                   : 0.1
                                          , 'MuTrackAngle4JPsiLow'               : 0.02
                                          , 'MuTrackAngle4JPsiHigh'              : 0.3
                                          , 'MuTrackDimuMass4JPsiLow'            : 2900
                                          , 'MuTrackDimuMass4JPsiHigh'           : -1 # <= 0 implies 'remove cut'
                                          , 'MuTrackMuChi24JPsi'                 : 50.
                                          , 'MuTrackTrChi24JPsi'                 : 16.
                                          }
                       , Hlt1L0LinesConf :{ 'Prescale'  : { 'Hlt1L0Any'            : 0.0001 
                                                            , 'Hlt1L0AnyRateLimited' : 1 }
                                            , 'Postscale' : { 'Hlt1L0(?!Any).*' : 0.000001
                                                              , 'Hlt1L0Any'            :  1
                                                              , 'Hlt1L0.*RateLimited' : 'RATE(10)' }
                                            # WARNING: key MUST match the one in Hlt1L0LinesConf
                                            
                                           }
                       , Hlt1MBLinesConf : { 'Prescale' : { 'Hlt1MBMicroBias.*' : 1.
                                                            ,'Hlt1MBMicroBias.*(?<!RateLimited)$' : 0.0001 
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
                       , Hlt1LumiLinesConf :     {  'Prescale'  : { 'Hlt1LumiLow.*(?<!RateLimited)$'   : 0.1 
                                                                    , 'Hlt1LumiMid.*(?<!RateLimited)$' : 1.
                                                                    }
                                                    , 'Postscale' : { 'Hlt1LumiLowNoBeamRateLimited'   : 'RATE(5)' 
                                                                      , 'Hlt1LumiLowBeam2RateLimited'  : 'RATE(10)' 
                                                                      , 'Hlt1LumiLowBeam1RateLimited'  : 'RATE(15)' 
                                                                      , 'Hlt1LumiLowBeamCrossingRateLimited': 'RATE(70)' 
                                                                      }
                                                    }
                       
                       , Hlt1BeamGasLinesConf : { 'ForcedInputRateLimit'  : 10000
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
                                                  , 'Prescale' :  {'Hlt2ExpressJPsiTagProbe': 'RATE(200)'}
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
        hlt2 = [ 'Hlt2PassThrough', 'Hlt2Transparent','Hlt2Forward','Hlt2DebugEvent','Hlt2ErrorEvent']
        from Express_Hlt2_Jan10 import Express_Hlt2_Jan10
        hlt2.extend( Express_Hlt2_Jan10().ActiveHlt2Lines() )

##         from DefaultHlt2Lines import DefaultHlt2Lines 
##         list.extend( DefaultHlt2Lines().ActiveHlt2Lines() )
##         return list

        return hlt2
      

       
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        lines = [ 'Hlt1ODINTechnical', 'Hlt1Tell1Error'
                 , 'Hlt1L0Any'
                 , 'Hlt1L0SPD', 'Hlt1L0CALO', 'Hlt1L0MUON,minbias', 'Hlt1L0PU', 'Hlt1L0SPD40', 'Hlt1L0PU20'
                 , 'Hlt1L0Electron', 'Hlt1L0Photon', 'Hlt1L0Hadron'
                 , 'Hlt1L0Muon', 'Hlt1L0DiMuon', 'Hlt1L0Muon,lowMult', 'Hlt1L0DiMuon,lowMult'
                 , 'Hlt1L0B1gas', 'Hlt1L0B2gas'
                 , 'Hlt1VeloASide', 'Hlt1VeloCSide'
                 , 'Hlt1Lumi'
                 , 'Hlt1LumiMidBeamCrossing'
                 , 'Hlt1LumiLowBeam1' , 'Hlt1LumiLowBeam2' , 'Hlt1LumiLowBeamCrossing' , 'Hlt1LumiLowNoBeam'
                 , 'Hlt1BeamGasBeam1', 'Hlt1BeamGasBeam2', 'Hlt1BeamGasCrossing', 'Hlt1BeamGasCrossingForcedRZReco'
                 , 'Hlt1MBNoBias'
                 #, 'Hlt1MBMicroBiasRZVelo', 'Hlt1MBMicroBiasTStation'
                 , 'Hlt1OneTrackAllL0', 'Hlt1OneTrackMuon', 'Hlt1OneTrackPhoton' #Add one track lines!
                 , 'Hlt1SingleHadron',     'Hlt1HadronMonConf1', 'Hlt1HadronMonConf2', 'Hlt1HadronMonComp'
                 , 'Hlt1DiHadron', 'Hlt1DiHadronLTUnbiased', 'Hlt1HadronMonVeloReco'
                 , 'Hlt1SingleMuonNoIPL0', 'Hlt1SingleMuonIPCL0'
                 , 'Hlt1DiMuonNoIPL0Di',  'Hlt1DiMuonNoPVL0Di', 'Hlt1DiMuonIPCL0Di'
                 , 'Hlt1DiMuonNoIP2L0',   'Hlt1DiMuonNoPV2L0',  'Hlt1DiMuonIPC2L0'
                 , 'Hlt1DiMuonNoIPL0Seg' , 'Hlt1DiMuonNoPVL0Seg', 'Hlt1DiMuonIPCL0Seg'
                 , 'Hlt1MuTrack', 'Hlt1MuTrack4JPsi'
                 , 'Hlt1SingleElectronNoIP', 'Hlt1SingleElectronWithIP'
                 , 'Hlt1ElectronTrackNoIP', 'Hlt1ElectronTrackWithIP'
                 , 'Hlt1PhotonTrack', 'Hlt1PhotonTrackFromEle'
                 , 'Hlt1PhotonDiTrack', 'Hlt1PhotonDiTrackFromEle'
                 , 'Hlt1Incident'
                 , 'Hlt1ErrorEvent'
                 ]

        lines += [ i + 'RateLimited' for i in lines 
                                     if i.startswith('Hlt1MBMicroBias') 
                                     or  i.startswith('Hlt1L0Any') ]

        return lines 






