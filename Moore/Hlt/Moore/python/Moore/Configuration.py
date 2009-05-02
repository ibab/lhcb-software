"""
High level configuration tool(s) for Moore
"""
__version__ = "$Id: Configuration.py,v 1.52 2009-05-02 08:26:57 graven Exp $"
__author__  = "Gerhard Raven <Gerhard.Raven@nikhef.nl>"

from os import environ, path
from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
from Configurables import ConfigFileAccessSvc, ConfigDBAccessSvc, HltConfigSvc, HltGenConfig
from Configurables import EventClockSvc
from Configurables import GaudiSequencer
from Configurables import HltConf, LHCbApp
from Configurables import L0DUMultiConfigProvider
from Configurables import LHCb__MDFWriter as MDFWriter
# from HltConf.Configuration import *

import GaudiKernel.ProcessJobOptions
from  ctypes import c_uint


## HistogramPersistencySvc().OutputFile = 'Moore_minbias.root'

def _ext(name) : return path.splitext(name)[-1].lstrip('.')

# canonicalize tck  -- eats integer + string, returns canonical string
def _tck(x) :
    if type(x) == str and x[0:2] == '0x' : return x
    return '0x%08x'%int(x)

class Moore(LHCbConfigurableUser):
    ## Possible used Configurables
    __used_configurables__ = [ HltConf
                             , LHCbApp ]


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
        , "replaceL0BanksWithEmulated" : False
        , "L0TCK"      :       ''  # which L0 TCKs to use for configuration
        , "prefetchConfigDir" :''  # which configurations to prefetch.
        , "generateConfig" :   False # whether or not to generate a configuration
        , "configLabel" :      ''    # label for generated configuration
        , "configAlgorithms" : ['Hlt']    # which algorithms to configure (automatically including their children!)...
        , "configServices" :   ['ToolSvc','HltDataSvc','HltANNSvc','LumiANNSvc','HistogramDataSvc' ]    # which services to configure (automatically including their dependencies!)...
        , "TCKData" :          '$TCKDATAROOT' # where do we read/write TCK data from/to?
        , "TCKpersistency" :   'file' # which method to use for TCK data? valid is 'file' and 'sqlite'
        , "enableAuditor" :    [ ]  # put here eg . [ NameAuditor(), ChronoAuditor(), MemoryAuditor() ]
        , "Verbose" :          True # whether or not to print Hlt sequence
        }   
                

    def _configureInput(self):
        files = self.getProp('inputFiles')
        extensions = { 'RAW' : "' SVC='LHCb::MDFSelector'",
                       'MDF' : "' SVC='LHCb::MDFSelector'",
                       'DST' : "' TYP='POOL_ROOTTREE' OPT='READ'" }
        EventSelector().Input = [ "DATAFILE='PFN:"+ f + extensions[ _ext(f).upper() ] for f in files ]

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


    def _generateConfig(self) :
        importOptions('$L0TCKROOT/options/L0DUConfig.opts')
        # make sure we load as many L0 TCKs as possible...
        L0DUMultiConfigProvider('L0DUConfig').Preload = True
        svcs = self.getProp("configServices")
        algs = self.getProp("configAlgorithms")
        # if not algs : algs = [ i.rsplit('/')[-1] if type(i) is str else i.getName() for i in ApplicationMgr().TopAlg ] # WARNING: this doesn't work on Gaudi v20r3 and later...
        gen = HltGenConfig( ConfigTop = [ i.rsplit('/')[-1] for i in algs ]
                          , ConfigSvc = [ i.rsplit('/')[-1] for i in svcs ]
                          , ConfigAccessSvc = self.getConfigAccessSvc().getName()
                          , hltType = self.getProp('HltType')
                          , mooreRelease = self.getRelease()
                          , label = self.getProp('configLabel'))
        # make sure gen is the very first Top algorithm...
        ApplicationMgr().TopAlg = [ gen.getFullName() ] + ApplicationMgr().TopAlg

    def __apply_configuration__(self):
        GaudiKernel.ProcessJobOptions.PrintOff()
        importOptions('$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts')
        EventPersistencySvc().CnvServices.append( 'LHCb::RawDataCnvSvc' )
        importOptions('$STDOPTS/DecodeRawEvent.py')
        if self.getProp('replaceL0BanksWithEmulated') :
            importOptions('$L0DUROOT/options/ReplaceL0BanksWithEmulated.opts')
        ApplicationMgr().TopAlg.append( GaudiSequencer('Hlt') )
        # needed for DecodeRawEvent 
        for i in [  'AuditorSvc',  'DataOnDemandSvc' ] : ApplicationMgr().ExtSvc.append( i ) 
        # forward some settings... 
        app = LHCbApp()
        self.setOtherProps( app, ['EvtMax','SkipEvents','Simulation', 'DataType','DDDBtag','CondDBtag' ] )
        # Get the event time (for CondDb) from ODIN 
        EventClockSvc().EventTimeDecoder = 'OdinTimeDecoder'
        # make sure we don't pick up small variations of the read current
        #MagneticFieldSvc().UseSetCurrent = True
        # output levels...
        ToolSvc().OutputLevel                     = INFO
        from Configurables import XmlParserSvc 
        XmlParserSvc().OutputLevel                = WARNING
        MessageSvc().OutputLevel                  = INFO
        ApplicationMgr().OutputLevel              = ERROR
        SequencerTimerTool().OutputLevel          = WARNING
        # Print algorithm name with 40 characters
        MessageSvc().Format = '% F%40W%S%7W%R%T %0W%M'
        # Need a defined HistogramPersistency to read some calibration inputs!!!
        ApplicationMgr().HistogramPersistency = 'ROOT'
        # Profiling
        ApplicationMgr().AuditAlgorithms = 1
        AuditorSvc().Auditors.append( 'TimingAuditor/TIMER' )
        for i in self.getProp('enableAuditor') : self.addAuditor( i )
        if self.getProp('useTCK') :
            if (self.getProp('L0TCK')) : raise RunTimeError( 'useTCK and L0TCK are mutually exclusive')
            # TODO: update to latest HltConfigSvc setup!!!
            tcks = [ _tck(i) for i in self.getProp('prefetchTCK') ]
            cfg = HltConfigSvc( prefetchTCK = tcks
                              , initialTCK = tcks[0]
                              , ConfigAccessSvc = self.getConfigAccessSvc().getFullName() ) 
            ApplicationMgr().ExtSvc.append(cfg.getFullName())
        else:
            hltConf = HltConf()
            self.setOtherProps( hltConf,  [ 'HltType','Verbose','L0TCK'])
            print hltConf
            log.info( hltConf )
            
        if self.getProp("generateConfig") : self._generateConfig()
        self._configureInput()
        self._configureOutput()
