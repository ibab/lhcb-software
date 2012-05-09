__author__ = 'Patrick Koppenburg'
__date__ = '25/02/2011'
__version__ = '$Revision: 2 $'

"""
Stripping selections or Minimum Bias physics.q
"""

# Begin StrippingMinBias.py

config_params = { "NoBiasLine_RE"       : "(HLT_PASS_RE('Hlt1MB.*NoBias.*Decision'))",
                  "NoBiasLine_Rate" : 1,
                  "NoBiasLine_Limiter" : "Hlt1MBNoBiasODINFilter",
                  "L0AnyLine_RE"        : "(HLT_PASS_RE('Hlt1L0Any.*Decision'))",
                  "L0AnyLine_Rate"  : 1,
                  "L0AnyLine_Limiter" : "Hlt1L0AnyRateLimitedPostScaler",
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
    L0AnyLine = None

    __configuration_keys__ = ( 
        "NoBiasLine_RE",
        "NoBiasLine_Rate" ,
        "NoBiasLine_Limiter",
        "L0AnyLine_RE",
        "L0AnyLine_Rate",
        "L0AnyLine_Limiter"
        )

    def __init__(self, name, config):
      LineBuilder.__init__(self, name, config)
      self.NoBiasLine = self._makeLine("MBNoBias",
                                       config["NoBiasLine_RE"],
                                       config["NoBiasLine_Limiter"],
                                       config["NoBiasLine_Rate"],
                                       UseConditionDB=True )
      self.NoBiasLine = self._makeLine("Hlt1L0Any",
                                       config["L0AnyLine_RE"],
                                       config["L0AnyLine_Limiter"],
                                       config["L0AnyLine_Rate"])

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
                           , algos = [ orl ] )

      self.registerLine( line )
      return line

