from HltSettings.Physics_MinBiasL0_PassThroughHlt_May10  import Physics_MinBiasL0_PassThroughHlt_May10 
class Physics_MinBiasL0_PassThroughHlt1_ExpressHlt2_May10 ( Physics_MinBiasL0_PassThroughHlt_May10 ) :
    """
    Threshold settings for Hlt1+Hlt2 for 2010 conditions
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, M. Gandelman, G. Raven
    @date 2009-08-26
    """
    
    __all__ = ( 'ActiveHlt2Lines' )

    def HltType(self) :
        self.verifyType( Physics_MinBiasL0_PassThroughHlt1_ExpressHlt2_May10 )
        return          'Physics_MinBiasL0_PassThroughHlt_ExpressHlt2_May10'

    def Thresholds(self) :
        thresholds = Physics_MinBiasL0_PassThroughHlt_May10.Thresholds(self)
        from Configurables import Hlt2ExpressLinesConf
        thresholds.update( { Hlt2ExpressLinesConf : { 'ExKSMassWin'         :  70
                                                    , 'ExLambdaMassWin'     :  40 # just catch the threshold which is at -37.2...
                                                    , 'ExLambdaMassWinWide' :  60
                                                    } } )
        return thresholds
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        hlt2 = [ 'Hlt2PassThrough', 'Hlt2Transparent','Hlt2Forward','Hlt2DebugEvent']
        from Express_Hlt2_Jan10 import Express_Hlt2_Jan10
        hlt2.extend( Express_Hlt2_Jan10().ActiveHlt2Lines() )
        return hlt2

