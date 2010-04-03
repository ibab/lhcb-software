from HltSettings.Physics_MinBiasL0_PassThroughHlt_Feb10  import Physics_MinBiasL0_PassThroughHlt_Feb10 
class Physics_MinBiasL0_PassThroughHlt1_ExpressHlt2_Feb10 ( Physics_MinBiasL0_PassThroughHlt_Feb10 ) :
    """
    Threshold settings for Hlt1+Hlt2 for 2010 conditions
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, M. Gandelman, G. Raven
    @date 2009-08-26
    """
    
    __all__ = ( 'ActiveHlt2Lines' )

    def HltType(self) :
        self.verifyType( Physics_MinBiasL0_PassThroughHlt1_ExpressHlt2_Feb10 )
        return          'Physics_MinBiasL0_PassThroughHlt_ExpressHlt2_Feb10'

    def Thresholds(self) :
        thresholds = Physics_MinBiasL0_PassThroughHlt_Feb10.Thresholds(self)
        from Configurables import Hlt2ExpressLinesConf
        thresholds.update( { Hlt2ExpressLinesConf : { 'Postscale'           :  { 'Hlt2ExpressJPsi'        : 'FALL'
                                                                               , 'Hlt2ExpressJPsiTagProbe': 'FALL'
                                                                               , 'Hlt2ExpressLambda'      : 'FALL'
                                                                               , 'Hlt2ExpressKS'          : 'FALL'
                                                                               , 'Hlt2ExpressDs2PhiPi'    : 'FALL'
                                                                               , 'Hlt2ExpressBeamHalo'    : 'FALL'
                                                                               , 'Hlt2ExpressDStar2D0Pi'  : 'FALL' 
                                                                               }
                                                    , 'ExKSMassWin'         :  70
                                                    , 'ExJPsiMassWindow'    :1000
                                                    #, 'ExJPsiMuPt'          : 250
                                                    , 'ExLambdaMassWin'     :  40 # just catch the threshold which is at -37.2...
                                                    , 'ExLambdaMassWinWide' : 100
                                                    , 'ExDsMassWinWide'     : 510
                                                    , 'ExDsMassWin'         : 500
                                                    , 'ExD0MassWinWide'     : 510
                                                    , 'ExD0MassWin'         : 500
                                                    , 'ExDStarMassDiff'     : 180
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

