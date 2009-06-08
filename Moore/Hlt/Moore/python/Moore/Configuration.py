"""
High level configuration tool(s) for Moore
"""
__version__ = "$Id: Configuration.py,v 1.54 2009-06-08 11:48:40 graven Exp $"
__author__  = "Gerhard Raven <Gerhard.Raven@nikhef.nl>"

from os import environ, path
from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
from Configurables import ConfigFileAccessSvc, ConfigDBAccessSvc, HltConfigSvc, HltGenConfig
from Configurables import EventClockSvc
from Configurables import GaudiSequencer
from Configurables import HltConf, LHCbApp, L0Conf
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
        , "useTCK"     :       False # use TCK instead of options...
        , "L0"         :       False # run L0
        , "ReplaceL0BanksWithEmulated" : False # rerun L0
        , "L0TCK"      :       ''  # which L0 TCKs to use for configuration
        , "CheckOdin"  :       False  # use TCK from ODIN
        , "initialTCK" :'0x80450000'  # which configuration to use during initialize
        , "prefetchConfigDir" :'MOORE_v7r0'  # which configurations to prefetch.
        , "generateConfig" :   False # whether or not to generate a configuration
        , "configLabel" :      ''    # label for generated configuration
        , "configAlgorithms" : ['Hlt']    # which algorithms to configure (automatically including their children!)...
        , "configServices" :   ['ToolSvc','HltDataSvc','HltANNSvc' ]    # which services to configure (automatically including their dependencies!)...
        , "TCKData" :          '$TCKDATAROOT' # where do we read/write TCK data from/to?
        , "TCKpersistency" :   'file' # which method to use for TCK data? valid is 'file' and 'sqlite'
        , "EnableAuditor" :    [ ]  # put here eg . [ NameAuditor(), ChronoAuditor(), MemoryAuditor() ]
        , "Verbose" :          True # whether or not to print Hlt sequence
        , "ThresholdSettings" : ''
        }   
                
    def _configureOnline(self) :
        import OnlineEnv as Online
        app=ApplicationMgr()
        EventPersistencySvc().CnvServices.append( 'LHCb::RawDataCnvSvc' )
        mepMgr = Online.mepManager(Online.PartitionID,Online.PartitionName,['Events','SEND'],True)
        app.Runable = Online.evtRunable(mepMgr)
        app.ExtSvc.append(mepMgr)
        evtMerger = Online.evtMerger(name='Output',buffer='SEND',datatype=MDF_NONE,routing=1)
        evtMerger.DataType = MDF_BANKS
        # append evtMerger to SendSequence, after Hlt1Global...
        #ApplicationMgr.TopAlg               += { "Sequencer/SendSequence" };
        #SendSequence.Members                 = {"Hlt1Global", "LHCb::RawEvent2MBMMergerAlg/Output"};
        #SendSequence.OutputLevel             = @OnlineEnv.OutputLevel;
        eventSelector = Online.mbmSelector(input='EVENT')
        app.ExtSvc.append(eventSelector)
        Online.evtDataSvc()
        #ToolSvc.SequencerTimerTool.OutputLevel = @OnlineEnv.OutputLevel;          
        Configs.AuditorSvc().Auditors = []
        app.MessageSvcType = 'LHCb::FmcMessageSvc'
        del allConfigurables['MessageSvc']
        app.SvcOptMapping.append('LHCb::FmcMessageSvc/MessageSvc')
        msg=Configs.LHCb__FmcMessageSvc('MessageSvc')
        msg.LoggerOnly = True
        #MessageSvc.LoggerOnly                = true;
        #MessageSvc.OutputLevel               = @OnlineEnv.OutputLevel;
        msg.fifoPath = os.environ['LOGFIFO']
        msg.OutputLevel = WARNING
        msg.doPrintAlways = False

    def _onlineSettings(self) :
        log.warning('overruling settings with online values')
        self.setProp('useTCK', True)
        HistogramPersistencySvc().Warnings = false;
        EventLoopMgr().Warnings = false;
        # configure services...
        HistogramDataSvc().Input = ["CaloPIDs DATAFILE='$PARAMFILESROOT/data/CaloPIDs_DC09_v1.root' TYP='ROOT'"];
        VFSSvc().FileAccessTools = ['FileReadTool', 'CondDBEntityResolver/CondDBEntityResolver'];
        ParticlePropertySvc().ParticlePropertiesFile = "conddb:///param/ParticleTable.txt";
        LHCb__ParticlePropertySvc().ParticlePropertiesFile = 'conddb:///param/ParticleTable.txt';


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
        if method not in [ 'file', 'sqlite' ] : raise TypeError("invalid TCK persistency '%s'"%method)
        TCKData = self.getProp('TCKData')
        if method == 'file' :
            return ConfigFileAccessSvc( Directory = TCKData +'/config' )
        if method == 'sqlite' :
            return ConfigDBAccessSvc( Connection = 'sqlite_file:' + TCKData +'/db/config.db' )

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


    def __apply_configuration__(self):
        GaudiKernel.ProcessJobOptions.PrintOff()
        self._l0()
        ApplicationMgr().TopAlg.append( GaudiSequencer('Hlt') )
        # forward some settings... 
        app = LHCbApp()
        self.setOtherProps( app, ['EvtMax','SkipEvents','Simulation', 'DataType' ] )
        # this is a hack. Why does setOtherProps not work?
        app.CondDBtag = self.getProp('CondDBtag')
        app.DDDBtag   = self.getProp('DDDBtag')
        print app
        # Get the event time (for CondDb) from ODIN 
        EventClockSvc().EventTimeDecoder = 'OdinTimeDecoder'
        # make sure we don't pick up small variations of the read current
        #MagneticFieldSvc().UseSetCurrent = True
        # Need a defined HistogramPersistency to read some calibration inputs!!!
        ApplicationMgr().HistogramPersistency = 'ROOT'
        self._outputLevel()
        self._profile()
        if self.getProp('useTCK') :
            if (self.getProp('L0TCK')) : raise RunTimeError( 'useTCK and L0TCK are mutually exclusive')
            # TODO: update to latest HltConfigSvc setup!!!
            tcks = [ _tck(i) for i in self.getProp('prefetchTCK') ]
            cfg = HltConfigSvc( prefetchDir = self.getProp('prefetchConfigDir')
                              , initialTCK = self.getProp('initialTCK')
                              , checkOdin = self.getProp('checkOdin')
                              , ConfigAccessSvc = self.getConfigAccessSvc().getFullName() ) 
            # TODO: make sure we are the first one...
            ApplicationMgr().ExtSvc.append(cfg.getFullName())
        else:
            hltConf = HltConf()
            self.setOtherProps( hltConf,  [ 'HltType','Verbose','L0TCK','DataType','ThresholdSettings'])
            print hltConf
            log.info( hltConf )
            
        if self.getProp("generateConfig") : self._generateConfig()
        online = False
        if online : 
            print 'not yet done'
        else :
            self._configureInput()
            self._configureOutput()
