__author__ = 'Patrick Koppenburg'
__date__ = '25/02/2011'
__version__ = '$Revision: 2 $'

"""
Stripping selections or Minimum Bias physics.
"""

# Begin StrippingMinBias.py

config_params = { "NoBiasLine_RE"       : "(HLT_PASS_RE('Hlt1.*NoBias.*Decision'))",
                  "NoBiasLine_Prescale" : 0.01,
                  "L0AnyLine_RE"        : "(HLT_PASS_RE('Hlt1L0Any.*Decision'))",
                  "L0AnyLine_Prescale"  : 0.01 }

__all__ = ('MiniBiasConf' )

from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

class MiniBiasConf(LineBuilder) :
    """
    Builder for Minimum bias lines
    """
    NoBiasLine = None
    L0AnyLine = None

    __configuration_keys__ = ( 
      "NoBiasLine_RE",
      "NoBiasLine_Prescale" ,
      "L0AnyLine_RE",
      "L0AnyLine_Prescale" 
      )

    def __init__(self, name, config):
      LineBuilder.__init__(self, name, config)
      NoBiasLine_name = "MBNoBias"
      L0AnyLine_name = "Hlt1L0Any"
      
      self.NoBiasLine = StrippingLine(NoBiasLine_name
                                      , HLT =  config["NoBiasLine_RE"]
                                      , checkPV = False
                                      , prescale = config['NoBiasLine_Prescale']
                                      , postscale = 1
                                    )
      
      self.L0AnyLine = StrippingLine(L0AnyLine_name
                                      , HLT =  config["L0AnyLine_RE"]
                                      , checkPV = False
                                      , prescale = config['L0AnyLine_Prescale']
                                      , postscale = 1
                                    )
      
      self.registerLine( self.NoBiasLine )
      self.registerLine( self.L0AnyLine )

