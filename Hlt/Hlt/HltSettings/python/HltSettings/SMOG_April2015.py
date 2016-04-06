class SMOG_April2015(object):
    """
    Settings for SMOG run in April 2015

    @author Roel Aaij
    @date 2015-04-22
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
        # VDM TCK
        # return '0x0041'
        return '0x0046'

    def HltType(self) :
        self.verifyType( SMOG_April2015 )
        return          'SMOG_April2015'

    def ActiveHlt1Lines(self) :
        """Return a list of active Hlt1 lines."""
        lines = [
            # General lines
            'Hlt1Lumi',
            'Hlt1VeloClosingMicroBias',
            # Beam-gas lines
            'Hlt1BeamGasNoBeamBeam1', 'Hlt1BeamGasNoBeamBeam2', 'Hlt1BeamGasBeam1', 'Hlt1BeamGasBeam2',
            'Hlt1BeamGasCrossingForcedReco', 'Hlt1BeamGasCrossingForcedRecoFullZ',
            # Extra stuff for testing
            'Hlt1CalibTrackingKPi' , 'Hlt1CalibTrackingKK' , 'Hlt1CalibTrackingPiPi',
            'Hlt1IncPhi',
            'Hlt1TrackAllL0', 'Hlt1SingleMuonNoIP', 'Hlt1DiMuonHighMass',
            # HLT1 Passthrough
            'Hlt1ODINPhysics', 'Hlt1ODINTechnical', 'Hlt1Tell1Error'
        ]
        return lines

    def ActiveHlt2Lines(self) :
        """Return a list of active Hlt2 Lines."""
        return ['Hlt2PassThrough', 'Hlt2KS0_DDTurbo', 'Hlt2KS0_LLTurbo', 'Hlt2Lumi']

    def Thresholds(self) :
        """Return a dictionary of cuts."""

        from Hlt1Lines.Hlt1L0Lines             import Hlt1L0LinesConf
        from Hlt1Lines.Hlt1LumiLines           import Hlt1LumiLinesConf
        from Hlt1Lines.Hlt1MBLines             import Hlt1MBLinesConf
        from Hlt1Lines.Hlt1BeamGasLines        import Hlt1BeamGasLinesConf
        from Hlt1Lines.Hlt1CommissioningLines  import Hlt1CommissioningLinesConf
        from Hlt1Lines.Hlt1TrackLines          import Hlt1TrackLinesConf
        from Hlt1Lines.Hlt1MuonLines           import Hlt1MuonLinesConf

        thresholds = {
            Hlt1LumiLinesConf : {
                'Prescale'   : { 'Hlt1Lumi' :  1. },
                'Postscale'  : { 'Hlt1Lumi' :  1. },
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
            # PassThrough
            Hlt1CommissioningLinesConf : {
                'Prescale' : { 'Hlt1ODINPhysics'    : 1.,
                               'Hlt1ODINTechnical'  : 0., # @OnlineEnv.AcceptRate
                               'Hlt1Tell1Error'     : 0.},
                'ODINPhysics'   : '( ODIN_TRGTYP == LHCb.ODIN.PhysicsTrigger ) | ( ODIN_TRGTYP == LHCb.ODIN.TimingTrigger )',
                'ODINTechnical' : '( ODIN_TRGTYP == LHCb.ODIN.TechnicalTrigger ) | ( ODIN_TRGTYP == LHCb.ODIN.AuxiliaryTrigger ) | ( ODIN_TRGTYP == LHCb.ODIN.NonZSupTrigger ) | ( ODIN_TRGTYP == LHCb.ODIN.CalibrationTrigger )'
            },
            # 1TrackAllL0 runs on Random ODIN triggers with very loose cuts.
            Hlt1TrackLinesConf : {
                'AllL0_Velo_NHits'  :    5,
                'AllL0_Velo_Qcut'   :  999, #OFF
                'AllL0_TrNTHits'    :    0,   #PFF
                'AllL0_PT'          :   80.,
                'AllL0_P'           : 3000.,
                'AllL0_IPChi2'      :    4.0,
                'AllL0_TrChi2'      :    3.0,
                'AllL0_GEC'         : 'Loose',
                'ODINFilter'        : {'AllL0'  : 'jbit(ODIN_EVTTYP, 2)'},
                'L0Channels'        : {'AllL0'  : '',
                                       'Muon'   : ('Muon', )}
            },
            # Muon runs on Random ODIN triggers and DiMuon on L0. P and PT very low.
            Hlt1MuonLinesConf :     {
                'SingleMuonNoIP_P'          : 3000.,
                'SingleMuonNoIP_PT'         :   80.,
                'SingleMuonNoIP_TrChi2'     :    4.,
                'SingleMuonNoIP_TrNTHits'   :   10,
                'SingleMuonNoIP_Velo_NHits' :    0,
                'SingleMuonNoIP_Velo_Qcut'  :  999,
                'SingleMuonNoIP_GEC'        : 'Loose',
                'DiMuonHighMass_VxDOCA'     :    0.2,
                'DiMuonHighMass_VxChi2'     :   25.,
                'DiMuonHighMass_P'          : 3000.,
                'DiMuonHighMass_PT'         :   80.,
                'DiMuonHighMass_TrChi2'     :    3.,
                'DiMuonHighMass_M'          :  500.,
                'DiMuonHighMass_GEC'        : 'Loose',
                'ODINFilter'        : {'SingleMuonNoIP' : 'jbit(ODIN_EVTTYP, 2)'},
                'L0Channels'                : {
                    'SingleMuonNoIP' : None,
                    'DiMuonHighMass' : ( 'Muon', )}
            }
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

    def NanoBanks(self):
        return ['ODIN', 'HltLumiSummary', 'HltRoutingBits', 'DAQ', 'Velo',
                'L0DU', 'HltDecReports', 'HC']
