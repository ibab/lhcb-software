"""
High level configuration tools for Moore
"""
__version__ = "$Id: Configuration.py,v 1.10 2008-07-31 15:51:24 graven Exp $"
__author__  = "Gerhard Raven <Gerhard.Raven@nikhef.nl>"

from os import environ
from Gaudi.Configuration import *
from GaudiConf.Configuration import *
from Configurables import ConfigFileAccessSvc, ConfigDBAccessSvc, HltConfigSvc, HltGenConfig
from Configurables import EventClockSvc

import GaudiKernel.ProcessJobOptions
from  ctypes import c_uint

class Moore(ConfigurableUser):
    __slots__ = {
          "EvtMax":            -1    # Maximum number of events to process
        , "DAQStudies":        False # use DAQ study version of options
        , "DDDBtag" :          'DEFAULT'
        , "condDBtag" :        'DEFAULT'
        , "inputType":         'dst' # must either 'mdf' or 'dst'
        , "runType" :          'Physics_Hlt1+Hlt2'
        , "runTiming"  :       False # include additional timing information
        , "useTCK"     :       False # use TCK instead of options...
        , "prefetchTCK" :      [ ] # which TCKs to prefetch. Initial TCK used is first one...
        , "generateConfig" :   False # whether or not to generate a configuration
        , "TCKData" :          '$TCKDATAROOT' # where do we read TCK data from?
        , "TCKpersistency" :   'file' # which method to use for TCK data? valid is 'file' and 'sqlite'
        , "enableAuditor" :    [ ]  # put here eg . [ NameAuditor(), ChronoAuditor(), MemoryAuditor() ]
        }   
                
    def getProp(self,name):
        if hasattr(self,name):
            return getattr(self,name)
        else:
            return self.getDefaultProperties()[name]

    def setProp(self,name,value):
        return setattr(self,name,value)

    def setOtherProp(self,other,name):
        # Function to propagate properties to other component, if not already set
        if hasattr(self,name):
            if hasattr(other,name) and len(other.getProp(name)) > 0 :
                print "# %s().%s already defined, ignoring Moore().%s"%(other.name(),name,name)
            else:
                setattr(other,name,self.getProp(name))

    def validRunTypes(self):
        return [ 'Physics_Hlt1+Hlt2', 'Physics_Hlt1', 'Commissioning' ] 

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

    def applyConf(self):
        GaudiKernel.ProcessJobOptions.printing_level += 1
        # importOptions('$STDOPTS/LHCbApplication.opts')
        importOptions('$STDOPTS/DstDicts.opts')

        inputType = self.getProp('inputType').upper()
        if inputType not in [ 'MDF','DST' ] : raise TypeError("Invalid input type '%s'"%inputType)
        if inputType != 'DST' : 
            EventPersistencySvc().CnvServices.append( 'LHCb::RawDataCnvSvc' )
            importOptions('$STDOPTS/DecodeRawEvent.opts')

        ApplicationMgr().ExtSvc.append(  "DataOnDemandSvc"   ); # needed for DecodeRawEvent...
        importOptions('$STDOPTS/DC06Conditions.opts')
        # forward some other settings... TODO: make a dictionary..
        self.setOtherProp( LHCbApp(), 'useOracleCondDB' )
        importOptions( "$DDDBROOT/options/DDDB.py" )
        from DetCond.Configuration import configureOnlineSnapshots
        configureOnlineSnapshots()
        self.setOtherProp( LHCbApp(), 'DDDBtag' )
        self.setOtherProp( LHCbApp(), 'condDBtag' )
        self.setOtherProp( LHCbApp(), 'skipEvents' )
        self.setOtherProp( ApplicationMgr(), 'EvtMax' )
        # Get the event time (for CondDb) from ODIN 
        EventClockSvc().EventTimeDecoder = 'OdinTimeDecoder'
        # output levels...
        ToolSvc().OutputLevel                     = INFO
        from Configurables import XmlParserSvc 
        XmlParserSvc().OutputLevel                = WARNING
        MessageSvc().OutputLevel                  = INFO
        ApplicationMgr().OutputLevel              = ERROR
        SequencerTimerTool().OutputLevel          = WARNING
        # Print algorithm name with 40 characters
        MessageSvc().Format = '% F%40W%S%7W%R%T %0W%M'
        # Profiling
        for i in [ 'ToolSvc' , 'AuditorSvc' ] : ApplicationMgr().ExtSvc.append( i ) 
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
            if self.getProp("DAQStudies") :
                importOptions('$HLTSYSROOT/options/L0DAQ.opts')
                importOptions('$HLTSYSROOT/options/HltDAQ.opts')
            else :
                importOptions('$HLTSYSROOT/options/HltInit.opts')
                runtype = self.getProp('runType')
                if runtype not in self.validRunTypes() :  raise TypeError("Unknown runtype '%s'"%runtype)
                if runtype in [ 'Physics_Hlt1+Hlt2', 'Physics_Hlt1' ] : importOptions('$HLTSYSROOT/options/Hlt1.opts')
                if runtype in [ 'Physics_Hlt1+Hlt2' ] :                 importOptions('$HLTSYSROOT/options/Hlt2.opts')
                if runtype in [ 'Commissioning' ] :                     importOptions('$HLTSYSROOT/options/RandomPrescaling.opts')
            if self.getProp('runTiming') :
                importOptions('$HLTSYSROOT/options/HltAlleysTime.opts')
                importOptions('$HLTSYSROOT/options/HltAlleysHistos.opts')
        if self.getProp("generateConfig") :
            gen = HltGenConfig( ConfigTop = [ i.rsplit('/')[-1] for i in ApplicationMgr().TopAlg ]
                              , ConfigSvc = [ 'ToolSvc','HltDataSvc','HltANNSvc' ]
                              , ConfigAccessSvc = self.getConfigAccessSvc().getName() )
            # make sure gen is the very first Top algorithm...
            ApplicationMgr().TopAlg = [ gen.getFullName() ] + ApplicationMgr().TopAlg
        LHCbApp().applyConf()
