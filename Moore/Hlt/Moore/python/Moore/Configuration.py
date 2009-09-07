"""
High level configuration tool(s) for Moore
"""
__version__ = "$Id: Configuration.py,v 1.79 2009-09-07 09:57:25 graven Exp $"
__author__  = "Gerhard Raven <Gerhard.Raven@nikhef.nl>"

from os import environ, path
from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
from Configurables import HltConf
from Configurables import HltConfigSvc, HltGenConfig
from Configurables import EventClockSvc
from Configurables import GaudiSequencer
from Configurables import LHCbApp, L0Conf
from Configurables import L0DUMultiConfigProvider
from Configurables import LHCb__MDFWriter as MDFWriter
# from HltConf.Configuration import *

import GaudiKernel.ProcessJobOptions
from  ctypes import c_uint


## HistogramPersistencySvc().OutputFile = 'Moore_minbias.root'

def _ext(name) : 
    x =  path.splitext(name)[-1].lstrip('.').upper()
    if x == 'MDF' : x = 'RAW'
    return x

def _datafmt(pfn) : 
    fmt = { 'RAW' : "DATAFILE='PFN:%s' SVC='LHCb::MDFSelector'"
          , 'DST' : "DATAFILE='PFN:%s' TYP='POOL_ROOTTREE' OPT='READ'" 
          }
    return fmt[ _ext(pfn) ] % pfn

# canonicalize tck  -- eats integer + string, returns canonical string
def _tck(x) :
    if type(x) == str and x[0:2] == '0x' : return x
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
        , "HltType" :          'Hlt1'
        , "UseTCK"     :       False # use TCK instead of options...
        , "L0"         :       False # run L0
        , "ReplaceL0BanksWithEmulated" : False # rerun L0
        , "L0TCK"      :       ''  # which L0 TCKs to use for configuration
        , "CheckOdin"  :       False  # use TCK from ODIN
        , "InitialTCK" :'0x80560000'  # which configuration to use during initialize
        , "prefetchConfigDir" :'MOORE_v7r4'  # which configurations to prefetch.
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
        , "Verbose" :           True # whether or not to print Hlt sequence
        , "ThresholdSettings" : ''
        , 'RequireL0ForEndSequence'     : False
        , 'SkipHltRawBankOnRejectedEvents' : True
        , 'HistogrammingLevel' : 'Line'
        , "RunOnline" : False
        }   
                

    def _enableDataOnDemand(self) :
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
        self._enableDataOnDemand()

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
        input   = 'EVENT'
        if TAE: input = 'MEP'

        mepMgr = OnlineEnv.mepManager(OnlineEnv.PartitionID,OnlineEnv.PartitionName,[input,'SEND'],False)
        app.Runable = OnlineEnv.evtRunable(mepMgr)
        app.ExtSvc.append(mepMgr)
        evtMerger = OnlineEnv.evtMerger(name='Output',buffer='SEND',location='DAQ/RawEvent',datatype=OnlineEnv.MDF_NONE,routing=1)
        evtMerger.DataType = OnlineEnv.MDF_BANKS
        eventSelector = OnlineEnv.mbmSelector(input=input, TAE=TAE)


        app.ExtSvc.append(eventSelector)
        OnlineEnv.evtDataSvc()

        # define the send sequence
        SendSequence =  GaudiSequencer('SendSequence')
        SendSequence.OutputLevel = OnlineEnv.OutputLevel
        from Configurables import HltLine
        SendSequence.Members = [ HltLine('Hlt1Global'), evtMerger ]
        ApplicationMgr().TopAlg.append(SendSequence)

        #ToolSvc.SequencerTimerTool.OutputLevel = @OnlineEnv.OutputLevel;          
        from Configurables import AuditorSvc
        AuditorSvc().Auditors = []
        self._configureOnlineMessageSvc()
        # self._configureOnlineDB()

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
        #msg.setError += ['LoKiSvc'];



    def _configureOnlineDB(self):
        ### TODO: make sure there is a FEST specific snapshot...

        tag = { "DDDB":     self.getProp('DDDBtag')   # "head-20090112",
              , "LHCBCOND": self.getProp('CondDBtag') # "head-20090112" 
              }
        for (k,v) in tag.iteritems() :
            if v is 'default' : raise keyError('must specify an explicit %s tag'%k)

        baseloc = "/group/online/hlt/conditions"

        from Configurables import CondDB
        conddb = CondDB()
        # hack to allow us to chance connectionstrings...
        conddb.UseOracle = True
        # Set alternative connection strings and tags
        for part in [ "DDDB", "LHCBCOND" ]:
            conddb.PartitionConnectionString[part] = "sqlite_file:%(dir)s/%(part)s_%(tag)s.db/%(part)s" % {"dir":  baseloc,
                                                                                                           "part": part,
                                                                                                           "tag":  tag[part]}
            # always use HEAD -- blindly trust the snapshot to be
            # right (this is faster, but less safe)
            conddb.Tags[part] = 'HEAD'
        part = "ONLINE"
        conddb.PartitionConnectionString[part] = "sqlite_file:%(dir)s/%(part)s_%(tag)s.db/%(part)s" % {"dir":  baseloc,
                                                                                                       "part": part,
                                                                                                       "tag":  "fake"}

        # Set the location of the Online conditions
        from Configurables import MagneticFieldSvc
        MagneticFieldSvc().UseSetCurrent = True
        from Configurables import RunChangeHandlerSvc
        rch = RunChangeHandlerSvc()
        rch.Conditions = { "Conditions/Online/LHCb/Magnet/Set"  : baseloc + "/%d/online.xml",
                           "Conditions/Online/Velo/MotionSystem": baseloc + "/%d/online.xml",
                           }
        ApplicationMgr().ExtSvc.append(rch)

    def _configureInput(self):
        files = self.getProp('inputFiles')
        if 'DST' in [ _ext(f) for f in files ] :
            importOptions('$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts')
        #if 'RAW' in [ _ext(f) for f in files ] :
        EventPersistencySvc().CnvServices.append( 'LHCb::RawDataCnvSvc' )
        self._enableDataOnDemand()
        EventSelector().Input = [ _datafmt(f) for f in files ]

    def _configureOutput(self):
        fname = self.getProp('outputFile')
        if not fname : return
        if _ext(fname).upper() != 'RAW'  : raise NameError('unsupported filetype for file "%s"'%fname)
        writer = MDFWriter( 'MDFWriter'
                          , Compress = 0
                          , ChecksumType = 1
                          , GenerateMD5 = True
                          , Connection = 'file://' + fname
                          )
        ApplicationMgr().OutStream.append( writer )

    def getRelease(self):
        import re,fileinput
        version = re.compile('^version (\w+)')
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
        importOptions('$L0TCKROOT/options/L0DUConfig.opts')
        # cannot write (yet) to a tarfile...
        if self.getProp('TCKpersistency').lower() == 'tarfile' :
            self.setProp('TCKpersistency','file')
        # make sure we load as many L0 TCKs as possible...
        L0DUMultiConfigProvider('L0DUConfig').Preload = True
        svcs = self.getProp("configServices")
        algs = self.getProp("configAlgorithms")
        gen = HltGenConfig( ConfigTop = [ i.rsplit('/')[-1] for i in algs ]
                          , ConfigSvc = [ i.rsplit('/')[-1] for i in svcs ]
                          , ConfigAccessSvc = self.getConfigAccessSvc().getName()
                          , hltType = self.getProp('HltType')
                          , mooreRelease = self.getRelease()
                          , label = self.getProp('configLabel'))
        # make sure gen is the very first Top algorithm...
        from HltLine.HltDecodeRaw import DecodeODIN
        ApplicationMgr().TopAlg = DecodeODIN.members() + [ gen.getFullName() ] + ApplicationMgr().TopAlg

    def _l0(self) :
        if ( (self.getProp("HltType")!='') and (self.getProp("DataType") == 'DC06') and (not self.getProp("L0")) ):
            log.warning("It is mandatory to run the L0 emulation on DC06 data to get the HLT to work correctly")
            log.warning("Will set ReplaceL0BanksWithEmulated = True")
            self.setProp("ReplaceL0BanksWithEmulated",True)
        if ( self.getProp("ReplaceL0BanksWithEmulated") and (not self.getProp("L0")) ):
            log.warning("You asked to replace L0 banks with emulation. Will set L0 = True")
            self.setProp("L0",True)
        if ( self.getProp("L0") ):
            l0seq = GaudiSequencer("seqL0")
            ApplicationMgr().TopAlg += [ l0seq ]
            L0Conf().setProp( "L0Sequencer", l0seq )
            self.setOtherProps( L0Conf(), [ "ReplaceL0BanksWithEmulated" , "DataType" ] )
            log.info("Will rerun L0")

    def _config_with_hltconf(self):
        hltConf = HltConf()
        self.setOtherProps( hltConf,  
                            [ 'HltType','ThresholdSettings'
                            , 'L0TCK','DataType','Verbose'
                            , 'RequireL0ForEndSequence'
                            , 'SkipHltRawBankOnRejectedEvents'
                            , 'HistogrammingLevel' 
                            , "EnableLumiEventWriting"
                            ]
                          )

    def _config_with_tck(self):
        if (self.getProp('L0TCK')) : raise RunTimeError( 'UseTCK and L0TCK are mutually exclusive')
        cfg = HltConfigSvc( prefetchDir = self.getProp('prefetchConfigDir')
                          , initialTCK = self.getProp('InitialTCK')
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
                self.setProp('InitialTCK', OnlineEnv.InitialTCK )
                self.setProp('CheckOdin',True)
            if OnlineEnv.PartitionName == 'FEST' :
                # This is a bad hack which is probably incompatible with the use of snapshots...
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
