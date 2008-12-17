"""
High level configuration tools for AnalysisConf
"""
__version__ = "$Id: Configuration.py,v 1.1.1.1 2008-12-17 19:55:25 pkoppenb Exp $"
__author__ = "Patrick Koppenburg <Patrick.Koppenburg@cern.ch>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
from Configurables import GaudiSequencer
import GaudiKernel.ProcessJobOptions

class AnalysisConf(LHCbConfigurableUser) :
    __slots__ = {
         "DataType"        : 'DC06' # Data type, can be ['DC06','2008']
        ,"Simulation"      : True   # set to True to use SimCond
        }

    __used_configurables__ = [ LHCbApp ]
            
    def __apply_configuration__(self):
        GaudiKernel.ProcessJobOptions.PrintOff()
        if ( self.getProp("Simulation" )):
            importOptions ("$DAVINCIASSOCIATORSROOT/options/DaVinciAssociators.opts")
            importOptions ("$ANALYSISCONFROOT/options/UnpackMC.py")
