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
        return '0xFF64' 

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
        from Hlt1Lines.Hlt1HighMultLines       import Hlt1HighMultLinesConf
        from Hlt1Lines.Hlt1BeamGasLines        import Hlt1BeamGasLinesConf
        from Hlt1Lines.Hlt1CommissioningLines  import Hlt1CommissioningLinesConf
        from Hlt1Lines.Hlt1CalibRICHMirrorLines     import Hlt1CalibRICHMirrorLinesConf

        from Hlt2Lines.Commissioning.Lines      import CommissioningLines
        from Hlt2Lines.DiMuon.Lines             import DiMuonLines
        from Hlt2Lines.SingleMuon.Lines         import SingleMuonLines
        from Hlt2Lines.CharmHad.Lines           import CharmHadXSecLines
        from Hlt2Lines.TrackEffDiMuon.Lines     import TrackEffDiMuonLines
        from Hlt2Lines.PID.Lines		import PIDLines 
        from GaudiKernel.SystemOfUnits import MeV, GeV, mm

        thresholds = { Hlt1TrackLinesConf :    {'AllL0_Velo_NHits'   : 9   
                                               , 'AllL0_Velo_Qcut'   : 999 #OFF
                                               , 'AllL0_TrNTHits'    : 0   #PFF
                                               , 'AllL0_PT'          : 800.
                                               , 'AllL0_P'           : 3000.
                                               , 'AllL0_IPChi2'      : 10.0
                                               , 'AllL0_TrChi2'      : 3.0
                                               , 'AllL0_GEC'         : 'Loose'
                                               , 'Muon_TrNTHits'     : 0   #OFF
                                               , 'Muon_Velo_NHits'   : 0   #OFF 
                                               , 'Muon_Velo_Qcut'    : 999 #OFF
                                               , 'Muon_PT'           : 800.
                                               , 'Muon_P'            : 3000.
                                               , 'Muon_IPChi2'       : 4.0
                                               , 'Muon_TrChi2'       : 3.0
                                               , 'Muon_GEC'          : 'Loose'
                                               , 'ODINFilter'        : {'AllL0'  : '(ODIN_EVTTYP == LHCb.ODIN.NoBias)'}
                                               , 'L0Channels'        : {'AllL0'  : '',
                                                                        'Muon'   : ('Muon',)}
                                               }
                                                       
                     , Hlt1MuonLinesConf :     { 'SingleMuonHighPT_P'        : 3000
                                               , 'SingleMuonHighPT_PT'      : 4800
                                               , 'SingleMuonHighPT_TrChi2'  :    3.
                                               , 'SingleMuonHighPT_TrNTHits'   : 0 #OFF
                                               , 'SingleMuonHighPT_Velo_NHits' : 0 #OFF
                                               , 'SingleMuonHighPT_Velo_Qcut'  : 999 #OFF
                                               , 'SingleMuonHighPT_GEC'     : 'Loose'
                                               , 'DiMuonHighMass_VxDOCA'    :  0.2
                                               , 'DiMuonHighMass_VxChi2'    :   25
                                               , 'DiMuonHighMass_P'         : 3000
                                               , 'DiMuonHighMass_PT'        :  500
                                               , 'DiMuonHighMass_TrChi2'    :    3
                                               , 'DiMuonHighMass_M'         : 2700
                                               , 'DiMuonHighMass_GEC'       : 'Loose'
                                               ,'L0Channels'               : {
                                                   'SingleMuonHighPT' : ( 'Muon', ),
                                                   'DiMuonHighMass'   : ( 'Muon', )}
                                                                  
                                               } 
                         , Hlt1L0LinesConf :     {  'Postscale' : { 'Hlt1L0AnyRateLimited'       : 'RATE(1)'
                                                                  , 'Hlt1L0AnyNoSPDRateLimited'  : 'RATE(1)'
                                                                  }
                                                  , 'Prescale' : { 'Hlt1L0AnyNoSPD'     : 0.01
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
                        , Hlt1MBLinesConf :     { 'Prescale' : { 'Hlt1MBNoBias'                : 1.0
                                                               , 'Hlt1MBMicroBiasVelo'            : 0 
                                                               , 'Hlt1MBMicroBiasTStation'            : 0 
                                                               , 'Hlt1MBMicroBiasVeloRateLimited'     : 0
                                                               , 'Hlt1MBMicroBiasTStationRateLimited' : 0 }
					       , 'NoBiasOdin'    : '(ODIN_EVTTYP == LHCb.ODIN.LeadingCrossing)' 
                                               }
			, Hlt1HighMultLinesConf: { 'MinVeloHits'        : 2400,
				                  'MaxVeloHits'        : 99999,
				                  'nPVs'               :  1 ,
				                  'MinVeloHits_PV'     : 2400,
				                  'ODIN'               : '(ODIN_EVTTYP == LHCb.ODIN.NoBias)'
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
                                     'Prescale' : {  'Hlt2DiMuon'         : 1.0
                                                   , 'Hlt2DiMuonJPsi'     : 1.0
                                                   , 'Hlt2DiMuonPsi2S'    : 1.0
                                                   , 'Hlt2DiMuonB'        : 1.0}

                    }
		, SingleMuonLines : 	{'Prescale' : {'SingleMuon'	: 0.5
						      , 'LowPT'		: 0.1},
                			 'HltReq'  : {"SingleMuon" :  "HLT_PASS_RE('Hlt1TrackMuonDecision')"
			                             },
                			 'Common' :        {'TrChi2'     :   3,    # Adimensional
                                    			     'Pt':            1300 * MeV },

			                 'SingleMuon' :    {'IP'     : 0.0 * mm,
                        			            'IPChi2' : 16, # Adimensional
                                  			 },

			                 'HighPT':         { 'HighPt' : 10000 *MeV },


			                 'LowPT'   :       { 'HighPt' : 4800 * MeV },


                			 }

			,CommissioningLines :  {'Prescale'    : {'Hlt2PassThrough' : 1.0,
	   			                                  'Hlt2Forward'     : 0.00001,
                	        			          'Hlt2DebugEvent'  : 0.00001},
						'PassThrough' : {'HLT1' : "HLT_PASS_RE('^Hlt1(MBNoBias|HighVeloMult|HighVeloMultSinglePV)Decision$')"},
				                'Postscale'   : {'Hlt2ErrorEvent'  : 'RATE(0.01)'}
			        	        }


   
			, PIDLines : {	'L0Req'	:	{'Muon'     : '',
							 'Electron' : ''},
					'Hlt1Req' :       {'Muon'     : '',
                                                         'Electron' : ''},
					'Lc2KPPiPrompt':  {'PromptTisTosSpec'   : 'Hlt1TrackAllL0%TOS'}
				     }
	 }



        return thresholds
    
                   
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        hlt2 = [
	    #single muon lines
	    'Hlt2SingleMuon',
            'Hlt2SingleMuonHighPT'
            ,'Hlt2SingleMuonLowPT'
            ,'Hlt2TopoMu2Body' 
	    #CEP lines
	    ,'Hlt2LowMultDiMuon_PS' 
            ,'Hlt2LowMultDiMuon'
            ,'Hlt2LowMultMuon'
            ,'Hlt2LowMultDiElectron_noTrFilt'
            ,'Hlt2LowMultDiElectron'
            ,'Hlt2LowMultDiPhoton'
            ,'Hlt2LowMultPi0'
            ,'Hlt2LowMultDiPhoton_HighMass'
            ,'Hlt2LowMultChiC2HHHHWS'
            ,'Hlt2LowMultD2KPi'
            ,'Hlt2LowMultChiC2HHWS'
            ,'Hlt2LowMultLMR2HH_mediumPS'
            ,'Hlt2LowMultChiC2HHHH'
            ,'Hlt2LowMultLMR2HHWS'
            ,'Hlt2LowMultLMR2HHWS_mediumPS'
            ,'Hlt2LowMultLMR2HHWS_heavyPS'
            ,'Hlt2LowMultD2K3PiWS'
            ,'Hlt2LowMultLMR2HH'
            ,'Hlt2LowMultHadron_noTrFilt'
            ,'Hlt2LowMultD2KKPiWS'
            ,'Hlt2LowMultChiC2PP'
            ,'Hlt2LowMultLMR2HHHHWS_heavyPS'
            ,'Hlt2LowMultD2KPiPi'
            ,'Hlt2LowMultChiC2HH'
            ,'Hlt2LowMultD2KKPi'
            ,'Hlt2LowMultLMR2HHHH_heavyPS'
            ,'Hlt2LowMultL2pPiWS'
            ,'Hlt2LowMultLMR2HHHHWS_mediumPS'
            ,'Hlt2LowMultL2pPi'
            ,'Hlt2LowMultChiC2PPWS'
            ,'Hlt2LowMultD2K3Pi'
            ,'Hlt2LowMultLMR2HHHH'
            ,'Hlt2LowMultD2KPiPiWS'
            ,'Hlt2LowMultLMR2HHHHWS'
            ,'Hlt2LowMultLMR2HH_heavyPS'
            ,'Hlt2LowMultD2KPiWS'
            ,'Hlt2LowMultLMR2HHHH_mediumPS'
            ,'Hlt2LowMultTechnical_MinBias'
            ,'Hlt2NonBeamBeamNoBias',
	    #dimuon lines
	    'Hlt2DiMuonJPsiTurbo',		
            'Hlt2DiMuonPsi2STurbo',
	    'Hlt2DiMuonBTurbo',
	    # PID lines
	    'Hlt2PIDDetJPsiMuMuPosTaggedTurbo', 
	    'Hlt2PIDDetJPsiMuMuNegTaggedTurbo', 
	    'Hlt2PIDB2KJPsiEESSTaggedTurbo',
	    'Hlt2PIDB2KJPsiEEOSTaggedTurbo',
	    'Hlt2PIDLambda2PPiLLTurbo',
	    'Hlt2PIDLambda2PPiLLhighPTurbo',
	    'Hlt2PIDLambda2PPiLLveryhighPTurbo',
	    'Hlt2PIDLambda2PPiLLisMuonTurbo',
	    'Hlt2PIDD02KPiTagTurbo',
	    'Hlt2PIDLb2LcMuNuTurbo',
	    'Hlt2PIDLb2LcPiTurbo',
	    # charm lines
	    'Hlt2CharmHadDpm2KPiPi_XSecTurbo',
            'Hlt2CharmHadDpm2KKPi_XSecTurbo', 
            'Hlt2CharmHadDs2KKPi_XSecTurbo',
            'Hlt2CharmHadDs2PiPiPi_XSecTurbo',
            'Hlt2CharmHadLc2KPPi_XSecTurbo',
	    'Hlt2CharmHadLc2KPK_XSecTurbo',
            'Hlt2CharmHadLc2PiPPi_XSecTurbo',
	    'Hlt2CharmHadD02KPi_XSecTurbo',
	    'Hlt2CharmHadDst_2D0Pi_D02KPi_XSecTurbo',
	    'Hlt2CharmHadDst_2D0Pi0_D02KPi_XSecTurbo', 
	    'Hlt2CharmHadDst_2DsGamma_Ds2KKPi_XSecTurbo',
	    'Hlt2CharmHadDst_2D0Pi_D02K3Pi_XSecTurbo',
            'Hlt2CharmHadSigmac_2LcPi_XSecTurbo',
            #tracking efficiency lines
	    'Hlt2TrackEffDiMuonMuonTTPlusTaggedTurbo', 
	    'Hlt2TrackEffDiMuonMuonTTMinusTaggedTurbo',
	    'Hlt2TrackEffDiMuonVeloMuonPlusTaggedTurbo',                
            'Hlt2TrackEffDiMuonVeloMuonMinusTaggedTurbo',                
	    'Hlt2TrackEffDiMuonDownstreamPlusTaggedTurbo',                
	    'Hlt2TrackEffDiMuonDownstreamMinusTaggedTurbo',                
	    'Hlt2TrackEffDiMuonMuonTTPlusMatchedTurbo',                
	    'Hlt2TrackEffDiMuonMuonTTMinusMatchedTurbo',
	    'Hlt2TrackEffDiMuonVeloMuonPlusMatchedTurbo',                
	    'Hlt2TrackEffDiMuonVeloMuonMinusMatchedTurbo',                
	    'Hlt2TrackEffDiMuonDownstreamPlusMatchedTurbo',                
	    'Hlt2TrackEffDiMuonDownstreamMinusMatchedTurbo',                
	    # pass through nobias
	    'Hlt2PassThrough'	    
            #'Hlt2Lumi','Hlt2DebugEvent','Hlt2Forward','Hlt2ErrorEvent','Hlt2Transparent', # technical lines
	 ]


        return hlt2
       
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        lines =  [ 'Hlt1TrackAllL0', 'Hlt1TrackMuon'
                 , 'Hlt1SingleMuonHighPT'
                 , 'Hlt1DiMuonHighMass'
        	 , 'Hlt1HighVeloMult'
		 , 'Hlt1HighVeloMultSinglePV' 
		 , 'Hlt1CEPVeloCut'
                 , 'Hlt1NoPVPassThrough'
	         , 'Hlt1NonBeamBeamNoBias'
                 , 'Hlt1MBNoBias'  # nobias leading crossing
                 , 'Hlt1CalibRICHMirror'
		 , 'Hlt1Lumi' # lumi lines
	         , 'Hlt1BeamGasNoBeamBeam1', 'Hlt1BeamGasNoBeamBeam2'  
                 , 'Hlt1BeamGasBeam1', 'Hlt1BeamGasBeam2'
                 , 'Hlt1BeamGasCrossingEnhancedBeam1', 'Hlt1BeamGasCrossingEnhancedBeam2'
                 , 'Hlt1BeamGasCrossingForcedReco', 'Hlt1BeamGasCrossingForcedRecoFullZ'
                 , 'Hlt1BeamGasCrossingParasitic', 'Hlt1BeamGasHighRhoVertices'
               #  , 'Hlt1CharmCalibrationNoBias' # technical lines
               #  , 'Hlt1L0Any','Hlt1L0AnyNoSPD'
               #  , 'Hlt1ODINTechnical', 'Hlt1Tell1Error' , 'Hlt1ErrorEvent'  
               #  , 'Hlt1MBMicroBiasVelo','Hlt1MBMicroBiasTStation'
               #  , 'Hlt1VeloClosingMicroBias'
            ]



        
        return lines 
