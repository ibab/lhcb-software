"""
High level configuration tools for PhysConf
"""
__version__ = "$Id: Configuration.py,v 1.1.1.1 2008-12-17 19:54:07 pkoppenb Exp $"
__author__ = "Patrick Koppenburg <Patrick.Koppenburg@cern.ch>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
from Configurables import GaudiSequencer
import GaudiKernel.ProcessJobOptions

class PhysConf(LHCbConfigurableUser) :
    __slots__ = {
        "DataType"        : 'DC06', # Data type, can be ['DC06','2008']
        "Simulation"      : True,  # set to True to use SimCond
        }

    __used_configurables__ = [ LHCbApp ]
    
#
# Initialization
#
    def configureInput(self):
        """
        Tune initialisation 
        """
        # POOL Persistency
        importOptions("$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts")
        # Get the event time (for CondDb) from ODIN
        from Configurables import EventClockSvc
        EventClockSvc().EventTimeDecoder = "OdinTimeDecoder";
        importOptions("$PHYSSYSROOT/options/DaVinciInit.opts") # @todo replace with contents        
        importOptions("$CALORECOROOT/options/CaloRecoOnDemand.opts")
        importOptions("$PHYSCONFROOT/options/PIDFromProtos.py")
        if ( self.getProp( "DataType" ) == 'DC06' ):
            from Configurables import (GaudiSequencer, ChargedProtoCombineDLLsAlg)
            GaudiSequencer("DaVinciInitSeq").Members += [ ChargedProtoCombineDLLsAlg() ]


#
# Main configuration
#
    def __apply_configuration__(self):
        print "# applying Phys configuration"
        GaudiKernel.ProcessJobOptions.PrintOff()
        importOptions("$PHYSSYSROOT/options/PhysCoreSys.opts")  # @todo replace with contents 
        self.configureInput()
        
#       @todo Remove this from Common and put it here
#        if ( self.getProp( "DataType" ) == 'DC06' ):
#            importOptions ("$DAVINCIROOT/options/DaVinciProtoPCalibrate.opts")
#        if ( self.getProp( "DataType" ) != 'DC06' ):
#            GaudiSequencer("ProtoPRecalibration").Members = []
