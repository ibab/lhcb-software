"""
High level configuration tools for Moore
"""
__version__ = "$Id: Configuration.py,v 1.40 2008-11-28 08:27:47 graven Exp $"
__author__  = "Gerhard Raven <Gerhard.Raven@nikhef.nl>"

from os import environ, path
from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *
from Configurables import ConfigFileAccessSvc, ConfigDBAccessSvc, HltConfigSvc, HltGenConfig
from Configurables import EventClockSvc
from Configurables import L0DUMultiConfigProvider
from Configurables import LHCb__MDFWriter as MDFWriter
from HltConf.Configuration import *

import GaudiKernel.ProcessJobOptions
from  ctypes import c_uint


## HistogramPersistencySvc().OutputFile = 'Moore_minbias.root'

def _ext(name) : return path.splitext(name)[-1].lstrip('.')


class Moore(LHCbConfigurableUser):
    ## Possible used Configurables
    __used_configurables__ = [ HltConf, LHCbApp ]


    __slots__ = {
          "EvtMax":            -1    # Maximum number of events to process
        , "SkipEvents":        0
        , "DataType":          '2008' # Data type, can be [ 'DC06','2008' ]
        , "DDDBtag" :          '' # default as set in DDDBConf for DataType
        , "CondDBtag" :        '' # default as set in DDDBConf for DataType
        , "UseOracle":         False  # if False, use SQLDDDB instead
        , "outputFile" :       '' # output filename
        , "inputFiles" :       [ ] # input
        , "hltType" :          'Hlt1'
        , "useTCK"     :       False # use TCK instead of options...
        , "prefetchTCK" :      [ ] # which TCKs to prefetch. Initial TCK used is first one...
        , "generateConfig" :   False # whether or not to generate a configuration
        , "configLabel" :      ''    # label for generated configuration
        , "TCKData" :          '$TCKDATAROOT' # where do we read TCK data from?
        , "TCKpersistency" :   'file' # which method to use for TCK data? valid is 'file' and 'sqlite'
        , "enableAuditor" :    [ ]  # put here eg . [ NameAuditor(), ChronoAuditor(), MemoryAuditor() ]
        , "userAlgorithms":    [ ]  # put here user algorithms to add
        , "oldStyle" :         False# old style options configuration
        , "verbose" :          True # whether or not to print Hlt sequence
        }   
                

    def configureInput(self):
        files = self.getProp('inputFiles')
        extensions = { 'RAW' : "' SVC='LHCb::MDFSelector'",
                       'MDF' : "' SVC='LHCb::MDFSelector'",
                       'DST' : "' TYP='POOL_ROOTTREE' OPT='READ'" }
        EventSelector().Input = [ "DATAFILE='PFN:"+ f + extensions[ _ext(f).upper() ] for f in files ]

    def configureOutput(self):
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

    def __apply_configuration__(self):
        #GaudiKernel.ProcessJobOptions.PrintOff() #TODO: waiting for next release of Gaudi
        importOptions('$STDOPTS/DstDicts.opts')
        EventPersistencySvc().CnvServices.append( 'LHCb::RawDataCnvSvc' )
        importOptions('$STDOPTS/DecodeRawEvent.py')
        # needed for DecodeRawEvent and LoKiTrigger
        for i in [ 'ToolSvc' , 'AuditorSvc',  'DataOnDemandSvc' , 'LoKiSvc' ] : ApplicationMgr().ExtSvc.append( i ) 
        # forward some settings... 
        self.setOtherProps( LHCbApp(), ['EvtMax','SkipEvents','DataType','DDDBtag','CondDBtag','UseOracle' ] )
        # Get the event time (for CondDb) from ODIN 
        EventClockSvc().EventTimeDecoder = 'OdinTimeDecoder'
        # make sure we don't pick up small variations of the read current
        # MagneticFieldSvc().UseSetCurrent = True
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
        # TODO: check for mutually exclusive options...
        if self.getProp('useTCK') :
            tcks = [ int(i) for i in self.getProp('prefetchTCK') ]
            cfg = HltConfigSvc( prefetchTCK = tcks
                              , initialTCK = tcks[0]
                              , ConfigAccessSvc = self.getConfigAccessSvc().getFullName() ) 
            ApplicationMgr().ExtSvc.append(cfg.getFullName())
        else:
            for i in [ 'hltType','oldStyle','userAlgorithms','verbose' ] : self.setOtherProp( HltConf(), i )
            print HltConf()

            
        if self.getProp("generateConfig") :
            # make sure we load as many L0 TCKs as possible...
            importOptions('$L0TCKROOT/options/L0DUConfig.opts')
            L0DUMultiConfigProvider('L0DUConfig').Preload = True
            # TODO: add properties for ConfigTop and ConfigSvc...
            gen = HltGenConfig( ConfigTop = [ i.rsplit('/')[-1] if type(i) is str else i.getName() for i in ApplicationMgr().TopAlg ]
                              , ConfigSvc = [ 'ToolSvc','HltDataSvc','HltANNSvc','LumiANNSvc' ]
                              , ConfigAccessSvc = self.getConfigAccessSvc().getName()
                              , hltType = self.getProp('hltType')
                              , mooreRelease = self.getRelease()
                              , label = self.getProp('configLabel'))
            # make sure gen is the very first Top algorithm...
            ApplicationMgr().TopAlg = [ gen.getFullName() ] + ApplicationMgr().TopAlg

            
        self.configureInput()
        self.configureOutput()
