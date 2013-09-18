__author__ = 'Marco Meissner and Ronan Wallace'
__date__ = '24/06/2013'
__version__ = '$Revision: 2 $'


"""
Minimum Bias Stripping selections for ProtonIon and IonProton physics
"""

# Begin StrippingProtonIonMinBias.py
config_params = { "ProtonIonMinBiasLine_RE"       : "(HLT_PASS('Hlt2PassThroughDecision'))",
                  "ProtonIonMagDownMinBiasLine_RunNumbers"            :  "(ODIN_RUNNUMBER(135576,136212))",
                  "ProtonIonMagUpMinBiasLine_RunNumbers"            :  "(ODIN_RUNNUMBER(136237,136341))",
                  "IonProtonMagDownMinBiasLine_RunNumbers"            :  "(ODIN_RUNNUMBER(136799,137045))",
                  "IonProtonMagUpMinBiasLine_RunNumbers"            :  "(ODIN_RUNNUMBER(136417,136758))",
                  "ProtonIonMagDownMinBiasLine_Prescale" : 0.025,
                  "ProtonIonMagUpMinBiasLine_Prescale" : 0.11,
                  "IonProtonMagDownMinBiasLine_Prescale" : 0.08,
                  "IonProtonMagUpMinBiasLine_Prescale" : 0.09,
                  "ProtonIonMinBiasLine_CondDB" : True ,
                  }


__all__ = ('ProtonIonMinBiasConf' )

#from LoKiHlt import *
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from PhysSelPython.Wrappers import Selection, DataOnDemand
from Gaudi.Configuration import *

class ProtonIonMinBiasConf(LineBuilder) :
    """
    Builder for pA - Ap minimum bias lines
    """
    MinBiasLine = None

    __configuration_keys__ = ( 
        "ProtonIonMinBiasLine_RE",
        "ProtonIonMagDownMinBiasLine_RunNumbers",
        "ProtonIonMagUpMinBiasLine_RunNumbers",
        "IonProtonMagDownMinBiasLine_RunNumbers",
        "IonProtonMagUpMinBiasLine_RunNumbers",
        "ProtonIonMagDownMinBiasLine_Prescale",
        "ProtonIonMagUpMinBiasLine_Prescale",
        "IonProtonMagDownMinBiasLine_Prescale",
        "IonProtonMagUpMinBiasLine_Prescale",
        "ProtonIonMinBiasLine_CondDB",
       )

    def __init__(self, name, config):
      LineBuilder.__init__(self, name, config)
      
      self._myname = name
      
      self.ProtonIonMagDownMinBias_line = self._makeLine("ProtonIonMagDownMinBiasLine",
                                              config["ProtonIonMinBiasLine_RE"],
                                              config["ProtonIonMagDownMinBiasLine_RunNumbers"],
                                              config["ProtonIonMagDownMinBiasLine_Prescale"],
                                              UseConditionDB=config["ProtonIonMinBiasLine_CondDB"] )


      self.ProtonIonMagUpMinBias_line = self._makeLine("ProtonIonMagUpMinBiasLine",
                                             config["ProtonIonMinBiasLine_RE"],
                                             config["ProtonIonMagUpMinBiasLine_RunNumbers"],
                                             config["ProtonIonMagUpMinBiasLine_Prescale"],
                                             UseConditionDB=config["ProtonIonMinBiasLine_CondDB"] )


      self.IonProtonMagDownMinBias_line = self._makeLine("IonProtonMagDownMinBiasLine",
                                               config["ProtonIonMinBiasLine_RE"],
                                               config["IonProtonMagDownMinBiasLine_RunNumbers"],
                                               config["IonProtonMagDownMinBiasLine_Prescale"],
                                               UseConditionDB=config["ProtonIonMinBiasLine_CondDB"] )


      self.IonProtonMagUpMinBias_line = self._makeLine("IonProtonMagUpMinBiasLine",
                                             config["ProtonIonMinBiasLine_RE"],
                                             config["IonProtonMagUpMinBiasLine_RunNumbers"],
                                             config["IonProtonMagUpMinBiasLine_Prescale"],
                                             UseConditionDB=config["ProtonIonMinBiasLine_CondDB"] )





    def _makeLine(self,name,RE,RunNumbers,Prescale,UseConditionDB=False):
      line = StrippingLine(name
                           , HLT =  RE
                           , ODIN = RunNumbers
                           , checkPV = False 
                           , prescale = Prescale )

      self.registerLine( line )
      return line

