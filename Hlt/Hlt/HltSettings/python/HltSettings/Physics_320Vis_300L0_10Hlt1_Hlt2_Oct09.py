from Physics_320Vis_300L0_10Hlt1_Aug09 import Physics_320Vis_300L0_10Hlt1_Aug09
class Physics_320Vis_300L0_10Hlt1_Hlt2_Oct09( Physics_320Vis_300L0_10Hlt1_Aug09 ) :
    """
    Threshold settings for Hlt1 and 2 for 2010 conditions
    Hlt2 : run (almost) all with default cuts
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg
    @date 2009-10-27
    """
    
    __all__ = ( 'ActiveHlt1Lines', 'ActiveHlt2Lines', 'Thresholds','L0TCK' )

    def HltType(self) :
        self.verifyType( Physics_320Vis_300L0_10Hlt1_Hlt2_Oct09 )
        return          'Physics_320Vis_300L0_10Hlt1_Hlt2_Oct09'

    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        d = Physics_320Vis_300L0_10Hlt1_Aug09.Thresholds(self)
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
        list = Physics_320Vis_300L0_10Hlt1_Aug09.ActiveHlt2Lines(self)
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
## (di)muons : Leptonic list
##
        list.extend( [ 'Hlt2SingleMuonLow',
                       'Hlt2IncMuTrack',
                       'Hlt2UnbiasedJPsi',
                       'Hlt2UnbiasedPsi2S',
                       'Hlt2UnbiasedBmm',
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
        list.extend( B2DX_320Vis().ActiveHlt2Lines() )
        
##
## exclusive : whole list
##
        
        from Exclusive_Nominal import Exclusive_Nominal
        list.extend( Exclusive_Nominal().ActiveHlt2Lines() )
##
        return list
        
