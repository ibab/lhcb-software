"""
Configure IO persistency and TES.

IOHelper class for whan you know the persistency services you want

IOExtention if you want to be clever and guess the service type from the file extension
"""
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

#from Gaudi.Configuration import *

from Manipulations import fullNameConfigurables, nameFromConfigurable, configurableInstanceFromString, removeConfigurables

class IOHelper(object):
    '''
    IOHelper class, for simpler configuration of input/output

    from GaudiConf import IOHelper
    ioh = IOHelper(Input=None,Output=None)

    "ioh" is now an instance of the helper which can be used in many places
    Defaults to ROOT.

    1) check if root framework is possible:
        IOHelper().isRootSupported()

    2) configuring services:
        IOHelper().setupServices()
        IOHelper("ROOT","ROOT").setupServices()
        ioh.setupServices()

    3) building an EventSelector from a list of files:
        evtsel=IOHelper().inputFiles(filelist)
        evtsel=IOHelper("ROOT","ROOT").inputFiles(filelist)
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

    c) Brunel.          Use ioh.outStream to define the single output file

    d) DaVinci.         BaseDSTWriter will use ioh.outputAlgs for each output file

    e) Stand-alone user. Needs to create the file list to be passed to IOHelper.
                         Adds ioh.outputAlgs to any of their personal sequencers not based on the BaseDSTwriter
                         Uses ioh.postConfigServices on old software stacks


    '''
    #the default persistencies
    _outputPersistency=_inputPersistency=_defaultPersistency="ROOT"
    #_inputPersistency=_defaultPersistency
    #_outputPersistency=_defaultPersistency

    _inputSvcTypDict = { 'ROOT' : "SVC='Gaudi::RootEvtSelector'",
                         'POOL' : "TYP='POOL_ROOT'",
                         'MDF'  : "SVC='LHCb::MDFSelector'"
                         }

    _outputSvcTypDict = { 'ROOT' : "SVC='RootCnvSvc'",
                          'POOL' : "TYP='POOL_ROOTTREE'",
                          'MDF'  : "SVC='LHCb::RawDataCnvSvc'",
                          'FSR'  : "SVC='FileRecordCnvSvc'"
                          }

    #services which vary depending on the input persistency
    _knownPerServices = {'PoolDbCnvSvc': 'POOL',
                         'PoolDbCacheSvc': 'POOL',
                         'RootCnvSvc' : 'ROOT',
                         "RawDataCnvSvc": 'MDF',
                         'MultiFileCatalog' : '',
                         'IODataManager' : '',
                         }

    #services which exist in Gaudi, independent of the persistency
    _externalPerServices = {"FileRecordDataSvc" : 'FSR',
                            'EventPersistencySvc' : 'EVT',
                            'EvtPersistencySvc' : 'EVT',
                            'EventDataSvc' : 'EVT',
                            'EvtDataSvc' : 'EVT',
                            "FileRecordPersistencySvc": 'FSR',
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

        if self._outputPersistency == "POOL" or self._inputPersistency == "POOL":
            raise ValueError("POOL is completely deprecated, fix your options or use an older software stack")

        if self._inputPersistency=='ROOT' or self._outputPersistency=='ROOT':
            if not self.isRootSupported():
                raise TypeError("ROOT persistency is not supported in this Application version"+
                                "Ask your release manager for details or change to POOL")

        if self._outputPersistency=="FSR" and self._inputPersistency not in ['ROOT']:
            raise TypeError("FSR is not a proper persistency type. To configure services, you would need to specify a proper type.")

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

    def _isSequencer(self, confstr):
        '''Helper:  returns true if the string is one of the known output streams'''
        #only check the type!
        if "GaudiSequencer" in confstr.split('/')[0]:
            return True
        return False

    def _isPersistencySvc(self,svcstring):
        '''Helper:  Returns true if the svcstring is one of the known persistencies'''
        for service in self._knownPerServices:
            if service in svcstring:
                return True
        return False

    def _persistencyType(self,svcstring):
        '''Helper:  Returns either ROOT or MDF'''

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

    def _isDressed(self, filename):
        '''Determine if a string is dressed or not
        '''
        return ("DATAFILE='" in filename or " OPT='" in filename or "SVC=" in filename or "TYP='" in filename)

    def _gaudiSvcVersion(self):
        '''Determine the version of GaudiSvc,
        return true if it is larger than v18r3'''
        apath="${GAUDISVCROOT}/cmt/requirements"
        import os
        #Without the env variable, all new Gaudi versions are fine
        if "GAUDISVCROOT" not in os.environ:
            return True
        apath=os.path.expandvars(apath)
        try:
            requirements=file(apath)
        except:
            return False

        if not requirements:
            requirements.close()
            return False
        version=None
        for line in requirements.readlines():
            if 'version' in line.strip()[:7]:
                version=line.split('n')[-1].strip().split('v')[-1]
                v=int(version.split('r')[0])
                r=None
                p=None
                if 'r' in version:
                    r=int(version.split('r')[-1].split('p')[0])
                if 'p' in version:
                    p=int(version.split('p')[-1])
                version=(v,r,p)
                break
        requirements.close()
        if version is None:
            return False
        #v17 or lower
        if version[0]<18:
            return False
        #higher than v18
        if version[0]>18:
            return True
        #v18rX
        if version[0]==18:
            #v18r<16
            if version[1] is None or version[1]<16:
                return False
            #v18r>16
            if version[1]>=16:
                return True
            #v18r16p>0
            if version[2] is None or version[2]<1:
                return False
            return True

        return False

    ###############################################################
    #              Information
    ###############################################################
    def defaultPersistency(self):
        '''Information: return the default persistency'''
        return str(IOHelper._defaultPersistency)

    def inputPersistency(self):
        '''Information: return the input persistency'''
        return str(self._inputPersistency)


    def outputPersistency(self):
        '''Information: return the output persistency'''
        return str(self._outputPersistency)

    def knownPersistencyServices(self):
        '''Information: return the list of known persistency services'''
        return self._knownPerServices.keys()

    def knownExternalServices(self):
        '''Information: return the list of known external persistency services'''
        return self._externalPerServices.keys()

    def knownOutputStreams(self):
        '''Information: return the list of known output streams'''
        return self._knownOutputStreams.keys()

    def debugIO(self):
        '''Information: print properties of all configured svc streams and selectors'''
        print "=========================="
        print "Debugging Persistencies"
        print self.activePersistencies()
        print "=========================="
        print "Debugging External Services"
        for svc in self.externalServices():
            if type(svc) is str:
                print svc
                continue
            print svc.getFullName()
            print svc.getValuedProperties()
        print "=========================="
        print "Debugging Persistency Services"
        for svc in self.activeServices():
            if type(svc) is str:
                print svc
                continue
            print svc.getFullName()
            print svc.getValuedProperties()
        print "=========================="
        print "Debugging Streams"
        for stream in self.activeStreams():
            if type(stream) is str:
                print stream
                continue

            print stream.getFullName()
            print stream.getValuedProperties()
        print "=========================="
        print "Debugging Input"
        from Gaudi.Configuration import EventSelector
        print EventSelector().getFullName()
        print EventSelector().getValuedProperties()
        print "=========================="

    def postConfigDebug(self):
        '''Print debug information as a post-config action'''
        from Gaudi.Configuration import appendPostConfigAction

        appendPostConfigAction(self.debugIO)

    ###############################################################
    #              Services
    ###############################################################

    def isRootSupported(self):
        '''Services:  Check if the ROOT services exist in this version'''
        import Configurables
        #check the RootCnv service exists and that the GaudiSvc version
        #is greater than v18r16, only print a warning for the moment.
        if not self._gaudiSvcVersion():
            print "# WARNING: It looks like this version of GaudiSvc is from before v18r16, so will suffer from bugs"
            print "# WARNING: To avoid segfaulting in finalize and writing out corrupted files you need to be using a patched version with at least revision r6649 of GaudiSvc"
            print "# WARNING: If you've already patched the software, you can ignore this warning."
        return hasattr(Configurables,"Gaudi__RootCnvSvc")

    def isPoolSupported(self):
        '''Services: Check if the POOL services exist in this version'''
        return False

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
            # disable caches by default
            if not rootSvc.isPropertySet("VetoBranches")  : rootSvc.VetoBranches = ["*"]
            if not rootSvc.isPropertySet("CacheBranches") : rootSvc.CacheBranches = []
            EventPersistencySvc().CnvServices += [ rootSvc ]
            ApplicationMgr().ExtSvc           += [ rootSvc ]

            fileSvc = Gaudi__RootCnvSvc( "FileRecordCnvSvc" )
            #optimization, see Core Soft meeting, indico.cern.ch/conferenceDisplay?confId=207776
            if hasattr(fileSvc,"BufferSize"):
                fileSvc.BufferSize=512

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

    def externalServices(self):
        '''Services: Return services configured irrespective of the persistency type '''
        retlist=[]

        allConfigurables=fullNameConfigurables()

        for key in allConfigurables:
            for extsvc in self._externalPerServices:
                if extsvc in key:
                    if allConfigurables[key] not in retlist:
                        retlist+=[allConfigurables[key]]

        #from Gaudi.Configuration import (EventDataSvc, ApplicationMgr, EventPersistencySvc)

        return retlist

    def activeServices(self):
        '''Services:  return all configured persistency services which depend on the persistency'''
        from Gaudi.Configuration import ApplicationMgr

        retlist=[]
        extsvc=self.externalServices()

        for svc in extsvc:
            if hasattr(svc,"CnvServices") or "CnvServices" in svc.__slots__:
                retlist+=self._getSvcList(svc.CnvServices)

        retlist+=self._getSvcList(ApplicationMgr().ExtSvc)

        reducedList=[]
        for svc in retlist:
            if svc not in reducedList:
                reducedList.append(svc)

        return reducedList

    def activePersistencies(self):
        '''Services:  return all configured persistency services'''
        return self._getPersistencyList(self.activeServices())

    def clearServices(self):
        '''Services:  remove all persistency services'''
        from Gaudi.Configuration import ApplicationMgr

        active=self.activeServices()

        ApplicationMgr().ExtSvc=[svc for svc in ApplicationMgr().ExtSvc if svc not in active]

        extsvc=self.externalServices()

        for svc in extsvc:
            if hasattr(svc, "CnvServices") or "CnvServices" in svc.__slots__:
                svc.CnvServices=[asvc for asvc in svc.CnvServices if asvc not in active]

        removeConfigurables(active)

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

    def postConfigServices(self):
        '''Services:  append a Post Config action to change the services, input and output
        this is a helper function for patching old software
        '''
        from Gaudi.Configuration import appendPostConfigAction

        appendPostConfigAction(self.changeServices)

    def detectServiceType(self, svc):
        '''Services: return the persistency type(s) of a given service'''
        if type(svc)!=str:
            svc=svc.getFullName()
        for service in self._knownPerServices:
            if service in svc:
                return self._knownPerServices[service]
        for service in self._externalPerServices:
            if service in svc:
                return self._externalPerServices[service]
        return "UNKNOWN"

    ###############################################################
    #              Filenames
    ###############################################################

    def dressFile(self,filename,IO):
        '''Filenames:  Go from file name to connection string'''
        IO=self.__chooseIO(IO)
        if self._isDressed(filename):
            raise ValueError, "the file has already been dressed, don't dress a dressed file!: "+filename
        retstr=""
        #allow filename to be an empty string, in the case of OutputStream where outputFile and Output are
        #set separately
        if filename.strip()!='':
            filename="DATAFILE='" + filename + "' "
        if IO=='O':
            return filename + self.svcTypString( 'O' ) + " OPT='REC'"

        return filename + self.svcTypString( 'I' ) + " OPT='READ'"

    def undressFile(self,filestring):
        '''Filenames:  Go from connection string to file name'''
        if not self._isDressed(filestring):
            return filestring
        #allow filename to be an empty string,
        #in the case of OutputStream where outputFile and Output are
        #set separately
        match="DATAFILE='"
        pos=filestring.find(match)
        if pos<0:
            return ''

        pos=pos+len(match)
        if pos <len(filestring):
            filestring=filestring[pos:]

        filestring=filestring.strip("'").split("'")[0]
        return filestring

    def detectFileType(self, filestring):
        '''Filenames:  Go from connection string to persistency type'''
        for atype in self._inputSvcTypDict:
            if self._inputSvcTypDict[atype] in filestring:
                return atype

        for atype in self._outputSvcTypDict:
            if self._outputSvcTypDict[atype] in filestring:
                return atype

        return "UNKNOWN"

    def convertConnectionStrings(self, filelist, IO, force=False):
        '''Filenames:  Go from a list of connection strings or file names to a new list of connection strings
        needs to know the IO type to know what to convert to'''
        IO=self.__chooseIO(IO)

        #don't do anything if my type is MDF or FSR
        if not force and IO=="I" and self._inputPersistency=="MDF":
            #print "skipping in ConnectionStrings 1"
            return filelist
        if not force and IO=="O" and (self._outputPersistency=="MDF" or self._outputPersistency=="FSR"):
            #print "skipping in ConnectionStrings 2"
            return filelist

        retlist=[]
        for file in filelist:
            #never convert an MDF or an output FSR file, it's not needed, filetype FSR implies FileRecordCnvSvc is already specified
            if not force and (self.detectFileType(file)=="MDF" or (self.detectFileType(file)=="FSR" and IO=="O")):
                retlist.append(file)
                #print "skipping in COnnectionStrings 3"
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
        #don't do anything if _my_ type is MDF to avoid overwiting everything forever
        if self._inputPersistency=="MDF":
            return eventSelector

        if eventSelector is None:
            from Gaudi.Configuration import EventSelector
            eventSelector=EventSelector()

        if type(eventSelector.Input) is not list:
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

        if type(files) is not list:
            raise TypeError, "You need to pass a list of files to InputFiles, you have passed a "+str(type(files))+" instead"

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
        if files is not None and type(files) is not list:
            raise TypeError, "The EventSelector does not have a list of input files"

        alen=4
        for file in files[:-1]:
            retstr+=' '*alen+'"'+file+'",\n'
        for file in files[-1:]:
            retstr+=' '*alen+'"'+file+'"\n'
        retstr+=' '*alen+']'
        return retstr

    def _subHelperString(self,files,setPersistency=False):
        '''Input:  return a string when the types of files are the same'''
        retstr=''
        atype=self.detectFileType(files[0])
        if(atype)=="MDF":
            if setPersistency:
                retstr+='IOHelper("MDF","'+self._outputPersistency+'").inputFiles([\n'
            else:
                retstr+='IOHelper("MDF").inputFiles([\n'
        else:
            if setPersistency:
                retstr+='IOHelper("'+self._inputPersistency+'","'+self._outputPersistency+'").inputFiles([\n'
            else:
                retstr+="IOHelper().inputFiles([\n"

        alen=4
        for file in files[:-1]:
            retstr+=' '*alen+'"'+self.undressFile(file)+'",\n'
        for file in files[-1:]:
            retstr+=' '*alen+'"'+self.undressFile(file)+'"\n'
        retstr+=' '*alen+'])'

        return retstr

    def helperString(self, eventSelector=None, setPersistency=False):
        '''Input:  return a string of the IOHelper which could be used in a new-style gaudi card
        if setPersistency is True, the given persistencies will be specified in the string
        '''

        if eventSelector is None:
            from Gaudi.Configuration import EventSelector
            eventSelector=EventSelector()

        retstr='from GaudiConf import IOHelper\n'

        files=eventSelector.Input
        if files is not None and type(files) is not list:
            raise TypeError, "The EventSelector does not have a list of input files"

        if not len(files): return retstr+'IOHelper().inputFiles([])\n'

        #needs to be more complicated to handle MDF and Root files
        #first group into lists of the different types
        atype=self.detectFileType(files[0])
        grouped_files=[[]]
        group=0
        for file in files:
            newtype=self.detectFileType(file)
            if newtype==atype:
                grouped_files[group].append(file)
                continue

            atype=newtype
            group=group+1
            grouped_files.append([])
            grouped_files[group].append(file)

        #then loop over the groups
        for agroup in grouped_files:
            retstr+=self._subHelperString(agroup,setPersistency)+'\n'

        return retstr

    ###############################################################
    #              Output
    ###############################################################

    def activeStreams(self):
        '''Output:  Find the list of Output Stream-type objects,
        search through allConfigurables AppMgr.TopAlg and and AppMgr.OutStream
        also search all gaudi sequencers'''
        streams=[]
        from Gaudi.Configuration import ApplicationMgr
        for alg in ApplicationMgr().TopAlg:
            algname=alg
            if type(algname) is not str:
                algname=alg.getFullName()
            if self._isOutputStream(algname):
                streams.append(alg)

        #everything in OutStream is an output stream
        if ApplicationMgr().OutStream is not None:
            streams+=ApplicationMgr().OutStream

        #all defined configurables which match my expressions are output streams
        allConfigurables=fullNameConfigurables()
        for key in allConfigurables:
            if self._isOutputStream(key):
                if key not in streams and allConfigurables[key] not in streams:
                    streams.append(allConfigurables[key])

        #all streams added to GaudiSequencers are Output Streams...
        for sequencer in allConfigurables:
            if self._isSequencer(sequencer):
                if (not hasattr(allConfigurables[sequencer], "Members")) or (allConfigurables[sequencer].Members is None):
                    continue
                for member in allConfigurables[sequencer].Members:
                    membername=nameFromConfigurable(member)
                    if self._isOutputStream(membername):
                        if member not in streams and membername not in streams:
                            streams.append(member)
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
            #print "skipping in stream type 0"
            return

        if streams is None:
            streams=self.activeStreams()

        for stream in streams:
            #for converting streams, only ignore MDF, its the only format
            #for which the old and new specifications are identical
            if self.detectStreamType(stream) in ["MDF","UNKNOWN"]:
                #print "skipping in stream type 1"
                continue

            if type(stream) is str:
                stream=configurableInstanceFromString(stream)

            #redo FSR streams completely so that they have all the right options
            if self.detectStreamType(stream) in ["FSR"]:
                if hasattr(stream,'Output') and stream.Output is not None:
                    self._fsrWriter(self.undressFile(stream.Output),stream)
                else:
                    self._fsrWriter(writer=stream)
                #print "Done conversion!"
                continue

            if hasattr(stream,'Output'):
                if stream.Output is not None:
                    #don't convert odd looking lists or empty strings
                    if len(stream.Output) and type(stream.Output) is str:
                        #I should also be converting "Unknown" types
                        if self.detectFileType(stream.Output) not in ["MDF","ETC","FSR"]:
                            stream.Output=self.dressFile(self.undressFile(stream.Output),"O")
                            continue
            #print "fallen through if statements in convertStreams"
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
            winstance=configurableInstanceFromString(writer)
        else:
            winstance=writer
            writer=writer.getFullName()

        dst_filename=""
        if hasattr(winstance,"Output") or (hasattr(winstance,'__slots__') and "Output" in winstance.__slots__):
            #build up the filename
            dst_filename=self.dressFile(filename,'O')
            winstance.Output = dst_filename
        elif hasattr(winstance, "Connection") or (hasattr(winstance,'__slots__') and "Connection" in winstance.__slots__):
            winstance.Connection="file://"+filename
            #build up the filename
            dst_filename=self.dressFile(filename,'O')
        else:
            raise TypeError, "writer class "+writer+" does not have any output to configure. Choose a class with either Output or Connection"

        if not writeFSR: return [winstance]

        #ignore name when name is type
        if writer.split('/')[0]== writer.split('/')[-1]:
            writer=writer.split('/')[0]

        FSRWriter = self._fsrWriter(filename, writer=GaudiConfigurables.RecordStream( "FSR"+writer.replace('/','').replace('::','').replace('__','')))

        #As far as I can tell, the ordering of the algs does not matter here
        return [winstance, FSRWriter]
        #both result in well-formed Lumi tests
        #return [FSRWriter, winstance]

    def _fsrWriter(self,filename=None,writer="RecordStream"):
        '''Configure FSR writer. Since it has odd options, reset them all
        If filename is None, do not attempt to dress it '''

        stype=self.detectStreamType(writer)
        if stype not in ["FSR"]:
            raise TypeError("Expecting a recognised FSR writer, but you have supplied something else")

        #find the writer
        winstance=None

        if type(writer) is str:
            winstance=configurableInstanceFromString(writer)
        else:
            winstance=writer
            writer=writer.getFullName()

        winstance.ItemList = [ "/FileRecords#999" ]
        winstance.EvtDataSvc = "FileRecordDataSvc"
        winstance.EvtConversionSvc = "FileRecordPersistencySvc"

        if filename is None:
            return

        #FSRs have a different output service
        FSRIO=None
        if self._outputPersistency in ["ROOT"]:
            FSRIO=IOHelper(self._outputPersistency,"FSR")
        elif self._inputPersistency in ["ROOT"]:
            FSRIO=IOHelper(self._inputPersistency,"FSR")
        else:
            raise TypeError("Something odd has occurred when setting FSRs")

        winstance.Output = FSRIO.dressFile(filename,"O")
        return winstance

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

    Persistency is then persistency type to use for Root-type files (ROOT)
    the default from IOHelper is maintained if None is passed.

    examples:

    1) building an EventSelector from a list of files of mixed types:
        evtsel=IOExtension().inputFiles(filelist)
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

    _defaultPersistency=_rootTypePersistency=IOHelper._defaultPersistency
    #_rootTypePersistency=IOExtension._defaultPersistency

    _knownExtensions={ 'XDIGI' : '',
                       'DIGI'  : '',
                       'SIM'   : '',
                       'DST'   : '',
                       'RDST'  : '',
                       'XDST'  : '',
                       'LDST'  : '',
                       'MDST'  : '',
                       'FMDST'  : '',
                       'SDST'  : 'Warning',
                       'GEN'   : 'Warning',
                       'XGEN'  : 'Warning',
                       'MDF'   : 'MDF',
                       'RAW'   : 'MDF'
                       }

    def __init__(self, Persistency=None):
        '''
        Initialise with persistency, which is the persistency to use for
        detected root-type files (ROOT)
        '''
        if Persistency is not None:
            #check the persistency is valid
            ioh=IOHelper(Persistency,Persistency)
            #reset own persistency
            self._rootTypePersistency=ioh._inputPersistency

    ###############################################################
    #              Information
    ###############################################################
    def defaultPersistency(self):
        '''Information: return the default persistency'''
        return str(IOExtension._defaultPersistency)

    def rootTypePersistency(self):
        '''Information: return the persistency type which is used for Root-like files'''
        return str(self._rootTypePersistency)

    def knownExtensions(self):
        '''Information: return the list of known file extensions'''
        return self._knownExtensions.keys()

    ###############################################################
    #              Extensions
    ###############################################################

    def detectFileType(self, filename):
        '''
        Extensions: From first the connection string, and second the extension
        determine the type of the file from its name when possible

        returns the string of the file type, or "UNKNOWN" if there is a problem
        '''
        iohtype = IOHelper().detectFileType(filename)
        if iohtype is not "UNKNOWN":
            return iohtype

        filename=IOHelper().undressFile(filename)
        #bugfix, also interpret svcClass correctly for PFNs
        ext=filename.split('.')[-1].strip().split('?')[0].strip().upper()

        if ext not in self._knownExtensions:
            return "UNKNOWN"

        retext=self._rootTypePersistency
        if 'MDF' in self._knownExtensions[ext]:
            retext='MDF'
        if 'Warning' in self._knownExtensions[ext]:
            print "# Warning, guessing type of "+ext+" to be "+retext+" use IOHelper instead to avoid this warning"

        return retext


    def detectMinType(self, files):
        '''Extensions:  from a list of files determine the minimal persistency required
        This will either be MDF or (ROOT)

        If ROOT is ecountered, but IOExtension was initialized with the
        other persistency, a TypeError is raised.

        If any one of the files is ROOT return ROOT
        Only if all the files are MDF return MDF
        '''
        for file in files:
            persistency=self.detectFileType(file)

            if persistency=="UNKNOWN":
                raise TypeError, "Type of file "+file+" could not be determined"+" use IOHelper with specified persistency instead"
            if persistency is not 'MDF':
                if persistency!=self._rootTypePersistency:
                    raise TypeError, "You are trying to parse "+persistency+" file types when you told IOExtension to expect "+self._rootTypePersistency
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

        if type(files) is not list:
            raise TypeError, "You need to pass a list of input files, but you passed a "+str(type(files))+ " instead"

        if eventSelector is None:
            from Gaudi.Configuration import EventSelector
            eventSelector=EventSelector()

        if clear:
            eventSelector.Input=[]

        for file in files:
            eventSelector=self.getIOHelper(file).inputFiles([file],clear=False,eventSelector=eventSelector)

        return eventSelector

    def extensionString(self,setPersistency=False,eventSelector=None):
        '''Input:  return a string of the IOExtension which could be used in a new-style gaudi card
        if setPersistency is True, the default persistency for DST-like files will be specified in the string'''

        if eventSelector is None:
            from Gaudi.Configuration import EventSelector
            eventSelector=EventSelector()


        retstr='from GaudiConf import IOExtension\n'

        files=eventSelector.Input
        if files is not None and type(files) is not list:
            raise TypeError, "The EventSelector does not have a list of input files"
        if not len(files): return retstr+'IOExtension().inputFiles([])\n'

        ioh=self.getIOHelper(files[0])

        if setPersistency:
            retstr+='IOExtension("'+self._rootTypePersistency+'").inputFiles([\n'
        else:
            retstr+="IOExtension().inputFiles([\n"
        alen=4
        for file in files[:-1]:
            retstr+=' '*alen+'"'+ioh.undressFile(file)+'",\n'
        for file in files[-1:]:
            retstr+=' '*alen+'"'+ioh.undressFile(file)+'"\n'
        retstr+=' '*alen+'])'

        return retstr

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


