class Physics_pA2013 :
    """
    Threshold settings for the pilot pA run in September 2012
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author Johannes Albrecht
    @date 2012-08-27
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

    def HltType(self) :  
        self.verifyType( Physics_pA2013 )
        return          'Physics_pA2013'
    
    def __init__(self) :
#        self.StripEndSequence = ['']
        self.NanoBanks = ['ODIN', 'HltLumiSummary', 'HltRoutingBits', 'DAQ', 'Velo']

    def L0TCK(self) :
        return '0x0050'

    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        return  [ 'Hlt2PassThrough','Hlt2Transparent', 'Hlt2Lumi' ]
   
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        return [
            'Hlt1TrackAllL0', 'Hlt1TrackMuon', 'Hlt1TrackAllL0Tight', 'Hlt1TrackPhoton',
            'Hlt1SingleMuonNoIP', 'Hlt1SingleMuonHighPT',
            'Hlt1DiMuonLowMass', 'Hlt1DiMuonHighMass',
            'Hlt1NoPVPassThrough',
            'Hlt1SingleElectronNoIP',            
            'Hlt1L0Any',
            #'Hlt1ActivitySPD', 
            #'Hlt1MicroBiasVeloLowMult',
            'Hlt1MBNoBias',
            'Hlt1Lumi',
            'Hlt1GECPassSPD',
            'Hlt1GECPassVelo',
            'Hlt1GECPassIT',
            'Hlt1GECPassOT'
            ]

    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        from Hlt1Lines.Hlt1TrackLines          import Hlt1TrackLinesConf
        from Hlt1Lines.Hlt1MuonLines           import Hlt1MuonLinesConf
        from Hlt1Lines.Hlt1ElectronLines       import Hlt1ElectronLinesConf
        from Hlt1Lines.Hlt1L0Lines             import Hlt1L0LinesConf
        from Hlt1Lines.Hlt1MBLines             import Hlt1MBLinesConf
        from Hlt1Lines.Hlt1LumiLines           import Hlt1LumiLinesConf
        from Hlt1Lines.Hlt1BeamGasLines        import Hlt1BeamGasLinesConf
        from Hlt1Lines.Hlt1CommissioningLines  import Hlt1CommissioningLinesConf
        from Hlt2Lines.Hlt2CommissioningLines  import Hlt2CommissioningLinesConf

        thresholds = { Hlt1TrackLinesConf :    { 'AllL0Tight_PT'         :  1300
                                               , 'AllL0Tight_P'          :  3000
                                               , 'AllL0Tight_IP'         :  0.100
                                               , 'AllL0Tight_IPChi2'     :  16  
                                               , 'AllL0Tight_TrChi2'     :  1.5 
                                               , 'AllL0Tight_GEC'        : 'Loose'
                                               , 'AllL0Tight_Velo_NHits' : 9 
                                               , 'AllL0Tight_Velo_Qcut'  : 3   
                                               , 'AllL0Tight_ValidateTT' : True 
                                               , 'AllL0_PT'         :  1200
                                               , 'AllL0_P'          :  3000
                                               , 'AllL0_IP'         :  0.100
                                               , 'AllL0_IPChi2'     :  16
                                               , 'AllL0_TrChi2'     :  2.0
                                               , 'AllL0_GEC'        : 'Loose'
                                               , 'AllL0_Velo_NHits' : 9 
                                               , 'AllL0_Velo_Qcut'  : 3  
                                               , 'AllL0_ValidateTT' : True 
                                               , 'Muon_PT'       :   300 
                                               , 'Muon_P'        :  1000 
                                               , 'Muon_IP'       :     0.100
                                               , 'Muon_IPChi2'   :    16
                                               , 'Muon_TrChi2'   :     2.5  
                                               , 'Muon_GEC'      : 'Loose'
                                               , 'Muon_ValidateTT' : False
                                               , 'Muon_L0Channels' : 'Muon,DiMuon,MuonNoSPD,DiMuonNoSPD' 
                                               , 'Photon_PT'     :   800
                                               , 'Photon_P'      :  2500
                                               , 'Photon_IP'     :     0.100
                                               , 'Photon_IPChi2' :    16
                                               , 'Photon_TrChi2' :     2.0
                                               , 'Photon_L0Channels' : 'Photon,Electron' 
                                               , 'Photon_GEC'        : 'Loose'
                                               , 'Photon_ValidateTT' : True
                                               , 'Prescale'          : {'Hlt1TrackAllL0'                   : 1.0, 
                                                                        'Hlt1TrackAllL0Tight'              : 1.0,
                                                                        'Hlt1TrackForwardPassThrough'      : 0,
                                                                        'Hlt1TrackForwardPassThroughLoose' : 0}
 
                                               }
                       , Hlt1ElectronLinesConf : { 'SingleElectronNoIP_P'          : 20000
                                               , 'SingleElectronNoIP_PT'         : 10000
                                               , 'SingleElectronNoIP_TrChi2'     :     3
                                               , 'SingleElectronNoIP_TrNTHits'   :     0 #OFF
                                               , 'SingleElectronNoIP_Velo_NHits' :     0 #OFF
                                               , 'SingleElectronNoIP_Velo_Qcut'  :   999 #OFF
                                               , 'SingleElectronNoIP_GEC'        : 'Loose'
                                               , 'L0Channels': { 'SingleElectronNoIP' : ( 'Electron', ) }
                                               }
                      , Hlt1MuonLinesConf :     { 'SingleMuonHighPT_P'      : 3000
                                               , 'SingleMuonHighPT_PT'      : 4800
                                               , 'SingleMuonHighPT_TrChi2'  :    3.
                                               , 'SingleMuonHighPT_GEC'     : 'Loose'
                                               , 'SingleMuonNoIP_P'         : 1000.
                                               , 'SingleMuonNoIP_PT'        :  600.
                                               , 'SingleMuonNoIP_TrChi2'    :    3.
                                               , 'SingleMuonNoIP_GEC'       : 'Loose'
                                               , 'DiMuonLowMass_VxDOCA'     :  0.2
                                               , 'DiMuonLowMass_VxChi2'     :   25
                                               , 'DiMuonLowMass_P'          :    0
                                               , 'DiMuonLowMass_PT'         :    0
                                               , 'DiMuonLowMass_TrChi2'     :    3
                                               , 'DiMuonLowMass_M'          :    0.
                                               , 'DiMuonLowMass_IPChi2'     :    6.
                                               , 'DiMuonLowMass_GEC'        : 'Loose'
                                               , 'DiMuonHighMass_VxDOCA'    :  0.2
                                               , 'DiMuonHighMass_VxChi2'    :   25
                                               , 'DiMuonHighMass_P'         : 1000
                                               , 'DiMuonHighMass_PT'        :  300
                                               , 'DiMuonHighMass_TrChi2'    :    3
                                               , 'DiMuonHighMass_M'         : 2700
                                               , 'DiMuonHighMass_GEC'       : 'Loose'
                                               , 'MultiMuonNoIP_P'          : 2000
                                               , 'MultiMuonNoIP_PT'         :  300
                                               , 'MultiMuonNoIP_TrChi2'     :    3.
                                               , 'MultiMuonNoIP_GT'         :  2.5
                                               , 'MultiMuonNoIP_GEC'        : 'Loose'
                                               ,'L0Channels'               : {
                                                   'SingleMuonHighPT' : ( 'Muon', 'MuonNoSPD'),
                                                   'SingleMuonNoIP'   : ( 'Muon', 'MuonNoSPD'),
                                                   'DiMuonLowMass'    : ( 'Muon', 'MuonNoSPD', 'DiMuon', 'DiMuonNoSPD' ),
                                                   'DiMuonHighMass'   : ( 'Muon', 'MuonNoSPD', 'DiMuon', 'DiMuonNoSPD' ),
                                                   'MultiMuonNoIP'    : ( 'Muon', 'MuonNoSPD', 'DiMuon', 'DiMuonNoSPD' ) }

                                               , 'Prescale'                 : { 'Hlt1SingleMuonNoIP' : 1.0,
                                                                                'Hlt1MultiMuonNoIP'  : 0.0 }
                                               }
                       , Hlt1L0LinesConf :     {  'Postscale' : { 'Hlt1L0AnyRateLimited'       : 'RATE(1)'
                                                                , 'Hlt1L0AnyNoSPDRateLimited'  : 'RATE(1)'
                                                                }
                                               ,  'Prescale' : { 'Hlt1L0HighSumETJet' : 1 
                                                               , 'Hlt1L0AnyNoSPD'     : 0.01
                                                               }
                                               }
                       , Hlt1BeamGasLinesConf : {
                                         'Prescale'  : { 'Hlt1BeamGasCrossingForcedRecoFullZ': 0.001 }
                                       , 'Postscale' : { 'Hlt1BeamGasNoBeamBeam1'            : 'RATE(0.5)'
                                                       , 'Hlt1BeamGasNoBeamBeam2'            : 'RATE(0.5)'
                                                       , 'Hlt1BeamGasBeam1'                  : 'RATE(2)'
                                                       , 'Hlt1BeamGasBeam2'                  : 'RATE(2)'
                                                       , 'Hlt1BeamGasCrossingEnhancedBeam1'  : 'RATE(0)'
                                                       , 'Hlt1BeamGasCrossingEnhancedBeam2'  : 'RATE(0)'
                                                       , 'Hlt1BeamGasCrossingForcedReco'     : 'RATE(0.5)'
                                                       , 'Hlt1BeamGasCrossingForcedRecoFullZ': 'RATE(0.5)'
                                                       , 'Hlt1BeamGasCrossingParasitic'      : 'RATE(1)'
                                                       , 'Hlt1BeamGasHighRhoVertices'        : 'RATE(4)'
                                                       }
                                                }

                       # micro bias lines switched off for high mu physics running              
                       , Hlt1MBLinesConf :     { 'Prescale' : { 'Hlt1MBMicroBiasVelo'                : 0
                                                              , 'Hlt1MBMicroBiasTStation'            : 0
                                                              , 'Hlt1MBMicroBiasVeloRateLimited'     : 0
                                                              , 'Hlt1MBMicroBiasTStationRateLimited' : 0 }
                                               , 'MaxNoBiasRate' : 1000000.
                                                 } 
                       , Hlt1LumiLinesConf : { 'EnableReco' : False }
                       , Hlt1MBLinesConf : {  'Prescale' : { 'Hlt1MBNoBias'  : 0.01 }}
                       , Hlt1CommissioningLinesConf : { 'Postscale' : { 'Hlt1ErrorEvent'   : 'RATE(0.01)'

                                                                        } }
                       , Hlt2CommissioningLinesConf : { 'Prescale' : { 'Hlt2PassThrough'  : 1.0 } }
                       }

        return thresholds
