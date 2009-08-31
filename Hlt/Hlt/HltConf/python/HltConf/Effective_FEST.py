## Inherit active lines
from HltConf.Effective_Nominal import Effective_Nominal
class Effective_FEST ( Effective_Nominal ) :
    """
    Threshold settings for Hlt1 and 2 in nominal conditions (Effective/FEST)
    
    WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
    
    @author P.Koppenburg, G. Raven
    @date 2009-08-06
    """
    
    def Thresholds(self) :
        """
        Returns a dictionary of thresholds
        """
        
        from Hlt1Lines.HltVeloLines import HltVeloLinesConf
        from Hlt1Lines.HltCommissioningLines import HltCommissioningLinesConf
        from Hlt1Lines.HltExpressLines import HltExpressLinesConf
        
        d = Effective_Nominal.Thresholds(self)
        
        d.update(  { HltVeloLinesConf          : { 'Prescale' : { '.*'                : 1 } }
                   , HltExpressLinesConf       : { 'Prescale' : { 'Hlt1XPress'        : 0.0025
                                                                , 'Hlt1AlignVelo'     : 1 } }
                   , HltCommissioningLinesConf : { 'Prescale' : { 'Hlt1NonRandomODIN' : 1 
                                                                , 'Hlt1RandomODIN'    : 1 # @OnlineEnv.AcceptRate
                                                                , 'Hlt1Tell1Error'    : 0 } }
                   }
                )
        
        return d
