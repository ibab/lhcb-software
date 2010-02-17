from Physics_320Vis_300L0_10Hlt1_Jan10 import Physics_320Vis_300L0_10Hlt1_Jan10
class Physics_320Vis_300L0_10Hlt1_Hlt2_Feb10( Physics_320Vis_300L0_10Hlt1_Jan10 ) :
    """
    Threshold settings for Hlt1 and 2 for 2010 conditions
    Hlt2 : run (almost) all with default cuts
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg
    @date 2010-02-08
    """
    
    __all__ = ( 'ActiveHlt1Lines', 'ActiveHlt2Lines', 'Thresholds','L0TCK' )

    def HltType(self) :
        self.verifyType( Physics_320Vis_300L0_10Hlt1_Hlt2_Feb10 )
        return          'Physics_320Vis_300L0_10Hlt1_Hlt2_Feb10'

    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        d = Physics_320Vis_300L0_10Hlt1_Jan10.Thresholds(self)
        # lower prescales
        from Hlt2Lines.Hlt2CommissioningLines import Hlt2CommissioningLinesConf
        d.update( {Hlt2CommissioningLinesConf : { 'Prescale' : { 'Hlt2PassThrough'  : 0.0001 # changed
                                                               , 'Hlt2Forward'      : 0. # first needs a check that /Hlt/Track/Long actually exists..
                                                               , 'Hlt2DebugEvent'   : 0.0001
                                                }              }
                   })
        # add any other non default cut here
        # return
        return d
            
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        # return [ 'Hlt2PassThrough', 'Hlt2Transparent','Hlt2Forward','Hlt2DebugEvent']
        list = Physics_320Vis_300L0_10Hlt1_Jan10.ActiveHlt2Lines(self)
##
## topo : leave out 4-body lines
##
        list.extend( [ 'Hlt2Topo2BodySA',
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
                  'Hlt2TopoTF2BodyReq2Yes' ] )
##
## charm : leave out 4-body lines
##
        list.extend( [ 'Hlt2Charm3BodySA',
                       'Hlt2Charm2BodySA',
                       #'Hlt2Charm4BodySA',
                       'Hlt2CharmTF2BodySignal',
                       'Hlt2CharmTF3BodySignal',
                       #'Hlt2CharmTF4BodySignal',
                       'Hlt2CharmTF2BodyWideMass',
                       'Hlt2CharmTF3BodyWideMass'
                       #'Hlt2CharmTF4BodyWideMass'
                       ] )
##
## (di)muons : Leptonic list
##
        list.extend( [ 'Hlt2SingleMuonLow',
                       'Hlt2IncMuTrack',
                       'Hlt2DiMuonUnbiasedJPsi',
                       'Hlt2DiMuonUnbiasedPsi2S',
                       'Hlt2DiMuonUnbiasedBmm',
                       'Hlt2DiMuonUnbiasedZmm',
                       'Hlt2DiMuonDY1',
                       'Hlt2DiMuonDY2',
                       'Hlt2DiMuonDY3',
                       'Hlt2DiMuonDY4',
                       'Hlt2BiasedDiMuonMass',
                       'Hlt2BiasedDiMuonIP'] )
##
## phi : whole list
##
        from Phi_Nominal import PhiHigh_Nominal
        list.extend( PhiHigh_Nominal().ActiveHlt2Lines() )
##
## B 2DX
##
        from B2DX_320Vis import B2DX_320Vis
        #list.extend( B2DX_320Vis().ActiveHlt2Lines() )
        
##
## exclusive : whole list
##
        
        from Exclusive_Nominal import Exclusive_Nominal
        list.extend( Exclusive_Nominal().ActiveHlt2Lines() )
##
## add express lines
##
        from Express_Hlt2_Jan10 import Express_Hlt2_Jan10
        list.extend( Express_Hlt2_Jan10().ActiveHlt2Lines() )
##
        return list
        
