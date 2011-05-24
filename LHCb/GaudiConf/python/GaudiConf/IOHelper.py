"""
Configure IO persistency and TES
"""
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from Gaudi.Configuration import *

class IOHelper(object):
    '''
    IOHelper class, for simpler configuration of input/output

    from GaudiConf.IOHelper import IOHelper
    ioh = IOHelper(Input=None,Output=None)

    "ioh" is now an instance of the helper which can be used in many places
    Defaults to POOL and POOL at the moment.
    At some point we will change the default to ROOT and ROOT.
    
    1) configuring services:
        IOHelper().setupServices()
        IOHelper("POOL","POOL").setupServices()
        ioh.setupServices()
      
    2) building an EventSelector from a list of files:
        evtsel=IOHelper().inputFiles(filelist)
        evtsel=IOHelper("POOL","POOL").inputFiles(filelist)
        evtsel=ioh.inputFiles(filelist)
      
    3) adding a simple OutputStream to the OutStream of Application Manager
        ioh.outStream(filename)
      
    4) adding a named special output stream to the OutputStream of Application Manager
        ioh.outStream(filename,"LHCbInputCopyStream/mystream")
      
    5) creating output algorithms to add to your sequence
        MySequence+=ioh.outputAlgs(filename,"InputCopyStream/mySelection")

    6) converting event selectors between formats
        IOHelper().convertSelector()
        evtsel=IOHelper("ROOT").convertSelector(evtsel)
        ioh.convertSelector()
    
    Specific uses:

    a) Ganga.           Needs to create the file name list to be passed to IOHelper.
                        Needs to pickle or otherwise flatten the result.

    b) LHCbApp          Setup the services, may need slots to do the steering
    
    b) Brunel.          Pass type to LHCbApp if it is an MDF.
                        Use ioh.outStream to define the single output file

    c) DaVinci.         BaseDSTWriter will use ioh.outputAlgs for each output file
    
    d) Stand-alone user. Needs to create the file list to be passed to IOHelper.
                         Adds ioh.outputAlgs to any of their personal sequencers
                         Adds ioh.convertSelector if using an old options file
    
    
    '''
    #the default persistencies
    _inputPersistency="POOL"
    _outputPersistency="POOL"
    
    _inputSvcTypDict = { 'ROOT' : "SVC='Gaudi::RootEvtSelector'",
                         'POOL' : "TYP='POOL_ROOT'"
                         }
    
    _outputSvcTypDict = { 'ROOT' : "SVC='RootCnvSvc'",
                          'POOL' : "TYP='POOL_ROOTTREE'"
                          }
    
    def __init__(self,Input=None,Output=None):
        '''
        IOHelper class, for simpler configuration of input/output
        
        ioh = IOHelper(Input=None,Output=None)
        
        "ioh" is now an instance of the helper which can be used in many places
        '''
        if Input is not None:
            if Input.upper() not in self._inputSvcTypDict:
                raise ValueError, Input+' input persistency not known'
            self._inputPersistency=Input.upper()
        
        if Output is not None:
            if Output.upper() not in self._outputSvcTypDict:
                raise ValueError, Output+' output persistency not known'
            self._outputPersistency=Output.upper()

        if self._outputPersistency == 'POOL':
            if self._inputPersistency == 'ROOT':
                if Output is not None:
                    raise TypeError( "Cannot write POOL file when reading with RootCnv" )
                else:
                    print "# ROOT input requested. Forcing ROOT output"
                    self._outputPersistency = 'ROOT'
                    
        if self._outputPersistency == 'ROOT':
            if self._inputPersistency == 'POOL':
                print "# ROOT output file requested. Forcing reading of POOL input files with RootCnv" 
                self._inputPersistency = 'ROOT'
    
    def __chooseIO(self,IO):
        '''IO = starts with one of r, w, i or o
        '''
        if len(IO)>1:
            IO=IO[0]
        IO=IO.upper()
        if IO == 'R' or IO=='I':
            return 'I'
        if IO == 'W' or IO=='O':
            return 'O'
        raise ValueError, 'IO type '+IO+' not known'
    
    def _doConfFileRecords(self,fileSvc):
        ''' create the file records service, should always be done.
        '''
        # Set up the FileRecordDataSvc
        FileRecordDataSvc( ForceLeaves        = "YES",
                           EnableFaultHandler = True,
                           RootCLID           = 1    )

        fileSvc.ShareFiles = "YES"
        ApplicationMgr().ExtSvc                                += [ fileSvc ]
        PersistencySvc("FileRecordPersistencySvc").CnvServices += [ fileSvc ]
        
        
    def svcTypString(self,IO):
        ''' given the IO type, return the selection string for the active services'''
        IO=self.__chooseIO(IO)
        # Return SVC/TYP for file IO definition
        if IO == 'I': return self._inputSvcTypDict[self._inputPersistency]
        
        return self._outputSvcTypDict[self._outputPersistency]
    
    def setupServices(self):
        '''Setup the pool/Root services, to be done by LHCbApp'''
    
        # Set up the TES
        EventDataSvc( ForceLeaves        = True,
                      RootCLID           =    1,
                      EnableFaultHandler = True )

        # Set up the IO
        ApplicationMgr().ExtSvc += [ "Gaudi::MultiFileCatalog/FileCatalog",
                                     "Gaudi::IODataManager/IODataManager"  ]
        
        # Set up the persistency
        if self._inputPersistency == 'ROOT' or self._outputPersistency == 'ROOT':
            from Configurables import Gaudi__RootCnvSvc
            rootSvc = Gaudi__RootCnvSvc( "RootCnvSvc" )
            EventPersistencySvc().CnvServices += [ rootSvc ]
            ApplicationMgr().ExtSvc           += [ rootSvc ]
            
            fileSvc = Gaudi__RootCnvSvc( "FileRecordCnvSvc" )
            self._doConfFileRecords(fileSvc)
        
        if self._inputPersistency == 'POOL' or self._outputPersistency == 'POOL':
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
        
    def dressFile(self,filename,IO):
        '''Go from file name to connection string'''
        IO=self.__chooseIO(IO)
        
        if IO=='O':
            return "DATAFILE='" + filename + "' " + self.svcTypString( 'O' ) + " OPT='REC'"
        
        return "DATAFILE='" + filename + "' " + self.svcTypString( 'I' ) + " OPT='READ'"
    
    def undressFile(self,filestring):
        '''Go from connection string to file name'''
        match="DATAFILE='"
        pos=filestring.find(match)
        if pos>=0:
            pos=pos+len(match)
            if pos <len(filestring):
                filestring=filestring[pos:]
        filestring=filestring.strip("'").split("'")[0]
        return filestring

    def convertConnectionStrings(self, filelist, IO):
        '''Go from a list of connection strings or file names to a new list of connection strings
        needs to know the IO type to know what to convert to'''
        IO=self.__chooseIO(IO)
        retlist=[]
        for file in filelist:
            retlist.append( self.dressFile( self.undressFile(file),IO) )
        return retlist
    
    def convertSelector(self, eventSelector=None):
        '''Modify an EventSelector to switch to the new input connection strings
        Go from a list of connection strings or file names to a new list of connection strings

        convertSelector(self, eventSelector=None)

        If eventSelector is None, the default EventSelector will be found.
        If an event selector is passed, that one will be modified
        
        '''
        
        if eventSelector is None:
            eventSelector=EventSelector()
        
        eventSelector.Input=self.convertConnectionStrings(eventSelector.Input, "I")
        
        return eventSelector
    
    def inputFiles(self,files,clear=True, eventSelector=None):
        '''Edit the content of EventSelector and fill the Inputs with 
        Go from a list of connection strings or file names to a new list of connection strings

        inputFiles(self,files,clear=True, eventSelector=None)

        If eventSelector is None, the default EventSelector will be found.
        If an event selector is passed, that one will be modified

        If clear is True the original Inputs are overwritten.
        If clear is False the original Inputs are also kept.
        '''
        
        if eventSelector is None:
            eventSelector=EventSelector()

        if clear is True:
            eventSelector.Input=[]
        
        for file in files:
            eventSelector.Input += [ self.dressFile(file,'I') ]
        return eventSelector
    
    def outputAlgs(self,filename,writer="OutputStream",writeFSR=True):
        '''Create a output stream and FSR writing algorithm instance
        to write into the given file.

        Any class inheriting from OutputStream can be used, with an instance name.

        e.g. ioh.outputAlgs("test.dst","InputCopyStream/Spam")

        returns a list of algorithms to append to your sequencer.
        '''
        
        #build up the filename
        filename=self.dressFile(filename,'O')

        writer=writer.replace('::','__')
        
        #find the writer
        wclass=None
        winstance=None

        #if it's in Gaudi.Configuration
        if writer.split('/')[0] in locals():
            wclass = locals()[writer.split('/')[0]]
        else:
            import Configurables
            wclass = getattr(Configurables,writer.split('/')[0])
        #otherwise it must be a configurable
        
        #check if it has an instance name
        if '/' not in writer:
            winstance=wclass()
        else:
            winstance=wclass(name=writer.split('/')[-1])
        
        winstance.Output = filename

        if not writeFSR: return [winstance]
        
        FSRWriter = RecordStream( "FSRWriter"+writer.replace('/',''),
                                  ItemList = [ "/FileRecords#999" ],
                                  EvtDataSvc = "FileRecordDataSvc",
                                  EvtConversionSvc = "FileRecordPersistencySvc" )
        
        FSRWriter.Output = filename

        return [winstance, FSRWriter]

    def outStream(self,filename,writer="OutputStream",writeFSR=True):
        '''Create a output stream and FSR writing algorithm instance
        to write into the given file.

        Any class inheriting from OutputStream can be used, with an instance name.

        e.g. ioh.outputAlgs("test.dst","InputCopyStream/Spam")

        Adds these streams directly to the Application manager OutStream
        '''
        
        algs=self.outputAlgs(filename,writer,writeFSR)
        
        if ApplicationMgr().OutStream is None or len(ApplicationMgr().OutStream)==0:
            ApplicationMgr().OutStream =[]
        
        for alg in algs:
            ApplicationMgr().OutStream.append(alg)
