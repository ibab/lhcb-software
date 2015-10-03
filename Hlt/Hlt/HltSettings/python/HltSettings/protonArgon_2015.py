from GaudiKernel.SystemOfUnits import GeV, MeV, mm
import importlib

def __get_conf__(folder, suffix):
    conf = folder + suffix
    module = importlib.import_module("HltSettings.{0}.{1}".format(folder, conf))
    return getattr(module, conf)()

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

class protonArgon_2015:
    """
    Settings for proton argon 2015 data taking
    Include the lines for BGI in parallel

    @author M. Kenzie + P. Robbe 
    @date 2015-09-30
    """

    def verifyType(self,ref) :
        # verify self.ActiveLines is still consistent with
        # our types self.ActiveHlt2Lines and self.ActiveHlt1Lines...
        # so we can force that classes which inherit from us
        # and overrule either ActiveHlt.Lines also overrule
        # HltType...
        if ( self.ActiveHlt1Lines() != ref.ActiveHlt1Lines(self) or
             self.ActiveHlt2Lines() != ref.ActiveHlt2Lines(self) ) :
            raise RuntimeError('Must update HltType when modifying ActiveHlt.Lines()')

    def L0TCK(self) :
        return '0x024F'

    def HltType(self) :
        self.verifyType( protonArgon_2015 )
        return          'protonArgon_2015'

    def ActiveHlt1Lines(self) :
        """Return a list of active Hlt1 lines."""
        lines = [
            # General lines
            'Hlt1Lumi',
            # proton Argon lines:
            'Hlt1MBMicroBiasVelo',  ## Beam1 Micro Bias, prescaled to get 200 M events
            'Hlt1DiMuonHighMass' ,  ## di-muon: active on beam1 beam2 and collisions
            'Hlt1MBNoBiasLeadingCrossing', ## and on collisions
            'Hlt1L0B1gas',
            'Hlt1L0B2gas',
            # SMOG Lines:
            'Hlt1SMOGKPi',
            'Hlt1SMOGKPiPi',
            'Hlt1SMOGKKPi',
            'Hlt1SMOGpKPi',
            'Hlt1SMOGGeneric',
            'Hlt1SMOGSingleTrack',
            # Calib lines
            'Hlt1CalibHighPTLowMultTrks',
            'Hlt1CalibRICHMirrorRICH1',
            'Hlt1CalibRICHMirrorRICH2',
            'Hlt1CalibTrackingKK',
            'Hlt1CalibTrackingKPi',
            'Hlt1CalibTrackingKPiDetached',
            'Hlt1CalibTrackingPiPi',
            'Hlt1CalibMuonAlignJpsi' 
        ]
        return lines

    def ActiveHlt2Lines(self) :
        """Return a list of active Hlt2 Lines."""
        lines = [
            'Hlt2PassThrough', 
            'Hlt2Lumi', 
            'Hlt2Transparent', 
            'Hlt2SMOGD02KPi',
            'Hlt2SMOGDpm2KPiPi',
            'Hlt2SMOGDs2KKPi',
            'Hlt2SMOGLc2KPPi',
            'Hlt2SMOGB2PiMu',
        ]

        # TrackEffDiMuon Calib lines
        conf = __get_conf__("TrackEffDiMuon", "_25ns_August2015")
        lines.extend(conf.ActiveHlt2Lines())

        # PID Calib lines
        conf = __get_conf__("PID","_25ns_August2015")
        lines.extend(conf.ActiveHlt2Lines())

        return lines

    def Thresholds(self) :
        """Return a dictionary of cuts."""

        from Hlt1Lines.Hlt1L0Lines             import Hlt1L0LinesConf
        from Hlt1Lines.Hlt1LumiLines           import Hlt1LumiLinesConf
        from Hlt1Lines.Hlt1MBLines             import Hlt1MBLinesConf
        from Hlt1Lines.Hlt1BeamGasLines        import Hlt1BeamGasLinesConf
        from Hlt1Lines.Hlt1MuonLines           import Hlt1MuonLinesConf
        from Hlt1Lines.Hlt1SMOGLines           import Hlt1SMOGLinesConf
        from Hlt1Lines.Hlt1CalibTrackingLines  import Hlt1CalibTrackingLinesConf

        thresholds = {
            Hlt1L0LinesConf : {
            'Prescale' : {
            'Hlt1L0CALO' : 1.,
            },
            'Postscale' : {
            'Hlt1L0CALO' : 'RATE(25)',
            },
            'L0Channels' : [ 'CALO' ],
            },
            Hlt1LumiLinesConf : {
            'Prescale'   : { 'Hlt1Lumi' :  1. },
            'Postscale'  : { 'Hlt1Lumi' :  1. },
            },
            Hlt1MBLinesConf : {
            'MicroBiasOdin': '(ODIN_BXTYP == LHCb.ODIN.Beam1)',
            'NoBiasOdin': 'jbit( ODIN_EVTTYP,5 ) | jbit( ODIN_EVTTYP,4)' ,
            'NoBiasLeadingCrossingOdin': 'jbit( ODIN_EVTTYP,2 )' ,
            'Prescale' : { 'Hlt1MBNoBias'                       : 0.00013,
                           'Hlt1MBNoBiasLeadingCrossing'        : 0.00075,
                           'Hlt1MBMicroBiasVelo'                : 0.1 }
            },
            Hlt1MuonLinesConf :     { 'DiMuonHighMass_VxDOCA'    :  0.2,
                                      'DiMuonHighMass_VxChi2'    :   25,
                                      'DiMuonHighMass_P'         : 3000,
                                      'DiMuonHighMass_PT'        :  500,
                                      'DiMuonHighMass_TrChi2'    :    3,
                                      'DiMuonHighMass_M'         : 2700,
                                      'DiMuonHighMass_GEC'       : 'Loose' ,
                                      'CalibMuonAlignJpsi_ParticlePT'             : 800,     # MeV
                                      'CalibMuonAlignJpsi_ParticleP'              : 6000,    # MeV
                                      'CalibMuonAlignJpsi_TrackCHI2DOF'           : 2,       # dimensionless
                                      'CalibMuonAlignJpsi_CombMaxDaughtPT'        : 800,     # MeV
                                      'CalibMuonAlignJpsi_CombAPT'                : 1500,    # MeV
                                      'CalibMuonAlignJpsi_CombDOCA'               : 0.2,     # mm
                                      'CalibMuonAlignJpsi_CombVCHI2DOF'           : 10,     # dimensionless
                                      'CalibMuonAlignJpsi_CombVCHI2DOFLoose'      : 10,      # dimensionless
                                      'CalibMuonAlignJpsi_CombDIRA'               : 0.9,     # dimensionless
                                      'CalibMuonAlignJpsi_CombTAU'                : 0.,     # ps
                                      'CalibMuonAlignJpsi_JpsiMassWinLoose'         : 150,     # MeV
                                      'CalibMuonAlignJpsi_JpsiMassWin'              : 100,     # MeV
                                      'L0Channels'               : {'DiMuonHighMass'     : ( 'Muon', ),
                                                                    'CalibMuonAlignJpsi' : ( 'Muon' )}
                                      },
            Hlt1BeamGasLinesConf : {
            # Global behaviour settings
            'TrackingConf'          : 'FastVelo',
            ### 'FitTracks'             : True,
            'PVFitter'              : 'LSAdaptPV3DFitter',
            'PVSeeding'             : 'PVSeed3DTool',
            'SplitVertices'         : True,
            'CreateGlobalSelection' : False,
            'Turbo'                 : False,
            'FullZVetoLumiTriggers' : False,
            'UseGEC'                : 'None',

            ##         # Minimum number of tracks for the produced vertices (#tr/vtx > X)
            'VertexMinNTracks' : 9,
            'FullZVertexMinNTracks' : 27,

            ##         # Luminous region exclusion range
            'BGVtxExclRangeMin' :  -250.,
            'BGVtxExclRangeMax' :   250.,

        ## ##         # Take any L0 channel
            'L0Filter' : {
            'Beam1'       : None,
            'Beam2'       : None,
            'BB'          : None,
            },

        ##     ##         # No L0 rate limiters, prescales or postscales for calibration!
            'L0RateLimit' : {
            'Beam1'           : None,
            'Beam2'           : None,
            'ForcedReco'      : None,
            'ForcedRecoFullZ' : None,
            },
            'Prescale' : {
            'Hlt1BeamGasBeam1'                   : 1.,
            'Hlt1BeamGasBeam2'                   : 1.,
            'Hlt1BeamGasCrossingForcedReco'      : 1.,
            'Hlt1BeamGasCrossingForcedRecoFullZ' : 1.,
            },
            'Postscale' : {
            'Hlt1BeamGasBeam1'                   : 1.,
            'Hlt1BeamGasBeam2'                   : 1.,
            'Hlt1BeamGasCrossingForcedReco'      : 1.,
            'Hlt1BeamGasCrossingForcedRecoFullZ' : 1.,
            },
            },
             Hlt1SMOGLinesConf :  { 'ParticlePT'            : 500     # MeV
                    ,'ParticleP'              : 3000    # MeV
                    ,'TrackCHI2DOF'           : 4       # dimensionless
                    ,'CombMaxDaughtPT'        : 500     # MeV
                    ,'CombDOCA'               : 1.0     # mm
                    ,'CombVCHI2DOF'           : 25      # dimensionless
                    ,'MassWinLoose'           : 250     # MeV
                    ,'MassWin'                : 150     # MeV
                    ,'GenericMassMinLoose'    : 0       # MeV
                    ,'GenericMassMin'         : 0       # MeV
                    ,'GenericMaxDaughtPT'     : 800     # MeV
                    ,'SingleTrackPT'          : 800    # MeV
                    ,'l0'                     : "L0_CHANNEL('B1gas')"
                    ,'odin'                   : "(ODIN_BXTYP == LHCb.ODIN.Beam1) | jbit(ODIN_EVTTYP,2)"
                   } ,
               Hlt1CalibRICHMirrorLinesConf : {
                     'Prescale' : {
                        'Hlt1CalibHighPTLowMultTrks' : 0.0001,
                        'Hlt1CalibRICHMirrorRICH1'   : 0.281,
                        'Hlt1CalibRICHMirrorRICH2'   : 1.0
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
        thresholds.update({
            CommissioningLines : {
                'Prescale'  : { 'Hlt2PassThrough' : 1 },
                'Postscale' : { 'Hlt2ErrorEvent'  : 'RATE(0.01)' },
                'SMOGPhysics' : { 'HLT1' : "HLT_PASS('Hlt1DiMuonHighMassDecision') | HLT_PASS('Hlt1MBMicroBiasVeloDecision') | HLT_PASS('Hlt1MBNoBiasDecision') | HLT_PASS('Hlt1MBNoBiasLeadingCrossingDecision')" }
            }
        })
        from GaudiKernel.SystemOfUnits import GeV, MeV, mm 
        from Hlt2Lines.SMOG.Lines import SMOGLines
        thresholds.update({
          SMOGLines : {
                 'Prescale'  : {},
                 'Postscale' : {},
                 'D02HH'     : {
                                  'TisTosSpec'               : "Hlt1SMOGSingleTrackDecision%TOS",
                                  'Pair_AMINDOCA_MAX'        : 0.10 * mm,
                                  'Trk_Max_APT_MIN'          : 500.0 * MeV, 
                                  'D0_VCHI2PDOF_MAX'         : 10.0,       # neuter
                                  'Comb_AM_MIN'              : 1775.0 * MeV, 
                                  'Comb_AM_MAX'              : 1955.0 * MeV, 
                                  'Trk_ALL_PT_MIN'           : 250.0 * MeV, 
                                  'Trk_ALL_P_MIN'            : 2.0  * GeV, 
                                  'Mass_M_MIN'               :  1784.0 * MeV,
                                  'Mass_M_MAX'               :  1944.0 * MeV,
                                },
                 'Dpm2HHH'    : {
                                 'TisTosSpec'               : "Hlt1SMOGSingleTrackDecision%TOS",
                                 'Trk_ALL_PT_MIN'           :  200.0 * MeV,
                                 'Trk_2OF3_PT_MIN'          :  400.0 * MeV,
                                 'Trk_1OF3_PT_MIN'          :  1000.0 * MeV,
                                 'VCHI2PDOF_MAX'            :  25.0,
                                 'ASUMPT_MIN'               :  0 * MeV,
                                 'AM_MIN'                   :  1779 * MeV,
                                 'AM_MAX'                   :  1959 * MeV,
                                 'Mass_M_MIN'               :  1789.0 * MeV,
                                 'Mass_M_MAX'               :  1949.0 * MeV,
                                },
                 'Ds2HHH'     : { 
                                 'TisTosSpec'               : "Hlt1SMOGSingleTrackDecision%TOS",
                                 'Trk_ALL_PT_MIN'           :  200.0 * MeV,
                                 'Trk_2OF3_PT_MIN'          :  400.0 * MeV,
                                 'Trk_1OF3_PT_MIN'          :  1000.0 * MeV,
                                 'VCHI2PDOF_MAX'            :  25.0,
                                 'ASUMPT_MIN'               :  0 * MeV,
                                 'AM_MIN'                   :  1879 * MeV,
                                 'AM_MAX'                   :  2059 * MeV,
                                 'Mass_M_MIN'               :  1889.0 * MeV,
                                 'Mass_M_MAX'               :  2049.0 * MeV,
                                },
                 'Lc2HHH'     : {
                                 'TisTosSpec'               : "Hlt1SMOGSingleTrackDecision%TOS",
                                 'Trk_ALL_PT_MIN'           :  200.0 * MeV,
                                 'Trk_2OF3_PT_MIN'          :  400.0 * MeV,
                                 'Trk_1OF3_PT_MIN'          :  1000.0 * MeV,
                                 'VCHI2PDOF_MAX'            :  25.0,
                                 'ASUMPT_MIN'               :  0 * MeV,
                                 'AM_MIN'                   :  2201 * MeV,
                                 'AM_MAX'                   :  2553 * MeV,
                                 'Mass_M_MIN'               :  2211.0 * MeV,
                                 'Mass_M_MAX'               :  2543.0 * MeV,
                                },
                 'B2PiMu' :     {
                                 'TisTosSpec'             : "Hlt1SMOGSingleTrackDecision%TOS",
                                 'AM_MIN'                 : 0 * MeV,
                                 'AM_MAX'                 : 1000.0 * GeV,
                                 'ASUMPT_MIN'             : 4.0 * GeV,
                                 'VCHI2PDOF_MAX'          : 30.0,
                                 'Mass_M_MIN'             : 0 * MeV,
                                 'Mass_M_MAX'             : 1000.0 * GeV
                                },
            }
        })
        
        # TrackEffDiMuon Calib thresholds
        conf = __get_conf__("TrackEffDiMuon", "_25ns_August2015")
        __update_conf__(thresholds, conf.Thresholds())

        # PID Calib thresholds
        conf = __get_conf__("PID","_25ns_August2015")
        __update_conf__(thresholds, conf.Thresholds())

        return thresholds


