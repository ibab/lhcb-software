class Charming_Nominal :
    """
    Threshold settings for Hlt1 and 2 in nominal conditions (charming scenario)
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, G. Raven
    @date 2009-08-06
    """
    
    __all__ = ( 'ActiveHlt1Lines', 'ActiveHlt2Lines', 'Thresholds' )
    
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
        d.update( { Hlt2TopologicalLinesConf  : { 'ComRobAllTrkPtLL' : 600.0
                                               , 'ComRobAllTrkPVIPLL' : 0.025
                                               , 'ComRobPairMinDocaUL' : 0.1
                                               , 'ComRobTrkMaxPtLL' : 1500.0
                                               , 'ComTFVtxPVDispChi2LL' : 81.0
                                               , 'RobustPointingUL' : 0.12
                                               , 'TFPointUL' : 0.12
                                               , 'CharmRobustPointUL' : 0.28
                                               , 'CharmTFPointUL' : 0.12
                                               }
                    })
        #
        # presently muon line rates are controlled by ActiveHlt2Lines below
        #
        return d 
    
######################################################################################################
    #
    # Active Hlt1 Lines
    #
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active Hlt1 lines: take them from  Hlt1_Nominal
        """
        from Hlt1_Nominal import Hlt1_Nominal
        return Hlt1_Nominal().ActiveHlt1Lines()
    
######################################################################################################
    #
    # Active Hlt2 Lines
    #
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        
       
##
## topo : leave out 4-body lines
##
        list = [ 'Hlt2Topo2BodySA',
                 #'Hlt2Topo4BodySA',
                 'Hlt2Topo3BodySA',
                 #'Hlt2TopoTF4BodySA',
                 'Hlt2TopoTF3BodySA',
                 'Hlt2TopoTF2BodySA',
                 #'Hlt2TopoTF4BodyReq4Yes',
                 #'Hlt2TopoTF4BodyReq3Yes',
                 #'Hlt2TopoTF4BodyReq2Yes',
                 #'Hlt2TopoTF3BodyReq4Yes',
                 'Hlt2TopoTF3BodyReq3Yes',
                 'Hlt2TopoTF3BodyReq2Yes',
                 #'Hlt2TopoTF2BodyReq4Yes',
                 'Hlt2TopoTF2BodyReq3Yes',
                 'Hlt2TopoTF2BodyReq2Yes' ] 
##
## charm : leave out 4-body lines
##
        list.extend( [ 'Hlt2Topo3BodyCharmSA',
                       'Hlt2Topo2BodyCharmSA',
                       #'Hlt2Topo4BodyCharmSA',
                       'Hlt2TopoTF2BodyCharmSignal',
                       'Hlt2TopoTF3BodyCharmSignal',
                       #'Hlt2TopoTF4BodyCharmSignal',
                       'Hlt2TopoTF2BodyCharmWideMass',
                       'Hlt2TopoTF3BodyCharmWideMass'
                       #'Hlt2TopoTF4BodyCharmWideMass'
                       ] )
##
## (di)muons : Charming list
##
        list.extend( [ 'Hlt2IncMuTrackMid',
                       'Hlt2UnbiasedJPsi',
                       'Hlt2UnbiasedPsi2S',
                       'Hlt2UnbiasedBmm'] )
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
        list.extend( Exclusive_Nominal().ActiveHlt2Lines() )
##
## commissioning : whole list
##
        from Commissioning_Nominal import Commissioning_Nominal
        list.extend( Commissioning_Nominal().ActiveHlt2Lines() )
        
        return list
    
    
    
