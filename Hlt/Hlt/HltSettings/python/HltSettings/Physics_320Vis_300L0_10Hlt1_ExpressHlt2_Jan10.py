from HltSettings.Physics_320Vis_300L0_10Hlt1_Jan10 import Physics_320Vis_300L0_10Hlt1_Jan10
class Physics_320Vis_300L0_10Hlt1_ExpressHlt2_Jan10(  Physics_320Vis_300L0_10Hlt1_Jan10 ):
    """
    Threshold settings for Hlt1 for 2010 conditions
    added Hlt2 Express lines for commisioning

    modified version from Physics_320Vis_300L0_10Hlt1_Aug09.py

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
        lines = [ 'Hlt2PassThrough'
                , 'Hlt2Transparent'
                , 'Hlt2DebugEvent'
                ]
        from Express_Hlt2_Jan10 import Express_Hlt2_Jan10
        lines.extend( Express_Hlt2_Jan10().ActiveHlt2Lines() )
                                       
        return lines
