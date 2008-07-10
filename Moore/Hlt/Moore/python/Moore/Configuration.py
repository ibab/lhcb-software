"""
High level configuration tools for Moore
"""
__version__ = "$Id: Configuration.py,v 1.4 2008-07-10 18:15:36 graven Exp $"
__author__  = "Gerhard Raven <Gerhard.Raven@nikhef.nl>"

from os import environ
from Gaudi.Configuration import *
from GaudiConf.Configuration import *
from Configurables import ConfigFileAccessSvc, ConfigDBAccessSvc, HltConfigSvc, HltGenConfig
from Configurables import EventClockSvc

import GaudiKernel.ProcessJobOptions

class Moore(ConfigurableUser):
    __slots__ = {
          "EvtMax":          -1 # Maximum number of events to process
        , "DAQStudies":    False# use DAQ study version of options
        , "IncludeL0":     True # include L0 options
        , "IncludeHlt1":   True # include Hlt1 options
        , "IncludeHlt2":   True # include Hlt2 options
        , "RunTiming"  :   False # include Hlt1 options
        , "UseTCK"     :   False # use TCK instead of options...
        , "PrefetchTCK" :   []   # which TCKs to prefetch. Initial TCK used is first one...
        , "GenerateConfig" : False # whether or not to generate a configuration
        , "TCKDirectory" : '/user/graven/tmp/config' # where do we read TCKs from?
        #TODO: the next can probably be done in better way...
        , "EnableMemoryAuditor" : False
        , "EnableChronoAuditor" : False
        , "EnableNameAuditor" : False
        }   
                
    def getProp(self,name):
        if hasattr(self,name):
            return getattr(self,name)
        else:
            return self.getDefaultProperties()[name]

    def setProp(self,name,value):
        return setattr(self,name,value)

    def evtMax(self):
        if hasattr(ApplicationMgr(),"EvtMax"):
            return getattr(ApplicationMgr(),"EvtMax")
        else:
            return ApplicationMgr().getDefaultProperties()["EvtMax"]

    def getConfigAccessSvc(self):
        # TODO: avoid hardwiring ConfigAccessSvc...
        return ConfigFileAccessSvc( Directory = self.getProp('TCKDirectory') )
    def enableAuditor(self,x) :
        AuditorSvc().Auditors.append( x.name() )
        x.Enable = True

    def applyConf(self):
        GaudiKernel.ProcessJobOptions.printing_level += 1
        importOptions('$STDOPTS/LHCbApplication.opts')
        importOptions('$STDOPTS/DstDicts.opts')
        importOptions('$STDOPTS/DC06Conditions.opts')
        # Get the event time (for CondDb) from ODIN 
        EventClockSvc().EventTimeDecoder = 'OdinTimeDecoder'
        # Profiling
        for i in [ 'ToolSvc' , 'AuditorSvc' ] : ApplicationMgr().ExtSvc.append( i ) 
        ApplicationMgr().AuditAlgorithms = 1
        AuditorSvc().Auditors.append( 'TimingAuditor/TIMER' )
        if self.getProp('EnableNameAuditor')   : self.enableAuditor( NameAuditor() )
        if self.getProp('EnableChronoAuditor') : self.enableAuditor( ChronoAuditor() )
        if self.getProp('EnableMemoryAuditor') : self.enableAuditor( MemoryAuditor() )
        # TODO: check for mutually exclusive options...
        if self.getProp('UseTCK') :
            cas = getConfigAccessSvc()
            tcks = self.getProp('PrefetchTCK')
            cfg = HltConfigSvc( prefetchTCK = tcks, initialTCK = tcks[0], ConfigAccessSvc = cas.getFullName() ) 
            ApplicationMgr().ExtSvc.append(cfg.getFullName())
        else:
            if self.getProp("DAQStudies") :
                if self.getProp('IncludeL0') : importOptions('$HLTSYSROOT/options/L0DAQ.opts')
                importOptions('$HLTSYSROOT/options/HltDAQ.opts')
            else :
                if self.getProp('IncludeL0') : importOptions('$HLTSYSROOT/options/L0.opts')
                importOptions('$HLTSYSROOT/options/HltInit.opts')
                if self.getProp('IncludeHlt1') : importOptions('$HLTSYSROOT/options/Hlt1.opts')
                if self.getProp('IncludeHlt2') : importOptions('$HLTSYSROOT/options/Hlt2.opts')
            if self.getProp('RunTiming') :
                importOptions('$HLTSYSROOT/options/HltAlleysTime.opts')
                importOptions('$HLTSYSROOT/options/HltAlleysHistos.opts')
        if self.getProp("GenerateConfig") :
            gen = HltGenConfig( ConfigTop = [ i.rsplit('/')[-1] for i in ApplicationMgr().TopAlg ],
                                ConfigSvc = [ 'ToolSvc','HltDataSvc','HltANNSvc' ],
                                ConfigAccessSvc = getConfigAccessSvc().getName() )
            # make sure gen is the very first Top algorithm...
            ApplicationMgr().TopAlg = [ gen.getFullName() ] + ApplicationMgr().TopAlg
        LHCbApp().applyConf()
