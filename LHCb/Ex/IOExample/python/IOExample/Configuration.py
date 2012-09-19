"""
Configurable for IO test applications
"""
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from os import environ
from Gaudi.Configuration import *
from LHCbKernel.Configuration import LHCbConfigurableUser
from Configurables import SimConf, DigiConf, DstConf

class IOTest(LHCbConfigurableUser):
    __slots__ = {
        "EvtMax"            : -1
       ,"DataContent"       : "DST"
       ,"WithMC"            : False
       ,"LoadAll"           : False
       ,"KillDAQ"           : False
       ,"DataType"          : "2011"
        }

    _propertyDocDct = {
        'EvtMax'            : """ Maximum number of events to process """
       ,'DataContent'       : """ Content of dataset (SIM, DIGI, RAW, DST, ...) """
       ,'WithMC'            : """ Flag to enable processing with MC Truth """
       ,'LoadAll'           : """ Load all leaves of input file with StoreExplorerAlg """
       ,'KillDAQ'           : """ Do not attempt to load Event/DAQ """
       ,'DataType'          : """ Data type, steers year dependent test configuraton """
       }

    __used_configurables__ = [ SimConf, DigiConf, DstConf ]

    def _defineEvents(self):
        # Delegate handling to ApplicationMgr configurable
        self.setOtherProps(ApplicationMgr(),["EvtMax"])
        EventSelector().PrintFreq = 1

    def _defineMonitors(self):
        if self.getProp( "KillDAQ" ):
            from Configurables import EventNodeKiller
            daqKiller = EventNodeKiller("DAQKiller")
            daqKiller.Nodes += [ "DAQ" ]
            ApplicationMgr().TopAlg += [ daqKiller ]
        ApplicationMgr().TopAlg += [ "PrintHeader" ]
        ApplicationMgr().ExtSvc += [ "ToolSvc", "DataOnDemandSvc" ]

        if self.getProp( "LoadAll" ):
            from Configurables import StoreExplorerAlg, TESFingerPrint
            storeExp = StoreExplorerAlg( Load = True, PrintFreq = 1, ExploreRelations = 1 )
            fingerPrint = TESFingerPrint( HeuristicsLevel = "Medium", OutputLevel = 1 )
            ApplicationMgr().TopAlg += [ storeExp, fingerPrint ]
        
        if self.getProp( "DataContent" ).upper() == "DST":
            from Configurables import DumpTracks
            dumpTracks = DumpTracks( OutputLevel = 2 )
            ApplicationMgr().TopAlg += [ "DumpTracks" ]
            DstConf().EnableUnpack = [ "Reconstruction", "Stripping" ]

        if self.getProp( "WithMC" ):
            DigiConf().EnableUnpack = True
            SimConf().EnableUnpack = True
            ApplicationMgr().TopAlg += [ "DumpMCEventAlg" ]

        if self.getProp( "DataContent" ).upper() == "SIM":
            ApplicationMgr().TopAlg += [ "DumpHepMC" ]

#        if self.getProp( "DataType" ).upper() == "MC09":
#            ApplicationMgr().Dlls += [ "HepMCBack" ]


    def __apply_configuration__(self):
        self._defineEvents()
        self._defineMonitors()
