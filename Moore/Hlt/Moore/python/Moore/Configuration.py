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
from Configurables import LHCbApp, L0Conf, L0DUFromRawAlg

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

class Moore(LHCbConfigurableUser):
    ## Possible used Configurables
    __used_configurables__ = [ HltConf
                             , LHCbApp
                             , L0Conf ]


    __slots__ = {
          "EvtMax":            -1    # Maximum number of events to process
        , "SkipEvents":        0
	, "NbOfSlaves":        0
        , "Simulation":        True # True implies use SimCond
        , "DataType":          '2010' # Data type, can be [ 'DC06','2008' ]
        , "DDDBtag" :          'default' # default as set in DDDBConf for DataType
        , "CondDBtag" :        'default' # default as set in DDDBConf for DataType
        , "outputFile" :       '' # output filename
        , "inputFiles" :       [ ] # input
        , "UseTCK"     :       False # use TCK instead of options...
        , 'ForceSingleL0Configuration' : True # use one single, fixed L0 configuration location (ToolSvc.L0DUConfig)
        , 'SkipDisabledL0Channels' : False # add Hlt1L0xxx even for disabled L0 channels 
        , "L0"         :       False # run L0
        , "ReplaceL0BanksWithEmulated" : False # rerun L0
        , "RunL0Emulator" : False # run L0 emulator for simulation mc production  
        , "CheckOdin"  :       True  # use TCK from ODIN
        , "InitialTCK" :'0x00012009'  # which configuration to use during initialize
        , "prefetchConfigDir" :'MOORE_v8r8'  # which configurations to prefetch.
        , "generateConfig" :   False # whether or not to generate a configuration
        , "configLabel" :      ''    # label for generated configuration
        , "configAlgorithms" : ['Hlt']    # which algorithms to configure (automatically including their children!)...
        , "configServices" :   ['ToolSvc','Hlt::Service','HltANNSvc' ]    # which services to configure (automatically including their dependencies!)...
        , "TCKData" :          '$HLTTCKROOT' # where do we read/write TCK data from/to?
        , "TCKpersistency" :   'tarfile' # which method to use for TCK data? valid is 'file','tarfile' and 'sqlite' 
        , "EnableAuditor" :    [ ]  # put here eg . [ NameAuditor(), ChronoAuditor(), MemoryAuditor() ]
        , "EnableDataOnDemand": False
        , "EnableLumiEventWriting"       : True
        , "EnableTimer" :       True
        , 'EnableRunChangeHandler' : False
        , 'EnableAcceptIfSlow' : False
        , 'WriterRequires' : [ 'HltDecisionSequence' ] # this contains Hlt1 & Hlt2
        , "Verbose" :           True # whether or not to print Hlt sequence
        , "ThresholdSettings" : ''
        , 'RequireL0ForEndSequence'     : False
        , 'SkipHltRawBankOnRejectedEvents' : True
        , 'HistogrammingLevel' : 'Line'
        , 'EnableMonitoring' : False
        , "RunOnline"         : False
        , "RunMonitoringFarm" : False
        , "UseDBSnapshot"     : True
        , "DBSnapshotDirectory" : "/group/online/hlt/conditions"
        , "PartitionName" : "LHCb"
        , 'IgnoreDBHeartBeat'  : False
        , 'TimeOutThreshold'  : 10000  # milliseconds before giving up, and directing event to time out stream
        , 'TimeOutBits'       : 0x200
        , 'RequireRoutingBits' : [] # to require not lumi exclusive, set to [ 0x0, 0x4, 0x0 ]
        , 'VetoRoutingBits'    : []
        , 'REQ1' : ''
        , 'Persistency' :  None #Root or Pool?
        , 'WriteFSR'    :  True #copy FSRs as required

        }   
                

    def _configureDataOnDemand(self) :
        if not self.getProp("EnableDataOnDemand") :
            if 'DataOnDemandSvc' in ApplicationMgr().ExtSvc : 
                ApplicationMgr().ExtSvc.pop('DataOnDemandSvc')
        else: 
            from Configurables import DataOnDemandSvc
            dod = DataOnDemandSvc()
            if dod not in ApplicationMgr().ExtSvc :
                ApplicationMgr().ExtSvc.append( dod ) 
            importOptions('$STDOPTS/DecodeRawEvent.py')
    
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
            if not self.getProp('RunMonitoringFarm'):
                if OnlineEnv.MooreStartupMode == 1:
                    self._configureOnlineForking()
                elif OnlineEnv.MooreStartupMode == 2:
                    self._configureOnlineCheckpointing()
            
            TAE = OnlineEnv.TAE != 0
            input   = 'Events' 
            output  = 'Send'
            mepMgr = OnlineEnv.mepManager(OnlineEnv.PartitionID,OnlineEnv.PartitionName,[input,output],False)
            mepMgr.ConnectWhen = 'start'
            app.Runable = OnlineEnv.evtRunable(mepMgr)
            app.ExtSvc.append(mepMgr)
            evtMerger = OnlineEnv.evtMerger(name='Output',buffer=output,location='DAQ/RawEvent',datatype=OnlineEnv.MDF_NONE,routing=1)
            evtMerger.DataType = OnlineEnv.MDF_BANKS
	    if TAE : eventSelector = OnlineEnv.mbmSelector(input=input, TAE=TAE, decode=False)
            else : eventSelector = OnlineEnv.mbmSelector(input=input, TAE=TAE)
            app.ExtSvc.append(eventSelector)

            OnlineEnv.evtDataSvc()
	    if self.getProp('REQ1') : eventSelector.REQ1 = self.getProp('REQ1')
            
            # define the send sequence
            writer =  GaudiSequencer('SendSequence')
            writer.OutputLevel = OnlineEnv.OutputLevel
            writer.Members = self.getProp('WriterRequires') + [ evtMerger ]
            app.TopAlg.append( writer )
            #app.OutStream.append( writer )
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
        forker.TaskType            = 'GauchoJob'
        forker.UseCores            = False
        forker.ChildSessions       = False
        forker.FirstChild          = 1
        # Sleep in [ms] for each child in batches of 10:
        forker.ChildSleep          = 500;
        forker.UtgidPattern        = "%NN_%T_%02d";
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
        tag = { "DDDB":     self.getProp('DDDBtag')
              , "LHCBCOND": self.getProp('CondDBtag')
              , "SIMCOND" : self.getProp('CondDBtag') 
              , "ONLINE"  : 'fake'
              }

        baseloc = self.getProp( "DBSnapshotDirectory" )

        from Configurables import CondDB
        conddb = CondDB()
        conddb.Tags = tag
        # hack to allow us to chance connectionstrings...
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
        
        if not files: return
        
        persistency=None
        if hasattr(self, "Persistency"):
            if self.getProp("Persistency") is not None:
                persistency=self.getProp("Persistency")
        from GaudiConf import IOExtension
        IOExtension(persistency).inputFiles(files,clear=True)
    
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
        #  do not pick up the pz in vxrypz
        version = re.compile('^version\s*(v\d+r\d+)(p\d+)?')
        for line in fileinput.input(os.environ.get('MOOREROOT')+'/cmt/requirements') :
            match = version.match(line)
            if not match: continue
            fileinput.close()
            return 'MOORE_'+match.group(1)
        raise TypeError('Could not deduce version from cmt/requirementes')

    def getConfigAccessSvc(self):
        method = self.getProp('TCKpersistency').lower()
        if method not in [ 'file', 'sqlite', 'tarfile','zipfile' ] : raise TypeError("invalid TCK persistency '%s'"%method)
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
        ApplicationMgr().OutputLevel              = ERROR
        SequencerTimerTool().OutputLevel          = WARNING
        # Print algorithm name with 40 characters
        MessageSvc().Format = '% F%40W%S%7W%R%T %0W%M'

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
        #L0DUFromRawAlg('L0DUFromRaw').ProcessorDataOnTES = False
        l0du   = L0DUFromRawAlg("L0DUFromRaw")
        #l0du.WriteProcData       = False
        l0du.addTool(L0DUFromRawTool,name = "L0DUFromRawTool")
        l0du = getattr( l0du, 'L0DUFromRawTool' )
        #l0du.FillDataMap         = False
        #l0du.EncodeProcessorData = False
        #l0du.Emulate             = False
        l0du.StatusOnTES         = False

        if ( self.getProp("DataType") == 'DC06' and not self.getProp("L0") ):
            log.warning("It is mandatory to rerun the L0 emulation on DC06 data to get the HLT to work correctly")
            log.warning("Will set ReplaceL0BanksWithEmulated = True")
            self.setProp("ReplaceL0BanksWithEmulated",True)
        if ( self.getProp("ReplaceL0BanksWithEmulated") and not self.getProp("L0") ):
            log.warning("You asked to replace L0 banks with emulation. Will set L0 = True")
            self.setProp("L0",True)
        if ( self.getProp("RunL0Emulator") and not self.getProp("L0") ):
            log.warning("You asked to run the L0 emulator. Will set L0 = True")
            self.setProp("L0",True)    
        if ( self.getProp("ReplaceL0BanksWithEmulated") and self.getProp("RunL0Emulator") ):
            raise TypeError('ReplaceL0BanksWithEmulated is not compatible with RunL0Emulator')
                     
        if ( self.getProp("L0") ):
            l0seq = GaudiSequencer("seqL0")
            ApplicationMgr().TopAlg += [ l0seq ]
            L0TCK = None
            if not self.getProp('UseTCK') :
                from HltConf.ThresholdUtils import Name2Threshold
                L0TCK = Name2Threshold(self.getProp('ThresholdSettings')).L0TCK()
            else  :
                L0TCK = '0x%s' % self.getProp('InitialTCK')[-4:]

            L0Conf().setProp( "TCK", L0TCK )
            L0Conf().setProp( "L0Sequencer", l0seq )
            if ( self.getProp("ReplaceL0BanksWithEmulated") ):
                self.setOtherProps( L0Conf(), [ "ReplaceL0BanksWithEmulated" , "DataType" ] )
                log.info("Will rerun L0")
            if ( self.getProp("RunL0Emulator") ):
                L0Conf().setProp("SimulateL0",True)
                self.setOtherProps( L0Conf(), [ "DataType" ] )
                log.info("Will run L0 emulator")



    def _config_with_hltconf(self):
        """
        Propagate settings to HltConf
        """
        hltConf = HltConf()
        self.setOtherProps( hltConf,  
                            [ 'ThresholdSettings'
                            , 'DataType','Verbose'
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
        if (self.getProp('L0')) :
            if (self.getProp('RunOnline')) : raise RuntimeError('NEVER try to rerun L0 online! -- aborting ')
            from Hlt1Lines.HltL0Candidates import decodeL0Channels
            decodeL0Channels( '0x%04X' % ( int( _tck(self.getProp('InitialTCK') ),16) & 0xFFFF )
                            , skipDisabled               = self.getProp('SkipDisabledL0Channels')
                            , forceSingleL0Configuration = self.getProp('ForceSingleL0Configuration') 
                            )
            # TODO: nasty hack to insure all L0 algorithms request the right type of config provider...
            #       should extend the L0 configurable to support this explicitly
            if self.getProp('ForceSingleL0Configuration') :
                    def _fixL0DUConfigProviderTypes() :
                        from Gaudi.Configuration import allConfigurables
                        for c in allConfigurables.values() :
                            if hasattr(c,'L0DUConfigProviderType') : c.L0DUConfigProviderType = 'L0DUConfigProvider' 
                
                    from Gaudi.Configuration import appendPostConfigAction
                    appendPostConfigAction( _fixL0DUConfigProviderTypes )

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
                self.setProp('CheckOdin',True)
            # determine the partition we run in, and adapt settings accordingly...
            if self.getProp('PartitionName') in [ 'FEST', 'LHCb' ] : 
                self.setProp('InitialTCK',OnlineEnv.InitialTCK)
                self.setProp('CheckOdin',True)


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
        from Configurables import EventClockSvc
        EventClockSvc().EventTimeDecoder = 'OdinTimeDecoder'
        import time
        EventClockSvc().InitialTime = int(time.time()*1e9)  #now

        # make sure we don't pick up small variations of the read current
        # Need a defined HistogramPersistency to read some calibration inputs!!!
        ApplicationMgr().HistogramPersistency = 'ROOT'
        self._outputLevel()

        if self.getProp('UseDBSnapshot') : self._configureDBSnapshot()

        if self.getProp('UseTCK') :
            self._config_with_tck()
        else:
            self._config_with_hltconf()
            
        self._definePersistency()
        
        if self.getProp("RunOnline") :
            self._configureOnline()
        else :
            self._profile()
            if self.getProp("generateConfig") : self._generateConfig()
            self._configureInput()
            self._configureOutput()
