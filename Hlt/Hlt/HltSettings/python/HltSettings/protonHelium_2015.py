class protonHelium_2015:
   """
   Settings for proton Helium 2015 data taking
   Include the lines for BGI in parallel
   @author K. Dreimanis, k.dreimanis@liverpool.ac.uk
   @date 2015-09-01
   """
   def verifyType( self,ref ) :
      # verify self.ActiveLines is still consistent with
      # our types self.ActiveHlt2Lines and self.ActiveHlt1Lines...
      # so we can force that classes which inherit from us
      # and overrule either ActiveHlt.Lines also overrule
      # HltType...
      if ( self.ActiveHlt1Lines() != ref.ActiveHlt1Lines( self ) or
            self.ActiveHlt2Lines() != ref.ActiveHlt2Lines( self ) ) :
         raise RuntimeError( 'Must update HltType when modifying ActiveHlt.Lines()' )

   def __init__( self ) :
      self.NanoBanks = [
            'ODIN', 'HltLumiSummary', 'HltRoutingBits',
            'DAQ', 'Velo', 'L0DU', 'HltDecReports', 'HC',
            ]

      def L0TCK( self ) :
         return '0x024f'

   def HltType( self ) :
      self.verifyType( protonHelium_2015 )
      return 'protonHelium_2015'

   def ActiveHlt1Lines( self ) :
      """Return a list of active Hlt1 lines."""
      lines = [
            # General lines:
            'Hlt1Lumi',
            'Hlt1L0CALO',
            'Hlt1VeloClosingMicroBias',
            # proton Helium lines:
            'Hlt1MBMicroBiasVelo',           ## Beam1 Micro Bias, prescaled to get 200 M events
            'Hlt1DiMuonHighMass' ,           ## di-muon: active on beam1 beam2 and collisions
            'Hlt1MBNoBias' ,                 ## A little bit of complete no bias on beam gas
            'Hlt1MBNoBiasLeadingCrossing',   ## and on collisions
            'Hlt1MBMicroBiasLowMultVelo',
            # Calib Lines:
            'Hlt1CalibMuonAlignJPsi',
            'Hlt1CalibHighPTLowMultTrks',
            'Hlt1CalibRICHMirrorRICH1',
            'Hlt1CalibRICHMirrorRICH2',
            'Hlt1CalibTrackingKK',
            'Hlt1CalibTrackingKPi',
            'Hlt1CalibTrackingKPiDetached',
            'Hlt1CalibTrackingPiPi'
            ]
      return lines

   def ActiveHlt2Lines( self ) :
      """Return a list of active Hlt2 Lines."""
         return [ 'Hlt2PassThrough', 'Hlt2Lumi', 'Hlt2Transparent', 'Hlt2SMOGPhysics' ]

      def Thresholds( self ) :
         """Return a dictionary of cuts."""
         from Hlt1Lines.Hlt1L0Lines             import Hlt1L0LinesConf
         from Hlt1Lines.Hlt1LumiLines           import Hlt1LumiLinesConf
         from Hlt1Lines.Hlt1MBLines             import Hlt1MBLinesConf
         from Hlt1Lines.Hlt1MuonLines           import Hlt1MuonLinesConf
         from Hlt1Lines.HltCalibRICHMirrorLines import Hlt1CalibRICHMirrorLinesConf
         from Hlt1Lines.HltCalibTrackingLines   import Hlt1CalibTrackingLinesConf
         from GaudiKernel.SystemOfUnits         import MeV, GeV, mm


         thresholds = {
               Hlt1L0LinesConf : {
                  'Prescale' : { 
                     'Hlt1L0CALO' : 1.0
                     },
                  'Postscale' : {
                     'Hlt1L0CALO' : 'RATE(25)'
                     },
                  'L0Channels' : [ 'CALO' ],
                  },

               Hlt1LumiLinesConf : {
                  'Prescale' : {
                     'Hlt1Lumi' :  1.0
                     },
                  'Postscale' : {
                     'Hlt1Lumi' :  1.0
                     },
                  },

               Hlt1MBLinesConf : {
                  'MicroBiasOdin'            : '(ODIN_BXTYP == LHCb.ODIN.Beam1)',
                  'NoBiasOdin'               : 'jbit( ODIN_EVTTYP,5 ) | jbit( ODIN_EVTTYP,4)' ,
                  'NoBiasLeadingCrossingOdin': 'jbit( ODIN_EVTTYP,2 )' ,
                  'Prescale'                 : {
                     'Hlt1MBNoBias'                       : 1,
                     'Hlt1MBNoBiasLeadingCrossing'        : 1,
                     'Hlt1MBMicroBiasVelo'                : 1,
                     'Hlt1MBMicroBiasLowMultVelo'         : 1
                     }
                  },

               Hlt1MuonLinesConf : {
                  'DiMuonHighMass_VxDOCA'                : 0.2,
                  'DiMuonHighMass_VxChi2'                : 25,
                  'DiMuonHighMass_P'                     : 3000,
                  'DiMuonHighMass_PT'                    : 500,
                  'DiMuonHighMass_TrChi2'                : 3,
                  'DiMuonHighMass_M'                     : 2700,
                  'DiMuonHighMass_GEC'                   : 'Loose',
                  'CalibMuonAlignJpsi_ParticlePT'        : 800,     # MeV,
                  'CalibMuonAlignJpsi_ParticleP'         : 6000,    # MeV
                  'CalibMuonAlignJpsi_TrackCHI2DOF'      : 2,       # dimensionless
                  'CalibMuonAlignJpsi_CombMaxDaughtPT'   : 800,     # MeV
                  'CalibMuonAlignJpsi_CombAPT'           : 1500,    # MeV
                  'CalibMuonAlignJpsi_CombDOCA'          : 0.2,     # mm
                  'CalibMuonAlignJpsi_CombVCHI2DOF'      : 10,      # dimensionless
                  'CalibMuonAlignJpsi_CombVCHI2DOFLoose' : 10,      # dimensionless
                  'CalibMuonAlignJpsi_CombDIRA'          : 0.9,     # dimensionless
                  'CalibMuonAlignJpsi_CombTAU'           : 0,       # ps
                  'CalibMuonAlignJpsi_JpsiMassWinLoose'  : 150,     # MeV
                  'CalibMuonAlignJpsi_JpsiMassWin'       : 100,     # MeV
                  'L0Channels'                           : {
                     'DiMuonHighMass'     : ( 'Muon' ),
                     'CalibMuonAlignJPsi' : ( 'Muon' )
                     }
                  },

               Hlt1CalibRICHMirrorLinesConf : {
                     'Prescale' : {
                        'Hlt1CalibHighPTLowMultTrks' : 0.0001,
                        'Hlt1CalibRICHMirrorRICH1'   : 0.05,
                        'Hlt1CalibRICHMirrorRICH2'   : 0.143
                        },
                     'DoTiming'   : False,
                     'R2L_PT'     : 500.   * MeV,
                     'R2L_P'      : 40000. * MeV,
                     'R2L_MinETA' : 2.59,
                     'R2L_MaxETA' : 2.97,
                     'R2L_Phis'   : [ ( -2.69, -2.29 ), ( -0.85, -0.45 ), ( 0.45, 0.85 ), ( 2.29, 2.69 ) ],
                     'R2L_TrChi2' : 2.,
                     'R2L_MinTr'  : 0.5,
                     'R2L_GEC'    : 'Loose',
                     'R1L_PT'     : 500.   * MeV,
                     'R1L_P'      : 10000. * MeV,
                     'R1L_MinETA' : 1.6,
                     'R1L_MaxETA' : 2.04,
                     'R1L_Phis'   : [ ( -2.65, -2.30 ), ( -0.80, -0.50 ), ( 0.50, 0.80 ), ( 2.30, 2.65 ) ],
                     'R1L_TrChi2' : 2.,
                     'R1L_MinTr'  : 0.5,
                     'R1L_GEC'    : 'Loose',
                     'LM_PT'      : 500.   * MeV,
                     'LM_P'       : 1000.  * MeV,
                     'LM_TrChi2'  : 2.,
                     'LM_MinTr'   : 1,
                     'LM_MaxTr'   : 40,
                     'LM_GEC'     : 'Loose'
                     },

               Hlt1CalibTrackingLinesConf :  { 'ParticlePT'            : 600     # MeV
                     ,'ParticleP'             : 4000    # MeV
                     ,'TrackCHI2DOF'          : 2       # dimensionless
                     ,'CombMaxDaughtPT'       : 900     # MeV 900
                     ,'CombAPT'               : 1800    # MeV 1200
                     ,'CombDOCA'              : 0.1     # mm
                     ,'CombVCHI2DOF'          : 10      # dimensionless
                     ,'CombVCHI2DOFLoose'     : 15      # dimensionless
                     ,'CombDIRA'              : 0.99    # dimensionless
                     ,'CombTAU'               : 0.25    # ps
                     ,'D0MassWinLoose'        : 100     # MeV
                     ,'D0MassWin'             : 60      # MeV
                     ,'B0MassWinLoose'        : 200     # MeV
                     ,'B0MassWin'             : 150     # MeV
                     ,'D0DetachedDaughtsIPCHI2': 9      # dimensionless
                     ,'D0DetachedIPCHI2'       : 9      # dimensionless
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
               }
               # HLT2 PassThrough
         from Hlt2Lines.Commissioning.Lines import CommissioningLines
         thresholds.update( {
            CommissioningLines : {
               'Prescale'  : {
                  'Hlt2PassThrough' : 1
                  },
               'Postscale' : {
                  'Hlt2ErrorEvent'  : 'RATE(0.01)'
                  },
               'SMOGPhysics' : {
                  'HLT1' : "HLT_PASS('Hlt1DiMuonHighMassDecision') | HLT_PASS('Hlt1MBMicroBiasVeloDecision') | HLT_PASS('Hlt1MBNoBiasDecision') | HLT_PASS('Hlt1MBNoBiasLeadingCrossingDecision') | HLT_PASS('Hlt1MBMicroBiasLowMultVeloDecision')" }
               }
            } )
         return thresholds
