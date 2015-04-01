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

class Hlt1_Commissioning_Physics_2015( object ):
    """
    Settings for 2015 commissioning of physics lines
    based on Hlt1_Physics_September2012
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author J. Albrecht, V. Gligorov, S.Neubert
    @date 2012-04-17
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
        return '0x0046'

    def HltType(self) :
        self.verifyType( Hlt1_Commissioning_Physics_2015 ) 
        return          'Hlt1_Commissioning_Physics_2015'
    
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """

        from Hlt1Lines.Hlt1TrackLines          import Hlt1TrackLinesConf
        from Hlt1Lines.Hlt1MuonLines           import Hlt1MuonLinesConf
        from Hlt1Lines.Hlt1ElectronLines       import Hlt1ElectronLinesConf
        from Hlt1Lines.Hlt1L0Lines             import Hlt1L0LinesConf
        from Hlt1Lines.Hlt1MBLines             import Hlt1MBLinesConf
        from Hlt1Lines.Hlt1CommissioningLines  import Hlt1CommissioningLinesConf
        from Hlt1Lines.Hlt1BeamGasLines        import Hlt1BeamGasLinesConf

        thresholds = { Hlt1TrackLinesConf :    {'AllL0_Velo_NHits'   : 9
                                               , 'AllL0_Velo_Qcut'   : 3
                                               , 'AllL0_TrNTHits'    : 16
                                               , 'AllL0_PT'          : 1300.
                                               , 'AllL0_P'           : 6000.
                                               , 'AllL0_IPChi2'      : 13.
                                               , 'AllL0_TrChi2'      : 3.
                                               , 'AllL0_GEC'         : 'Loose'
                                               , 'Muon_TrNTHits'     : 0 #OFF
                                               , 'Muon_Velo_NHits'   : 0 #OFF
                                               , 'Muon_Velo_Qcut'    : 999 #OFF
                                               , 'Muon_PT'           : 800.
                                               , 'Muon_P'            : 6000.
                                               , 'Muon_IPChi2'       : 6.
                                               , 'Muon_TrChi2'       : 5.
                                               , 'Muon_GEC'          : 'Loose'
                                               , 'Photon_PT'         : 1200.
                                               , 'Photon_P'          : 6000.
                                               , 'Photon_IPChi2'     : 13.
                                               , 'Photon_TrChi2'     : 4.
                                               , 'Photon_GEC'        : 'Loose'
                                               , 'L0Channels'        : {'AllL0'  : 'L0_DECISION_PHYSICS',
                                                                        'Muon'   : ('Muon', 'DiMuon'),
                                                                        'Photon' : ("Photon", "Electron")}
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
                     , Hlt1MuonLinesConf :     { 'SingleMuonHighPT_P'        : 3000
                                               , 'SingleMuonHighPT_PT'      : 4800
                                               , 'SingleMuonHighPT_TrChi2'  :    3.
                                               , 'SingleMuonHighPT_GEC'     : 'Loose'
                                               , 'SingleMuonNoIP_P'         : 3000
                                               , 'SingleMuonNoIP_PT'        : 1300
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
                                               , 'DiMuonHighMass_P'         : 3000
                                               , 'DiMuonHighMass_PT'        :  500
                                               , 'DiMuonHighMass_TrChi2'    :    3
                                               , 'DiMuonHighMass_M'         : 2700
                                               , 'DiMuonHighMass_GEC'       : 'Loose'
                                               , 'MultiMuonNoIP_P'          : 3000
                                               , 'MultiMuonNoIP_PT'         :  500
                                               , 'MultiMuonNoIP_TrChi2'     :    3.
                                               , 'MultiMuonNoIP_GT'         :  2.5
                                               , 'MultiMuonNoIP_GEC'        : 'Loose'
                                               ,'L0Channels'               : {
                                                   'SingleMuonHighPT' : ( 'Muon', 'MuonNoSPD'),
                                                   'SingleMuonNoIP'   : ( 'Muon', 'MuonNoSPD'),
                                                   'DiMuonLowMass'    : ( 'Muon', 'MuonNoSPD', 'DiMuon', 'DiMuonNoSPD' ),
                                                   'DiMuonHighMass'   : ( 'Muon', 'MuonNoSPD', 'DiMuon', 'DiMuonNoSPD' ),
                                                   'MultiMuonNoIP'    : ( 'Muon', 'MuonNoSPD', 'DiMuon', 'DiMuonNoSPD' ) }

                                               , 'Prescale'                 : { 'Hlt1SingleMuonNoIP' : 0.01,
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
                       , Hlt1CommissioningLinesConf : { 'Postscale' : { 'Hlt1ErrorEvent'   : 'RATE(0.01)'

                                                                        } }
                       # micro bias lines switched off for high mu physics running              
                       , Hlt1MBLinesConf :     { 'Prescale' : { 'Hlt1MBMicroBiasVelo'                : 0
                                                              , 'Hlt1MBMicroBiasTStation'            : 0
                                                              , 'Hlt1MBMicroBiasVeloRateLimited'     : 0
                                                              , 'Hlt1MBMicroBiasTStationRateLimited' : 0 }
                                                 , 'MaxNoBiasRate' : 1000000.
                                                 }
                       

                       }
        
        return thresholds
                       
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        return []
       
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        lines =  [ 'Hlt1TrackAllL0', 'Hlt1TrackMuon'
                  , 'Hlt1SingleMuonNoIP', 'Hlt1SingleMuonHighPT'
                  , 'Hlt1SingleElectronNoIP'
                  , 'Hlt1DiMuonLowMass', 'Hlt1DiMuonHighMass'
                  , 'Hlt1L0HighSumETJet','Hlt1HighPtJetsSinglePV'
                  , 'Hlt1TrackMVA', 'Hlt1TwoTrackMVA'
                  , 'Hlt1CalibRICHMirror' ]

        
        #from Hlt1TechnicalLines import Hlt1TechnicalLines 
        #lines.extend( Hlt1TechnicalLines().ActiveHlt1Lines() )
        
        return lines 
