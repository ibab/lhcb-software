__author__ = 'Marco Meissner'
__date__ = '08/05/2013'
__version__ = '$Revision: 1 $'

"""
Minimum Bias Stripping selections for ProtonIon and IonProton physics
"""

# Begin StrippingProtonIonMinBias.py

config_params = { "ProtonIonMinBiasLine_RE"       : "(HLT_PASS('Hlt2PassThroughDecision'))",
                  "ProtonIonMinBiasLine_Prescale" : 0.10,
                  "ProtonIonMinBiasLine_CondDB" : True ,
                  }

__all__ = ('ProtonIonMinBiasConf' )

from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from PhysSelPython.Wrappers import Selection, DataOnDemand

class ProtonIonMinBiasConf(LineBuilder) :
    """
    Builder for pA - Ap minimum bias line
    """
    ProtonIonMinBiasLine = None

    __configuration_keys__ = ( 
        "ProtonIonMinBiasLine_RE",
        "ProtonIonMinBiasLine_Prescale",
        "ProtonIonMinBiasLine_CondDB",
       )

    def __init__(self, name, config):
      LineBuilder.__init__(self, name, config)
      self.ProtonIonMinBiasLine = self._makeLine("ProtonIonMinBias",
                                       config["ProtonIonMinBiasLine_RE"],
                                       config["ProtonIonMinBiasLine_Prescale"],
                                       UseConditionDB=config["ProtonIonMinBiasLine_CondDB"] )
      
    def _makeLine(self,name,RE,Prescale,UseConditionDB=False):

      line = StrippingLine(name
                           , HLT =  RE
                           , checkPV = False 
                           , prescale = Prescale )

      self.registerLine( line )
      return line

