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

class TurboOnly( object ):
    """
    Settings for Hlt2 Turbo lines USED IN NIGHTLY TESTS OF THE TURBO STREAM.

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
        return '0xFF66' 

    def HltType(self) :
        self.verifyType( TurboOnly ) 
        return          'TurboOnly'
    
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

        from Hlt2Lines.Commissioning.Lines      import CommissioningLines
        from Hlt2Lines.DiMuon.Lines             import DiMuonLines
        from Hlt2Lines.SingleMuon.Lines         import SingleMuonLines
        from Hlt2Lines.TrackEffDiMuon.Lines     import TrackEffDiMuonLines
        from Hlt2Lines.TrackEff.Lines           import TrackEffLines
        from GaudiKernel.SystemOfUnits import MeV, GeV, mm

        thresholds = { 
                       }

        return thresholds
    
                   
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        hlt2 = [
	        'Hlt2PassThrough', 'Hlt2Lumi','Hlt2DebugEvent','Hlt2Forward','Hlt2ErrorEvent','Hlt2Transparent', # technical lines
                'Hlt2CharmHadDpm2KPiPi_XSecTurbo',
                'Hlt2CharmHadDpm2KKPi_XSecTurbo',
                'Hlt2CharmHadDs2KKPi_XSecTurbo',
                'Hlt2CharmHadDs2PiPiPi_XSecTurbo',
                'Hlt2CharmHadLc2KPPi_XSecTurbo',
                'Hlt2CharmHadLc2PiPPi_XSecTurbo',
                'Hlt2CharmHadSigmac_2LcPi_XSecTurbo',
                'Hlt2CharmHadDst_2D0Pi0_D02KPi_XSecTurbo',
		'Hlt2DiMuonJPsiTurbo',
                'Hlt2DiMuonPsi2STurbo',
		'Hlt2DiMuonBTurbo',
		'Hlt2JPsiReFitPVsTurbo',
		'Hlt2RadiativeB2GammaGammaTurbo'
          ]


        return hlt2
       
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        lines =  [ 'Hlt1TrackAllL0', 'Hlt1TrackMuon'
                 , 'Hlt1SingleMuonHighPT'
                 , 'Hlt1DiMuonHighMass'
                 , 'Hlt1B2GammaGamma'
                 , 'Hlt1BeamGasNoBeamBeam1', 'Hlt1BeamGasNoBeamBeam2'   # technical lines
                 , 'Hlt1BeamGasBeam1', 'Hlt1BeamGasBeam2'
                 , 'Hlt1BeamGasCrossingEnhancedBeam1', 'Hlt1BeamGasCrossingEnhancedBeam2'
                 , 'Hlt1BeamGasCrossingForcedReco', 'Hlt1BeamGasCrossingForcedRecoFullZ'
                 , 'Hlt1BeamGasCrossingParasitic', 'Hlt1BeamGasHighRhoVertices'
                 , 'Hlt1NoPVPassThrough'
                 , 'Hlt1MBNoBias' 
                 , 'Hlt1ODINTechnical', 'Hlt1Tell1Error' , 'Hlt1ErrorEvent'   
                 , 'Hlt1MBMicroBiasVelo','Hlt1MBMicroBiasTStation' 
                 , 'Hlt1VeloClosingMicroBias' 
                 , 'Hlt1CharmCalibrationNoBias'
                 , 'Hlt1Lumi', 'Hlt1LumiMidBeamCrossing'
                 , 'Hlt1L0Any','Hlt1L0AnyNoSPD'
            ]



        
        return lines 
