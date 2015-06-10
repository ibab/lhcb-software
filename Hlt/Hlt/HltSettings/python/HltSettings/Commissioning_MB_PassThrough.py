class Commissioning_MB_PassThrough(object) :
    """
    Threshold settings for PassThrough

    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS

    @author R. Aaij
    @date 2015-06-07
    """


    def __init__(self) :
        self.StripEndSequence = ['']

    def HltType(self) :
        return 'Hlt_PassThrough'

    def L0TCK(self) :
        return None

    def ActiveHlt2Lines(self) :
        """
        Returns a list of active Hlt2 lines
        """
        # Disable afterburner because it runs the reco
        from Configurables import HltConf
        HltConf().EnableHltAfterburner = False

        return  ['Hlt2PassThrough']

    def ActiveHlt1Lines(self) :
        """
        Returns a list of active Hlt1 lines
        """
        return [ 'Hlt1ODINPhysics','Hlt1ODINTechnical','Hlt1Tell1Error',
                 'Hlt1MBMicroBiasVelo' ]

    def Thresholds(self) :
        """
        Returns a dictionary of cuts
        """
        from Hlt1Lines.Hlt1CommissioningLines  import Hlt1CommissioningLinesConf
        d = { Hlt1CommissioningLinesConf : { 'Prescale' : { 'Hlt1ODINPhysics'    : 1.
                                                          , 'Hlt1ODINTechnical'  : 0. # @OnlineEnv.AcceptRate
                                                          , 'Hlt1Tell1Error'     : 0.}
                                            , 'ODINPhysics'   : '( ODIN_TRGTYP == LHCb.ODIN.PhysicsTrigger ) | ( ODIN_TRGTYP == LHCb.ODIN.TimingTrigger )'
                                            , 'ODINTechnical' : '( ODIN_TRGTYP == LHCb.ODIN.TechnicalTrigger ) | ( ODIN_TRGTYP == LHCb.ODIN.AuxiliaryTrigger ) | ( ODIN_TRGTYP == LHCb.ODIN.NonZSupTrigger ) | ( ODIN_TRGTYP == LHCb.ODIN.CalibrationTrigger )'
                                            }
            }

        from Hlt2Lines.Commissioning.Lines      import CommissioningLines
        d.update({CommissioningLines : {'Prescale' : {'Hlt2PassThrough' : 1.0}}})
        return d
