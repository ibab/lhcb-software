class Exclusive_Leptonic :
    """
    Threshold settings for Hlt1 and 2 in nominal conditions (list of exclusive)
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author J. Albrecht
    @date 2010-07-14
    """
    
    __all__ = ( 'ActiveHlt2Lines' )
    
    
    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """

        lines = [

            # Dstar for D2mumu
            'Hlt2Dst2PiD02KMu', 
            'Hlt2Dst2PiD02KPi', 
            'Hlt2Dst2PiD02MuMu', 
            'Hlt2Dst2PiD02PiMu',
            'Hlt2Dst2PiD02PiPi', 
            'Hlt2Dst2PiD02EMu', 

            # another D2mumu -- this one lines in Hlt2CharmLines
            'Hlt2CharmD02MuMu',
            'Hlt2CharmD02PiPiForD02MuMu',
            'Hlt2CharmD02KPiForD02MuMu',
            'Hlt2CharmD02KKForD02MuMu',
            
            # Bc2JpsiX
            'Hlt2TFBc2JpsiH', 
            'Hlt2TFBc2JpsiHDetached', 
            'Hlt2TFBc2JpsiHPrescaled', 
            'Hlt2TFBc2JpsiMuX', 
            'Hlt2TFBc2JpsiMuXSignal',

            # Exclusive DiMuons
            'Hlt2diphotonDiMuon'
             ]
        
        return lines

   
    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        
        # keep pass through thresholds
        d = { }

        from Hlt2Lines.Hlt2Dst2D2XXLines import Hlt2Dst2D2XXLinesConf
                    
        d.update( { Hlt2Dst2D2XXLinesConf : { 'Prescale'   : { 'Hlt2Dst2PiD02PiPi'       :  0.1
                                                             , 'Hlt2Dst2PiD02KPi'        :  0.02
                                                             , 'Hlt2Dst2PiD02MuMu'       :  1
                                                             , 'Hlt2Dst2PiD02PiMu'       :  1
                                                             , 'Hlt2Dst2PiD02KMu'        :  0.2
                                                             , 'Hlt2Dst2PiD02EMu'        :  1
                                                             , 'Hlt2Dst2PiD02EPi'        :  1
                                                             , 'Hlt2Dst2PiD02EK'         :  1
                                                             }
                  }                         }
                )

        from Hlt2Lines.Hlt2CharmLines import Hlt2CharmLinesConf

        d.update( { Hlt2CharmLinesConf : { 'Prescale' : { 'Hlt2CharmD02PiPiForD02MuMu' : 0.001
                                                        , 'Hlt2CharmD02KPiForD02MuMu'  : 0.001
                                                        , 'Hlt2CharmD02KKForD02MuMu'   : 0.001
                                                        } 
                                         } 
                  } 
                )

        
        return d
