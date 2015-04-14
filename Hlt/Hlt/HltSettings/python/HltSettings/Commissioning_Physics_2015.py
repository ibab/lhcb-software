from GaudiKernel.SystemOfUnits import GeV, MeV, mm

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

class Commissioning_Physics_2015( object ):
    """
    Settings for Commissioning trigger 2015 
    Derived from Physics_Septemeber2012.

    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author S. Neubert, J. Albrecht, V. Gligorov
    @date 2014-12-10
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
        return '0xFF66'

    def HltType(self) :
        self.verifyType( Commissioning_Physics_2015 ) 
        return          'Commissioning_Physics_2015'
    
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
        from Hlt1Lines.Hlt1MVALines            import Hlt1MVALinesConf
        
        thresholds = { Hlt1TrackLinesConf :   {  'AllL0_Velo_NHits'  : 9
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
                     , Hlt1MVALinesConf :     {'DoTiming'                     : False,
                                               'TrackMVA' :    {'TrChi2'      :     2.5,
                                                                'MinPT'       :  1000.  * MeV,
                                                                'MaxPT'       : 25000.  * MeV,
                                                                'MinIPChi2'   :     7.4,
                                                                'Param1'      :     0.6,
                                                                'Param2'      :     1.0,
                                                                'Param3'      :     1.1,
                                                                'GEC'         : 'Loose'},
                                               'TwoTrackMVA' : {'P'           :  5000. * MeV,
                                                                'PT'          :   500. * MeV,
                                                                'TrChi2'      :     2.5,
                                                                'IPChi2'      :     4.,
                                                                'MinMCOR'     :  1000. * MeV,
                                                                'MaxMCOR'     :   1e9  * MeV,
                                                                'MinETA'      :     2.,
                                                                'MaxETA'      :     5.,
                                                                'MinDirA'     :     0.,
                                                                'V0PT'        :  2000. * MeV,
                                                                'VxChi2'      :    10.,
                                                                'Threshold'   :     0.937,
                                                                'MvaVars'     : {'chi2'   : 'VFASPF(VCHI2)',
                                                                                 'fdchi2' : 'BPVVDCHI2',
                                                                                 'sumpt'  : 'SUMTREE(PT, ISBASIC, 0.0)',
                                                                                 'nlt16'  : 'NINTREE(ISBASIC & (BPVIPCHI2() < 16))'},
                                                                'Classifier'  : {'Type'   : 'MatrixNet',
                                                                                 'File'   : '$PARAMFILESROOT/data/Hlt1TwoTrackMVA.mx'},
                                                                'GEC'         : 'Loose'},
                                               'L0Channels'  : {'TrackMVA'    : 'L0_DECISION_PHYSICS',
                                                                'TwoTrackMVA' : 'L0_DECISION_PHYSICS'},
                                               'Priorities'  : {'TrackMVA'    : 20,
                                                                'TwoTrackMVA' : 21}
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
                     , Hlt1MuonLinesConf :     { 'SingleMuonHighPT_P'       : 3000
                                               , 'SingleMuonHighPT_PT'      : 4800
                                               , 'SingleMuonHighPT_TrChi2'  :    3.
                                               , 'SingleMuonHighPT_GEC'     : 'Loose'
                                               , 'SingleMuonNoIP_P'         : 3000
                                               , 'SingleMuonNoIP_PT'        : 1300
                                               , 'SingleMuonNoIP_TrChi2'    :    3.
                                               , 'SingleMuonNoIP_GEC'       : 'Loose'
                                               , 'DiMuonLowMass_VxDOCA'     :  0.2
                                               , 'DiMuonLowMass_VxChi2'     :   25
                                               , 'DiMuonLowMass_P'          : 3000
                                               , 'DiMuonLowMass_PT'         :  300
                                               , 'DiMuonLowMass_TrChi2'     :    4
                                               , 'DiMuonLowMass_M'          :    0.
                                               , 'DiMuonLowMass_IPChi2'     :    6.
                                               , 'DiMuonLowMass_GEC'        : 'Loose'
                                               , 'DiMuonHighMass_VxDOCA'    :  0.2
                                               , 'DiMuonHighMass_VxChi2'    :   25
                                               , 'DiMuonHighMass_P'         : 3000
                                               , 'DiMuonHighMass_PT'        :  300
                                               , 'DiMuonHighMass_TrChi2'    :    4
                                               , 'DiMuonHighMass_M'         : 2700
                                               , 'DiMuonHighMass_GEC'       : 'Loose'
                                               , 'MultiMuonNoIP_P'          : 3000
                                               , 'MultiMuonNoIP_PT'         :  500
                                               , 'MultiMuonNoIP_TrChi2'     :    3.
                                               , 'MultiMuonNoIP_GT'         :  2.5
                                               , 'MultiMuonNoIP_GEC'        : 'Loose'
                                               ,'L0Channels'               : {'SingleMuonHighPT' : ( 'Muon',),
                                                                              'SingleMuonNoIP'   : ( 'Muon',),
                                                                              'DiMuonLowMass'    : ( 'Muon', 'DiMuon' ),
                                                                              'DiMuonHighMass'   : ( 'Muon', 'DiMuon' ),
                                                                              'MultiMuonNoIP'    : ( 'Muon', 'DiMuon' ) }
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
                       #, Hlt1ProtonLinesConf        : { 'Prescale' : {  'Hlt1DiProtonLowMult'        : 0.01, 
                       #                                                 'Hlt1DiProton'               : 0.01
                       #                                                } } 
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

        from Hlt2Lines.Commissioning.Lines  import CommissioningLines
        thresholds.update({CommissioningLines : { 'Prescale' : { 'Hlt2PassThrough'  : 0.0001 
                                                               , 'Hlt2Forward'      : 0.00001
                                                               , 'Hlt2DebugEvent'   : 0.000001  }
                                                , 'Postscale' : { 'Hlt2ErrorEvent'   : 'RATE(0.01)' }
                                                }
                          })

        from Muons_March2015 import Muons_March2015
        __update_conf__(thresholds,  Muons_March2015().Thresholds() )

        from Hadrons_March2015 import Hadrons_March2015
        __update_conf__(thresholds,  Hadrons_March2015().Thresholds() )

        return thresholds
                       
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        hlt2 = ['Hlt2PassThrough','Hlt2Lumi','Hlt2DebugEvent',
                'Hlt2Forward','Hlt2ErrorEvent','Hlt2Transparent']

        from Muons_March2015 import Muons_March2015
        hlt2.extend( Muons_March2015().ActiveHlt2Lines() )

        from Hadrons_March2015 import Hadrons_March2015
        hlt2.extend( Hadrons_March2015().ActiveHlt2Lines() )
        
        from Radiative_March2015 import Radiative_March2015
        hlt2.extend( Radiative_March2015().ActiveHlt2Lines() )
       
        return hlt2
       
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        lines =  [ 'Hlt1TrackAllL0', 'Hlt1TrackMuon'
                 , 'Hlt1SingleMuonNoIP', 'Hlt1SingleMuonHighPT'
                 , 'Hlt1SingleElectronNoIP'
                 , 'Hlt1DiMuonLowMass', 'Hlt1DiMuonHighMass'
                 , 'Hlt1TrackMVA', 'Hlt1TwoTrackMVA'
                 , 'Hlt1CalibRICHMirror' ]

        
        ## from Hlt1TechnicalLines import Hlt1TechnicalLines 
        ## lines.extend( Hlt1TechnicalLines().ActiveHlt1Lines() )
        
        return lines 
