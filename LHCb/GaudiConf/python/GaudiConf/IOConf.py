"""
Configure IO persistency and TES
"""
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

__all__ = [
    'IOConf'  ## the configurable, configures IO persistency and TES   
    ]

from Gaudi.Configuration import *
import GaudiKernel.ProcessJobOptions
from Configurables   import LHCbConfigurableUser

class IOConf(LHCbConfigurableUser):
    
    __slots__ = {
         'InputPersistency'  : "POOL"
       , 'OutputPersistency' : "ROOT"
       , 'InputFiles'        : []
       , 'OutputFile'        : ""
       , 'Writer'            : "" 
       , 'WriteFSR'          : True 
         }

    _propertyDocDct = { 
        'InputPersistency'  : """ Technology of  input persistency, can be ['MDF','ROOT','POOL'] """
       ,'OutputPersistency' : """ Technology of output persistency, can be ['MDF','ROOT','POOL'] """
       ,'InputFiles'        : """ Connection strings of input files """
       ,'OutputFile'        : """ Connection string of output file. Must be given if output wanted """
       ,'Writer'            : """ Name of OutputStream writing the output. If not given, use InputCopyStream """
       ,'WriteFSR'          : """ Flags whether to write out an FSR """
       }

    InputSvcTypDict = { 'ROOT' : "SVC='Gaudi::RootEvtSelector'",
                        'POOL' : "TYP='POOL_ROOT'"
                        }

    OutputSvcTypDict = { 'ROOT' : "SVC='RootCnvSvc'",
                         'POOL' : "TYP='POOL_ROOTTREE'"
                       }

    def _checkSlots(self):
        KnownPersistencyTypes  = ['MDF','ROOT','POOL']
        inType  = self.getProp( "InputPersistency" ).upper()
        outType = self.getProp( "OutputPersistency" ).upper()

        if inType not in KnownPersistencyTypes:
            raise TypeError( "Unknown InputType '%s'"%inType )

        if outType not in KnownPersistencyTypes:
            raise TypeError( "Unknown OutputType '%s'"%outType )

        if inType == 'MDF' or outType == 'MDF':
            raise TypeError( "MDF type not yet implemented" )

        if self.getProp( "OutputFile" ) != "":
            if inType == 'ROOT' and outType == 'POOL':
                raise TypeError( "Cannot write POOL file when reading ROOT files" )

            if inType == 'POOL' and outType == 'ROOT':
                log.info( "ROOT output file requested. Forcing reading of POOL input files with RootCnv" )
                self.setProp( "InputPersistency", "ROOT" )

    def _doConfIO(self):
        inType  = self.getProp( "InputPersistency" ).upper()
        outType = self.getProp( "OutputPersistency" ).upper()
        if self.getProp( "OutputFile" ) == "":
            outType = ""

        # Set up the TES
        EventDataSvc( ForceLeaves        = True,
                      RootCLID           =    1,
                      EnableFaultHandler = True )

        # Set up the IO
        ApplicationMgr().ExtSvc += [ "Gaudi::MultiFileCatalog/FileCatalog",
                                     "Gaudi::IODataManager/IODataManager"  ]

        # Set up the persistency
        if inType == 'ROOT' or outType == 'ROOT':
            from Configurables import Gaudi__RootCnvSvc
            rootSvc = Gaudi__RootCnvSvc( "RootCnvSvc" )
            EventPersistencySvc().CnvServices += [ rootSvc ]
            ApplicationMgr().ExtSvc           += [ rootSvc ]

            fileSvc = Gaudi__RootCnvSvc( "FileRecordCnvSvc" )
            self._doConfFileRecords(fileSvc)

        if inType == 'POOL' or outType == 'POOL':
            from Configurables import (PoolDbCnvSvc, PoolDbCacheSvc)
            evtSvc  = PoolDbCnvSvc( "PoolRootEvtCnvSvc",     DbType = "POOL_ROOT",     EnableIncident = 1 )
            keySvc  = PoolDbCnvSvc( "PoolRootKeyEvtCnvSvc",  DbType = "POOL_ROOTKEY",  EnableIncident = 1 )
            treeSvc = PoolDbCnvSvc( "PoolRootTreeEvtCnvSvc", DbType = "POOL_ROOTTREE", EnableIncident = 1 )
            EventPersistencySvc().CnvServices += [ evtSvc, treeSvc, keySvc ]

            cacheSvc = PoolDbCacheSvc( Dlls        = ["lcg_RootStorageSvc", "GaudiKernelDict"],
                                       OutputLevel = 4,
                                       DomainOpts  = ["Domain[ROOT_All].TREE_MAX_SIZE=500000000000 TYP=longlong"]
                                       )
            ApplicationMgr().ExtSvc += [ cacheSvc, evtSvc, keySvc, treeSvc ]
            fileSvc = PoolDbCnvSvc( "FileRecordCnvSvc", DbType = "POOL_ROOTTREE" )
            self._doConfFileRecords(fileSvc)

    def _doConfFileRecords(self,fileSvc):

        # Set up the FileRecordDataSvc
        FileRecordDataSvc( ForceLeaves        = "YES",
                           EnableFaultHandler = True,
                           RootCLID           = 1    )

        fileSvc.ShareFiles = "YES"
        ApplicationMgr().ExtSvc                                += [ fileSvc ]
        PersistencySvc("FileRecordPersistencySvc").CnvServices += [ fileSvc ]


    def _doSetupFiles(self):
        # Set up input file
        files = self.getProp( "InputFiles" )
        for file in files:
            datafile = "DATAFILE='" + file + "' " + self.svcTypString( 'I' ) + " OPT='READ'"
            log.info( "Input file: %s"%datafile )
            EventSelector().Input += [ datafile ]

        # Set up output file
        if self.getProp( "OutputFile" ) != "":
            writer = self.getProp("Writer")
            if writer == "":
                outStream = InputCopyStream()
            else:
                outStream = OutputStream( writer )
                
            if not outStream.isPropertySet("Output"):
                datafile = "DATAFILE='" + self.getProp( "OutputFile" ) + "' " + self.svcTypString( 'O' ) + " OPT='REC'"
                log.info( "Output file: %s"%datafile )
                outStream.Output = datafile
            else:
                datafile = outStream.getProp("Output")
            ApplicationMgr().OutStream += [ outStream ]

            # Add the FSRWriter to the same output file
            if self.getProp("WriteFSR"):
                FSRWriter = RecordStream( "FSRWriter",
                                          ItemList = [ "/FileRecords#999" ],
                                          EvtDataSvc = "FileRecordDataSvc",
                                          EvtConversionSvc = "FileRecordPersistencySvc" )
                # Write the FSRs to the same file as the events
                if FSRWriter.isPropertySet( "Output" ):
                    log.warning("Ignoring FSRWriter.Output option, FSRs will be written to same file as the events")
                FSRWriter.Output = datafile
                ApplicationMgr().OutStream.append(FSRWriter)

    def svcTypString(self,IO):
        # Return SVC/TYP for file IO definition
        if IO == 'I': return self.InputSvcTypDict[self.getProp( "InputPersistency" ).upper()]
        if IO == 'O': return self.OutputSvcTypDict[self.getProp( "OutputPersistency" ).upper()]
        raise TypeError( "Unknown IO Type '%s'"%IO )

    def __apply_configuration__(self):
        self._checkSlots()
        self._doConfIO()
        self._doSetupFiles()
