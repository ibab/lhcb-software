class Express_Hlt2_Aug11(object) :
    """
    Threshold settings for Hlt2 Express stream lines
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author J.Albrecht, P. Koppenburg
    @date 2010-02-08
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [ 'Hlt2ExpressJPsi'
                , 'Hlt2ExpressJPsiTagProbe'
                , 'Hlt2ExpressLambda'
                , 'Hlt2ExpressKS'
                , 'Hlt2ExpressDs2PhiPi'
                , 'Hlt2ExpressBeamHalo'
                , 'Hlt2ExpressDStar2D0Pi'
                ]
        return lines
