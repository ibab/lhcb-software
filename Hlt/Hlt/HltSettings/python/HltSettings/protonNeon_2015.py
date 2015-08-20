class protonNeon_2015:
    """
    Settings for proton neon 2015 data taking
    Include the lines for BGI in parallel

    @author P. Robbe
    @date 2015-08-17
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
        return '0x024f'

    def HltType(self) :
        self.verifyType( protonNeon_2015 )
        return          'protonNeon_2015'

    def ActiveHlt1Lines(self) :
        """Return a list of active Hlt1 lines."""
        lines = [
            # General lines
            'Hlt1Lumi',
            'Hlt1L0CALO', 'Hlt1VeloClosingMicroBias',
            # Beam-gas lines
            'Hlt1BeamGasBeam1', 'Hlt1BeamGasBeam2', 
            'Hlt1BeamGasCrossingForcedReco', 'Hlt1BeamGasCrossingForcedRecoFullZ',
            # proton Neon lines:
            'Hlt1MBMicroBiasVelo',  ## Beam1 Micro Bias, prescaled to get 200 M events
            'Hlt1DiMuonHighMass' ,  ## di-muon: active on beam1 beam2 and collisions
            'Hlt1MBNoBias' , ## A little bit of complete no bias on beam gas
            'Hlt1MBNoBiasLeadingCrossing' ## and on collisions
        ]
        return lines

    def ActiveHlt2Lines(self) :
        """Return a list of active Hlt2 Lines."""
        return ['Hlt2PassThrough', 'Hlt2Lumi', 'Hlt2Transparent', 'Hlt2SMOGPhysics']

    def Thresholds(self) :
        """Return a dictionary of cuts."""

        from Hlt1Lines.Hlt1L0Lines             import Hlt1L0LinesConf
        from Hlt1Lines.Hlt1LumiLines           import Hlt1LumiLinesConf
        from Hlt1Lines.Hlt1MBLines             import Hlt1MBLinesConf
        from Hlt1Lines.Hlt1BeamGasLines        import Hlt1BeamGasLinesConf
        from Hlt1Lines.Hlt1MuonLines           import Hlt1MuonLinesConf

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
                                      'L0Channels'               : {
            'DiMuonHighMass'   : ( 'Muon', ),
            }
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
        return thresholds
