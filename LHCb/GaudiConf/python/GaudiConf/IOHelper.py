"""
Configure IO persistency and TES.

IOHelper class for whan you know the persistency services you want

IOExtention if you want to be clever and get the 
"""
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

#from Gaudi.Configuration import *

class IOHelper(object):
    '''
    IOHelper class, for simpler configuration of input/output
    
    from GaudiConf import IOHelper
    ioh = IOHelper(Input=None,Output=None)
    
    "ioh" is now an instance of the helper which can be used in many places
    Defaults to POOL and POOL at the moment.
    At some point we will change the default to ROOT and ROOT.
    
    1) check if root framework is possible:
        IOHelper().isRootSupported()
    
    2) configuring services:
        IOHelper().setupServices()
        IOHelper("POOL","POOL").setupServices()
        ioh.setupServices()
      
    3) building an EventSelector from a list of files:
        evtsel=IOHelper().inputFiles(filelist)
        evtsel=IOHelper("POOL","POOL").inputFiles(filelist)
        evtsel=ioh.inputFiles(filelist)
      
    4) adding a simple OutputStream to the OutStream of Application Manager
        ioh.outStream(filename)
      
    5) adding a named special output stream to the OutputStream of Application Manager
        ioh.outStream(filename,"LHCbInputCopyStream/mystream")
      
    6) creating output algorithms to add to your sequence
        MySequence+=ioh.outputAlgs(filename,"InputCopyStream/mySelection")
        
    7) converting event selectors between formats
        IOHelper().convertSelector()
        evtsel=IOHelper("ROOT").convertSelector(evtsel)
        ioh.convertSelector()
    
    8) changing the services of old software stacks:
        IOHelper().postConfigServices()
        IOHelper("ROOT","ROOT").postConfigServices()
        ioh.postConfigServices()
    
    Specific uses:
    
    a) Ganga.           Needs to create the file name list to be passed to IOHelper.
                        Needs to be sensitive to presence of this class.
                        
    b) LHCbApp          Setup the services, may need slots to do the steering
    
    b) Brunel.          Use ioh.outStream to define the single output file
                        
    c) DaVinci.         BaseDSTWriter will use ioh.outputAlgs for each output file
    
    d) Stand-alone user. Needs to create the file list to be passed to IOHelper.
                         Adds ioh.outputAlgs to any of their personal sequencers not based on the BaseDSTwriter
                         Uses ioh.postConfigServices on old software stacks
    
    
    '''
    #the default persistencies
    _inputPersistency="POOL"
    _outputPersistency="POOL"
    
    _inputSvcTypDict = { 'ROOT' : "SVC='Gaudi::RootEvtSelector'",
                         'POOL' : "TYP='POOL_ROOT'",
                         'MDF'  : "SVC='LHCb::MDFSelector'"
                         }
    
    _outputSvcTypDict = { 'ROOT' : "SVC='RootCnvSvc'",
                          'POOL' : "TYP='POOL_ROOTTREE'",
                          'MDF'  : "SVC='LHCb::RawDataCnvSvc'"
                          }
    
    _knownPerServices = {'PoolDbCnvSvc': 'POOL',
                         'PoolDbCacheSvc': 'POOL',
                         'RootCnvSvc' : 'ROOT',
                         "RawDataCnvSvc": 'MDF',
                         'MultiFileCatalog' : '',
                         'IODataManager' : ''
                         }
    
    _knownOutputStreams = {'InputCopyStream' : 'DST',
                           'OutputStream' : 'DST',
                           'MDFWriter' : 'MDF',
                           'TagCollectionStream' : 'ETC',
                           'RunRecordStream' : 'FSR',
                           'RecordStream' : 'FSR'                           
                           }
    
    def __init__(self,Input=None,Output=None):
        '''
        IOHelper class, for simpler configuration of input/output
        
        ioh = IOHelper(Input=None,Output=None)
        
        "ioh" is now an instance of the helper which can be used in many places
        '''
        if Input is not None:
            if Input.upper()=="RAW": Input="MDF"
            if Input.upper() not in self._inputSvcTypDict:
                raise ValueError, Input+' input persistency not known'
            self._inputPersistency=Input.upper()
        
        if Output is not None:
            if Output.upper()=="RAW": Output="MDF"
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

        if self._inputPersistency=='ROOT' or self._outputPersistency=='ROOT':
            if not self.isRootSupported():
                raise TypeError("ROOT persistency is not supported in this Application version"+
                                "Ask your release manager for details or change to POOL")
    
    ###############################################################
    #              Helper functions
    ###############################################################
    
    def __str__(self):
        '''Helper:  print the class
        '''
        return "<IOHelper Input='"+self._inputPersistency+"' Output='" +self._outputPersistency+"'>"
    
    def __chooseIO(self,IO):
        '''Helper:  IO = starts with one of r, w, i or o
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
        '''Helper:   create the file records service, should always be done.
        '''
        from Gaudi.Configuration import (FileRecordDataSvc, ApplicationMgr, PersistencySvc)
        # Set up the FileRecordDataSvc
        FileRecordDataSvc( ForceLeaves        = True,
                           EnableFaultHandler = True,
                           RootCLID           = 1, #was the next line missed accidentally?
                           PersistencySvc     = "PersistencySvc/FileRecordPersistencySvc")
        
        fileSvc.ShareFiles = "YES"
        ApplicationMgr().ExtSvc                                += [ fileSvc ]
        PersistencySvc("FileRecordPersistencySvc").CnvServices += [ fileSvc ]

    def _isOutputStream(self, streamstr):
        '''Helper:  returns true if the string is one of the known output streams'''
        for stream in self._knownOutputStreams:
            #only check the type!
            if stream in streamstr.split('/')[0]:
                return True
        return False
    
    def _isPersistencySvc(self,svcstring):
        '''Helper:  Returns true if the svcstring is one of the known persistencies'''
        for service in self._knownPerServices:
            if service in svcstring:
                return True
        return False
    
    def _persistencyType(self,svcstring):
        '''Helper:  Returns either ROOT POOL or MDF'''
        
        for service in self._knownPerServices:
            if service in svcstring:
                return self._knownPerServices[service]
        return ''
    
    def _getPersistencyList(self, svclist):
        '''Helper:  From a list of services, return list of persistencies which are active'''
        retlist=[]
        for service in svclist:
            servicestring=''
            if type(service)==str:
                servicestr=service
            else:
                servicestr=service.getFullName()
                pertype=self._persistencyType(servicestr)
                if len(pertype):
                    if pertype not in retlist:
                        retlist.append(pertype)
        return retlist
    
    def _getSvcList(self,svclist):
        '''Helper: From a list of services return all persistency services'''
        retlist=[]
        for service in svclist:
            servicestring=''
            if type(service)==str:
                servicestr=service
            else:
                servicestr=service.getFullName()
            
            if self._isPersistencySvc(servicestr):
                retlist.append(service)
        
        return retlist
    
    def _removeConfigurables(self, conf_list):
        '''Helper: get rid of all configurables from a list'''
        from Gaudi.Configuration import allConfigurables
        conftodel=[k for k in allConfigurables if allConfigurables[k] in conf_list]
        for k in conftodel:
            del allConfigurables[k]

    def _fullNameConfigurables(self):
        import Gaudi.Configuration as GaudiConfigurables
        retdict={}
        for key in GaudiConfigurables.allConfigurables:
            retdict[GaudiConfigurables.allConfigurables[key].getFullName()]=GaudiConfigurables.allConfigurables[key]
        return retdict
    
    def _configurableInstanceFromString(self, config):
        '''Get a configurable instance given only the string'''
        
        import Gaudi.Configuration as GaudiConfigurables
        
        #if it's in Gaudi.Configuration
        
        if config in self._fullNameConfigurables():
            return self._fullNameConfigurables()[config]
        
        if config in GaudiConfigurables.allConfigurables:
            return GaudiConfigurables.allConfigurables[config]
        
        config=config.replace('::','__')
        
        #if it's in Gaudi.Configuration
        
        if config in self._fullNameConfigurables():
            return self._fullNameConfigurables()[config]
        
        if config in GaudiConfigurables.allConfigurables:
            return GaudiConfigurables.allConfigurables[config]
        
        wclass=None
        
        if hasattr(GaudiConfigurables, config.split('/')[0]):
            wclass = getattr(GaudiConfigurables,config.split('/')[0])
        else:
            import Configurables
            wclass = getattr(Configurables,config.split('/')[0])
            #otherwise it must be a configurable
            
        #check if it has an instance name
        if '/' not in config:
            return wclass()
        else:
            return wclass(name=config.split('/')[-1])
    
    ###############################################################
    #              Services
    ###############################################################
    
    def isRootSupported(self):
        '''Services:  Check if the root services exist in this version'''
        import Configurables
        return hasattr(Configurables,"Gaudi__RootCnvSvc")
    
    def svcTypString(self,IO):
        '''Services:  given the IO type, return the selection string for the active services'''
        IO=self.__chooseIO(IO)
        # Return SVC/TYP for file IO definition
        if IO == 'I': return self._inputSvcTypDict[self._inputPersistency]
        
        return self._outputSvcTypDict[self._outputPersistency]
    
    def setupServices(self):
        '''Services:  Setup the pool/Root services, to be done by LHCbApp'''
        from Gaudi.Configuration import (EventDataSvc, ApplicationMgr, EventPersistencySvc)
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
            #it's important also to enable the incidents for FSRs
            rootSvc = Gaudi__RootCnvSvc( "RootCnvSvc", EnableIncident = 1 )
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
        
        # Always enable reading/writing of MDF
        EventPersistencySvc().CnvServices.append("LHCb::RawDataCnvSvc")
        
        #always convert the event selector
        #if this is a PostConfigAction, then conversion will happen as a posConfig also
        #if this is a normal job with no flattened files, UserConfigurables are anyway called last
        #and so converting when the services are established is the best way.
        self.convertSelector()
        
        #always convert defined streams
        self.convertStreams()
    
    def activeServices(self):
        '''Services:  return all configured persistency services'''
        from Gaudi.Configuration import (ApplicationMgr, EventPersistencySvc, PersistencySvc)
        retlist=[]
        retlist+=self._getSvcList(ApplicationMgr().ExtSvc)
        retlist+=self._getSvcList(EventPersistencySvc().CnvServices)
        retlist+=self._getSvcList(PersistencySvc("FileRecordPersistencySvc").CnvServices)
        
        return retlist
    
    def activePersistencies(self):
        '''Services:  return all configured persistency services'''
        return self._getPersistencyList(self.activeServices())
    
    def clearServices(self):
        '''Services:  remove all persistency services'''
        from Gaudi.Configuration import (ApplicationMgr, EventPersistencySvc, PersistencySvc)
        
        active=self.activeServices()
        
        ApplicationMgr().ExtSvc=[svc for svc in ApplicationMgr().ExtSvc if svc not in active]
        EventPersistencySvc().CnvServices=[svc for svc in EventPersistencySvc().CnvServices if svc not in active]
        PersistencySvc("FileRecordPersistencySvc").CnvServices=[
            svc for svc in PersistencySvc("FileRecordPersistencySvc").CnvServices if svc not in active]
        
        self._removeConfigurables(active)
    
    def servicesExist(self):
        '''Services:  Check if any of the the services required for this persistency are set up'''
        return (self._inputPersistency in self.activePersistencies() and self._outputPersistency in self.activePersistencies())
    
    def changeServices(self):
        '''Services:  Go through the list of gaudi services, remove any persistency services and add different ones
        Only change services if the services already set up are not correct.
        '''
        if self.servicesExist():
            self.convertSelector()
            self.convertStreams()
            return
        
        self.clearServices()
        self.setupServices()
    
    def postConfigServices(self, eventSelector):
        '''Services:  append a Post Config action to change the services, input and outpu
        this is a helper function for patching old software
        '''
        from Gaudi.Configuration import appendPostConfigAction
        
        appendPostConfigAction(self.changeServices)
    
    
    ###############################################################
    #              Filenames
    ###############################################################
    
    def dressFile(self,filename,IO):
        '''Filenames:  Go from file name to connection string'''
        IO=self.__chooseIO(IO)
        if "DATAFILE=" in filename:
            raise ValueError, "the file has already been dressed, don't dress a dressed file!: "+filename
        if IO=='O':
            return "DATAFILE='" + filename + "' " + self.svcTypString( 'O' ) + " OPT='REC'"
        
        return "DATAFILE='" + filename + "' " + self.svcTypString( 'I' ) + " OPT='READ'"
    
    def undressFile(self,filestring):
        '''Filenames:  Go from connection string to file name'''
        match="DATAFILE='"
        pos=filestring.find(match)
        if pos>=0:
            pos=pos+len(match)
            if pos <len(filestring):
                filestring=filestring[pos:]
        filestring=filestring.strip("'").split("'")[0]
        return filestring
    
    def detectFileType(self, filestring):
        '''Filenames:  Go from connection string to persistency type'''
        for type in self._inputSvcTypDict:
            if self._inputSvcTypDict[type] in filestring:
                return type
        
        for type in self._outputSvcTypDict:
            if self._outputSvcTypDict[type] in filestring:
                return type
        
        return "UNKNOWN"
    
    def convertConnectionStrings(self, filelist, IO, force=False):
        '''Filenames:  Go from a list of connection strings or file names to a new list of connection strings
        needs to know the IO type to know what to convert to'''
        IO=self.__chooseIO(IO)

        #don't do anything if my type is MDF
        if not force and IO=="I" and self._inputPersistency=="MDF":
            return filelist
        if not force and IO=="O" and self._outputPersistency=="MDF":
            return filelist
        
        retlist=[]
        for file in filelist:
            #never convert an MDF file, it's not needed
            if not force and self.detectFileType(file)=="MDF":
                retlist.append(file)
                continue
            #otherwise convert it
            retlist.append( self.dressFile( self.undressFile(file),IO) )
        return retlist
    
    
    ###############################################################
    #              Input
    ###############################################################
    
    def convertSelector(self, eventSelector=None):
        '''Input:  Modify an EventSelector to switch to the new input connection strings
        Go from a list of connection strings or file names to a new list of connection strings
        
        convertSelector(self, eventSelector=None)
        
        If eventSelector is None, the default EventSelector will be found.
        If an event selector is passed, that one will be modified
        
        '''
        if eventSelector is None:
            from Gaudi.Configuration import EventSelector
            eventSelector=EventSelector()
        
        #don't do anything if _my_ type is MDF to avoid overwiting everything forever
        if self._inputPersistency=="MDF":
            return eventSelector
        
        eventSelector.Input=self.convertConnectionStrings(eventSelector.Input, "I")
        
        return eventSelector
    
    def inputFiles(self,files,clear=False, eventSelector=None):
        '''Input:  Edit the content of EventSelector and fill the Inputs with 
        Go from a list of connection strings or file names to a new list of connection strings
        
        inputFiles(self,files,clear=True, eventSelector=None)
        
        If eventSelector is None, the default EventSelector will be found.
        If an event selector is passed, that one will be modified
        
        If clear is True the original Inputs are overwritten.
        If clear is False the original Inputs are also kept.
        '''
        
        if eventSelector is None:
            from Gaudi.Configuration import EventSelector
            eventSelector=EventSelector()
        
        if clear:
            eventSelector.Input=[]
        
        if not clear:
            self.convertSelector(eventSelector)
        
        for file in files:
            #never convert a dressed MDF file, it's not needed
            if self.detectFileType(file)=="MDF":
                eventSelector.Input.append(file)
                continue
            eventSelector.Input += [ self.dressFile(self.undressFile(file),'I') ]
        return eventSelector
    
    def selectorString(self, eventSelector=None):
        '''Input:  return a string of the event selector which could be used in an old-style gaudi card
        '''
        
        if eventSelector is None:
            from Gaudi.Configuration import EventSelector
            eventSelector=EventSelector()
        
        retstr='from Gaudi.Configuration import EventSelector\n'
        retstr+=eventSelector.getType()+'('
        if eventSelector.getType()!=eventSelector.getName():
            retstr+=eventSelector.getType()
        
        retstr+=').Input=[\n'
        
        files=eventSelector.Input
        
        alen=4
        for file in files[:-1]:
            retstr+=' '*alen+'"'+file+'",\n'
        for file in files[-1:]:
            retstr+=' '*alen+'"'+file+'"\n'
        retstr+=' '*alen+']'
        return retstr
    
    def _subHelperString(self,files):
        '''Input:  return a string when the types of files are the same'''
        retstr=''
        type=self.detectFileType(files[0])
        if(type)=="MDF":
            retstr+='IOHelper("MDF").inputFiles([\n'
        else:
            retstr+="IOHelper().inputFiles([\n"
        
        alen=4
        for file in files[:-1]:
            retstr+=' '*alen+'"'+self.undressFile(file)+'",\n'
        for file in files[-1:]:
            retstr+=' '*alen+'"'+self.undressFile(file)+'"\n'
        retstr+=' '*alen+'])'
        
        return retstr
    
    def helperString(self, eventSelector=None):
        '''Input:  return a string of the IOHelper which could be used in a new-style gaudi card
        '''
        
        if eventSelector is None:
            from Gaudi.Configuration import EventSelector
            eventSelector=EventSelector()
        
        retstr='from GaudiConf import IOHelper\n'
        
        files=eventSelector.Input
        if not len(files): return retstr+'IOHelper().inputFiles([])\n'
        
        #needs to be more complicated to handle MDF and Root files
        #first group into lists of the different types
        type=self.detectFileType(files[0])
        grouped_files=[[]]
        group=0
        for file in files:
            newtype=self.detectFileType(file)
            if newtype==type:
                grouped_files[group].append(file)
                continue
            
            type=newtype
            group=group+1
            grouped_files.append([])
            grouped_files[group].append(file)
        
        #then loop over the groups
        for agroup in grouped_files:
            retstr+=self._subHelperString(agroup)+'\n'
        
        return retstr
    
    ###############################################################
    #              Output
    ###############################################################
    
    def activeStreams(self):
        '''Output:  Find the list of Output Stream-type objects,
        search through allConfigurables AppMgr.TopAlg and and AppMgr.OutStream'''
        streams=[]
        from Gaudi.Configuration import ApplicationMgr
        for alg in ApplicationMgr().TopAlg:
            algname=alg
            if type(algname) is not str:
                algname=alg.getFullName()
            if self._isOutputStream(algname):
                streams.append(alg)
        
        if ApplicationMgr().OutStream is not None:
            streams+=ApplicationMgr().OutStream
        
        allConfigurables=self._fullNameConfigurables()
        #from Gaudi.Configuration import allConfigurables
        for key in allConfigurables:
            if self._isOutputStream(key):
                if key not in streams and allConfigurables[key] not in streams:
                    streams.append(allConfigurables[key])
        
        return streams
    
    def detectStreamType(self, stream):
        '''Output:  From the name of the stream, deduce its type'''
        
        if type(stream)!=str:
            stream=stream.getFullName()
        
        #only the type!
        stream=stream.split('/')[0]
        if stream in self._knownOutputStreams:
            return self._knownOutputStreams[stream]
        
        return "UNKNOWN"
    
    def convertStreams(self, streams=None):
        '''Output:  convert a list of OutputStreams, so that their Output files go to the new services
        If no list of streams is given, it will search through allConfigurables and AppMgr.OutStream
        '''
        #don't do anything if _my_ type is MDF to avoid overwiting everything forever
        if self._outputPersistency=="MDF":
            return
        
        if streams is None:
            streams=self.activeStreams()
        
        for stream in streams:
            if self.detectStreamType(stream) in ["MDF","UNKNOWN"]:
                continue
            if type(stream) is str:
                stream=self._configurableInstanceFromString(stream)
                
            if hasattr(stream,'Output'):
                if stream.Output is not None:
                    #don't convert odd looking lists or empty strings
                    if len(stream.Output) and type(stream.Output) is str:
                        if self.detectFileType(stream.Output) not in ["MDF","ETC","UNKNOWN"]:
                            stream.Output=self.dressFile(self.undressFile(stream.Output),"O")
        return
    
    def outputAlgs(self,filename,writer="OutputStream",writeFSR=True):
        '''Output:  Create a output stream and FSR writing algorithm instance
        to write into the given file.
        
        Any class inheriting from OutputStream can be used, with an instance name or configurable.
        
        e.g. ioh.outputAlgs("test.dst","InputCopyStream/Spam")
        e.g. ioh.outputAlgs("test.dst",InputCopyStream("Spam"))
        
        returns a list of algorithms to append to your sequencer.
        '''
                
        if self._outputPersistency=="MDF" and writeFSR:
            print "# WARNING: FSRs can not be written to MDF file ",filename,  ". Set writeFSR to False to supress this warning"
            writeFSR=False
        
        stype=self.detectStreamType(writer)
        if stype not in ["DST", "ETC"] and writeFSR:
            writeFSR=False
            print "# WARNING: ",
            if stype in ["FSR"]:
                print "this is already an FSR-stream, it makes no sense to write _more_ FSRs to the file.",filename,
            else:
                print "FSRs can not be written to streams of the form ",stype,
            
            print ". Set writeFSR to False to supress this warning"
            writeFSR=False
        
        #find the writer
        winstance=None
        
        import Gaudi.Configuration as GaudiConfigurables
        
        if type(writer) is str:
            winstance=self._configurableInstanceFromString(writer)
        else:
            winstance=writer
            writer=writer.getFullName()
        
        if hasattr(winstance,"Output") or (hasattr(winstance,'__slots__') and "Output" in winstance.__slots__):
            #build up the filename
            filename=self.dressFile(filename,'O')
            winstance.Output = filename
        elif hasattr(winstance, "Connection") or (hasattr(winstance,'__slots__') and "Connection" in winstance.__slots__):
            winstance.Connection="file://"+filename
            #build up the filename
            filename=self.dressFile(filename,'O')
        else:
            raise TypeError, "writer class "+writer+" does not have any output to configure. Choose a class with either Output or Connection"
        
        if not writeFSR: return [winstance]
        
        #ignore name when name is type
        if writer.split('/')[0]== writer.split('/')[-1]:
            writer=writer.split('/')[0]
        
        FSRWriter = GaudiConfigurables.RecordStream( "FSR"+writer.replace('/','').replace('::','').replace('__',''),
                                                     ItemList = [ "/FileRecords#999" ],
                                                     EvtDataSvc = "FileRecordDataSvc",
                                                     EvtConversionSvc = "FileRecordPersistencySvc" )
        
        FSRWriter.Output = filename
        
        #As far as I can tell, the ordering of the algs does not matter here
        return [winstance, FSRWriter]
        #both result in well-formed Lumi tests
        #return [FSRWriter, winstance]
    
    def outStream(self,filename,writer="OutputStream",writeFSR=True):
        '''Output:  Create a output stream and FSR writing algorithm instance
        to write into the given file.
        
        Any class inheriting from OutputStream can be used, with an instance name or configurable.
        
        e.g. ioh.outputAlgs("test.dst","InputCopyStream/Spam")
        e.g. ioh.outputAlgs("test.dst",InputCopyStream("Spam"))
        
        Adds these streams directly to the Application manager OutStream
        '''
        from Gaudi.Configuration import ApplicationMgr
        
        algs=self.outputAlgs(filename,writer,writeFSR)
        
        if ApplicationMgr().OutStream is None or len(ApplicationMgr().OutStream)==0:
            ApplicationMgr().OutStream =[]
        
        for alg in algs:
            ApplicationMgr().OutStream.append(alg)

class IOExtension(object):
    '''Helper class when you want to construct specific IO
    using the file extensions rather than flagging the MDF persistencies
    
    Uses an internal map of known extensions
    
    Internally uses IOHelper wherever possible
    
    from GaudiConf import IOExtension
    iox = IOExtension(Persistency=None)
    
    Persistency is then persistency type to use for Root-type files (ROOT/POOL)
    the default from IOHelper is maintained if None is passed.
    
    examples:
    
    1) building an EventSelector from a list of files of mixed types:
        evtsel=IOExtension().inputFiles(filelist)
        evtsel=IOExtension("POOL").inputFiles(filelist)
        evtsel=iox.inputFiles(filelist)
      
    2) adding a simple OutputStream to the OutStream of Application Manager
        iox.outStream(filename)
      
    3) adding a named special output stream to the OutputStream of Application Manager
        iox.outStream(filename,"LHCbInputCopyStream/mystream")
      
    4) creating output algorithms to add to your sequence
        MySequence+=iox.outputAlgs(filename,"InputCopyStream/mySelection")
        
    Specific examples:
    
    a) Ganga:       needs only to pass the persistency once if requested
                    backwards compatible with respect to MDF files in ganga
                    
                    from GaudiConf import IOExtension
                    IOExtension(Persistency).inputFiles(file_list)
                    
    b) MOORE:       uses extensions to determine type
                    will use detectMinType, maybe both input and output are MDF
                    this result would then be passed to LHCbApp
                    
                    from GaudiConf import IOExtension
                    iox=IOExtension(Persistency)
                    LHCbApp().Persistency=iox.detectMinType(input+output)
    '''
    
    _defaultPersistency=IOHelper._inputPersistency
    
    _knownExtensions={ 'DIGI' : '',
                       'SIM'  : '',
                       'DST'  : '',
                       'XDST'  : '',
                       'MDST'  : '',
                       'SDST'  : 'Warning',
                       'MDF'    : 'MDF',
                       'RAW'  : 'MDF'
                       }
    
    def __init__(self, Persistency=None):
        '''
        Initialise with persistency, which is the persistency to use for
        detected root-type files (POOL/ROOT)
        '''
        if Persistency is not None:
            #check the persistency is valid
            ioh=IOHelper(Persistency,Persistency)
            #reset own persistency
            self._defaultPersistency=ioh._inputPersistency
    
    ###############################################################
    #              Extensions
    ###############################################################
    
    def detectFileType(self, filename):
        '''
        Extensions: From first the connection string, and second the extension
        determine the type of the file from its name when possible
        
        returns the string of the file type, or "UNKNOWN" if there is a problem
        '''
        iohtype=IOHelper().detectFileType(filename)
        if iohtype is not "UNKNOWN":
            return iohtype
        
        filename=IOHelper().undressFile(filename)
        ext=filename.split('.')[-1].strip().upper()
        
        if ext not in self._knownExtensions:
            return "UNKNOWN"
        
        retext=self._defaultPersistency
        if 'MDF' in self._knownExtensions[ext]:
            retext='MDF'
        if 'Warning' in self._knownExtensions[ext]:
            print "# Warning, guessing type of "+ext+" to be "+retext+" use IOHelper instead to avoid this warning"
        
        return retext
    
    
    def detectMinType(self, files):
        '''Extensions:  from a list of files determine the minimal persistency required
        This will either be MDF or (POOL/ROOT)
        
        If POOL/ROOT is ecountered, but IOExtension was initialized with the
        other persistency, a TypeError is raised.
        
        If any one of the files is POOL/ROOT return POOL/ROOT
        Only if all the files are MDF return MDF
        '''
        for file in files:
            persistency=self.detectFileType(file)
            if persistency=="UNKNOWN":
                raise TypeError, "Type of file "+filename+" could not be determined"+" use IOHelper with specified persistency instead"
            if persistency is not 'MDF':
                if persistency!=self._defaultPersistency:
                    raise TypeError, "You are trying to parse "+persistency+" file types when you told IOExtension to expect "+self._defaultPersistency
                return persistency
        
        return 'MDF'
    
    def getIOHelper(self, filename):
        '''Extensions: Determine the persistency of a given file, and then grab an IOHelper
        instance with that persistency
        '''
        persistency=self.detectFileType(filename)
        if persistency=="UNKNOWN":
            raise TypeError, "Type of file "+filename+" could not be determined"+" use IOHelper with specified persistency instead"
        return IOHelper(persistency,persistency)
    
    def getCompleteIOHelper(self, infiles, outfile):
        '''Extensions:  for a set of input and output files, determine their persistencies
        then grab an IOHelper instance with those persistencies
        '''
        inputs=self.detectMinType(infiles)
        
        output=self.detectFileType(outfile)
        
        return IOHelper(inputs, output)
    
    ###############################################################
    #              Filenames
    ###############################################################
    
    def dressFile(self, filename, IO):
        '''Filenames: wrapper for IOHelper.dressFile, where the persistency
        type is guessed from the file extension

        takes a simple filename and returns the connection string
        '''
        return self.getIOHelper(filename).dressFile(filename,IO)
    
    
    ###############################################################
    #              Input
    ###############################################################
    
    def inputFiles(self,files,clear=False, eventSelector=None):
        '''Input: wrapper for IOHelper.inputFiles, where the persistency
        type is guessed from the file extension

        inputFiles(<list_of_files>, clear=False, eventSelector=None)

        if clear is True, empty the existing EventSelector list
        '''
        #print eventSelector
        #print eventSelector.__slots__
        
        if eventSelector is None:
            from Gaudi.Configuration import EventSelector
            eventSelector=EventSelector()
        
        if clear:
            eventSelector.Input=[]
        
        for file in files:
            eventSelector=self.getIOHelper(file).inputFiles([file],clear=False,eventSelector=eventSelector)
        
        return eventSelector
    
    
    ###############################################################
    #              Output
    ###############################################################
    
    def outputAlgs(self,filename,writer="OutputStream"):
        '''Output: wrapper for IOHelper.outputAlgs, where the persistency
        type is guessed from the file extension
        In the case of MDF, no FSR will be written

        returns a list of algorithms to add to your sequence
        '''
        ftype=self.detectFileType(filename)
        return IOHelper(ftype,ftype).outputAlgs(filename,writer,writeFSR=(ftype!='MDF'))
    
    def outStream(self,filename,writer="OutputStream"):
        '''Output: wrapper for IOHelper.outStream, where the persistency
        type is guessed from the file extension
        In the case of MDF, no FSR will be written
        
        Adds the resultant algorithms to ApplicationMgr().OutStream
        '''
        ftype=self.detectFileType(filename)
        return IOHelper(ftype,ftype).outStream(filename,writer,writeFSR=(ftype!='MDF'))
    
    
