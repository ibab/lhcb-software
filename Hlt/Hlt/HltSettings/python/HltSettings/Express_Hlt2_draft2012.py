class Express_Hlt2_draft2012(object) :
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
                , 'Hlt2ExpressD02KPi'
                ]
        return lines


    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        
        d = { }

        from Hlt2Lines.Hlt2ExpressLines import Hlt2ExpressLinesConf

        d.update( { Hlt2ExpressLinesConf : { 

            'TrackChi2'                    :    4
            , 'ExJPsiMuIPChi2'             :   25
            , 'ExPhiDOCAMax'		   :  0.3 # mm
            , 'ExPhiMIPCHI2DV'             :   16
            , 'ExPhiKPt'                   :  500 # MeV
            , 'ExPhiKMIPCHI2DV'            :   16
            , 'ExDsVCHI2'                  :    9
            , 'ExDsMIPCHI2DV'	           :    9
            , 'ExDsPiPt'                   :  500 # MeV
            , 'ExDsPiMIPCHI2DV'            :   16
            


            , 'Prescale'   : {   'Hlt2ExpressJPsi'            :  1
                                 , 'Hlt2ExpressJPsiTagProbe'  :  0
                                 , 'Hlt2ExpressLambda'        : 0.01
                                 , 'Hlt2ExpressKS'            : 0.001
                                 , 'Hlt2ExpressDs2PhiPi'      : 1
                                 , 'Hlt2ExpressBeamHalo'      : 0.001
                                 , 'Hlt2ExpressDStar2D0Pi'    : 0.1
                                 , 'Hlt2ExpressD02KPi'        : 0.1
                                 }
         
             
            }})

        return d
