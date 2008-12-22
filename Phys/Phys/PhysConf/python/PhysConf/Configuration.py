"""
High level configuration tools for PhysConf
"""
__version__ = "$Id: Configuration.py,v 1.3 2008-12-22 18:06:58 pkoppenb Exp $"
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
        # PPSvc
        from Configurables import LHCb__ParticlePropertySvc
        pp = LHCb__ParticlePropertySvc ( ParticlePropertiesFile = 'conddb:///param/ParticleTable.txt' )
        ApplicationMgr().ExtSvc +=  [ pp ]            
        # Get the event time (for CondDb) from ODIN
        from Configurables import EventClockSvc
        EventClockSvc().EventTimeDecoder = "OdinTimeDecoder";
        importOptions("$CALORECOROOT/options/CaloRecoOnDemand.opts")
        importOptions("$PHYSCONFROOT/options/PIDFromProtos.py")
        if ( self.getProp( "DataType" ) == 'DC06' ):
            from Configurables import (GaudiSequencer, ChargedProtoCombineDLLsAlg)
            GaudiSequencer("DaVinciInitSeq").Members += [ ChargedProtoCombineDLLsAlg() ]
#
# Data on demand
#
    def dataOnDemand(self):
        """
        dataOnDemand service
        """
        DataOnDemandSvc().NodeMap['/Event/Rec']            = 'DataObject'
        DataOnDemandSvc().NodeMap['/Event/Rec/Muon']       = 'DataObject'
        DataOnDemandSvc().NodeMap['/Event/Rec/Rich']       = 'DataObject'
        DataOnDemandSvc().NodeMap['/Event/Phys']           = 'DataObject'
        DataOnDemandSvc().NodeMap['/Event/Relations/Phys'] = 'DataObject'

#
# Main configuration
#
    def __apply_configuration__(self):
        print "# applying Phys configuration"
        GaudiKernel.ProcessJobOptions.PrintOff()
        self.configureInput()
        self.dataOnDemand()
        
#       @todo Remove this from Common and put it here
#        if ( self.getProp( "DataType" ) == 'DC06' ):
#            importOptions ("$DAVINCIROOT/options/DaVinciProtoPCalibrate.opts")
#        if ( self.getProp( "DataType" ) != 'DC06' ):
#            GaudiSequencer("ProtoPRecalibration").Members = []
