class Physics_MinBiasL0_PassThroughHlt_Jan10 :
    """
    Threshold settings for Hlt1 and 2 in nominal conditions (list of commissioning)
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, G. Raven
    @date 2009-08-11
    """
    
    
    def __init__(self) :
        self.StripEndSequence = False

    def HltType(self) :
        return 'Physics_MinBiasL0_PassThroughHlt_Jan10'

    def L0TCK(self) :
        return "0x1409"

    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        return  [ ]
   
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        return [ 'Hlt1ODINPhysics','Hlt1ODINTechnical','Hlt1Tell1Error'
               , 'Hlt1L0Any'
               , 'Hlt1LumiNoBeam','Hlt1LumiBeamCrossing','Hlt1LumiBeam1','Hlt1LumiBeam2'
               #, 'Hlt1LumiLowNoBeam','Hlt1LumiLowBeamCrossing','Hlt1LumiLowBeam1','Hlt1LumiLowBeam2'
               , 'Hlt1MiniBiasNoBeam','Hlt1MiniBiasBeamCrossing','Hlt1MiniBiasBeam1','Hlt1MiniBiasBeam2'
               , 'Hlt1NoBiasNoBeam','Hlt1NoBiasBeamCrossing','Hlt1NoBiasBeam1','Hlt1NoBiasBeam2'
               , 'Hlt1VeloASide','Hlt1VeloCSide'
               ]

    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        from Hlt1Lines.Hlt1CommissioningLines  import Hlt1CommissioningLinesConf
        from Hlt1Lines.Hlt1LumiLines  import Hlt1LumiLinesConf
        from Hlt1Lines.Hlt1MiniBiasLines  import Hlt1MiniBiasLinesConf
        from Hlt1Lines.Hlt1NoBiasLines  import Hlt1NoBiasLinesConf
        from Hlt1Lines.Hlt1VeloLines  import Hlt1VeloLinesConf
        from Hlt1Lines.Hlt1L0Lines  import Hlt1L0LinesConf
        return { Hlt1CommissioningLinesConf : { 'Prescale' : { 'Hlt1ODINPhysics'    : 1.
                                                             , 'Hlt1ODINTechnical'  : 0. # @OnlineEnv.AcceptRate
                                                             , 'Hlt1Tell1Error'     : 0.
                                                             } 
                                              }
               , Hlt1LumiLinesConf : { 'EnableReco' : True  }
               , Hlt1L0LinesConf   : { 'Prescale' : { '.*' : 1 } }
               , Hlt1VeloLinesConf : { 'Prescale' : { '.*' : 0 } }
               , Hlt1MiniBiasLinesConf : { 'Prescale' : { '.*' : 0.5 } }
               , Hlt1NoBiasLinesConf : { 'Prescale' : { 'Hlt1NoBiasNoBeam'        : 0.01,
                                                        'Hlt1NoBiasBeam1'         : 0.05,
                                                        'Hlt1NoBiasBeam2'         : 0.01,
                                                        'Hlt1NoBiasBeamCrossing'  : 0.1,
                                                        } }
                 }
