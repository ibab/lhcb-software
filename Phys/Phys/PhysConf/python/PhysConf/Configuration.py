"""
High level configuration tools for PhysConf
"""
__version__ = "$Id: Configuration.py,v 1.4 2008-12-23 09:07:35 pkoppenb Exp $"
__author__ = "Patrick Koppenburg <Patrick.Koppenburg@cern.ch>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
from Configurables import GaudiSequencer
import GaudiKernel.ProcessJobOptions

class PhysConf(LHCbConfigurableUser) :
    from Configurables import (GaudiSequencer)
    __slots__ = {
        "DataType"        : 'DC06'                             # Data type, can be ['DC06','2008']
     ,  "Simulation"      : True                               # set to True to use SimCond
     ,  "InitSeq"         : GaudiSequencer("DaVinciInitSeq")   # Initialisation sequence in the application. Is set from e.g. DaVinci()
        }

    __used_configurables__ = [ LHCbApp ]

#
# configure reconstruction to be redone
#
    def configureReco(self):
        """
        Configure Reconstruction to be redone
        """
        from Configurables import (GaudiSequencer)
        recalib = GaudiSequencer("ProtoPRecalibration")
        recalib.IgnoreFilterPassed = True 
        init = self.getProp("InitSeq")
        init.Members += [ recalib ]
        importOptions("$CALORECOROOT/options/CaloRecoOnDemand.opts")
        if ( self.getProp( "DataType" ) == 'DC06' ):
            from Configurables import (ChargedProtoCombineDLLsAlg)
            recalib.Members += [ ChargedProtoCombineDLLsAlg() ]
        # @todo Should use DoD Svc, but there are some problems
        from Configurables import (MuonPIDsFromProtoParticlesAlg, RichPIDsFromProtoParticlesAlg)
        recalib.Members += [ MuonPIDsFromProtoParticlesAlg("MuonPIDsFromProtos") ]
        recalib.Members += [ RichPIDsFromProtoParticlesAlg("RichPIDsFromProtos") ]
        
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
        self.dataOnDemand()
        self.configureInput()
        self.configureReco()
        
#       @todo Remove this from Common and put it here
#        if ( self.getProp( "DataType" ) == 'DC06' ):
#            importOptions ("$DAVINCIROOT/options/DaVinciProtoPCalibrate.opts")
#        if ( self.getProp( "DataType" ) != 'DC06' ):
#            GaudiSequencer("ProtoPRecalibration").Members = []
