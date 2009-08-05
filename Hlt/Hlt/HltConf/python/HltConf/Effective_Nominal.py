class Effective_Nominal :
    """
    Threshold settings for Hlt1 and 2 in nominal conditions
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, M. Gandelman, G. Raven
    @date 2009-08-05
    """
    
    __all__ = ( 'ActiveLines', 'Thresholds' )
    
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        
        from Hlt2Lines.Hlt2InclusiveMuonLines import Hlt2InclusiveMuonLinesConf
        from Hlt2Lines.Hlt2InclusiveDiMuonLines import Hlt2InclusiveDiMuonLinesConf
        from Hlt2Lines.Hlt2TopologicalLines  import Hlt2TopologicalLinesConf
        from Hlt2Lines.Hlt2InclusivePhiLines import Hlt2InclusivePhiLinesConf
        from Hlt2Lines.Hlt2XGammaLines import Hlt2XGammaLinesConf
    
        from Hlt1_Nominal import Hlt1_Nominal
        d = Hlt1_Nominal().Thresholds()
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
                   
                   , Hlt2XGammaLinesConf : {  'BsDirAngle' : 0.998
                                              , 'B0DirAngle' : 0.996
                                              , 'BsPVIPchi2' : 20
                                              , 'B0PVIPchi2' : 20  }
                   
                   , Hlt2TopologicalLinesConf : {  'ComRobAllTrkPtLL' : 850
                                                   , 'ComRobAllTrkPVIPLL' : 0.12
                                                   , 'ComRobPairMinDocaUL' : 0.50
                                                   , 'ComRobTrkMaxPtLL' : 1600
                                                   , 'ComTFVtxPVDispChi2LL' : 67
                                                   , 'RobustPointingUL' : 0.284
                                                   , 'TFPointUL' : 0.163
                                                   , 'CharmRobustPointUL' : 0.032
                                                   , 'CharmTFPointUL' : 0.032  }
                   })
        return d 
    
    def ActiveLines(self) :
        """
        Returns a list of active lines
        """
        
        from Hlt1_Nominal import Hlt1_Nominal
        
        list = Hlt1_Nominal().ActiveLines()
        list.extend( [ 'Hlt2Topo2BodySA',
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
                       'Hlt2TopoTF2BodyReq2Yes',
                       'Hlt2Topo3BodyCharmSA',
                       'Hlt2Topo2BodyCharmSA',
                       'Hlt2Topo4BodyCharmSA',
                       'Hlt2TopoTF2BodyCharmSignal',
                       'Hlt2TopoTF3BodyCharmSignal',
                       'Hlt2TopoTF4BodyCharmSignal',
                       'Hlt2TopoTF2BodyCharmWideMass',
                       'Hlt2TopoTF3BodyCharmWideMass',
                       'Hlt2TopoTF4BodyCharmWideMass',
                       'Hlt2DX3BodyCopKsDD',
                       'Hlt2DX3BodyCopNoKs',
                       'Hlt2Bs2JpsiPhi',
                       'Hlt2Bs2JpsiPhiSignal',
# Not in effective                       'Hlt2Bu2eeK',
# Not in effective                       'Hlt2Bu2eeKSignal',
# Not in effective                       'Hlt2Bu2eeKJpsi',
# Not in effective                       'Hlt2Bu2eeKHighMass',
# Not in effective                       'Hlt2Bu2MuMuK',
# Not in effective                       'Hlt2Bu2MuMuKSignal',
# Not in effective                       'Hlt2Bu2MuMuKJpsi',
# Not in effective                       'Hlt2Bu2MuMuKHighMass',
# Not in effective                       'Hlt2Bd2JpsiKstar',
# Not in effective                       'Hlt2Bd2JpsiKstarSignal',
# Not in effective                       'Hlt2Bs2PhiPhi',
# Not in effective                       'Hlt2DisplVertices',
# Not in effective                       'Hlt2Bd2JpsiMuMuKsLLBiased',
# Not in effective                       'Hlt2Bd2JpsiMuMuKsDDBiased',
# Not in effective                       'Hlt2Bd2JpsiMuMuKsLLUnbiased',
# Not in effective                       'Hlt2Bd2JpsiMuMuKsDDUnbiased',
# Not in effective                       'Hlt2B2JpsiX_MuMu',
# Not in effective                       'Hlt2Bu2JpsiK',
# Not in effective                       'Hlt2Bu2JpsiKSignal',
                       'Hlt2IncPhi',
                       'Hlt2IncPhiRobust',
                       'Hlt2IncPhiTF',
                       'Hlt2IncPhiSidebands',
                       'Hlt2PhiGamma',
                       'Hlt2KstGamma',
                       'Hlt2Bs2JpsiPhiPrescaled',
                       'Hlt2Bs2JpsiPhiDetached',
                       'Hlt2UnbiasedDiMuon',
                       'Hlt2UnbiasedJPsi',
                       'Hlt2UnbiasedPsi2S',
                       'Hlt2UnbiasedBmm',
                       'Hlt2UnbiasedJPsiLow',
                       'Hlt2BiasedDiMuonRobust',
                       'Hlt2BiasedDiMuonRefined',
                       'Hlt2BiasedDiMuonMass',
                       'Hlt2BiasedDiMuonIP',
                       'Hlt2B2HH',
                       'Hlt2SingleMuonMid',
                       'Hlt2SingleMuonLow',
                       'Hlt2SingleHighPTMuon',
                       'Hlt2IncMuTrackMid',
                       'Hlt2IncMuTrack4Jpsi' ])
        
        return list
    
    
    
