"""
High level configuration tool(s) for Moore
"""
__version__ = "$Id: Configuration.py,v 1.131 2010-09-01 16:39:02 raaij Exp $"
__author__  = "Gerhard Raven <Gerhard.Raven@nikhef.nl>"

from os import environ, path
from LHCbKernel.Configuration import *
from Gaudi.Configuration import *
from Configurables import HltConf
from Configurables import GaudiSequencer
from Configurables import LHCbApp #L0Conf, L0DUFromRawAlg
from Configurables import DecodeRawEvent, RawEventFormatConf

import GaudiKernel.ProcessJobOptions
from  ctypes import c_uint


## HistogramPersistencySvc().OutputFile = 'Moore_minbias.root'

# canonicalize tck  -- eats integer + string, returns canonical string
def _tck(x) :
    if type(x) == str and x[0:2] == '0x' : return '0x%08x'%int(x,16)
    return '0x%08x'%int(x)
    
from Gaudi.Configuration import appendPostConfigAction

def fixDQ():
    from Configurables import CondDBDispatcherSvc
    c = CondDBDispatcherSvc("MainCondDBReader")
    del c.Alternatives['/Conditions/DQ']
#appendPostConfigAction(fixDQ)

def _choose(orig, repl, m):
    if m!=orig:
        return m
    return repl

#Ternary operator not valid for old python versions, replacing...
_replace = lambda orig, repl, members : [ _choose(orig,repl,m) for m in members ]
_remove  = lambda remove,     members : [ m for m in members if m.name() not in remove ]
_remove_re  = lambda re,      members : [ m for m in members if not re.match(m.name()) ]



## FIXME: how to catch HltUnit's RunAll???
def _walkAlgorithms( c , descend = [ 'Members','Prescale','ODIN','L0DU','HLT','Filter0','Filter1','Postscale'] ) :
    for p in descend :
        if not hasattr(c,p) : continue
        x = getattr(c,p)
        import GaudiKernel
        if isinstance(x,GaudiKernel.Configurable.Configurable) : x = [x]
        for i in x :
            for j in _walkAlgorithms(i,descend) : yield j
    yield c
def _updateProperties( top, table, value ) :
    for conf in _walkAlgorithms( top ) :
        prop = table.get( conf.getType().replace(':','_') , None )
        if prop : setattr( conf, prop, value )

def _zipdict(dict1,dict2):
    """
    return a dictionary with all entries of both dict1 and dict2
    if entry is a dictionary, recurse, if entry is a list, append
    """
    retdict={}
    
    for k,v in dict1.iteritems():
        #unique to dict 1
        if k not in dict2:
            retdict[k]=v
            #else they are not unique, I need to decide what to do about that...
        elif type(v) != type(dict2[k]):
            raise TypeError("Cannot zip dicts with identical entries who differ in type! "+str(type(v))+"!="+str(type(dict2[k]))+" for "+str(k))
        elif type(v) is dict: 
            #recurse, transforms are dicts of dicts of dicts...
            retdict[k]=_zipdict(v,dict2[k])
        elif type(v) is list:
            retdict[k]=v+[a for a in dict2[k] if a not in v]
        else:
            raise TypeError("No rule to zip entries of type "+str(type(v))+" for "+str(k))
    
    for k,v in dict2.iteritems():
        #unique to dict 2
        if k not in retdict:
            retdict[k]=v
    
    return retdict

def _mergeTransform(trans):
    """
    Concatenate transforms into the HltConfigSvc
    """
    from Configurables import HltConfigSvc
    prop="ApplyTransformation"
    svc=HltConfigSvc()
    if (svc.getProp(prop) is not None and type(svc.getProp(prop)) is dict and len(svc.getProp(prop))):
        #if it's already set, or the default is some non-zero dictionary, merge it
        svc.setProp(prop,_zipdict(svc.getProp(prop),trans))
    else:
        #else set it
        svc.setProp(prop,trans)

class Moore(LHCbConfigurableUser):
    ## Possible used Configurables
    __used_configurables__ = [ HltConf
                             , LHCbApp
                             #, L0Conf
                             , DecodeRawEvent ]


    __slots__ = {
        #########################################
        # Basic options, used to set LHCbApp
        #########################################
          "EvtMax":            -1    # Maximum number of events to process
        , "SkipEvents":        0
        , "Simulation":        True # True implies use SimCond
        , "DataType":          '2010' # Data type, can be [ 'DC06','2008' ]
        , "DDDBtag" :          'default' # default as set in DDDBConf for DataType
        , "CondDBtag" :        'default' # default as set in DDDBConf for DataType
        , 'Persistency' :  None #Root or Pool?
        , 'WriteFSR'    :  True #copy FSRs as required
        #########################################
        # Mandatory options to consider
        #########################################
        # either run from a threshold setting
        , "ThresholdSettings" : ''
        #or decide how to handle a TCK
        , "UseTCK"     :       False # use TCK instead of options...
        , "InitialTCK" :'0x00012009'  # which configuration to use during initialize
        , "CheckOdin"  :       True  # use TCK from ODIN
        , 'EnableRunChangeHandler' : False #allow run boundaries and first event to reselect the correct TCK
        #########################################
        # Common options, used to configure Moore
        #########################################
        , "outputFile" :       '' # output filename
        , "inputFiles" :       [ ] # input
        , "EnableTimer" :       True
        , "EnableDataOnDemand": False
        , "OutputLevel" : INFO #if this is set to WARNING (or higher) Moore will become almost silent.
                             #if this is set to DEBUG or VERBOSE, this mimics the previous verbose setting
                             #if this is set to INFO no changes to default printout is made
        , 'Split'       : '' # HLT1 or HLT2?
        , "EnableAuditor" :    [ ]  # put here eg . [ NameAuditor(), ChronoAuditor(), MemoryAuditor() ]
        #########################################
        # Options used to make/manipulate TCKs
        #########################################
        , "configLabel" :      ''    # label for generated configuration
        , "configAlgorithms" : ['Hlt']    # which algorithms to configure (automatically including their children!)...
        , "configServices" :   ['ToolSvc','Hlt::Service','HltANNSvc' ]    # which services to configure (automatically including their dependencies!)...
        , "TCKData" :          '$HLTTCKROOT' # where do we read/write TCK data from/to?
        , "TCKpersistency" :   'tarfile' # which method to use for TCK data? valid is 'file','tarfile' and 'sqlite' 
        , "generateConfig" :   False # whether or not to generate a configuration
        #######################################
        # Options nominally for online running
        #######################################
        , "NbOfSlaves":        0
        , "RunOnline"         : False
        , 'EnableMonitoring' : False
        , "RunMonitoringFarm" : False
        , "UseDBSnapshot"     : True
        , "DBSnapshotDirectory" : "/group/online/hlt/conditions"
        , "PartitionName" : "LHCb"
        , 'REQ1' : ''
        #########################################
        # Expert options, only set if you know what you are doing
        #########################################
        , "NbOfSlaves":        0
        , 'ForceSingleL0Configuration' : True # use one single, fixed L0 configuration location (ToolSvc.L0DUConfig)
        , 'SkipDisabledL0Channels' : False # add Hlt1L0xxx even for disabled L0 channels 
        , "prefetchConfigDir" :'MOORE_v8r8'  # which configurations to prefetch.
        , "EnableLumiEventWriting"       : True
        , 'EnableAcceptIfSlow' : False
        , 'WriterRequires' : [ 'HltDecisionSequence' ] # this contains Hlt1 & Hlt2
        , 'RequireL0ForEndSequence'     : False
        , 'SkipHltRawBankOnRejectedEvents' : True
        , 'HistogrammingLevel' : 'Line'
        , 'IgnoreDBHeartBeat'  : False
        , 'TimeOutThreshold'  : 10000  # milliseconds before giving up, and directing event to time out stream
        , 'TimeOutBits'       : 0x200
        , 'RequireRoutingBits' : [] # to require not lumi exclusive, set to [ 0x0, 0x4, 0x0 ]
        , 'VetoRoutingBits'    : []
        #########################################
        # Deprecated former options
        #########################################
        , "L0"         :       False # run L0, deprecated
        , "ReplaceL0BanksWithEmulated" : False # rerun L0, deprecated
        , "RunL0Emulator" : False # run L0 emulator for simulation mc production  , deprecated
        , "Verbose" :           True # whether or not to print Hlt sequence, deprecated, please use OutputLevel
        }
    
    _propertyDocDct={
        #########################################
        # Basic options, used to set LHCbApp
        #########################################
          "EvtMax":   "Maximum number of events to process"
        , "SkipEvents":        "Events to skip, default 0"
        , "Simulation":        "True implies use SimCond"
        , "DataType":          'Data type, can be 2010, 2012, etc.'
        , "DDDBtag" :          'database tag, default as set in DDDBConf for DataType'
        , "CondDBtag" :        'database tag, default as set in DDDBConf for DataType'
        , 'Persistency' :      'Formerly used to decide between Pool and Root, not needed any longer'
        , 'WriteFSR'    :      'copy FSRs if required'
        #########################################
        # Mandatory options to consider
        #########################################
        # either run from a threshold setting
        , "ThresholdSettings" : 'Which threshold setting to use, set either this, or the TCK'
        #or decide how to handle a TCK
        , "UseTCK"     :       "use TCK instead of options. Set either this or ThresholdSettings"
        , "InitialTCK" :       'which configuration to use during initialize. If there is no run change hadler enabled, the this will be the only TCK'
        , "CheckOdin"  :       "use TCK from ODIN, need EnableRunChangeHandler"
        , 'EnableRunChangeHandler' : "allow run boundaries and first event to reselect the correct TCK"
        #########################################
        # Common options, used to configure Moore
        #########################################
        , "outputFile" :       'output filename'
        , "inputFiles" :       "input, can be a simple list of files"
        , "EnableTimer" :      "Turn on the timing table, Warning! it's very very verbose"
        , "EnableDataOnDemand": "Activate the DataOnDemand service, sometimes needed during testing"
        , "OutputLevel" : """Multi-level option, keeps same logic as standard OutputLevel.
        if this is set to WARNING (or higher) Moore will become almost silent.
        if this is set to DEBUG or VERBOSE, this mimics the previous verbose setting
        if this is set to INFO no changes to default printout is made"""
        , 'Split'       : 'HLT1 or HLT2?'
        , "EnableAuditor" :    'put here eg . [ NameAuditor(), ChronoAuditor(), MemoryAuditor() ]'
        #########################################
        # Options used to make/manipulate TCKs
        #########################################
        , "configLabel" :      'label for generated configuration'
        , "configAlgorithms" : 'which algorithms to configure (automatically including their children!)..., default [Hlt]'
        , "configServices" :   "['ToolSvc','Hlt::Service','HltANNSvc' ]    # which services to configure (automatically including their dependencies!)..."
        , "TCKData" :          "'$HLTTCKROOT' # where do we read/write TCK data from/to?"
        , "TCKpersistency" :   "'tarfile' # which method to use for TCK data? valid is 'file','tarfile' and 'sqlite' "
        , "generateConfig" :   "False # whether or not to generate a configuration"
        #######################################
        # Options nominally for online running
        #######################################
        , "NbOfSlaves":        "Only for online running"
        , "RunOnline"         : "Only for online running"
        , 'EnableMonitoring' : "Turn on monitoring"
        , "RunMonitoringFarm" : "Only for online running"
        , "UseDBSnapshot"     : "True for online running, False otherwise"
        , "DBSnapshotDirectory" : "/group/online/hlt/conditions, where to find the DB snapshot"
        , "PartitionName" : "LHCb, only for online running"
        , 'REQ1' : 'Option to evt selector used only in online running'
        #########################################
        # Expert options, only set if you know what you are doing
        #########################################
        , 'ForceSingleL0Configuration' : "use one single, fixed L0 configuration location (ToolSvc.L0DUConfig), shoucl always be set to true"
        , 'SkipDisabledL0Channels' : "False # add Hlt1L0xxx even for disabled L0 channels "
        , "prefetchConfigDir" :"'MOORE_v8r8'  # which configurations to prefetch."
        , "EnableLumiEventWriting"       : "True"
        , 'EnableAcceptIfSlow' : "False"
        , 'WriterRequires' : "[ 'HltDecisionSequence' ] # this contains Hlt1 & Hlt2"
        , 'RequireL0ForEndSequence'     : "False"
        , 'SkipHltRawBankOnRejectedEvents' : "True"
        , 'HistogrammingLevel' : 'Line'
        , 'IgnoreDBHeartBeat'  : "False, needed workaround to avoid certain DB problems"
        , 'TimeOutThreshold'  : "10000  # milliseconds before giving up, and directing event to time out stream"
        , 'TimeOutBits'       : "0x200, unsure"
        , 'RequireRoutingBits' : "[] # to require not lumi exclusive, set to [ 0x0, 0x4, 0x0 ]"
        , 'VetoRoutingBits'    : "[]"
        #########################################
        # Deprecated former options
        #########################################
        , "L0"         :       "False # run L0, deprecated"
        , "ReplaceL0BanksWithEmulated" : "False # rerun L0, deprecated"
        , "RunL0Emulator" : "False # run L0 emulator for simulation mc production  , deprecated"
        , "Verbose" :           "True # whether or not to print Hlt sequence, deprecated, please use OutputLevel"
        }
    
    #for deprecation warnings, a map from deprecated option, to what you should use instead (if anything)
    __deprecated__={"L0":"L0App","ReplaceL0BanksWithEmulated":"L0App","RunL0Emulator":"L0App","Verbose":"Moore().OutputLevel=VERBOSE"}
    
    def _configureDataOnDemand(self) :
        if not self.getProp("EnableDataOnDemand") :
            if 'DataOnDemandSvc' in ApplicationMgr().ExtSvc : 
                ApplicationMgr().ExtSvc.pop('DataOnDemandSvc')
        else: 
            from Configurables import DataOnDemandSvc
            dod = DataOnDemandSvc()
            if dod not in ApplicationMgr().ExtSvc :
                ApplicationMgr().ExtSvc.append( dod ) 
    #        importOptions('$STDOPTS/DecodeRawEvent.py')
    
    def _configureOnline(self) :
        from Configurables import LoKiSvc
        LoKiSvc().Welcome = False
        
        import OnlineEnv

        self.setProp('UseTCK', True)
        self._configureDataOnDemand()
        
        #from Configurables import LHCb__RawDataCnvSvc as RawDataCnvSvc
        #done in LHCbApp
        #EventPersistencySvc().CnvServices.append( RawDataCnvSvc('RawDataCnvSvc') )
        EventLoopMgr().Warnings = False
        
        from Configurables import MonitorSvc
        MonitorSvc().disableDimPropServer      = 1
        MonitorSvc().disableDimCmdServer       = 1
        MonitorSvc().disableMonRate            = 0
        MonitorSvc().CounterUpdateInterval     = 15
        
        app=ApplicationMgr()
        
        # setup the histograms and the monitoring service
        from Configurables import UpdateAndReset
        app.TopAlg = [ UpdateAndReset() ] + app.TopAlg
        app.ExtSvc.append( 'MonitorSvc' ) 
        HistogramPersistencySvc().OutputFile = ''
        HistogramPersistencySvc().Warnings = False
        from Configurables import RootHistCnv__PersSvc
        RootHistCnv__PersSvc().OutputEnabled = False
        
        # set up the event selector
        if 'EventSelector' in allConfigurables : 
            del allConfigurables['EventSelector']
        
        if not self.getProp('RunMonitoringFarm') :
            ## Setup Checkpoint & forking: Do this EXACTLY here. Just befor the MEPManager & event selector.
            ## It will not work if these are created before.
            
            if OnlineEnv.MooreStartupMode == 1:
                self._configureOnlineForking()
            elif OnlineEnv.MooreStartupMode == 2:
                self._configureOnlineCheckpointing()
            
            importOptions('$MBM_SETUP_OPTIONS')
            mbm_setup = allConfigurables['OnlineEnv']
            task_type = os.environ['TASK_TYPE']
            input   = mbm_setup.__getattribute__(task_type+'_Input')   #'Events' 
            output  = mbm_setup.__getattribute__(task_type+'_Output')  #'Send'

            TAE = OnlineEnv.TAE != 0
            mepMgr = OnlineEnv.mepManager(OnlineEnv.PartitionID,OnlineEnv.PartitionName,[input,output],False)
            mepMgr.PartitionBuffers = True
            mepMgr.PartitionName    = OnlineEnv.PartitionName
            mepMgr.PartitionID      = OnlineEnv.PartitionID
            mepMgr.ConnectWhen      = 'start'
            
            app.Runable = OnlineEnv.evtRunable(mepMgr)
            app.ExtSvc.append(mepMgr)
            evtMerger = OnlineEnv.evtMerger(name='Output',buffer=output,location='DAQ/RawEvent',datatype=OnlineEnv.MDF_NONE,routing=1)
            evtMerger.DataType = OnlineEnv.MDF_BANKS
            if TAE : eventSelector = OnlineEnv.mbmSelector(input=input, TAE=TAE, decode=False)
            else   : eventSelector = OnlineEnv.mbmSelector(input=input, TAE=TAE)
            app.ExtSvc.append(eventSelector)

            OnlineEnv.evtDataSvc()
            if self.getProp('REQ1') : eventSelector.REQ1 = self.getProp('REQ1')

            # define the send sequence
            writer =  GaudiSequencer('SendSequence')
            writer.OutputLevel = OnlineEnv.OutputLevel
            writer.Members = self.getProp('WriterRequires') + [ evtMerger ]
            app.TopAlg.append( writer )
        else :
            input = 'Events'
            mepMgr = OnlineEnv.mepManager(OnlineEnv.PartitionID,OnlineEnv.PartitionName,[input],True)
            app.Runable = OnlineEnv.evtRunable(mepMgr)
            app.ExtSvc.append(mepMgr)
            eventSelector = OnlineEnv.mbmSelector(input=input,decode=False)
            app.ExtSvc.append(eventSelector)
            OnlineEnv.evtDataSvc()
            if self.getProp('REQ1') : eventSelector.REQ1 = self.getProp('REQ1')
        
        #ToolSvc.SequencerTimerTool.OutputLevel = @OnlineEnv.OutputLevel;          
        from Configurables import AuditorSvc
        AuditorSvc().Auditors = []
        # Now setup the message service
        self._configureOnlineMessageSvc()
    
    def _configureOnlineForking(self):
        import os, socket, OnlineEnv
        from Configurables import LHCb__CheckpointSvc
        numChildren = os.sysconf('SC_NPROCESSORS_ONLN')
        host = socket.gethostname().split('.')[0].upper()
        #if host[:3]=='HLT':
        #  if host[3]=='F': numChildren=23
        #  if host[3]!='F':
        #    if len(host)==8:
        #      id = int(host[6:])
        #      if id < 12: numChildren=9
        #      if id > 11: numChildren=23
        if self.getProp("NbOfSlaves") > 0 : numChildren = self.getProp("NbOfSlaves")

        forker = LHCb__CheckpointSvc("CheckpointSvc")
        forker.NumberOfInstances   = numChildren
        forker.Partition           = OnlineEnv.PartitionName
        forker.TaskType            = os.environ['TASK_TYPE'] ###'GauchoJob'
        forker.UseCores            = False
        forker.ChildSessions       = False
        forker.FirstChild          = 0
        # Sleep in [ms] for each child in batches of 10:
        forker.ChildSleep          = 500;
        forker.UtgidPattern        = "%P_%NN_%T_%02d";
        forker.PrintLevel          = 3  # 1=MTCP_DEBUG 2=MTCP_INFO 3=MTCP_WARNING 4=MTCP_ERROR
        forker.OutputLevel         = 4  # 1=VERBOSE 2=DEBUG 3=INFO 4=WARNING 5=ERROR 6=FATAL
        ApplicationMgr().ExtSvc.append(forker)
    
    def _configureOnlineCheckpointing(self):
        pass
    
    def _configureOnlineMessageSvc(self):
        # setup the message service
        from Configurables import LHCb__FmcMessageSvc as MessageSvc
        if 'MessageSvc' in allConfigurables :
            del allConfigurables['MessageSvc']
        msg=MessageSvc('MessageSvc')
        app=ApplicationMgr()
        app.MessageSvcType = msg.getType()
        app.SvcOptMapping.append( msg.getFullName() )
        msg.LoggerOnly = True
        if 'LOGFIFO' not in os.environ :
            os.environ['LOGFIFO'] = '/tmp/logGaudi.fifo'
            log.warning( '# WARNING: LOGFIFO was not set -- now set to ' + os.environ['LOGFIFO'] )
        msg.fifoPath = os.environ['LOGFIFO']
        import OnlineEnv 
        msg.OutputLevel = OnlineEnv.OutputLevel
        #msg.OutputLevel = '2'
        msg.doPrintAlways = False
    
    def _configureDBSnapshot(self):
        from Configurables import CondDB
        conddb = CondDB()
        conddb.Tags =  { "DDDB"    : self.getProp('DDDBtag')
                       , "LHCBCOND": self.getProp('CondDBtag')
                       , "SIMCOND" : self.getProp('CondDBtag') 
                       , "ONLINE"  : 'fake'
                       }
        conddb.setProp('IgnoreHeartBeat',self.getProp('IgnoreDBHeartBeat')  )
        self.setOtherProps( conddb, [ 'UseDBSnapshot',
                                      'DBSnapshotDirectory',
                                      'PartitionName',
                                      'EnableRunChangeHandler'])
                
        # https://savannah.cern.ch/bugs/?94454#comment12
        from Configurables import MagneticFieldSvc
        MagneticFieldSvc().UseSetCurrent = True

    def _configureInput(self):
        files = self.getProp('inputFiles')
        #    #  veto lumi events..
        #    #ApplicationMgr().EvtSel.REQ1 = "EvType=2;TriggerMask=0x0,0x4,0x0,0x0;VetoMask=0,0,0,0;MaskType=ANY;UserType=USER;Frequency=PERC;Perc=100.0"
        self._configureDataOnDemand()
        
        if not files:
            return
        
        persistency=None
        if hasattr(self, "Persistency"):
            if self.getProp("Persistency") is not None:
                persistency=self.getProp("Persistency")
        from GaudiConf import IOExtension
        IOExtension(persistency).inputFiles(files,clear=True)
        
    def _setRawEventLocations(self):
        """
        Check that I can set DecodeRawEvent.py options correctly.
        """
        #if not set, I will override the inputs with the "Pit locations"
        if (not DecodeRawEvent().isPropertySet("OverrideInputs")) or DecodeRawEvent().getProp("OverrideInputs") is None:
            #default version which comes out of the Pit,
            #currently just DAQ/RawEvent
            DecodeRawEvent().OverrideInputs="Pit" 
            return
        from RawEventCompat.Configuration import _checkv
        from Configurables import RawEventFormatConf
        RawEventFormatConf().loadIfRequired()
        #if set explicitly to the pit locations, all is good already
        if _checkv(DecodeRawEvent().getProp("OverrideInputs"))==_checkv("Pit"):
            return
        #else it's a screw-up
        if self.getProp("RunOnline"):
            raise ValueError("When running in Online Mode, you'd better not try and reset the RawEventLocations, this is baaaad")
        if self.getProp("UseTCK"):
            raise ValueError("You are trying to reset RawEvent inputs/outputs when running from a TCK, this won't work because they are fixed inside the TCK anyway! (they're part of the Hlt sequence), try adding RecombineRawEvent() or RawEventJuggler() instead, or some other such trick.")
        if self.getProp("generateConfig"):
            raise ValueError("When generating a TCK, you'd better not be trying to overwrite the RawEvent input/output locations, this would be a disaster!, try adding RecombineRawEvent() or RawEventJuggler() instead or some other such trick.")
        # if input is MDF, RAW, DIGI, XDIGI, then raise an error,
        # these locations are always "DAQ/RawEvent"
        files = self.getProp('inputFiles')
        if files is None or not len(files):
            return
        
        ext=files[0].split('.')[-1].strip().split('?')[0].strip().upper()
        if ext in ["MDF","RAW","DIGI","XDIGI"]:
            raise ValueError("When running from a DIGI, XDIGI or RAW file, the only raw event location is DAQ/RawEvent, but you're resetting it into"+DecodeRawEvent().getProp("OverrideInputs"))
    
    def _configureOutput(self):
        fname = self.getProp('outputFile')
        if not fname : return
        writer = None
        
        #retrieve the persistency
        persistency=None
        if hasattr(self, "Persistency"):
            if self.getProp("Persistency") is not None:
                persistency=self.getProp("Persistency")
        from GaudiConf import IOExtension, IOHelper
        iox=IOExtension(persistency)
        
        #check the file type and use MDF writer or InputCopyStream
        if iox.detectFileType(fname) == 'MDF'  : 
            from Configurables import LHCb__MDFWriter as MDFWriter
            writer = IOHelper("MDF","MDF").outputAlgs(fname
                                                     ,writer = MDFWriter( 'Writer' , Compress = 0 )
                                                     ,writeFSR=False)
            if self.getProp('WriterRequires') :
                from Configurables import LoKi__VoidFilter as VoidFilter
                writer = GaudiSequencer( 'WriteSequence'
                                       , Members = [ VoidFilter( "WriterFilter" 
                                                               , Preambulo = [ 'from LoKiHlt.algorithms import ALG_EXECUTED, ALG_PASSED' ]
                                                               , Code = ' & '.join( [ "ALG_EXECUTED('%s') & ALG_PASSED('%s')" % (i,i) for i in self.getProp('WriterRequires') ] ) 
                                                               )
                                                   ] + writer
                                       )
            ApplicationMgr().OutStream.append( writer )
        else : 
            from Configurables import InputCopyStream
            writer = InputCopyStream("Writer"
                                    , RequireAlgs = self.getProp('WriterRequires')
                                    )
            IOHelper(persistency,persistency).outStream(fname,writer,writeFSR=self.getProp('WriteFSR'))
        

    def getRelease(self):
        import re,fileinput
        #  do NOT pick up the pz in vxrypz
        version = re.compile('^version\s*(v\d+r\d+)(p\d+)?')
        fname = os.environ.get('MOORESYSROOT')+'/cmt/requirements'
        for line in fileinput.input(fname) :
            match = version.match(line)
            if not match: continue
            fileinput.close()
            return 'MOORE_'+match.group(1)
        raise ValueError('Could not deduce MOORE (base)version from %s',fname)

    def getConfigAccessSvc(self):
        method  = self.getProp('TCKpersistency').lower()
        TCKData = self.getProp('TCKData')
        if method == 'file' :
            from Configurables import ConfigFileAccessSvc
            return ConfigFileAccessSvc( Directory = TCKData +'/config' )
        if method == 'sqlite' :
            from Configurables import ConfigDBAccessSvc
            return ConfigDBAccessSvc( Connection = 'sqlite_file:' + TCKData +'/db/config.db' )
        if method == 'tarfile' :
            from Configurables import ConfigTarFileAccessSvc
            return ConfigTarFileAccessSvc( File = TCKData +'/config.tar' )
        if method == 'zipfile' :
            from Configurables import ConfigZipFileAccessSvc
            return ConfigZipFileAccessSvc( File = TCKData +'/config.zip' )
        raise TypeError("invalid TCK persistency '%s'"%method)

    def addAuditor(self,x) :
        if  'AuditorSvc' not in ApplicationMgr().ExtSvc : 
            ApplicationMgr().ExtSvc.append( 'AuditorSvc' ) 
        AuditorSvc().Auditors.append( x )
        x.Enable = True

    def _outputLevel(self) :
        from Configurables import Hlt__Service
        if not Hlt__Service().isPropertySet('Pedantic') : Hlt__Service().Pedantic = False
        # output levels...
        ToolSvc().OutputLevel                     = INFO
        from Configurables import XmlParserSvc 
        XmlParserSvc().OutputLevel                = WARNING
        MessageSvc().OutputLevel                  = INFO
        ApplicationMgr().OutputLevel              = INFO #I still want the Application Manager Finalized Sucessfully printout
        SequencerTimerTool().OutputLevel          = WARNING
        # Print algorithm name with 40 characters
        MessageSvc().Format = '% F%40W%S%7W%R%T %0W%M'
        
        if self.getProp("OutputLevel")>INFO:
            level=self.getProp("OutputLevel")
            MessageSvc().OutputLevel = level
            ToolSvc().OutputLevel = level
            if self.isPropertySet("EnableTimer") and self.getProp("EnableTimer"):
                raise AttributeError("Timing table is very far from silent, please disable timing if you want to run with lower verbosity")
            self.setProp("EnableTimer",False)
            from Configurables import LoKiSvc
            LoKiSvc().Welcome = False
            #################################################
            # Running from thresholds, use post config action
            #################################################
            if not self.getProp("UseTCK"):
                #post config to really reset all the output to null
                from DAQSys.Decoders import DecoderDB
                from GaudiConf.Manipulations import postConfForAll#,fullNameConfigurables
                props={"StatPrint":False,
                       "ErrorsPrint":False,
                       "PropertiesPrint":False,
                       "OutputLevel":level
                       }
                from DAQSys.Decoders import DecoderDB
                for k,v in DecoderDB.iteritems():
                    for pk,pv in props.iteritems():
                        v.Properties[pk]=pv
                #only for GaudiHistoAlgs...
                props["HistoCountersPrint"]=False
                postConfForAll(head=None, prop_value_dict=props,force=True)
                #now turn off the calo tool finalize printout
                tools={"CaloECorrection/ECorrection":{"OutputLevel":level},
                       "CaloSCorrection/SCorrection":{"OutputLevel":level},
                       "CaloLCorrection/LCorrection":{"OutputLevel":level}
                       }
                postConfForAll(head=None, prop_value_dict={},types=["CaloSinglePhotonAlg","CaloElectronAlg","CaloMergedPi0Alg"],force=True,tool_value_dict=tools)
                #three extras for merged pi0
                tools={"CaloCorrectionBase/ShowerProfile":{"OutputLevel":level},
                       "CaloCorrectionBase/Pi0SCorrection":{"OutputLevel":level},
                       "CaloCorrectionBase/Pi0LCorrection":{"OutputLevel":level}
                       }
                postConfForAll(head=None, prop_value_dict={},types=["CaloMergedPi0Alg"],force=True,tool_value_dict=tools)
                #I still want to print "Application Manager Finalized Successfully"
                #and "End of event input reached"
                def AppMrgOP():
                    ApplicationMgr().OutputLevel=INFO
                    EventSelector().OutputLevel=INFO
                appendPostConfigAction(AppMrgOP)
                #################################################
                # Running from TCK define a similar transform
                #################################################
            else:
                trans={".*":{"OutputLevel"        : {"^.*$":str(level)}
                             ,"StatPrint"         : {"^.*$":'False'}
                             ,"ErrorsPrint"       : {"^.*$":'False'}
                             ,"PropertiesPrint"   : {"^.*$":'False'}
                             ,"HistoCountersPrint": {"^.*$":'False'}
                             }
                       }
                _mergeTransform(trans)
                from Configurables import HltConfigSvc
                cfg = HltConfigSvc()
                #self-defeating warnings!
                cfg.OutputLevel=ERROR
                
    
    def _profile(self) :
        ApplicationMgr().AuditAlgorithms = 1
        auditors = self.getProp('EnableAuditor')
        if self.getProp('EnableTimer') : 
            from Configurables import TimingAuditor
            auditors = [ TimingAuditor('TIMER') ] + auditors
        for i in auditors : self.addAuditor( i )

    def _generateConfig(self) :
        from HltConf.ThresholdUtils import Name2Threshold
        settings = Name2Threshold(self.getProp('ThresholdSettings'))
        svcs = self.getProp("configServices")
        algs = self.getProp("configAlgorithms")
        if self.getProp('TCKpersistency').lower() == 'tarfile' :
            self.getConfigAccessSvc().Mode = 'ReadWrite'
            self.getConfigAccessSvc().OutputLevel = 1

        from Configurables import HltGenConfig
        print 'requesting following  svcs: %s ' % svcs
        gen = HltGenConfig( ConfigTop = [ i.rsplit('/')[-1] for i in algs ]
                          , ConfigSvc = [ i.rsplit('/')[-1] for i in svcs ]
                          , ConfigAccessSvc = self.getConfigAccessSvc().getName()
                          , HltType = settings.HltType()
                          , MooreRelease = self.getRelease()
                          , Label = self.getProp('configLabel'))
        # make sure gen is the very first Top algorithm...
        from HltLine.HltDecodeRaw import DecodeODIN
        ApplicationMgr().TopAlg = DecodeODIN.members() + [ gen.getFullName() ] + ApplicationMgr().TopAlg
        def genConfigAction() :
            def gather( c, overrule ) :
                    def check(config,prop,value) :
                        if prop not in config.getDefaultProperties() : return False
                        if hasattr(config,prop) : return getattr(config,prop) == value 
                        return config.getDefaultProperties()[prop] == value
                    def addOverrule(config,rule):
                        if c.name() not in overrule.keys() :
                           overrule[c.name()] = []     
                        if rule not in overrule[c.name()] :
                           overrule[c.name()] += [ rule ]

                    if check(c,'HistoProduce',False) :
                        addOverrule(c,'HistoProduce:@OnlineEnv.Monitor@False')
                    if c.getType() in ['FilterDesktop','CombineParticles' ] and  check(c,'Monitor',False) :
                        addOverrule(c,'Monitor:@OnlineEnv.Monitor@False')
                    if check(c,'Enable',False) :
                        addOverrule(c,'OutputLevel:3')
                    for p in [ 'Members','Filter0','Filter1' ] :
                        if not hasattr(c,p) : continue
                        x = getattr(c,p)
                        if list is not type(x) : x = [ x ]
                        for i in x : gather(i,overrule) 
            from Configurables import HltGenConfig,GaudiSequencer
            HltGenConfig().Overrule = { 'Hlt1ODINTechnicalPreScaler' : [ 'AcceptFraction:@OnlineEnv.AcceptRate@0' ] }
            gather( GaudiSequencer('Hlt'), HltGenConfig().Overrule )
            print HltGenConfig()
        from Gaudi.Configuration import appendPostConfigAction
        appendPostConfigAction( genConfigAction )

    def _l0(self) :
        from Configurables import L0DUFromRawTool
        from DAQSys.Decoders import DecoderDB
        #L0DUFromRawAlg('L0DUFromRaw').ProcessorDataOnTES = False
        l0dutool   = DecoderDB["L0DUFromRawTool"]
        l0dutool.Properties["StatusOnTES"]=False
        DecoderDB["L0DUFromRawAlg/L0DUFromRaw"].setup()
        
        # TODO: nasty hack to insure all L0 algorithms request the right type of config provider...
        #       should extend the L0 configurable to support this explicitly
        if self.getProp('ForceSingleL0Configuration') :
            def _fixL0DUConfigProviderTypes() :
                from Gaudi.Configuration import allConfigurables
                for c in allConfigurables.values() :
                    if hasattr(c,'L0DUConfigProviderType') : c.L0DUConfigProviderType = 'L0DUConfigProvider' 
                
            from Gaudi.Configuration import appendPostConfigAction
            appendPostConfigAction( _fixL0DUConfigProviderTypes )
        


    def _config_with_hltconf(self):
        """
        Propagate settings to HltConf
        """
        hltConf = HltConf()
        self.setOtherProps( hltConf,  
                            [ 'ThresholdSettings'
                            , 'DataType'
                            , 'RequireL0ForEndSequence'
                            , 'SkipHltRawBankOnRejectedEvents'
                            , 'HistogrammingLevel' 
                            , 'EnableMonitoring'
                            , "EnableLumiEventWriting"
                            , "EnableAcceptIfSlow"
                            , 'ForceSingleL0Configuration'
                            , 'SkipDisabledL0Channels'
                            , 'RequireRoutingBits'
                            , 'VetoRoutingBits' 
                            ]
                          )
        if self.getProp("OutputLevel")<INFO:
            hltConf.setProp("Verbose",True)
        
        if self.getProp("Simulation") is True:
            hltConf.setProp("EnableHltGlobalMonitor",False)
            hltConf.setProp("EnableBeetleSyncMonitor",False)
            hltConf.setProp("EnableHltL0GlobalMonitor",False)                     
            
    def _config_with_tck(self):
        from Configurables import HltConfigSvc
        cfg = HltConfigSvc( prefetchDir = self.getProp('prefetchConfigDir')
                          , initialTCK =  _tck(self.getProp('InitialTCK'))
                          , checkOdin = self.getProp('CheckOdin')
                          , ConfigAccessSvc = self.getConfigAccessSvc().getFullName() ) 
        # TODO: make sure we are the first one...
        ApplicationMgr().ExtSvc.append(cfg.getFullName())
        # configure services...
        VFSSvc().FileAccessTools = ['FileReadTool', 'CondDBEntityResolver/CondDBEntityResolver'];
        from Configurables import LHCb__ParticlePropertySvc
        LHCb__ParticlePropertySvc().ParticlePropertiesFile = 'conddb:///param/ParticleTable.txt';

    def _definePersistency(self):
        
        #configure persistency services
        persistency=None

        #online, do the minimum possible, of only setting up MDF
        if self.getProp("RunOnline") :
            persistency="MDF"
        else:
            #offline is more complicated, depending on the options
            if hasattr(self, "Persistency"):
                if self.getProp("Persistency") is not None:
                    persistency=self.getProp("Persistency")
            
            
            #could use IOExtension to determine the minimum persistency.
            #but then GaudiCards would be problematic!
            #from GaudiConf import IOExtension
            #iox=IOExtension(persistency)
            #minpersistency=iox.detectMinType([self.getProp("outputFile")]+self.getProp("inputFiles"))
            #
            #if minpersistency=="MDF":
            #    persistency="MDF"
        
        if persistency is not None:
            LHCbApp().setProp("Persistency",persistency)

    def _split(self, useTCK ): 
        if not self.getProp('Split') : return
        def hlt1_only() :
            from Configurables import GaudiSequencer as gs
            seq = gs('Hlt')
            seq.Members = _replace( gs('HltDecisionSequence'), gs('Hlt1'), seq.Members )
            ## adapt HltGlobalMonitor for Hlt1 only...
            from Configurables import HltGlobalMonitor
            HltGlobalMonitor().DecToGroupHlt2 = {}
            ##

        def hlt1_only_tck() :
            trans = { 'GaudiSequencer/Hlt$' :               { 'Members' : { 'HltDecisionSequence' : 'Hlt1' } } 
                      , 'HltGlobalMonitor/HltGlobalMonitor' : { 'DecToGroupHlt2' : { '^.*$' : "{  }" } }
                      }
            _mergeTransform(trans)

        def hlt2_only() :
            from Configurables import GaudiSequencer as gs
            seq = gs('Hlt')
            seq.Members = _replace( gs('HltDecisionSequence'), gs('Hlt2'), seq.Members )
            # TODO: shunt lumi nano events...
            # globally prepend a HltDecReportsDecoder for Hlt1...
            # TODO: find a better way of doing this... ditto for L0 decoding...
            from DAQSys.Decoders import DecoderDB
            dec=DecoderDB["HltDecReportsDecoder/Hlt1DecReportsDecoder"]
            decAlg=dec.setup()
            seq.Members.insert( seq.Members.index(gs('Hlt2')), decAlg )
            sel=DecoderDB["HltSelReportsDecoder/Hlt1SelReportsDecoder"]
            selAlg=sel.setup()
            seq.Members.insert( seq.Members.index(gs('Hlt2')), selAlg )
            # TODO: replace Hlt1 filter in endsequence by Hlt2 filter...
            # remove LumuWriter, LumiStripper
            end = gs('HltEndSequence')
            end.Members = _remove( ( 'HltL0GlobalMonitor','Hlt1Global','HltLumiWriter','LumiStripper'), end.Members )

            ## adapt HltGlobalMonitor for Hlt2 only...
            from Configurables import HltGlobalMonitor
            HltGlobalMonitor().DecToGroupHlt1 = {}

            # shunt Hlt1 decreports
            _updateProperties( gs('Hlt')
                             , dict( LoKi__HDRFilter      = 'Location'
                                   , TisTosParticleTagger = 'HltDecReportsInputLocation'
                                   , HltRoutingBitsWriter = 'Hlt1DecReportsLocation'
                                   )
                             , dec.listOutputs()[0]
                             )

            # shunt Hlt2 decreports
            dec2=DecoderDB["HltDecReportsDecoder/Hlt2DecReportsDecoder"]
            _updateProperties( gs('Hlt')
                             , dict( Hlt__Line            = 'HltDecReportsLocation'
                                   , HltRoutingBitsWriter = 'Hlt2DecReportsLocation'
                                   , HltDecReportsWriter  = 'InputHltDecReportsLocation'
                                   , HltSelReportsMaker   = 'InputHltDecReportsLocation'
                                   , HltGlobalMonitor     = 'HltDecReports'
                                   )
                             , dec2.listOutputs()[0]
                             )

            # shunt selreports
            dec3=DecoderDB["HltSelReportsDecoder/Hlt2SelReportsDecoder"]
            _updateProperties( gs('Hlt')
                             , dict( HltSelReportsMaker   = 'OutputHltSelReportsLocation'
                                   , HltSelReportsWriter  = 'InputHltSelReportsLocation' )
                             ,  dec3.listOutputs()[0]
                             )
           
            _updateProperties( gs('Hlt')
                             , dict( TisTosParticleTagger = 'PassOnAll' )
                             , 'True'
                             )


        def hlt2_only_tck() :
            from DAQSys.Decoders import DecoderDB
            hlt1decoder_name="HltDecReportsDecoder/Hlt1DecReportsDecoder"
            dec=DecoderDB[hlt1decoder_name]
            decAlg=dec.setup()
            dec2=DecoderDB["HltDecReportsDecoder/Hlt2DecReportsDecoder"]
            dec3=DecoderDB["HltSelReportsDecoder/Hlt2SelReportsDecoder"]
            hlt1seloder_name="HltSelReportsDecoder/Hlt1SelReportsDecoder"
            dec4=DecoderDB[hlt1seloder_name]
            dec4Alg=dec4.setup()
            hlt1decrep_location = dec.listOutputs()[0]
            hlt2decrep_location = dec2.listOutputs()[0]
            hlt2selrep_location = dec3.listOutputs()[0]
            trans = { 'GaudiSequencer/Hlt$' :               { 'Members' : { 'GaudiSequencer/HltDecisionSequence' : hlt1decoder_name+"', '"+hlt1seloder_name+"', 'GaudiSequencer/Hlt2"  } }#is this OK?
                      , 'GaudiSequencer/HltEndSequence' :     { 'Members' : { ", '.*/HltL0GlobalMonitor'" : '' 
                                                                              , ", '.*/Hlt1Global'"         : ''
                                                                              , ", '.*/HltLumiWriter'"      : ''
                                                                              , ", '.*/LumiStripper'"       : '' } }
                      , 'HltGlobalMonitor/HltGlobalMonitor' : { 'DecToGroupHlt1'             : { '^.*$' : '{ }'               } }
                      , '.*HDRFilter/.*'                    : { 'Location'                   : { '^.*$' : hlt1decrep_location } }
                      , 'TisTosParticleTagger/.*'           : { 'HltDecReportsInputLocation' : { '^.*$' : hlt1decrep_location } 
                                                                , 'PassOnAll'                  : { '^.*$' : 'True'              }
                                                                # , 'TriggerTisTosName'          : { '^.*$' : foobar              }
                                                                              }
                      , '.*/HltRoutingBitsWriter'           : { 'Hlt1DecReportsLocation'     : { '^.*$' : hlt1decrep_location } 
                                                                , 'Hlt2DecReportsLocation'     : { '^.*$' : hlt2decrep_location } }
                      , 'Hlt::Line/.*'                      : { 'HltDecReportsLocation'      : { '^.*$' : hlt2decrep_location } }
                      , 'HltDecReportsWriter/.*'            : { 'InputHltDecReportsLocation' : { '^.*$' : hlt2decrep_location } }
                      , 'HltSelReportsMaker/.*'             : { 'InputHltDecReportsLocation' : { '^.*$' : hlt2decrep_location } }
                      , 'HltGlobalMonitor/.*'               : { 'HltDecReports'              : { '^.*$' : hlt2decrep_location } }
                      , 'HltSelReportsMaker/.*'             : { 'OutputHltSelReportsLocation': { '^.*$' : hlt2selrep_location } }
                      , 'HltSelReportsWriter/.*'            : { 'InputHltSelReportsLocation' : { '^.*$' : hlt2selrep_location } }
                      }
            _mergeTransform(trans)

        def hlt1hlt2() :
            from Configurables import GaudiSequencer as gs
            _updateProperties( gs('Hlt2') , dict( TisTosParticleTagger = 'PassOnAll' ) , True)

        def hlt1hlt2_tck() : 
            trans = { 'TisTosParticleTagger/.*' : { 'PassOnAll' : { '^.*$' : 'True' } } }
            _mergeTransform(trans)
        
        # rather nasty way of doing this.. but it is 'hidden' 
        # if you're reading this: don't expect this to remain like this!!!
        try : 
            if useTCK :
                splitter = { 'Hlt1'     : hlt1_only_tck 
                           , 'Hlt2'     : hlt2_only_tck
                           , 'Hlt1Hlt2' : hlt1hlt2_tck }
                action = splitter[ self.getProp('Split') ]
                if action : action()
            else :
                splitter = { 'Hlt1'     : hlt1_only 
                           , 'Hlt2'     : hlt2_only
                           , 'Hlt1Hlt2' : hlt1hlt2 }
                action = splitter[ self.getProp('Split') ]
                if action :
                    from Gaudi.Configuration import appendPostConfigAction
                    appendPostConfigAction( action )
        except :
            raise ValueError("Invalid option for Moore().Split: '%s'"% self.getProp("Split") )



    def _setIfNotSet(self,prop,value) :
        if not self.isPropertySet(prop) : self.setProp(prop,value)
        return self.getProp(prop) == value

    def __apply_configuration__(self):
        GaudiKernel.ProcessJobOptions.PrintOff()
        # verify mutually exclusive settings:
        #  eg.  Online vs. any L0 setting
        #       Online vs. generateConfig
        #       Online vs. DB tags...
        #       Online vs. EvtMax, SkipEvents, DataType, ...
        #       Online requires UseTCK
        # L0 decoding to look in a single place  
        # L0Conf().RawEventLocations = ['DAQ/RawEvent']        
        #L0DUFromRawAlg("L0DUFromRaw").Hlt1 = True 
        for prop in self.getProperties():
            if prop in self.__deprecated__:
                if self.isPropertySet(prop):
                    warningprint="You are trying to set the deprecated property " + prop
                    if self.__deprecated__[prop] is not None and len(self.__deprecated__[prop].strip()):
                        warningprint=warningprint+" please use "+self.__deprecated__[prop]+" instead"
                    raise AttributeError(warningprint)
        #turn off LoKi::Distance print outs, which are very frequent!
        #todo: put this in a "quiet" option of Moore
        from Configurables import LoKi__DistanceCalculator
        LoKi__DistanceCalculator().MaxPrints=0

        if not self.getProp("RunOnline") : self._l0()
        if self.getProp("RunOnline") : 
            import OnlineEnv 
            self.setProp('EnableTimer',False)
            self.setProp('UseTCK',True)
            self.setProp('Simulation',False)
            self.setProp('DataType','2012' )
            ### TODO: see if 'OnlineEnv' has InitialTCK attibute. If so, set it
            ## in case of LHCb or FEST, _REQUIRE_ it exists...
            if hasattr(OnlineEnv,'InitialTCK') :
                self.setProp('InitialTCK',OnlineEnv.InitialTCK)
                if not self._setIfNotSet('CheckOdin',True) :
                    print 'WARNING '*10
                    print 'WARNING: you are running online, using a TCK, but ignoring ODIN. Are you sure this is really what you want???'
                    print 'WARNING '*10
                      
            # determine the partition we run in, and adapt settings accordingly...
            if self.getProp('PartitionName') in [ 'FEST', 'LHCb' ] : 
                self.setProp('InitialTCK',OnlineEnv.InitialTCK)
                if not self._setIfNotSet('CheckOdin',True) :
                    print 'WARNING '*10
                    print 'WARNING: you are running in the %s partition, using a TCK, but ignoring ODIN. Make sure this is really what you want.' % self.getProp('PartitionName')
                    print 'WARNING '*10


        from Configurables import MooreInitSvc
        ApplicationMgr().ExtSvc.append( MooreInitSvc() )
        #from Configurables import LbAppInit
        #ApplicationMgr().TopAlg.append(LbAppInit(PreloadGeometry=True))
        ApplicationMgr().TopAlg.append( GaudiSequencer('Hlt') )


        # forward some settings... 
        # WARNING: this triggers setup of /dd -- could be avoided in PA only mode...
        app = LHCbApp()
        self.setOtherProps( app, ['EvtMax','SkipEvents','Simulation', 'DataType' ] )
        
        # this was a hack. Someone thought setOtherProps did not work?
        #app.CondDBtag = self.getProp('CondDBtag')
        #app.DDDBtag   = self.getProp('DDDBtag')
        self.setOtherProps( app, ['CondDBtag','DDDBtag'])
        # Get the event time (for CondDb) from ODIN
        from DAQSys.Configuration import SetEvtClock
        SetEvtClock("ODIN")
        from Configurables import EventClockSvc
        import time
        EventClockSvc().InitialTime = int(time.time()*1e9)  #now
        
        # make sure we don't pick up small variations of the read current
        # Need a defined HistogramPersistency to read some calibration inputs!!!
        ApplicationMgr().HistogramPersistency = 'ROOT'
        self._outputLevel()
        
        #set the decoders to read from the default location
        self._setRawEventLocations()
        
        if self.getProp('UseDBSnapshot') : self._configureDBSnapshot()
        
        if self.getProp('UseTCK') :
            self._config_with_tck()
            self._split( useTCK = True )
        else:
            self._config_with_hltconf()
            self._split( useTCK = False  )
            
        self._definePersistency()
        
        if self.getProp("RunOnline") :
            self._configureOnline()
        else :
            self._profile()
            if self.getProp("generateConfig") : self._generateConfig()
            self._configureInput()
            self._configureOutput()
