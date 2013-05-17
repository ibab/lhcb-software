class Commissioning_PassThrough :
    """
    Threshold settings for Hlt1 and 2 in nominal conditions (list of commissioning)
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, G. Raven
    @date 2009-08-11
    """
    
    
    def __init__(self) :
        self.StripEndSequence = ['']

    def HltType(self) :
        return 'PassThrough'

    def L0TCK(self) :
        return None

    def ActiveHlt2Lines(self) :
        """
        Returns a list of active Hlt2 lines
        """
        return  [ ]
   
    def ActiveHlt1Lines(self) :
        """
        Returns a list of active Hlt1 lines
        """
        return [ 'Hlt1ODINPhysics','Hlt1ODINTechnical','Hlt1Tell1Error' ]

    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        from Hlt1Lines.Hlt1CommissioningLines  import Hlt1CommissioningLinesConf
        return { Hlt1CommissioningLinesConf : { 'Prescale' : { 'Hlt1ODINPhysics'    : 1.
                                                             , 'Hlt1ODINTechnical'  : 0. # @OnlineEnv.AcceptRate
                                                             , 'Hlt1Tell1Error'     : 0.
                                                             }              
                                              , 'ODINPhysics'   : '( ODIN_TRGTYP == LHCb.ODIN.PhysicsTrigger ) | ( ODIN_TRGTYP == LHCb.ODIN.TimingTrigger )'
                                              , 'ODINTechnical' : '( ODIN_TRGTYP == LHCb.ODIN.TechnicalTrigger ) | ( ODIN_TRGTYP == LHCb.ODIN.AuxiliaryTrigger ) | ( ODIN_TRGTYP == LHCb.ODIN.NonZSupTrigger ) | ( ODIN_TRGTYP == LHCb.ODIN.CalibrationTrigger )'
                                              } 
               }
