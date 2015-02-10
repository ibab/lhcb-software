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
        return '0x0033'

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
        from Hlt1Lines.Hlt1BeamGasLines        import Hlt1BeamGasLinesConf
        from Hlt1Lines.Hlt1CommissioningLines  import Hlt1CommissioningLinesConf
        
        from Hlt2Lines.Hlt2InclusiveMuonLines  import Hlt2InclusiveMuonLinesConf
        from Hlt2Lines.Hlt2InclusiveDiMuonLines  import Hlt2InclusiveDiMuonLinesConf
        from Hlt2Lines.Hlt2MuNTrackLines  import Hlt2MuNTrackLinesConf
        from Hlt2Lines.Hlt2CharmHadD02HHLines   import Hlt2CharmHadD02HHLinesConf
        from Hlt2Lines.Hlt2CommissioningLines  import Hlt2CommissioningLinesConf
        
        from GaudiKernel.SystemOfUnits import MeV, GeV, mm

        thresholds = { Hlt1TrackLinesConf :    {'AllL0_Velo_NHits'   : 9
                                               , 'AllL0_Velo_Qcut'   : 3
                                               , 'AllL0_TrNTHits'    : 16
                                               , 'AllL0_PT'          : 1600.
                                               , 'AllL0_P'           : 3000.
                                               , 'AllL0_IPChi2'      : 16.
                                               , 'AllL0_TrChi2'      : 2.
                                               , 'AllL0_GEC'         : 'Loose'
                                               , 'Muon_TrNTHits'     : 0 #OFF
                                               , 'Muon_Velo_NHits'   : 0 #OFF
                                               , 'Muon_Velo_Qcut'    : 999 #OFF
                                               , 'Muon_PT'           : 1000.
                                               , 'Muon_P'            : 3000.
                                               , 'Muon_IPChi2'       : 16.
                                               , 'Muon_TrChi2'       : 2.5
                                               , 'Muon_GEC'          : 'Loose'
                                               , 'Photon_PT'         : 1200.
                                               , 'Photon_P'          : 3000.
                                               , 'Photon_IPChi2'     : 16.
                                               , 'Photon_TrChi2'     : 2.
                                               , 'Photon_GEC'        : 'Loose'
                                               , 'L0Channels'        : {'AllL0'  : 'L0_DECISION_PHYSICS',
                                                                        'Muon'   : ('Muon', 'DiMuon', 'MuonNoSPD', 'DiMuonNoSPD'),
                                                                        'Photon' : ("PhotonHi", "ElectronHi")}
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

                         , Hlt1BeamGasLinesConf : {'Prescale'  : { 'Hlt1BeamGasCrossingForcedRecoFullZ': 0.001 }
                                                 , 'Postscale' : { 'Hlt1BeamGasNoBeamBeam1'            : 'RATE(0.5)'
                                                                 , 'Hlt1BeamGasNoBeamBeam2'            : 'RATE(0.5)'
                                                                 , 'Hlt1BeamGasBeam1'                  : 'RATE(2)'
                                                                 , 'Hlt1BeamGasBeam2'                  : 'RATE(2)'
                                                                 , 'Hlt1BeamGasCrossingEnhancedBeam1'  : 'RATE(0)'
                                                                 , 'Hlt1BeamGasCrossingEnhancedBeam2'  : 'RATE(0)'
                                                                 , 'Hlt1BeamGasCrossingForcedReco'     : 'RATE(0.5)'
                                                                 , 'Hlt1BeamGasCrossingForcedRecoFullZ': 'RATE(0.5)'
                                                                 , 'Hlt1BeamGasCrossingParasitic'      : 'RATE(1)'
                                                                 , 'Hlt1BeamGasHighRhoVertices'        : 'RATE(4)'}
                                                   }

			, Hlt1CommissioningLinesConf : { 'Postscale' : { 'Hlt1ErrorEvent'   : 'RATE(0.01)' }

                                                       } 
                       , Hlt1MBLinesConf :     { 'Prescale' : { 'Hlt1MBMicroBiasVelo'                : 0
                                                              , 'Hlt1MBMicroBiasTStation'            : 0 
                                                             , 'Hlt1MBMicroBiasVeloRateLimited'     : 0
                                                              , 'Hlt1MBMicroBiasTStationRateLimited' : 0 }

                                               , 'MaxNoBiasRate' : 1000000.
                                               }
        	       , Hlt2InclusiveDiMuonLinesConf : {      'TrChi2'                   : 10
                                                               ,'TrChi2Tight'              : 4
                                                               ,'UnbiasedDiMuonMinMass'    : 2900      # MeV
                                                               ,'UnbiasedDiMuonPt'        : 0.0      # MeV
                                                               ,'UnbiasedDiMuonMuPt'      : 0.0      # MeV
                                                               ,'UnbiasedDiMuonLowPt'     : 0.0      # MeV
                                                               ,'UnbiasedDiMuonLowMuPt'   : 0.0      # MeV
                                                               ,'UnbiasedJPsiPt'          : 0.0      # MeV
                                                               ,'UnbiasedJPsiMuPt'        : 0.0      # MeV
                                                               ,'UnbiasedJPsiMassWindow'  : 120      # MeV
                                                               ,'UnbiasedJPsiHighPt'      : 2000     # MeV
                                                               ,'UnbiasedJPsiHighPTMassWindow' : 100
                                                               ,'UnbiasedPsi2SPt'         :   0      # MeV
                                                               ,'UnbiasedPsi2SMuPt'       : 0.0      # MeV
                                                               ,'UnbiasedPsi2SMassWindow' : 120      # MeV
                                                               ,'UnbiasedPsi2SPtHigh'     : 3500      #MeV
                                                               ,'UnbiasedBmmMinMass'      : 4700     # MeV
                                                               ,'UnbiasedBmmVertexChi2'   : 10
                                                               ,'DetachedDiMuonPt'        :   600     #MeV 
                                                               ,'DetachedDiMuonDLS'       :   7
                                                               ,'DetachedDiMuonMuPt'      : 300      #Mev 
                                                               ,'DetachedDiMuonMinMass'   :   0 #MeV 
                                                               ,'DetachedDiMuIPChi2'      :   25
                                                               ,'DetachedDiMuonVertexChi2':   8
                                                               ,'DetachedHeavyDiMuonMinMass': 2950 # MeV
                                                               ,'DetachedHeavyDiMuonDLS'  :   5
                                                               ,'DetachedJPsiDLS'           : 3
                                                               ,'DetachedJPsiMassWindow'    : 120 #MeV 
                                                   		, 'Prescale'   : { 'Hlt2DiMuon'                  :  1.0
                                                                                  , 'Hlt2DiMuonLowMass'         :  0
                                                                                  , 'Hlt2DiMuonJPsi'            :  1.0
                                                                                  , 'Hlt2DiMuonJPsiTurbo'            :  1.0
                                                                                  , 'Hlt2DiMuonPsi2S'           :  1.0
                                                                                  , 'Hlt2DiMuonB'               : 1.0
                                                                                  , 'Hlt2DiMuonDY1'             :  0.005
                                                                                  , 'Hlt2DiMuonDY2'             :  0.03
                                                                                  , 'Hlt2DiMuonAndGamma'        :  0
                                                                                  }
                                                        }

                    ,Hlt2InclusiveMuonLinesConf : {  'TrChi2'              :    2      #chi2PerDof 
                                                    ,'TrChi2Tight'         :    5      #chi2PerDof         
                                                    ,'TrChi2VeryTight'     :    2      #chi2PerDof 
                                                    ,'SingleMuonPt'        : 1300      # MeV
                                                    ,'SingleMuonIP'        : 0.0       # mm #0.5
                                                    ,'SingleMuonIPChi2'    : 4         # 200
                                                    ,'SingleMuHLT1Filter'  :  ""  #"HLT_PASS_RE('Hlt1TrackMuonDecision')"            
                                                    ,'SingleMuonHighPt'    : 10000     # MeV
                                                    ,'SingleMuonLowPt'     : 1300      # MeV
                                                    ,'Prescale'   : { #'Hlt2MuonFromHLT1'       : 0.0
                                                                      'Hlt2SingleMuon'        : 1.0
                                                                      ,'Hlt2SingleHighPTMuon'  : 1.0
                                                                      ,'Hlt2SingleMuonLowPT'   : 1.0
                                                                      }
                                                  }   
                     ,Hlt2CharmHadD02HHLinesConf       : {  'Trk_PT_MIN'               : 0.0 * MeV
                                                           , 'Trk_P_MIN'                : 3.0  * GeV
                                                           , 'Trk_MIPCHI2DV_MIN'        : 4.0
                                                           , 'Trk_TRCHI2DOF_MAX'        : 500.0
                                                           , 'Pair_AMINDOCA_MAX'        : 0.10 * mm
                                                           , 'Trk_Max_APT_MIN'          : 0.0 * MeV
                                                           , 'D0_BPVVDCHI2_MIN'         : 20.0
                                                           , 'D0_BPVDIRA_MIN'           : 0.999
                                                           , 'D0_VCHI2PDOF_MAX'         : 25.0
                                                           , 'D0_PT_MIN'                : 2000.0 * MeV
                                                           , 'Sig_M_MIN'                : 1790.0 * MeV
                                                           , 'Sig_M_MAX'                : 1950.0 * MeV
                                                           , 'WideMass_M_MIN'           : 1715.0 * MeV
                                                           , 'WideMass_M_MAX'           : 2085.0 * MeV
                                                           ## 2-body inclusive!
                                                           , 'Inc_Trk_PT_MIN'           : 0.0 * MeV
                                                           , 'Inc_Trk_P_MIN'            : 3.0 * GeV
                                                           , 'Inc_Trk_MIPCHI2DV_MIN'    : 4.0       # neuter
                                                           , 'Inc_Trk_TRCHI2DOF_MAX'    : 500.0        # neuter
                                                           , 'Inc_Pair_AMINDOCA_MAX'    : 0.10 * mm
                                                           , 'Inc_D0_BPVVDCHI2_MIN'     : 20.0      # neuter
                                                           , 'Inc_Trk_Max_APT_MIN'      : 0.0 * MeV
                                                           , 'Inc_D0_APT_MIN'           : 2000.0 * MeV
                                                           , 'Inc_D0_SumAPT_MIN'        : 1500.0 * MeV
                                                           , 'Inc_D0_BPVIPCHI2_MIN'     : 2.0        # neuter
                                                           , 'Inc_D0_BPVCORRM_MIN'      : 300.0 * MeV
                                                           , 'Inc_D0_BPVCORRM_MAX'      : 3200.0 * MeV
                                                           ## GEC
                                                           , 'GEC_Filter_NTRACK'        : True       # 
                                                           , 'GEC_NTRACK_MAX'           : 180        # 
                                                           , 'Postscale' : {
                                                                             'Hlt2CharmHadD02KKWideMass'    : 1.0
                                                                            , 'Hlt2CharmHadD02KPiWideMass'  : 1.0
                                                                            , 'Hlt2CharmHadD02PiPiWideMass' : 1.0
                                                                           }
                                                        }
						      
                       , Hlt2CommissioningLinesConf : { 'Prescale' : { 'Hlt2PassThrough'  : 0.0001
                                                                       , 'Hlt2Forward'      : 0.00001
                                                                       , 'Hlt2DebugEvent'   : 0.000001  }
                                                        , 'Postscale' : { 'Hlt2ErrorEvent'   : 'RATE(0.01)' } 
						      }

        }



        return thresholds
                       
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        hlt2 = ['Hlt2PassThrough', 'Hlt2Lumi','Hlt2DebugEvent','Hlt2Forward','Hlt2ErrorEvent','Hlt2Transparent',
            'Hlt2SingleMuon',
            'Hlt2SingleMuonLowPT',
            'Hlt2DiMuon',
            'Hlt2DiMuonJPsi',
            'Hlt2DiMuonJPsiTurbo',
            'Hlt2DiMuonPsi2S',
            'Hlt2DiMuonB',
            'Hlt2DiMuonDetached',
            'Hlt2DiMuonDetachedJPsi',
            'Hlt2DiMuonDetachedPsi2S',                
            'Hlt2CharmHadD02HH_D02KKWideMass',
            'Hlt2CharmHadD02HH_D02KPiWideMass',
            'Hlt2CharmHadD02HH_D02PiPiWideMass'    
         ]


        return hlt2
       
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        lines =  [ 'Hlt1TrackAllL0', 'Hlt1TrackMuon'
                 , 'Hlt1SingleMuonNoIP'
                 , 'Hlt1DiMuonLowMass'
                 , 'Hlt1DiMuonHighMass'
                 , 'Hlt1BeamGasNoBeamBeam1', 'Hlt1BeamGasNoBeamBeam2'   # technical lines
                 , 'Hlt1BeamGasBeam1', 'Hlt1BeamGasBeam2'
                 , 'Hlt1BeamGasCrossingEnhancedBeam1', 'Hlt1BeamGasCrossingEnhancedBeam2'
                 , 'Hlt1BeamGasCrossingForcedReco', 'Hlt1BeamGasCrossingForcedRecoFullZ'
                 , 'Hlt1BeamGasCrossingParasitic', 'Hlt1BeamGasHighRhoVertices'
                 , 'Hlt1NoPVPassThrough'
                 , 'Hlt1CharmCalibrationNoBias'
                 , 'Hlt1Lumi', 'Hlt1LumiMidBeamCrossing'
                 , 'Hlt1L0Any','Hlt1L0AnyNoSPD'
                 , 'Hlt1MBNoBias'
                 , 'Hlt1ODINTechnical', 'Hlt1Tell1Error' , 'Hlt1ErrorEvent' # , 'Hlt1Incident' 
                 , 'Hlt1MBMicroBiasVelo','Hlt1MBMicroBiasTStation'
                 , 'Hlt1VeloClosingMicroBias'
            ]



        
        return lines 
