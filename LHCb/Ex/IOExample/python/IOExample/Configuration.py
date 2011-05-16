"""
Configurable for IO test applications
"""
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from os import environ
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import SimConf, DigiConf, DstConf

class IOTest(LHCbConfigurableUser):
    __slots__ = {
        "EvtMax"            : -1
       ,"DataContent"       : "DST"
       ,"WithMC"            : False
       ,"LoadAll"           : False
        }

    _propertyDocDct = {
        'EvtMax'            : """ Maximum number of events to process """
       ,'DataContent'       : """ Content of dataset (SIM, DIGI, RAW, DST, ...) """
       ,'WithMC'            : """ Flag to enable processing with MC Truth """
       ,'LoadAll'           : """ Load all leaves of input file with StoreExplorerAlg """
       }

    __used_configurables__ = [ SimConf, DigiConf, DstConf ]

    def _defineEvents(self):
        # Delegate handling to ApplicationMgr configurable
        self.setOtherProps(ApplicationMgr(),["EvtMax"])
        EventSelector().PrintFreq = 1

    def _defineMonitors(self):
        ApplicationMgr().Dlls += [ "HepMCBack" ]
        ApplicationMgr().TopAlg += [ "PrintHeader" ]
        ApplicationMgr().ExtSvc += [ "DataOnDemandSvc" ]

        if self.getProp( "LoadAll" ):
            from Configurables import StoreExplorerAlg
            storeExp = StoreExplorerAlg( Load = True, PrintFreq = 1, ExploreRelations = 1 )
            ApplicationMgr().TopAlg += [ storeExp ]
        
        if self.getProp( "DataContent" ).upper() == "DST":
            from Configurables import DumpTracks
            dumpTracks = DumpTracks( OutputLevel = 2 )
            ApplicationMgr().TopAlg += [ "DumpTracks" ]
            DstConf().EnableUnpack = True

        if self.getProp( "WithMC" ):
            DigiConf().EnableUnpack = True
            SimConf().EnableUnpack = True
            ApplicationMgr().TopAlg += [ "DumpMCEventAlg" ]

        if self.getProp( "DataContent" ).upper() == "SIM":
            ApplicationMgr().TopAlg += [ "DumpHepMC" ]

    def __apply_configuration__(self):
        self._defineEvents()
        self._defineMonitors()
