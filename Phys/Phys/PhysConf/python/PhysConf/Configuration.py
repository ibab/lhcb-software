"""
High level configuration tools for PhysConf
"""
__version__ = "$Id: Configuration.py,v 1.6 2009-01-06 17:19:41 pkoppenb Exp $"
__author__ = "Patrick Koppenburg <Patrick.Koppenburg@cern.ch>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
from Configurables import GaudiSequencer
from Configurables import LoKiSvc
import GaudiKernel.ProcessJobOptions
from Configurables import ( DstConf )

class PhysConf(LHCbConfigurableUser) :
    from Configurables import (GaudiSequencer)
    __slots__ = {
        "DataType"        : 'DC06'                             # Data type, can be ['DC06','2008']
     ,  "Simulation"      : True                               # set to True to use SimCond
     ,  "InitSeq"         : GaudiSequencer("DaVinciInitSeq")   # Initialisation sequence in the application. Is set from e.g. DaVinci()
        }

    __used_configurables__ = [ DstConf, LHCbApp ]

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
        # DST unpacking
        DstConf().EnableUnpack = True
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
# LoKi
#
    def loki(self) :
        """
        Define loki service
        """
        lokiService = LoKiSvc()
        ApplicationMgr().ExtSvc +=  [ lokiService ]
        
#
# Main configuration
#
    def __apply_configuration__(self):
        print "# applying Phys configuration"
        GaudiKernel.ProcessJobOptions.PrintOff()
        self.dataOnDemand()
        self.configureInput()
        self.configureReco()
        self.loki()
        
#       @todo Remove this from Common and put it here
#        if ( self.getProp( "DataType" ) == 'DC06' ):
#            importOptions ("$DAVINCIROOT/options/DaVinciProtoPCalibrate.opts")
#        if ( self.getProp( "DataType" ) != 'DC06' ):
#            GaudiSequencer("ProtoPRecalibration").Members = []
