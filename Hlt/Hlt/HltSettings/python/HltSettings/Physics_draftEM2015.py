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
        return '0x0046'

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
        from Hlt1Lines.Hlt1CalibRICHMirrorLines     import Hlt1CalibRICHMirrorLinesConf

        from Hlt2Lines.Hlt2InclusiveMuonLines   import Hlt2InclusiveMuonLinesConf
        from Hlt2Lines.Hlt2InclusiveDiMuonLines import Hlt2InclusiveDiMuonLinesConf
        from Hlt2Lines.Hlt2CommissioningLines   import Hlt2CommissioningLinesConf
        from Hlt2Lines.DiMuon.Lines             import DiMuonLines
        from Hlt2Lines.Hlt2TrackingEfficiencyLines import Hlt2TrackingEfficiencyLinesConf
        from GaudiKernel.SystemOfUnits import MeV, GeV, mm

        thresholds = { Hlt1TrackLinesConf :    {'AllL0_Velo_NHits'   : 0   #OFF
                                               , 'AllL0_Velo_Qcut'   : 999 #OFF
                                               , 'AllL0_TrNTHits'    : 0   #PFF
                                               , 'AllL0_PT'          : 1000.
                                               , 'AllL0_P'           : 3000.
                                               , 'AllL0_IPChi2'      : 9.0
                                               , 'AllL0_TrChi2'      : 3.0
                                               , 'AllL0_GEC'         : 'Loose'
                                               , 'Muon_TrNTHits'     : 0 #OFF
                                               , 'Muon_Velo_NHits'   : 0 #OFF
                                               , 'Muon_Velo_Qcut'    : 999 #OFF
                                               , 'Muon_PT'           : 1200.
                                               , 'Muon_P'            : 3000.
                                               , 'Muon_IPChi2'       : 4.0
                                               , 'Muon_TrChi2'       : 3.0
                                               , 'Muon_GEC'          : 'Loose'
                                               , 'ODINTriggers'      : {'AllL0'  : ['LHCb.ODIN.PhysicsTrigger']}
                                               , 'L0Channels'        : {'AllL0'  : '',
                                                                        'Muon'   : ('Muon',)}
                                               }
                     , Hlt1CalibRICHMirrorLinesConf : {     'RICHMirror_Velo_NHits'  : 0 
                                                        ,   'RICHMirror_Velo_Qcut'   : 3 
                                                        ,   'RICHMirror_TrNTHits'    : 16
                                                        ,   'RICHMirror_PT'          : 500.
                                                        ,   'RICHMirror_P'           : 30000.
                                                        ,   'RICHMirror_ETA'         : 2.55 
                                                        ,   'RICHMirror_TrChi2'      : 3.
                                                      }   
                     , Hlt1MuonLinesConf :     { 'SingleMuonHighPT_P'        : 3000
                                               , 'SingleMuonHighPT_PT'      : 4800
                                               , 'SingleMuonHighPT_TrChi2'  :    3.
                                               ,'SingleMuonHighPT_TrNTHits'   : 0 #OFF
                                               ,'SingleMuonHighPT_Velo_NHits' : 0 #OFF
                                               ,'SingleMuonHighPT_Velo_Qcut'  : 999 #OFF
                                               , 'SingleMuonHighPT_GEC'     : 'Loose'
                                               ,'SingleMuonHighPT_TrackType' : 'Long'
                                               , 'DiMuonHighMass_VxDOCA'    :  0.2
                                               , 'DiMuonHighMass_VxChi2'    :   25
                                               , 'DiMuonHighMass_P'         : 3000
                                               , 'DiMuonHighMass_PT'        :  500
                                               , 'DiMuonHighMass_TrChi2'    :    3
                                               , 'DiMuonHighMass_M'         : 2700
                                               , 'DiMuonHighMass_GEC'       : 'Loose'
                                               ,'DiMuonHighMass_TrackType' :'Long'                                               
                                               ,'L0Channels'               : {
                                                   'SingleMuonHighPT' : ( 'Muon', ),
                                                   'DiMuonHighMass'   : ( 'Muon', )}
                                                                  
                                                }
                         , Hlt1L0LinesConf :     {  'Postscale' : { 'Hlt1L0AnyRateLimited'       : 'RATE(1)'
                                                                , 'Hlt1L0AnyNoSPDRateLimited'  : 'RATE(1)'
                                                                }
                                               ,  'Prescale' : { 'Hlt1L0AnyNoSPD'     : 0.01
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
        	    , Hlt2InclusiveDiMuonLinesConf : {'TrChi2Tight'              : 4
                                                     ,'UnbiasedJPsiPt'          : 0.0      # MeV
                                                     ,'UnbiasedJPsiMuPt'        : 0.0      # MeV
                                                     ,'UnbiasedJPsiMassWindow'  : 150      # MeV
                                                     ,'UnbiasedJPsiVertexChi2'  :   25
                                                        }
                    , DiMuonLines : {'Common' :        {'TrChi2Tight':    4},
                                     'JPsi' :          {'MassWindow' :   150 * MeV,
                                                        'Pt'         :     0 * MeV,
                                                        'MuPt'       :     0 * MeV,
                                                        'VertexChi2' :    25},


                                     'Psi2S' :         {'MassWindow' :   150 * MeV,
                                                        'Pt'         :     0 * MeV,
                                                        'MuPt'       :     0 * MeV,
                                                        'VertexChi2' :    25},
                 
                                     'B' :             {'MinMass'    :  4700 * MeV,
                                                        'VertexChi2' :    10},

                                     'Z' :             {'MinMass'    : 40000 * MeV,
                                                        'Pt'         :     0 * MeV},
                                     'Prescale' : {  'Hlt2DiMuon'         : 1.0
                                                   , 'Hlt2DiMuonJPsi'     : 1.0
                                                   , 'Hlt2DiMuonPsi2S'    : 1.0
                                                   , 'Hlt2DiMuonB'        : 1.0}

                    }
                    ,Hlt2InclusiveMuonLinesConf : { 'TrChi2VeryTight'     :    3      #chi2PerDof 
                                                    ,'SingleMuonPt'        : 1500      # MeV
                                                    ,'SingleMuonIP'        : 0.0       # mm #0.5
                                                    ,'SingleMuonIPChi2'    : 16         # 200
                                                    ,'SingleMuHLT1Filter'  :  "HLT_PASS_RE('Hlt1TrackMuonDecision')"            
                                                    ,'SingleMuonHighPt'    : 10000     # MeV
                                                    ,'Prescale'   : { 'Hlt2SingleMuon'        : 1.0
                                                                      ,'Hlt2SingleHighPTMuon'  : 1.0
                                                                      }
                                                  }   
						      
                      , Hlt2CommissioningLinesConf : { 'Prescale' : { 'Hlt2PassThrough'  : 0.0001
                                                                       , 'Hlt2Forward'      : 0.00001
                                                                       , 'Hlt2DebugEvent'   : 0.000001  }
                                                        , 'Postscale' : { 'Hlt2ErrorEvent'   : 'RATE(0.01)' } 
						      }
                      , Hlt2TrackingEfficiencyLinesConf : { 'Prescale'   : { 'Hlt2TrackEffMuonTT1'      	    : 1.0
				   ,'Hlt2TrackEffMuonTT2'      	    : 1.0
				   ,'Hlt2TrackEffVeloMuon1'   	    : 1.0
				   ,'Hlt2TrackEffVeloMuon2'   	    : 1.0
				   ,'Hlt2TrackEffDownstream1'	    : 1.0
				   ,'Hlt2TrackEffDownstream2'	    : 1.0
                                   }
                  
		  # Muon TT cuts
                  ,'MuonTTProbeP'              :    0 #MeV
                  ,'MuonTTProbePt'             :    0 #MeV
                  ,'MuonTTLongMuonPID'         :    2 #dimensionless
                  ,'MuonTTLongP'               :10000 #MeV
                  ,'MuonTTLongPt'              : 1300 #MeV
                  ,'MuonTTLongTrChi2'          :    5 #dimensionless
                  ,'MuonTTJPsiP'               : 1000 #MeV
                  ,'MuonTTJPsiPt'              : 1000 #MeV
                  ,'MuonTTJPsiMaxIP'           :    1 #mm
                  ,'MuonTTJPsiVtxChi2'         :    5 #dimensionless
                  ,'MuonTTMassWindow'          :  500 #MeV
		  # VeloMuon cuts
                  ,'VeloProbePt'                  :   0 #MeV
		  ,'VeloProbeTrChi2'		  :   5 #dimensionless
                  ,'VeloLongMuonPID'              :   1 #dimensionless
                  ,'VeloLongPt'                   :   0 #MeV
                  ,'VeloLongP'                    :7000 #MeV
		  ,'VeloLongTrchi2'		  :   3 #dimensionless
                  ,'VeloMassWindow'               : 500 #MeV
                  ,'VeloJPsiPt'                   : 500 #MeV
		  ,'VeloVertchi2'		  :   2 #dimensionless
		  # DownstreamMuon cuts
                  ,'DownstreamProbePt'                  : 200 #MeV
                  ,'DownstreamProbeP'                   :3000 #MeV
		  ,'DownstreamProbeTrchi2'	        :  10 #dimensionless
                  ,'DownstreamLongPt'                   : 700 #MeV
                  ,'DownstreamLongP'                    :5000 #MeV
		  ,'DownstreamLongTrchi2'		:  10 #dimensionless
                  ,'DownstreamLongMuonPID'	        :   2 #dimensionless
                  ,'DownstreamLongMinIP'	        : 0.5 #mm
                  ,'DownstreamJPsiDOCA'                 :   5 #mm
                  ,'DownstreamMassWindow'               : 200 #MeV
                  ,'DownstreamJPsiPt'                   :   0 #MeV
		  ,'DownstreamVertchi2'			:   5 #dimensionless
                  }
                        

        }



        return thresholds
                       
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        hlt2 = ['Hlt2SingleMuon',
                'Hlt2SingleMuonHighPT',
                'Hlt2DiMuonJPsi',
                'Hlt2DiMuonJPsiTurbo',
                'Hlt2DiMuonPsi2S',
                'Hlt2DiMuonB',
	        'Hlt2PassThrough', 'Hlt2Lumi','Hlt2DebugEvent','Hlt2Forward','Hlt2ErrorEvent','Hlt2Transparent', # technical lines
                'Hlt2TrackEffMuonTT1', # track efficiency lines
                'Hlt2TrackEffMuonTT2',
                'Hlt2TrackEffVeloMuon1',
                'Hlt2TrackEffVeloMuon2',
                'Hlt2TrackEffDownstream1',
                'Hlt2TrackEffDownstream2',
         ]


        return hlt2
       
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        lines =  [ 'Hlt1TrackAllL0', 'Hlt1TrackMuon'
                 , 'Hlt1CalibRICHMirror'
                 , 'Hlt1SingleMuonHighPT'
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
                 , 'Hlt1ODINTechnical', 'Hlt1Tell1Error' , 'Hlt1ErrorEvent'  
                 , 'Hlt1MBMicroBiasVelo','Hlt1MBMicroBiasTStation'
                 , 'Hlt1VeloClosingMicroBias'
            ]



        
        return lines 
