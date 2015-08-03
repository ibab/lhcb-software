__author__ = 'Patrick Koppenburg'
__date__ = '25/02/2011'
__version__ = '$Revision: 2 $'

"""
Stripping selections or Minimum Bias physics.
"""

# Begin StrippingMinBias.py

default_config = { 'NAME': 'MiniBias',
                   'WGs': ['ALL'],
                   'BUILDERTYPE': 'MiniBiasConf',
                   'STREAMS': ['MiniBias'],
                   'CONFIG' : { "NoBiasLine_RE"       : "(HLT_PASS('Hlt1MBNoBiasDecision'))",
                                "NoBiasLine_Rate" : 1,
                                "NoBiasLine_Limiter" : "Hlt1MBNoBiasODINFilter",
                                "NoBiasLine_CondDB" : True , # False before 2012
                                "L0AnyRLLine_RE"        : "(HLT_PASS('Hlt1L0AnyRateLimitedDecision'))",
                                "L0AnyRLLine_Rate"  : 1,
                                "L0AnyRLLine_Limiter" : "Hlt1L0AnyRateLimitedPostScaler",
                                "L0AnyPSLine_RE"      : "(HLT_PASS_RE('Hlt1L0Any.*Decision'))",
                                "L0AnyPSLine_PS"  : 0.1,
                              }
                 }

__all__ = ('MiniBiasConf' )

from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from PhysSelPython.Wrappers import Selection, DataOnDemand

class MiniBiasConf(LineBuilder) :
    """
    Builder for Minimum bias lines
    """
    NoBiasLine = None
    L0AnyLineRateLimited = None
    L0AnyLinePrescaled = None

    __configuration_keys__ = ( 
        "NoBiasLine_RE",
        "NoBiasLine_Rate" ,
        "NoBiasLine_Limiter",
        "NoBiasLine_CondDB",
        "L0AnyRLLine_RE",
        "L0AnyRLLine_Rate",
        "L0AnyRLLine_Limiter",
        "L0AnyPSLine_RE"  ,
        "L0AnyPSLine_PS"  
       )

    def __init__(self, name, config):
      LineBuilder.__init__(self, name, config)
      self.NoBiasLine = self._makeLine("MBNoBias",
                                       config["NoBiasLine_RE"],
                                       config["NoBiasLine_Limiter"],
                                       config["NoBiasLine_Rate"],
                                       UseConditionDB=config["NoBiasLine_CondDB"] )
      
      self.L0AnyLineRateLimited = self._makeLine("Hlt1L0AnyRateLimited",
                                                 config["L0AnyRLLine_RE"],
                                                 config["L0AnyRLLine_Limiter"],
                                                 config["L0AnyRLLine_Rate"])

      self.L0AnyLinePrescaled = StrippingLine("Hlt1L0AnyPrescaled"
                                              , HLT = config["L0AnyPSLine_RE"]
                                              , checkPV = False
                                              , prescale = config["L0AnyPSLine_PS"]
                                              , RequiredRawEvents = ["Trigger", "Muon", "Calo", "Rich", "Velo", "Tracker"])
      self.registerLine( self.L0AnyLinePrescaled )

    def _makeLine(self,name,RE,limiter,rate,UseConditionDB=False):
      from Configurables import OfflineRateLimiter  

      orl = OfflineRateLimiter(name+"ORL",
                               HltLimiter = limiter,  
                               Rate = rate,
                               UseCondition = UseConditionDB,
                               FallBack = UseConditionDB)

      line = StrippingLine(name
                           , HLT =  RE
                           , checkPV = False
                           , algos = [ orl ] 
                           , RequiredRawEvents = ["Trigger", "Muon", "Calo", "Rich", "Velo", "Tracker"])

      self.registerLine( line )
      return line

