class Commissioning_PassThrough :
    """
    Threshold settings for Hlt1 and 2 in nominal conditions (list of commissioning)
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, G. Raven
    @date 2009-08-11
    """
    
    
    def __init__(self) :
        self.StripEndSequence = True

    def HltType(self) :
        return 'PassThrough'

    def L0TCK(self) :
        return None

    def ActiveHlt2Lines(self) :
        """
        Returns a list of active lines
        """
        
        return  [ ]
   
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active lines
        """
        
        return [ 'Hlt1ODINPhysics','Hlt1ODINTechnical','Hlt1Tell1Error' ]

    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        from Hlt1Lines.HltCommissioningLines  import HltCommissioningLinesConf
        return { HltCommissioningLinesConf : { 'Prescale' : { 'Hlt1ODINPhysics'  : 1.
                                                            , 'Hlt1ODINTechnical'     : 0. # @OnlineEnv.AcceptRate
                                                            , 'Hlt1Tell1Error'     : 1.
                                             }              } 
               }
