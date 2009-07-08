"""
High level configuration tool(s) for Moore
"""
__version__ = "$Id: Configuration.py,v 1.59 2009-07-08 11:34:58 graven Exp $"
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

def _ext(name) : return path.splitext(name)[-1].lstrip('.')

def _datafmt(pfn) : 
        fmt = { 'RAW' : "DATAFILE='PFN:%s' SVC='LHCb::MDFSelector'"
              , 'MDF' : "DATAFILE='PFN:%s' SVC='LHCb::MDFSelector'"
              , 'DST' : "DATAFILE='PFN:%s' TYP='POOL_ROOTTREE' OPT='READ'" 
              }
        return fmt[ _ext(pfn).upper() ] % pfn

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
        , "DataType":          '2008' # Data type, can be [ 'DC06','2008' ]
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
        , "InitialTCK" :'0x804a0000'  # which configuration to use during initialize
        , "prefetchConfigDir" :'MOORE_v7r1'  # which configurations to prefetch.
        , "generateConfig" :   False # whether or not to generate a configuration
        , "configLabel" :      ''    # label for generated configuration
        , "configAlgorithms" : ['Hlt']    # which algorithms to configure (automatically including their children!)...
        , "configServices" :   ['ToolSvc','HltDataSvc','HltANNSvc' ]    # which services to configure (automatically including their dependencies!)...
        , "TCKData" :          '$HLTTCKROOT' # where do we read/write TCK data from/to?
        , "TCKpersistency" :   'tarfile' # which method to use for TCK data? valid is 'file','tarfile' and 'sqlite' 
        , "EnableAuditor" :    [ ]  # put here eg . [ NameAuditor(), ChronoAuditor(), MemoryAuditor() ]
        , "Verbose" :          True # whether or not to print Hlt sequence
        , "ThresholdSettings" : ''
        , "RunOnline" : False
        }   
                
    def _configureOnline(self) :
        import OnlineEnv as Online
        # log.warning('overruling settings with online values')
        self.setProp('UseTCK', True)
        from Configurables import LHCb__RawDataCnvSvc as RawDataCnvSvc
        EventPersistencySvc().CnvServices.append( RawDataCnvSvc('RawDataCnvSvc') )
        from Configurables import DataOnDemandSvc
        ApplicationMgr().ExtSvc.append( DataOnDemandSvc() ) 
        importOptions('$STDOPTS/DecodeRawEvent.py')
        ApplicationMgr().ExtSvc.append( 'MonitorSvc' ) 
        #MagneticFieldSvc().UseSetCurrent = True
        HistogramPersistencySvc().OutputFile = ''
        HistogramPersistencySvc().Warnings = False
        EventLoopMgr().Warnings = False
        app=ApplicationMgr()
        from Configurables import UpdateAndReset
        app.TopAlg = [ UpdateAndReset() ] + app.TopAlg
        ### TODO: if FEST partition, change DB setup???
        #mepMgr = Online.mepManager(Online.PartitionID,Online.PartitionName,['Events','SEND'],True)
        mepMgr = Online.mepManager(Online.PartitionID,Online.PartitionName,['EVENT','SEND'],False)
        app.Runable = Online.evtRunable(mepMgr)
        app.ExtSvc.append(mepMgr)
        evtMerger = Online.evtMerger(name='Output',buffer='SEND',datatype=Online.MDF_NONE,routing=1)
        evtMerger.DataType = Online.MDF_BANKS
        # append evtMerger to SendSequence, after Hlt1Global...
        #SendSequence.OutputLevel             = @OnlineEnv.OutputLevel;
        if 'EventSelector' in allConfigurables : 
            del allConfigurables['EventSelector']
        eventSelector = Online.mbmSelector(input='EVENT',TAE=( Online.TAE != 0 ))
        app.ExtSvc.append(eventSelector)
        Online.evtDataSvc()
        #ToolSvc.SequencerTimerTool.OutputLevel = @OnlineEnv.OutputLevel;          
        from Configurables import AuditorSvc
        AuditorSvc().Auditors = []
        app.MessageSvcType = 'LHCb::FmcMessageSvc'

        del allConfigurables['MessageSvc']
        app.SvcOptMapping.append('LHCb::FmcMessageSvc/MessageSvc')
        from Configurables import LHCb__FmcMessageSvc as MessageSvc
        msg=MessageSvc('MessageSvc')
        msg.LoggerOnly = True
        if 'LOGFIFO' not in os.environ :
            os.environ['LOGFIFO'] = '/tmp/logGaudi.fifo'
            print '# WARNING: LOGFIFO was not set -- now set to ' + os.environ['LOGFIFO']
        msg.fifoPath = os.environ['LOGFIFO']
        msg.OutputLevel = Online.OutputLevel
        msg.doPrintAlways = False
        SendSequence =  GaudiSequencer('SendSequence')
        from Configurables import HltLine
        SendSequence.Members = [ HltLine('Hlt1Global'), evtMerger ]
        ApplicationMgr().TopAlg.append(SendSequence)

    def _configureInput(self):
        files = self.getProp('inputFiles')
        if files : importOptions('$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts')
        EventPersistencySvc().CnvServices.append( 'LHCb::RawDataCnvSvc' )
        ApplicationMgr().ExtSvc.append( 'DataOnDemandSvc' ) 
        importOptions('$STDOPTS/DecodeRawEvent.py')
        EventSelector().Input = [ _datafmt(f) for f in files ]

    def _configureOutput(self):
        fname = self.getProp('outputFile')
        if not fname : return
        if _ext(fname).upper() not in [ 'MDF','RAW' ] : raise NameError('unsupported filetype for file "%s"'%fname)
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
        AuditorSvc().Auditors.append( x.name() )
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
        if  'AuditorSvc' not in ApplicationMgr().ExtSvc : 
            ApplicationMgr().ExtSvc.append( 'AuditorSvc' ) 
        AuditorSvc().Auditors.append( 'TimingAuditor/TIMER' )
        for i in self.getProp('EnableAuditor') : self.addAuditor( i )

    def _generateConfig(self) :
        importOptions('$L0TCKROOT/options/L0DUConfig.opts')
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
        ApplicationMgr().TopAlg = [ gen.getFullName() ] + ApplicationMgr().TopAlg

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
        self.setOtherProps( hltConf,  [ 'HltType','Verbose','L0TCK','DataType','ThresholdSettings'])
        log.info( hltConf )

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
            import OnlineEnv as Online
            self.setProp('UseTCK',True)
            self.setProp('Simulation',False)
            self.setProp('DataType','2009' )
            # determine the partition we run in, and adapt settings accordingly...
            if Online.PartitionName == 'FEST' or Online.PartitionName == 'LHCb' :
                self.setProp('InitialTCK', Online.InitialTCK )
                self.setProp('CheckOdin',True)
            if Online.PartitionName == 'FEST' :
		# This is a bad hack which is probably incompatible with the use of snapshots...
                self.setProp('Simulation',True)


        ApplicationMgr().TopAlg.append( GaudiSequencer('Hlt') )
        # forward some settings... 
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
        self._profile()
        if self.getProp('UseTCK') :
            self._config_with_tck()
        else:
            self._config_with_hltconf()
            
        if self.getProp("RunOnline") :
            self._configureOnline()
        else :
            if self.getProp("generateConfig") : self._generateConfig()
            self._configureInput()
            self._configureOutput()
