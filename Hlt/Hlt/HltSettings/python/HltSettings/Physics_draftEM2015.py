def __update_conf__( current, extra ) :
    for (conf,d) in extra.iteritems() :
        if conf not in current :
            current[conf] = d
            continue
        cur = current[conf]
        for (k,v) in d.iteritems() :
            if k not in cur :
                cur[k] = v
                continue
            if cur[k] == v : continue
            print 'Warning: potential collision detected: %s -- %s' % (conf,k)
            print 'current: %s' % cur[k]
            print 'request: %s' % v
            if type(cur[k])==dict :
                cur[k].update( v )
            else :
                cur[k] = v
            print 'result: %s' % cur[k]

class Physics_draftEM2015( object ):
    """
    Settings for 2015 early measurements.

    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS

    @author C. Linn
    @date 2012-07-15
    """

    __all__ = ( 'ActiveHlt1Lines', 'ActiveHlt2Lines', 'Thresholds', 'L0TCK' )

    def verifyType(self,ref) :
        # verify self.ActiveLines is still consistent with
        # our types self.ActiveHlt2Lines and self.ActiveHlt1Lines...
        # so we can force that classes which inherit from us
        # and overrule either ActiveHlt.Lines also overrule
        # HltType...
        if ( self.ActiveHlt1Lines() != ref.ActiveHlt1Lines(self)  or
             self.ActiveHlt2Lines() != ref.ActiveHlt2Lines(self) ) :
            raise RuntimeError( 'Must update HltType when modifying ActiveHlt.Lines()' )

    def L0TCK(self) :
        return '0x014E'

    def HltType(self) :
        self.verifyType( Physics_draftEM2015 )
        return          'Physics_draftEM2015'

    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """

        from Hlt1Lines.Hlt1TrackLines          import Hlt1TrackLinesConf
        from Hlt1Lines.Hlt1MuonLines           import Hlt1MuonLinesConf
        from Hlt1Lines.Hlt1L0Lines             import Hlt1L0LinesConf
        from Hlt1Lines.Hlt1MBLines             import Hlt1MBLinesConf
        from Hlt1Lines.Hlt1NBLines             import Hlt1NBLinesConf
        from Hlt1Lines.Hlt1HighMultLines       import Hlt1HighMultLinesConf
        from Hlt1Lines.Hlt1CEPLines            import Hlt1CEPLinesConf
        from Hlt1Lines.Hlt1BeamGasLines        import Hlt1BeamGasLinesConf
        from Hlt1Lines.Hlt1CommissioningLines  import Hlt1CommissioningLinesConf
        from Hlt1Lines.Hlt1CalibRICHMirrorLines     import Hlt1CalibRICHMirrorLinesConf
        from Hlt1Lines.Hlt1CalibTrackingLines  import Hlt1CalibTrackingLinesConf

        from Hlt2Lines.Commissioning.Lines      import CommissioningLines

        from GaudiKernel.SystemOfUnits import MeV, GeV, mm

        thresholds = { Hlt1TrackLinesConf :    {'AllL0_Velo_NHits'   : 9
                                               , 'AllL0_Velo_Qcut'   : 999 #OFF
                                               , 'AllL0_TrNTHits'    : 0   #PFF
                                               , 'AllL0_PT'          : 800.
                                               , 'AllL0_P'           : 3000.
                                               , 'AllL0_IPChi2'      : 10.0
                                               , 'AllL0_TrChi2'      : 3.0
                                               , 'AllL0_GEC'         : 'Loose'
                                               , 'Muon_TrNTHits'     : 0   #OFF
                                               , 'Muon_Velo_NHits'   : 0   #OFF
                                               , 'Muon_Velo_Qcut'    : 999 #OFF
                                               , 'Muon_PT'           : 800.
                                               , 'Muon_P'            : 3000.
                                               , 'Muon_IPChi2'       : 8.0
                                               , 'Muon_TrChi2'       : 3.0
                                               , 'Muon_GEC'          : 'Loose'
                                               , 'ODINFilter'        : {'AllL0'  : 'jbit( ODIN_EVTTYP, 2)'}
                                               , 'L0Channels'        : {'AllL0'  : '',
                                                                        'Muon'   : ('Muon',)}
                                               }

                     , Hlt1MuonLinesConf :     { 'SingleMuonHighPT_P'        : 3000
                                               , 'SingleMuonHighPT_PT'      : 4800
                                               , 'SingleMuonHighPT_TrChi2'  :    3.
                                               , 'SingleMuonHighPT_TrNTHits'   : 0 #OFF
                                               , 'SingleMuonHighPT_Velo_NHits' : 0 #OFF
                                               , 'SingleMuonHighPT_Velo_Qcut'  : 999 #OFF
                                               , 'SingleMuonHighPT_GEC'     : 'Loose'
                                               , 'DiMuonLowMass_VxDOCA'     :  0.2
                                               , 'DiMuonLowMass_VxChi2'     :   25.
                                               , 'DiMuonLowMass_P'          : 3000.
                                               , 'DiMuonLowMass_PT'         :  200.
                                               , 'DiMuonLowMass_TrChi2'     :    3.
                                               , 'DiMuonLowMass_M'          :    0.
                                               , 'DiMuonLowMass_IPChi2'     :    6.
                                               , 'DiMuonLowMass_GEC'        : 'Loose'
                                               , 'DiMuonHighMass_VxDOCA'    :  0.2
                                               , 'DiMuonHighMass_VxChi2'    :   25
                                               , 'DiMuonHighMass_P'         : 3000
                                               , 'DiMuonHighMass_PT'        :  500
                                               , 'DiMuonHighMass_TrChi2'    :    3
                                               , 'DiMuonHighMass_M'         : 2700
                                               , 'DiMuonHighMass_GEC'       : 'Loose'
                                               , 'CalibMuonAlignJpsi_ParticlePT'             : 800     # MeV
                                               , 'CalibMuonAlignJpsi_ParticleP'              : 6000    # MeV
                                               , 'CalibMuonAlignJpsi_TrackCHI2DOF'           : 2       # dimensionless
                                               , 'CalibMuonAlignJpsi_CombMaxDaughtPT'        : 800     # MeV
                                               , 'CalibMuonAlignJpsi_CombAPT'                : 1500    # MeV
                                               , 'CalibMuonAlignJpsi_CombDOCA'               : 0.2     # mm
                                               , 'CalibMuonAlignJpsi_CombVCHI2DOF'           : 10      # dimensionless
                                               , 'CalibMuonAlignJpsi_CombVCHI2DOFLoose'      : 10      # dimensionless
                                               , 'CalibMuonAlignJpsi_CombDIRA'               : 0.9     # dimensionless
                                               , 'CalibMuonAlignJpsi_CombTAU'                : 0.     # ps
                                               , 'CalibMuonAlignJpsi_JpsiMassWinLoose'         : 150     # MeV
                                               , 'CalibMuonAlignJpsi_JpsiMassWin'              : 100     # MeV
                                               , 'NoPVPassThrough_L0ChannelRe' : "(L0_CHANNEL_RE('.*lowMult')) & ~(L0_CHANNEL_RE('.*DiHadron,lowMult'))"
                                               , 'L0Channels'               : {
                                                   'SingleMuonHighPT' : ( 'Muon', ),
                                                   'DiMuonLowMass'    : ( 'Muon', ),
                                                   'DiMuonHighMass'   : ( 'Muon', ),
                                                   'CalibMuonAlignJpsi'    : ( 'Muon', )}

                                               }
                       , Hlt1CEPLinesConf : {    'SpdMult'    :   100.   # dimensionless, Spd Multiplicy cut
                                               , 'MaxNVelo'   :  1000    # dimensionless,
                                               , 'MinNVelo'   :     2    # dimensionless,
                                               , 'TrChi2'     :     5.   # dimensionless,
                                               , 'PT'         :   200.   # MeV
                                               , 'P'          :  1000.   # MeV
                                               , 'VeloCutLineL0Dependency' : '.*DiHadron,lowMult'
                                               , 'NoVeloCutLineL0Dependency':'.*DiHadron,lowMult'
                                               , 'NoBiasTriggerType' : 'LumiTrigger' # dimensionless
                                               , 'NoBiasBeamCrossingTypeVeto' : 'BeamCrossing' # dimensionless
                                               }

                     , Hlt1HighMultLinesConf: {'Prescale' : { 'Hlt1HighVeloMult'     : 0.001 } ,
                                                        'MinVeloHits'        : 2400,
                                                        'MaxVeloHits'        : 99999,
                                                        'nPVs'               :  1 ,
                                                        'MinVeloHits_PV'     : 2400,
                                                        'ODIN'               : 'jbit( ODIN_EVTTYP, 2)'
                                                   }

                     , Hlt1CalibRICHMirrorLinesConf :    { 'Prescale' : { 'Hlt1CalibHighPTLowMultTrks'     : 0.0001,
                                                                          'Hlt1CalibRICHMirrorRICH1'       : 0.05,
                                                                          'Hlt1CalibRICHMirrorRICH2'       : 0.143}
                                                          , 'DoTiming' : False
                                                         , 'R2L_PT'       : 500. * MeV
                                                         , 'R2L_P'        : 40000. * MeV
                                                         , 'R2L_MinETA'   : 2.59
                                                         , 'R2L_MaxETA'   : 2.97
                                                         , 'R2L_Phis'     : [(-2.69, -2.29 ), (-0.85, -0.45), (0.45, 0.85), (2.29, 2.69)]
                                                         , 'R2L_TrChi2'   : 2.
                                                         , 'R2L_MinTr'    : 0.5
                                                         , 'R2L_GEC'      : 'Loose'
                                                         , 'R1L_PT'       : 500. * MeV
                                                         , 'R1L_P'        : 10000. * MeV
                                                         , 'R1L_MinETA'   : 1.6
                                                         , 'R1L_MaxETA'   : 2.04
                                                         , 'R1L_Phis'     : [(-2.65, -2.30 ), (-0.80, -0.50), (0.50, 0.80), (2.30, 2.65)]
                                                         , 'R1L_TrChi2'   : 2.
                                                         , 'R1L_MinTr'    : 0.5
                                                         , 'R1L_GEC'      : 'Loose'
                                                         , 'LM_PT'    : 500. * MeV
                                                         , 'LM_P'     : 1000. * MeV
                                                         , 'LM_TrChi2': 2.
                                                         , 'LM_MinTr' : 1
                                                         , 'LM_MaxTr' : 40
                                                         , 'LM_GEC'   : 'Loose'
                                                         }
                       , Hlt1CalibTrackingLinesConf :  { 'ParticlePT'            : 600     # MeV
                                                        ,'ParticleP'             : 4000    # MeV
                                                        ,'TrackCHI2DOF'          : 4       # dimensionless
                                                        ,'CombMaxDaughtPT'       : 900     # MeV 900
                                                        ,'CombAPT'               : 1800    # MeV 1200
                                                        ,'CombDOCA'              : 0.2     # mm
                                                        ,'CombVCHI2DOF'          : 20      # dimensionless
                                                        ,'CombVCHI2DOFLoose'     : 15      # dimensionless
                                                        ,'CombDIRA'              : 0.99    # dimensionless
                                                        ,'CombTAU'               : 0.25    # ps
                                                        ,'D0MassWinLoose'        : 100     # MeV
                                                        ,'D0MassWin'             : 60      # MeV
                                                        ,'B0MassWinLoose'        : 200     # MeV
                                                        ,'B0MassWin'             : 150     # MeV
                                                        ,'D0DetachedDaughtsIPCHI2': 9      # dimensionless
                                                        ,'D0DetachedIPCHI2'       : 16      # dimensionless
                                                        ,'BsPhiGammaMassMinLoose': 3350    # MeV
                                                        ,'BsPhiGammaMassMaxLoose': 6900    # MeV
                                                        ,'BsPhiGammaMassMin'     : 3850    # MeV
                                                        ,'BsPhiGammaMassMax'     : 6400    # MeV
                                                        ,'PhiMassWinLoose'       : 50      # MeV
                                                        ,'PhiMassWin'            : 30      # MeV
                                                        ,'PhiMassWinTight'       : 20      # MeV
                                                        ,'PhiPT'                 : 1800    # MeV
                                                        ,'PhiPTLoose'            : 800     # MeV
                                                        ,'PhiSumPT'              : 3000    # MeV
                                                        ,'PhiIPCHI2'             : 16      # dimensionless
                                                        ,'B0SUMPT'               : 4000    # MeV
                                                        ,'B0PT'                  : 1000    # MeV
                                                        ,'GAMMA_PT_MIN'          : 2000    # MeV
                                                        ,'Velo_Qcut'             : 999     # OFF
                                                        ,'TrNTHits'              : 0       # OFF
                                                        ,'ValidateTT'            : False
                                                       }
                         , Hlt1L0LinesConf :     {  'Postscale' : { 'Hlt1L0AnyRateLimited'       : 'RATE(1)'
                                                                  , 'Hlt1L0AnyNoSPDRateLimited'  : 'RATE(1)'
                                                                  }
                                                  , 'Prescale' : { 'Hlt1L0AnyNoSPD'     : 0.01
                                                               }
                                                 }

                         , Hlt1BeamGasLinesConf : {'Prescale'  : { 'Hlt1BeamGasCrossingForcedRecoFullZ': 0
                                                                 , 'Hlt1BeamGasCrossingEnhancedBeam1': 0
                                                                 , 'Hlt1BeamGasCrossingEnhancedBeam2': 0
                                                                 }
                                                 , 'Postscale' : { 'Hlt1BeamGasNoBeamBeam1'            : 'RATE(0.5)'
                                                                 , 'Hlt1BeamGasNoBeamBeam2'            : 'RATE(0.5)'
                                                                 , 'Hlt1BeamGasBeam1'                  : 1
                                                                 , 'Hlt1BeamGasBeam2'                  : 1
                                                                 , 'Hlt1BeamGasCrossingEnhancedBeam1'  : 0
                                                                 , 'Hlt1BeamGasCrossingEnhancedBeam2'  : 0
                                                                 , 'Hlt1BeamGasCrossingForcedReco'     : 'RATE(0.5)'
                                                                 , 'Hlt1BeamGasCrossingForcedRecoFullZ': 0
                                                                 , 'Hlt1BeamGasCrossingParasitic'      : 'RATE(1)'
                                                                 , 'Hlt1BeamGasHighRhoVertices'        : 'RATE(4)'}
                                                   }

                         , Hlt1CommissioningLinesConf : { 'Postscale' : { 'Hlt1ErrorEvent'   : 'RATE(0.01)' }

                                                       }
                         , Hlt1NBLinesConf : {   'Prescale' : { 'Hlt1NoBiasPrescaled'   : 0.5 },
                                                 'NoBiasPrescaledOdin'    : '(ODIN_TRGTYP == LHCb.ODIN.LumiTrigger)'
                                             }

                        , Hlt1MBLinesConf :     { 'Prescale' : { 'Hlt1MBNoBias'                       : 0.00025
                                                               , 'Hlt1MBMicroBiasVelo'                : 0
                                                               , 'Hlt1MBMicroBiasTStation'            : 0
                                                               , 'Hlt1MBMicroBiasVeloRateLimited'     : 0
                                                               , 'Hlt1MBMicroBiasTStationRateLimited' : 0 }
                                                , 'Postscale'   : {'Hlt1MBNoBiasRateLimited'          : 'RATE(200.0)'}
                                                , 'NoBiasOdin'                   : 'jbit( ODIN_EVTTYP, 2)'
                                                , 'NoBiasLeadingCrossingOdin'    : 'jbit( ODIN_EVTTYP, 14)'
                                               }
                        ,CommissioningLines :  {'Prescale'    : {'Hlt2PassThrough' : 1.0,
                                                                 'Hlt2Forward'     : 0.00001,
                                                                 'Hlt2DebugEvent'  : 0.00001},
                                                'PassThrough' : {'HLT1' : "HLT_PASS_RE('^Hlt1(HighVeloMult|HighVeloMultSinglePV)Decision$')"},
                                                'NoBiasPassThrough' : {'HLT1' : "HLT_PASS('Hlt1NoBiasPrescaledDecision')"},
                                                'Postscale'   : {'Hlt2ErrorEvent'  : 'RATE(0.01)'}
                                                }

        }

        from CharmHad_EM2015 import CharmHad_EM2015
        __update_conf__(thresholds,  CharmHad_EM2015().Thresholds() )

        from SingleMuon_EM2015 import SingleMuon_EM2015
        __update_conf__(thresholds,  SingleMuon_EM2015().Thresholds() )

        from DiMuon_EM2015 import DiMuon_EM2015
        __update_conf__(thresholds,  DiMuon_EM2015().Thresholds() )

        from PID_EM2015 import PID_EM2015
        __update_conf__(thresholds,  PID_EM2015().Thresholds() )

        from TrackEffDiMuon_EM2015 import TrackEffDiMuon_EM2015
        __update_conf__(thresholds,  TrackEffDiMuon_EM2015().Thresholds() )

        from LowMult_EM2015 import LowMult_EM2015
        __update_conf__(thresholds,  LowMult_EM2015().Thresholds() )

        from Topo_EM2015 import Topo_EM2015
        __update_conf__(thresholds,  Topo_EM2015().Thresholds() )

        return thresholds


    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        hlt2 = ['Hlt2PassThrough', # passthrough of high mult
                'Hlt2Lumi','Hlt2DebugEvent','Hlt2ErrorEvent','Hlt2Transparent', # technical lines
                'Hlt2MBNoBiasLeadingCrossing'] # Special line for leading crossing

        from CharmHad_EM2015 import CharmHad_EM2015
        hlt2.extend(CharmHad_EM2015().ActiveHlt2Lines() )

        from SingleMuon_EM2015 import SingleMuon_EM2015
        hlt2.extend(SingleMuon_EM2015().ActiveHlt2Lines() )

        from DiMuon_EM2015 import DiMuon_EM2015
        hlt2.extend(DiMuon_EM2015().ActiveHlt2Lines() )

        from PID_EM2015 import PID_EM2015
        hlt2.extend(PID_EM2015().ActiveHlt2Lines() )

        from TrackEffDiMuon_EM2015 import TrackEffDiMuon_EM2015
        hlt2.extend(TrackEffDiMuon_EM2015().ActiveHlt2Lines() )

        from LowMult_EM2015 import LowMult_EM2015
        hlt2.extend(LowMult_EM2015().ActiveHlt2Lines() )

        from Topo_EM2015 import Topo_EM2015
        hlt2.extend(Topo_EM2015().ActiveHlt2Lines() )

        return hlt2

    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        lines =  [ 'Hlt1TrackAllL0', 'Hlt1TrackMuon'
                 , 'Hlt1SingleMuonHighPT'
                 , 'Hlt1DiMuonHighMass'
                 , 'Hlt1DiMuonLowMass'
                 , 'Hlt1HighVeloMult'
                 , 'Hlt1HighVeloMultSinglePV'
                 , 'Hlt1CEPVeloCut'
                 , 'Hlt1NoPVPassThrough'
                 , 'Hlt1NonBeamBeamNoBias'
                 , 'Hlt1MBNoBiasLeadingCrossing'                                 # nobias leading crossing
                 , 'Hlt1CalibRICHMirrorRICH1', 'Hlt1CalibRICHMirrorRICH2'        # RICH mirror alignment lines
                 , 'Hlt1CalibHighPTLowMultTrks' , 'Hlt1CalibTrackingKPiDetached' # Tracker alignment lines
                 , 'Hlt1CalibMuonAlignJpsi'                                      # Muon alignment line
                 , 'Hlt1Lumi'                                                    # lumi lines 'Hlt1LumiMidBeamCrossing' excluded
                 , 'Hlt1BeamGasNoBeamBeam1', 'Hlt1BeamGasNoBeamBeam2'
                 , 'Hlt1BeamGasBeam1', 'Hlt1BeamGasBeam2'
                 , 'Hlt1BeamGasCrossingEnhancedBeam1', 'Hlt1BeamGasCrossingEnhancedBeam2'
                 , 'Hlt1BeamGasCrossingForcedReco', 'Hlt1BeamGasCrossingForcedRecoFullZ'
                 , 'Hlt1BeamGasCrossingParasitic', 'Hlt1BeamGasHighRhoVertices'
                 , 'Hlt1MBNoBias'
                 , 'Hlt1MBNoBiasRateLimited'
                 , 'Hlt1L0Any','Hlt1L0AnyNoSPD'                                  # technical lines
                 , 'Hlt1ODINTechnical', 'Hlt1Tell1Error' , 'Hlt1ErrorEvent'
                 , 'Hlt1VeloClosingMicroBias'
            ]

        return lines
