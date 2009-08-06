class Hadronic_Nominal :
    """
    Threshold settings for Hlt1 and 2 in nominal conditions (hadronic scenario)
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, G. Raven
    @date 2009-08-06
    """
    
    __all__ = ( 'ActiveLines', 'Thresholds' )
    
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        
        from Hlt2Lines.Hlt2InclusiveMuonLines import Hlt2InclusiveMuonLinesConf
        from Hlt2Lines.Hlt2InclusiveDiMuonLines import Hlt2InclusiveDiMuonLinesConf
        from Hlt2Lines.Hlt2TopologicalLines  import Hlt2TopologicalLinesConf
    
        # Hlt thresholds
        from Hlt1_Nominal import Hlt1_Nominal
        d = Hlt1_Nominal().Thresholds()
        # Phi thresholds
        from Phi_Nominal import PhiLow_Nominal
        d.update( PhiLow_Nominal().Thresholds() )
        # Remaining
        d.update(  { Hlt2TopologicalLinesConf  : { 'ComRobAllTrkPtLL' : 400.0
                                               , 'ComRobAllTrkPVIPLL' : 0.025
                                               , 'ComRobPairMinDocaUL' : 0.15
                                               , 'ComRobTrkMaxPtLL' : 1500.0
                                               , 'ComTFVtxPVDispChi2LL' : 196.0
                                               , 'RobustPointingUL' : 0.28
                                               , 'TFPointUL' : 0.16
                                               , 'CharmRobustPointUL' : 0.04
                                               , 'CharmTFPointUL' : 0.04
                                               , 'Prescale' : { 'Hlt2TopoTF4BodyCharmSignal' : 0.2
                                                              , 'Hlt2TopoTF4BodyReq4Yes' : 0.2
                                                              , 'Hlt2TopoTF4BodyReq3Yes' : 0.2
                                                              , 'Hlt2TopoTF4BodyReq2Yes' : 0.2 } 
                                               }
                     })
        #
        # presently muon line rates are controlled by ActiveLines below
        #
        return d 
    
######################################################################################################
    #
    # Active Lines
    #
    def ActiveLines(self) :
        """
        Returns a list of active lines
        """
        
        from Hlt1_Nominal import Hlt1_Nominal
        
        list = Hlt1_Nominal().ActiveLines()
       
##
## topo : whole list
##
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
                       'Hlt2TopoTF2BodyReq2Yes' ] )
##
## charm : whole list
##
        list.extend( [ 'Hlt2Topo3BodyCharmSA',
                       'Hlt2Topo2BodyCharmSA',
                       'Hlt2Topo4BodyCharmSA',
                       'Hlt2TopoTF2BodyCharmSignal',
                       'Hlt2TopoTF3BodyCharmSignal',
                       'Hlt2TopoTF4BodyCharmSignal',
                       'Hlt2TopoTF2BodyCharmWideMass',
                       'Hlt2TopoTF3BodyCharmWideMass',
                       'Hlt2TopoTF4BodyCharmWideMass',
                       'Hlt2DX3BodyCopKsDD',
                       'Hlt2DX3BodyCopNoKs' ] )
##
## (di)muons : Hadronic list
##
        list.extend( [ 'Hlt2IncMuTrackLow',
                       'Hlt2UnbiasedJPsiLow',
                       'Hlt2UnbiasedPsi2S',
                       'Hlt2UnbiasedBmm' ] )
##
## phi : whole list
##
        list.extend( [ 'Hlt2IncPhi',
                       'Hlt2IncPhiRobust',
                       'Hlt2IncPhiTF',
                       'Hlt2IncPhiSidebands' ])
##
## exclusive : whole list
##
        from Exclusive_Nominal import Exclusive_Nominal
        list.extend( Exclusive_Nominal().ActiveLines() )
        
        return list
    
    
    
