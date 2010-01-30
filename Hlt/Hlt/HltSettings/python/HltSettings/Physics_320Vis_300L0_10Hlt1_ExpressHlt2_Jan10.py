from HltSettings.Physics_320Vis_300L0_10Hlt1_Jan10 import Physics_320Vis_300L0_10Hlt1_Jan10
class Physics_320Vis_300L0_10Hlt1_ExpressHlt2_Jan10(  Physics_320Vis_300L0_10Hlt1_Jan10 ):
    """
    Threshold settings for Hlt1 for 2010 conditions
    added Hlt2 Express lines for commisioning

    modified version from Physics_320Vis_300L0_10Hlt1_Aug09.py

    WARNING: Express line output rates are not scaled,
             don't expect meaningful rates here!
    
    @date 2010-01-25
    """
    
    __all__ = ( 'ActiveHlt1Lines', 'ActiveHlt2Lines', 'Thresholds','L0TCK' )

    def HltType(self) :
        self.verifyType( Physics_320Vis_300L0_10Hlt1_ExpressHlt2_Jan10 )
        return          'Physics_320Vis_300L0_10Hlt1_ExpressHlt2_Jan10'

    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        return [ 'Hlt2PassThrough'
               , 'Hlt2Transparent'
               , 'Hlt2DebugEvent'
               , 'Hlt2ExpressJPsi'
               , 'Hlt2ExpressJPsiTagProbe'
               , 'Hlt2ExpressLambda'
               , 'Hlt2ExpressKS'
               , 'Hlt2ExpressDs2PhiPi'
               , 'Hlt2ExpressBeamHalo'
               ]
