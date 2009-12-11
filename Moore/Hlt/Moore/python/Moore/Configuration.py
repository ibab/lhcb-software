"""
High level configuration tool(s) for Moore
"""
__version__ = "$Id: Configuration.py,v 1.99 2009-12-11 19:09:45 graven Exp $"
__author__  = "Gerhard Raven <Gerhard.Raven@nikhef.nl>"

from os import environ, path
from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
from Configurables import HltConf
from Configurables import GaudiSequencer
from Configurables import LHCbApp, L0Conf

import GaudiKernel.ProcessJobOptions
from  ctypes import c_uint


## HistogramPersistencySvc().OutputFile = 'Moore_minbias.root'

def _ext(name) : 
    x =  path.splitext(name)[-1].lstrip('.').upper()
    if x == 'MDF' : x = 'RAW'
    return x

def _datafmt(fn) : 
    pfn = 'PFN:%s' % fn if fn.find(':') == -1  else fn
    fmt = { 'RAW' : "DATAFILE='%s' SVC='LHCb::MDFSelector'"
          , 'DST' : "DATAFILE='%s' TYP='POOL_ROOTTREE' OPT='READ'" 
          }
    return fmt[ _ext(pfn) ] % pfn

# canonicalize tck  -- eats integer + string, returns canonical string
def _tck(x) :
    if type(x) == str and x[0:2] == '0x' : return '0x%08x'%int(x,16)
    return '0x%08x'%int(x)

class Moore(LHCbConfigurableUser):
    ## Possible used Configurables
    __used_configurables__ = [ HltConf
                             , LHCbApp
                             , L0Conf ]


    __slots__ = {
          "EvtMax":            -1    # Maximum number of events to process
        , "SkipEvents":        0
        , "Simulation":        True # True implies use SimCond
        , "DataType":          '2009' # Data type, can be [ 'DC06','2008' ]
        , "DDDBtag" :          'default' # default as set in DDDBConf for DataType
        , "CondDBtag" :        'default' # default as set in DDDBConf for DataType
        , "outputFile" :       '' # output filename
        , "inputFiles" :       [ ] # input
        , "UseTCK"     :       False # use TCK instead of options...
        , "L0"         :       False # run L0
        , "ReplaceL0BanksWithEmulated" : False # rerun L0
        , "CheckOdin"  :       False  # use TCK from ODIN
        , "InitialTCK" :'0x00012009'  # which configuration to use during initialize
        , "prefetchConfigDir" :'MOORE_v8r2'  # which configurations to prefetch.
        , "generateConfig" :   False # whether or not to generate a configuration
        , "configLabel" :      ''    # label for generated configuration
        , "configAlgorithms" : ['Hlt']    # which algorithms to configure (automatically including their children!)...
        , "configServices" :   ['ToolSvc','HltDataSvc','HltANNSvc' ]    # which services to configure (automatically including their dependencies!)...
        , "TCKData" :          '$HLTTCKROOT' # where do we read/write TCK data from/to?
        , "TCKpersistency" :   'tarfile' # which method to use for TCK data? valid is 'file','tarfile' and 'sqlite' 
        , "EnableAuditor" :    [ ]  # put here eg . [ NameAuditor(), ChronoAuditor(), MemoryAuditor() ]
        , "EnableDataOnDemand": False
        , "EnableLumiEventWriting"       : True
        , "EnableTimer" :       True
        , 'EnableRunChangeHandler' : False
        , 'EnableAcceptIfSlow' : True
        , 'WriterRequires' : [ 'Hlt1Global' ]
        , "Verbose" :           True # whether or not to print Hlt sequence
        , "ThresholdSettings" : ''
        , 'RequireL0ForEndSequence'     : False
        , 'SkipHltRawBankOnRejectedEvents' : True
        , 'HistogrammingLevel' : 'Line'
        , "RunOnline"         : False
        , "UseDBSnapshot"     : True
        , "DBSnapshotDirectory" : "/group/online/hlt/conditions"
        , 'EnableTimeOutCatcher' : True
        , 'TimeOutThreshold'  : 10000  # milliseconds before giving up, and directing event to time out stream
        , 'TimeOutBits'       : 0x200
        , 'RequireRoutingBits' : [] # to require not lumi exclusive, set to [ 0x0, 0x4, 0x0 ]
        , 'VetoRoutingBits'    : []
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

        from Configurables import LHCb__RawDataCnvSvc as RawDataCnvSvc
        EventPersistencySvc().CnvServices.append( RawDataCnvSvc('RawDataCnvSvc') )
        EventLoopMgr().Warnings = False

        app=ApplicationMgr()

        # setup the histograms and the monitoring service
        from Configurables import UpdateAndReset
        app.TopAlg = [ UpdateAndReset() ] + app.TopAlg
        ApplicationMgr().ExtSvc.append( 'MonitorSvc' ) 
        HistogramPersistencySvc().OutputFile = ''
        HistogramPersistencySvc().Warnings = False
        from Configurables import RootHistCnv__PersSvc
        RootHistCnv__PersSvc().OutputEnabled = False

        # set up the event selector
        if 'EventSelector' in allConfigurables : 
            del allConfigurables['EventSelector']

        TAE = OnlineEnv.TAE != 0
        input   = 'EVENT' if not TAE else 'MEP'
        output  = 'SEND'
        mepMgr = OnlineEnv.mepManager(OnlineEnv.PartitionID,OnlineEnv.PartitionName,[input,output],False)
        app.Runable = OnlineEnv.evtRunable(mepMgr)
        app.ExtSvc.append(mepMgr)
        evtMerger = OnlineEnv.evtMerger(name='Output',buffer=output,location='DAQ/RawEvent',datatype=OnlineEnv.MDF_NONE,routing=1)
        evtMerger.DataType = OnlineEnv.MDF_BANKS
        eventSelector = OnlineEnv.mbmSelector(input=input, TAE=TAE)
        app.ExtSvc.append(eventSelector)
        OnlineEnv.evtDataSvc()

        # Event processing timeout catcher
        # ======================================================
        #>>> To test: force delay bigger than the timeout....
        #delay = OnlineEnv.delayAlg(5500,1)
        #delay.OutputLevel = 1
        #ApplicationMgr().TopAlg.append(delay)
        #
        #>>> Configure timeout catch algorithm
        # event processing timeout: 5000ms, print trace=True
        # Events with timeout will have routing bit 0x200 set
        # and could be redirected at the storage level to a seperate
        # output stream
        if self.getProp('EnableTimeOutCatcher') :
            tmoCatcher = OnlineEnv.timeoutAlg(self.getProp('TimeOutThreshold'),True)
            tmoCatcher.OutputLevel = 2
            evtMerger.TimeoutBits = self.getProp('TimeOutBits')
            #ExceptionSvc().Catch = 'NONE'
            ApplicationMgr().TopAlg.append(tmoCatcher)

        # define the send sequence
        writer =  GaudiSequencer('SendSequence')
        writer.OutputLevel = OnlineEnv.OutputLevel
        writer.Members = self.getProp('WriterRequires') + [ evtMerger ]
        ApplicationMgr().TopAlg.append( writer )
        #ApplicationMgr().OutStream.append( writer )

        #ToolSvc.SequencerTimerTool.OutputLevel = @OnlineEnv.OutputLevel;          
        from Configurables import AuditorSvc
        AuditorSvc().Auditors = []
        self._configureOnlineMessageSvc()
        if self.getProp('UseDBSnapshot') : self._configureDBSnapshot()

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
        # hack to allow us to chance connectionstrings...
        conddb.UseOracle = True
        # Set alternative connection strings and tags
        # if simulation is False, we use DDDB, LHCBCOND and ONLINE
        #                  True          DDDB, SIMCOND
        # (see Det/DetCond's configurable... )
        dbPartitions = { False : [ "DDDB", "LHCBCOND" , "ONLINE" ]
                       , True :  [ "DDDB", "SIMCOND" ]
                       }
        for part in dbPartitions[ self.getProp('Simulation') ] :
            if tag[part] is 'default' : raise KeyError('must specify an explicit %s tag'%part)
            conddb.PartitionConnectionString[part] = "sqlite_file:%(dir)s/%(part)s_%(tag)s.db/%(part)s" % {"dir":  baseloc,
                                                                                                           "part": part,
                                                                                                           "tag":  tag[part]}
            # always use HEAD -- blindly trust the snapshot to be
            # right (this is faster, but less safe)
            # conddb.Tags[part] = 'HEAD'
            conddb.Tags[part] = tag[part]

        # Set the location of the Online conditions
        from Configurables import MagneticFieldSvc
        MagneticFieldSvc().UseSetCurrent = True

        if self.getProp('EnableRunChangeHandler') : 
            import OnlineEnv
            online_xml = '%s/%s/online_%%d.xml' % (baseloc, OnlineEnv.PartitionName )
            from Configurables import RunChangeHandlerSvc
            rch = RunChangeHandlerSvc()
            rch.Conditions = { "Conditions/Online/LHCb/Magnet/Set"  : online_xml
                             , "Conditions/Online/Velo/MotionSystem": online_xml
                             }
            ApplicationMgr().ExtSvc.append(rch)


    def _configureInput(self):
        files = self.getProp('inputFiles')
        if len(files)==0 or 'DST' in [ _ext(f) for f in files ] :
            importOptions('$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts')
        if 'RAW' in [ _ext(f) for f in files ] :
            #  veto lumi events..
            #ApplicationMgr().EvtSel.REQ1 = "EvType=2;TriggerMask=0x0,0x4,0x0,0x0;VetoMask=0,0,0,0;MaskType=ANY;UserType=USER;Frequency=PERC;Perc=100.0"
            EventPersistencySvc().CnvServices.append( 'LHCb::RawDataCnvSvc' )
        self._configureDataOnDemand()
        if files : EventSelector().Input = [ _datafmt(f) for f in files ]

    def _configureOutput(self):
        fname = self.getProp('outputFile')
        if not fname : return
        writer = None 
        if _ext(fname).upper() == 'RAW'  : 
            from Configurables import LHCb__MDFWriter as MDFWriter
            writer = MDFWriter( 'MDFWriter'
                              , Compress = 0
                              , ChecksumType = 1
                              , GenerateMD5 = True
                              , Connection = 'file://' + fname
                              )
        if _ext(fname).upper() == 'DST'  : 
            importOptions("$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts")
            from Configurables import InputCopyStream
            writer = InputCopyStream("InputCopyStream")
            writer.RequireAlgs = self.getProp('WriterRequires')
            writer.Output = "DATAFILE='PFN:%s' TYP='POOL_ROOTTREE' OPT='REC'" % fname
        if not writer : raise NameError('unsupported filetype for file "%s"'%fname)
        ApplicationMgr().OutStream.append( writer )

    def getRelease(self):
        import re,fileinput
        #  do not pick up the pz in vxrypz
        version = re.compile('^version (v\dr\d)(p\d)?')
        for line in fileinput.input(os.environ.get('MOOREROOT')+'/cmt/requirements') :
            match = version.match(line)
            if not match: continue
            fileinput.close()
            return 'MOORE_'+match.group(1)
        raise TypeError('Could not deduce version from cmt/requirementes')

    def getConfigAccessSvc(self):
        method = self.getProp('TCKpersistency').lower()
        if method not in [ 'file', 'sqlite', 'tarfile' ] : raise TypeError("invalid TCK persistency '%s'"%method)
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

    def addAuditor(self,x) :
        if  'AuditorSvc' not in ApplicationMgr().ExtSvc : 
            ApplicationMgr().ExtSvc.append( 'AuditorSvc' ) 
        AuditorSvc().Auditors.append( x )
        x.Enable = True

    def _outputLevel(self) :
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

        importOptions('$L0TCKROOT/options/L0DUConfig.opts')
        # cannot write (yet) to a tarfile...
        if self.getProp('TCKpersistency').lower() == 'tarfile' :
            self.setProp('TCKpersistency','file')
        # make sure we load as many L0 TCKs as possible...
        from Configurables import L0DUMultiConfigProvider
        L0DUMultiConfigProvider('L0DUConfig').Preload = True
        svcs = self.getProp("configServices")
        algs = self.getProp("configAlgorithms")
        from Configurables import HltGenConfig
        gen = HltGenConfig( ConfigTop = [ i.rsplit('/')[-1] for i in algs ]
                          , ConfigSvc = [ i.rsplit('/')[-1] for i in svcs ]
                          , ConfigAccessSvc = self.getConfigAccessSvc().getName()
                          , HltType = settings.HltType()
                          , MooreRelease = self.getRelease()
                          , Label = self.getProp('configLabel'))
        # make sure gen is the very first Top algorithm...
        from HltLine.HltDecodeRaw import DecodeODIN
        ApplicationMgr().TopAlg = DecodeODIN.members() + [ gen.getFullName() ] + ApplicationMgr().TopAlg

    def _l0(self) :
        #from Configurables import L0DUFromRawAlg
        #L0DUFromRawAlg().ProcessorDataOnTES = False

        if ( self.getProp("DataType") == 'DC06' and not self.getProp("L0") ):
            log.warning("It is mandatory to rerun the L0 emulation on DC06 data to get the HLT to work correctly")
            log.warning("Will set ReplaceL0BanksWithEmulated = True")
            self.setProp("ReplaceL0BanksWithEmulated",True)
        if ( self.getProp("ReplaceL0BanksWithEmulated") and not self.getProp("L0") ):
            log.warning("You asked to replace L0 banks with emulation. Will set L0 = True")
            self.setProp("L0",True)
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
            self.setOtherProps( L0Conf(), [ "ReplaceL0BanksWithEmulated" , "DataType" ] )
            log.info("Will rerun L0")

    def _config_with_hltconf(self):
        hltConf = HltConf()
        self.setOtherProps( hltConf,  
                            [ 'ThresholdSettings'
                            , 'DataType','Verbose'
                            , 'RequireL0ForEndSequence'
                            , 'SkipHltRawBankOnRejectedEvents'
                            , 'HistogrammingLevel' 
                            , "EnableLumiEventWriting"
                            , "EnableAcceptIfSlow"
                            ]
                          )

    def _config_with_tck(self):
        from Configurables import HltConfigSvc
        cfg = HltConfigSvc( prefetchDir = self.getProp('prefetchConfigDir')
                          , initialTCK =  _tck(self.getProp('InitialTCK'))
                          , checkOdin = self.getProp('CheckOdin')
                          , ConfigAccessSvc = self.getConfigAccessSvc().getFullName() ) 
        # TODO: make sure we are the first one...
        ApplicationMgr().ExtSvc.append(cfg.getFullName())
        # configure services...
        HistogramDataSvc().Input = ["CaloPIDs DATAFILE='$PARAMFILESROOT/data/CaloPIDs_DC09_v1.root' TYP='ROOT'"];
        VFSSvc().FileAccessTools = ['FileReadTool', 'CondDBEntityResolver/CondDBEntityResolver'];
        ParticlePropertySvc().ParticlePropertiesFile = "conddb:///param/ParticleTable.txt";
        from Configurables import LHCb__ParticlePropertySvc
        LHCb__ParticlePropertySvc().ParticlePropertiesFile = 'conddb:///param/ParticleTable.txt';

    def __apply_configuration__(self):
        GaudiKernel.ProcessJobOptions.PrintOff()
        # verify mutually exclusive settings:
        #  eg.  Online vs. any L0 setting
        #       Online vs. generateConfig
        #       Online vs. DB tags...
        #       Online vs. EvtMax, SkipEvents, DataType, ...
        #       Online requires UseTCK
        if not self.getProp("RunOnline") : self._l0()
        if self.getProp("RunOnline") : 
            import OnlineEnv
            self.setProp('EnableTimer',False)
            self.setProp('UseTCK',True)
            self.setProp('Simulation',False)
            self.setProp('DataType','2009' )
            ### TODO: see if 'OnlineEnv' has InitialTCK attibute. If so, set it
            ## in case of LHCb or FEST, _REQUIRE_ it exists...
            if hasattr(OnlineEnv,'InitialTCK') :
                self.setProp('InitialTCK',OnlineEnv.InitialTCK)
                self.setProp('CheckOdin',True)
            # determine the partition we run in, and adapt settings accordingly...
            if OnlineEnv.PartitionName == 'FEST' or OnlineEnv.PartitionName == 'LHCb' :
                self.setProp('InitialTCK',OnlineEnv.InitialTCK)
                self.setProp('CheckOdin',True)
            if OnlineEnv.PartitionName == 'FEST' :
                self.setProp('Simulation',True)


        ApplicationMgr().TopAlg.append( GaudiSequencer('Hlt') )
        # forward some settings... 
        # WARNING: this triggers setup of /dd -- could be avoided in PA only mode...
        app = LHCbApp()
        self.setOtherProps( app, ['EvtMax','SkipEvents','Simulation', 'DataType' ] )
        # this is a hack. Why does setOtherProps not work?
        app.CondDBtag = self.getProp('CondDBtag')
        app.DDDBtag   = self.getProp('DDDBtag')
        # Get the event time (for CondDb) from ODIN 
        from Configurables import EventClockSvc
        EventClockSvc().EventTimeDecoder = 'OdinTimeDecoder'
        # make sure we don't pick up small variations of the read current
        # Need a defined HistogramPersistency to read some calibration inputs!!!
        ApplicationMgr().HistogramPersistency = 'ROOT'
        self._outputLevel()
        if self.getProp('UseTCK') :
            self._config_with_tck()
        else:
            self._config_with_hltconf()
            
        if self.getProp("RunOnline") :
            self._configureOnline()
        else :
            self._profile()
            if self.getProp("generateConfig") : self._generateConfig()
            self._configureInput()
            self._configureOutput()
            if self.getProp('RequireRoutingBits') or self.getProp('VetoRoutingBits') :
                from Configurables import HltRoutingBitsFilter
                filter = HltRoutingBitsFilter( "PhysFilter" )
                if self.getProp('RequireRoutingBits') : filter.RequireMask = self.getProp('RequireRoutingBits')
                if self.getProp('VetoRoutingBits') : filter.VetoMask = self.getProp('VetoRoutingBits') 
                topalgs = ApplicationMgr().TopAlg
                wrapper = GaudiSequencer('HltWrapper',Members = [ filter ] + topalgs)
                ApplicationMgr().TopAlg = [ wrapper ]

