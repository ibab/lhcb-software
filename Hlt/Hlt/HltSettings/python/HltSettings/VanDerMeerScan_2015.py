class VanDerMeerScan_2015:
    """
    Settings for VDM scans in 2015.
    Derived from VanDerMeerScan_Jul12

    @author A. Weiden, R. Matev
    @date 2015-04-17
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

    def __init__(self) :
        self.NanoBanks = [
            'ODIN', 'HltLumiSummary', 'HltRoutingBits', 'DAQ', 'Velo',
            'L0DU', 'HltDecReports', 'HC',
        ]

    def L0TCK(self) :
        return '0x0041'

    def HltType(self) :
        self.verifyType( VanDerMeerScan_2015 )
        return          'VanDerMeerScan_2015'

    def ActiveHlt1Lines(self) :
        """Return a list of active Hlt1 lines."""
        lines = [
            # General lines
            'Hlt1Lumi',
            'Hlt1L0CALO', 'Hlt1L0PU', 'Hlt1MBMicroBiasVeloRateLimited', 'Hlt1VeloClosingMicroBias',
            # Beam-gas lines
            'Hlt1BeamGasNoBeamBeam1', 'Hlt1BeamGasNoBeamBeam2', 'Hlt1BeamGasBeam1', 'Hlt1BeamGasBeam2',
            'Hlt1BeamGasCrossingForcedReco', 'Hlt1BeamGasCrossingForcedRecoFullZ',
        ]
        return lines

    def ActiveHlt2Lines(self) :
        """Return a list of active Hlt2 Lines."""
        return ['Hlt2PassThrough', 'Hlt2Lumi', 'Hlt2Transparent']

    def Thresholds(self) :
        """Return a dictionary of cuts."""

        from Hlt1Lines.Hlt1L0Lines             import Hlt1L0LinesConf
        from Hlt1Lines.Hlt1LumiLines           import Hlt1LumiLinesConf
        from Hlt1Lines.Hlt1MBLines             import Hlt1MBLinesConf
        from Hlt1Lines.Hlt1BeamGasLines        import Hlt1BeamGasLinesConf

        thresholds = {
            Hlt1L0LinesConf : {
                'Prescale' : {
                    'Hlt1L0CALO' : 1.,
                    'Hlt1L0PU'   : 1.,
                },
                'Postscale' : {
                    'Hlt1L0CALO' : 'RATE(25)',
                    'Hlt1L0PU'   : 'RATE(25)'
                },
                'L0Channels' : [ 'PU', 'CALO' ],
            },
            Hlt1LumiLinesConf : {
                'Prescale'   : { 'Hlt1Lumi' :  1. },
                'Postscale'  : { 'Hlt1Lumi' :  1. },
            },
            Hlt1MBLinesConf : {
                'Postscale'  : { 'Hlt1MBMicroBias.*RateLimited' : 'RATE(100)' },
                'ODIN'       : { 'MicroBias'               : 'scale( ODIN_TRGTYP == LHCb.ODIN.LumiTrigger, RATE(2000,LoKi.Scalers.RandomPhasePeriodicLimiter) )'
                               , 'MicroBiasLowMultVelo'    : 'jbit( ODIN_EVTTYP,2 )'
                               , 'NoBias'                  : 'jbit( ODIN_EVTTYP,2 )'
                               , 'CharmCalibrationNoBias'  : 'jbit( ODIN_EVTTYP,2 )'
                               , 'NoBiasLeadingCrossing'   : 'jbit( ODIN_EVTTYP,14 )'}
            },
            Hlt1BeamGasLinesConf : {
                # Global behaviour settings
                'TrackingConf'          : 'FastVelo',
                'FitTracks'             : True,
                'PVFitter'              : 'LSAdaptPV3DFitter',
                'PVSeeding'             : 'PVSeed3DTool',
                'SplitVertices'         : True,
                'CreateGlobalSelection' : False,
                'Turbo'                 : False,
                'FullZVetoLumiTriggers' : False,
                'UseGEC'                : 'None',

                # Minimum number of tracks for the produced vertices (#tr/vtx > X)
                'VertexMinNTracks' : 9,

                # Luminous region exclusion range
                'BGVtxExclRangeMin' :  -250.,
                'BGVtxExclRangeMax' :   250.,

                # Take any L0 channel
                'L0Filter' : {
                    'NoBeamBeam1' : None,
                    'NoBeamBeam2' : None,
                    'Beam1'       : None,
                    'Beam2'       : None,
                    'BB'          : None,
                },

                # No L0 rate limiters, prescales or postscales for calibration!
                'L0RateLimit' : {
                    'NoBeamBeam1'     : None,
                    'NoBeamBeam2'     : None,
                    'Beam1'           : None,
                    'Beam2'           : None,
                    'ForcedReco'      : None,
                    'ForcedRecoFullZ' : None,
                },
                'Prescale' : {
                    'Hlt1BeamGasNoBeamBeam1'             : 1.,
                    'Hlt1BeamGasNoBeamBeam2'             : 1.,
                    'Hlt1BeamGasBeam1'                   : 1.,
                    'Hlt1BeamGasBeam2'                   : 1.,
                    'Hlt1BeamGasCrossingForcedReco'      : 1.,
                    'Hlt1BeamGasCrossingForcedRecoFullZ' : 1.,
                },
                'Postscale' : {
                    'Hlt1BeamGasNoBeamBeam1'             : 1.,
                    'Hlt1BeamGasNoBeamBeam2'             : 1.,
                    'Hlt1BeamGasBeam1'                   : 1.,
                    'Hlt1BeamGasBeam2'                   : 1.,
                    'Hlt1BeamGasCrossingForcedReco'      : 1.,
                    'Hlt1BeamGasCrossingForcedRecoFullZ' : 1.,
                },
            },
        }

        # HLT2 PassThrough
        from Hlt2Lines.Commissioning.Lines import CommissioningLines
        thresholds.update({
            CommissioningLines : {
                'Prescale'  : { 'Hlt2PassThrough' : 1 },
                'Postscale' : { 'Hlt2ErrorEvent'  : 'RATE(0.01)' }
            }
        })
        return thresholds

    def Streams(self):
        return {
            'FULL' : "HLT_PASS('Hlt2SMOGPhysicsDecision') | HLT_PASS('Hlt2LumiDecision')",
            'BEAMGAS' : "HLT_PASS_SUBSTR('Hlt1BeamGas') | HLT_PASS_RE('Hlt1L0.*Decision') | HLT_PASS_SUBSTR('Hlt1Lumi')",
            'VELOCLOSING' : "HLT_PASS_RE('Hlt1Velo.*Decision')",
        }
