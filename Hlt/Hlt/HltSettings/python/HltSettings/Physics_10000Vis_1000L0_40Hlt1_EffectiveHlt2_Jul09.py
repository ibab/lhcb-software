from HltSettings.Physics_10000Vis_1000L0_40Hlt1_Apr09  import Physics_10000Vis_1000L0_40Hlt1_Apr09
class Physics_10000Vis_1000L0_40Hlt1_EffectiveHlt2_Jul09( Physics_10000Vis_1000L0_40Hlt1_Apr09 ) :
    """
    Threshold settings for Hlt1 and 2 in nominal conditions
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, M. Gandelman, G. Raven
    @date 2009-08-05
    """

    def HltType(self) :
        self.verifyType( Physics_10000Vis_1000L0_40Hlt1_EffectiveHlt2_Jul09 )
        return          'Physics_10000Vis_1000L0_40Hlt1_EffectiveHlt2_Jul09'
    
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        
        from Hlt2Lines.Hlt2InclusiveMuonLines import Hlt2InclusiveMuonLinesConf
        from Hlt2Lines.Hlt2InclusiveDiMuonLines import Hlt2InclusiveDiMuonLinesConf
        from Hlt2Lines.Hlt2TopologicalLines  import Hlt2TopologicalLinesConf
        from Hlt2Lines.Hlt2InclusivePhiLines import Hlt2InclusivePhiLinesConf
        from Hlt2Lines.Hlt2B2XGammaLines import Hlt2B2XGammaLinesConf
        from Hlt2Lines.Hlt2CharmLines        import Hlt2CharmLinesConf
    
        # Hlt thresholds
        # pick up inherited thresholds, and update them...
        d = Physics_10000Vis_1000L0_40Hlt1_Apr09.Thresholds(self)
        # switch the Hlt2 pass through line back to normal
        from Hlt2Lines.Hlt2CommissioningLines import Hlt2CommissioningLinesConf
        d.update( { Hlt2CommissioningLinesConf : { 'Prescale' : { 'Hlt2PassThrough' : 0.0001 
                                                                , 'Hlt2Forward'     : 0. # first needs a check that /Hlt/Track/Long actually exists..
                                                                , 'Hlt2DebugEvent'  : 0.0001
                                                                } 
                                                 } } )
        # Remaining
        d.update(  { Hlt2InclusiveMuonLinesConf : { 'SingleMuonPt' : 5550  
                                                  , 'SingleMuonIP' : 0.73        
                                                  , 'MuTrackMuPt' : 1500        
                                                  , 'MuTrackTrPt' : 950       
                                                  , 'MuTrackMuIP' : 0.1        
                                                  , 'MuTrackTrIP' : 0.17        
                                                  , 'MuTrackPoint' : 0.96    }
                   
                   , Hlt2InclusiveDiMuonLinesConf : { 'BiasedTMass' : 3000
                                                    , 'BiasedSingleMuonTIP' : 0.08
                                                    , 'BiasedLTimeT' : 0.16
                                                    , 'BiasedSingleIPTChi2' : 65
                                                    , 'BiasedPVDistanceTChi2' : 154
                                                    , 'UnbiasedJPsiPt' : 250
                                                    , 'UnbiasedJPsiMuPt' : 550
                                                    , 'UnbiasedJPsiVertexChi2' : 22
                                                    , 'UnbiasedBmmMinMass' : 5050
                                                    , 'UnbiasedBmmVertexChi2' : 25  }
                   
                   , Hlt2InclusivePhiLinesConf : { 'KaonIP' : 0.06
                                                 , 'PhiPT' : 1800
                                                 , 'TFKaonIPS' : 7
                                                 , 'TFPhiPT' : 1800
                                                 , 'TFKaonRichPID' : 0.453  }
                   
                   , Hlt2B2XGammaLinesConf : {  'BsDirAngle' : 0.998
                                            , 'B0DirAngle' : 0.996
                                            , 'BsPVIPchi2' : 20
                                            , 'B0PVIPchi2' : 20  }
                   
                   , Hlt2TopologicalLinesConf : {  'ComRobAllTrkPtLL' : 850
                                                 , 'ComRobAllTrkPVIPLL' : 0.12
                                                 , 'ComRobPairMinDocaUL' : 0.50
                                                 , 'ComRobTrkMaxPtLL' : 1600
                                                 , 'ComTFVtxPVDispChi2LL' : 67
                                                 , 'RobustPointingUL' : 0.284
                                                 , 'TFPointUL' : 0.163 }
                   , Hlt2CharmLinesConf       : {  'ComRobAllTrkPtLL' : 850
                                                 , 'ComRobAllTrkPVIPLL' : 0.12
                                                 , 'ComRobPairMinDocaUL' : 0.50
                                                 , 'ComRobTrkMaxPtLL' : 1600
                                                 , 'ComTFVtxPVDispChi2LL' : 67
                                                 , 'RobustPointingUL' : 0.032
                                                 , 'TFPointUL' : 0.032  }
                   })
        return d 
######################################################################################################
    #
    # Active Lines
    #
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        list = [ 'Hlt2Topo2BodySA',
                 'Hlt2Topo4BodySA',
                 'Hlt2Topo3BodySA',
                 'Hlt2TopoTF4BodySA',
                 'Hlt2TopoTF3BodySA',
                 'Hlt2TopoTF2BodySA',
                 'Hlt2TopoTF4BodyReq4Yes',
                 'Hlt2TopoTF4BodyReq3Yes',
                 'Hlt2TopoTF4BodyReq2Yes',
                 'Hlt2TopoTF3BodyReq4Yes',
                 'Hlt2TopoTF3BodyReq3Yes',
                 'Hlt2TopoTF3BodyReq2Yes',
                 'Hlt2TopoTF2BodyReq4Yes',
                 'Hlt2TopoTF2BodyReq3Yes',
                 'Hlt2TopoTF2BodyReq2Yes' ] 
##
## charm : whole list
##
        list.extend( [ 'Hlt2Charm3BodySA',
                       'Hlt2Charm2BodySA',
                       'Hlt2Charm4BodySA',
                       'Hlt2CharmTF2BodySignal',
                       'Hlt2CharmTF3BodySignal',
                       'Hlt2CharmTF4BodySignal',
                       'Hlt2CharmTF2BodyWideMass',
                       'Hlt2CharmTF3BodyWideMass',
                       'Hlt2CharmTF4BodyWideMass',
                       'Hlt2DX3BodyCopKsDD',
                       'Hlt2DX3BodyCopNoKs' ] )
##
## (di)muons : whole default list
##
        list.extend( [ 'Hlt2UnbiasedJPsi',
                       'Hlt2UnbiasedPsi2S',
                       'Hlt2UnbiasedBmm',
                       'Hlt2UnbiasedJPsiLow',
                       'Hlt2BiasedDiMuonMass',
                       'Hlt2BiasedDiMuonIP',                      
                       'Hlt2SingleMuonMid', 
                       'Hlt2SingleMuonLow', 
                       'Hlt2SingleHighPTMuon', 
                       'Hlt2IncMuTrackMid', 
                       'Hlt2IncMuTrack4Jpsi' ] )
##
## phi : whole list
##
        from Phi_Nominal import PhiHigh_Nominal
        list.extend( PhiHigh_Nominal().ActiveHlt2Lines() )
##
## exclusive : whole list
##
        from Exclusive_Nominal import Exclusive_Nominal
        list.extend( Exclusive_Nominal().ActiveHlt2Lines() )
        return list
