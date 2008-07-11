"""
High level configuration tools for Moore
"""
__version__ = "$Id: Configuration.py,v 1.7 2008-07-11 12:06:53 graven Exp $"
__author__  = "Gerhard Raven <Gerhard.Raven@nikhef.nl>"

from os import environ
from Gaudi.Configuration import *
from GaudiConf.Configuration import *
from Configurables import ConfigFileAccessSvc, ConfigDBAccessSvc, HltConfigSvc, HltGenConfig
from Configurables import EventClockSvc

import GaudiKernel.ProcessJobOptions
from  ctypes import c_uint

class Moore(ConfigurableUser):
    #TODO: add some higher level configuration,
    #      which sets the right combinations...
    __slots__ = {
          "EvtMax":            -1    # Maximum number of events to process
        , "DAQStudies":        False # use DAQ study version of options
        , "InputType":         'dst' # must either 'mdf' or 'dst'
        , "IncludeL0Emulator": True  # re-run L0 using the emulator
        , "IncludeHlt1":       True  # include Hlt1...
        , "IncludeHlt2":       True  # include Hlt2...
        , "RunTiming"  :       False # include additional timing information
        , "UseTCK"     :       False # use TCK instead of options...
        , "PrefetchTCK" :      []    # which TCKs to prefetch. Initial TCK used is first one...
        , "GenerateConfig" :   False # whether or not to generate a configuration
        , "TCKDirectory" :     '/user/graven/tmp/config' # where do we read TCKs from?
        #TODO: the next can probably be done in better and more generic way...
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
    def convertToInt(self,x) :
        # TODO: use c_uint instead??
        if type(x)==int : return x
        if type(x)==str : 
            if x[0:2] == '0x' : return int(x,16)
            return int(x)

    def applyConf(self):
        GaudiKernel.ProcessJobOptions.printing_level += 1
        importOptions('$STDOPTS/LHCbApplication.opts')
        importOptions('$STDOPTS/DstDicts.opts')
        inputType = self.getProp('InputType').upper()
        if inputType not in [ "MDF","DST" ] : raise TypeError("Invalid input type '%s'"%inputType)
        #TODO: add flag for other types of data...
        if inputType == "MDF" : EventPersistencySvc().CnvServices.append( 'LHCb::RawDataCnvSvc' )
            

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
            tcks = [ int(i) for i in self.getProp('PrefetchTCK') ]
            cfg = HltConfigSvc( prefetchTCK = tcks
                              , initialTCK = tcks[0]
                              , ConfigAccessSvc = self.getConfigAccessSvc().getFullName() ) 
            ApplicationMgr().ExtSvc.append(cfg.getFullName())
        else:
            if self.getProp("DAQStudies") :
                importOptions('$HLTSYSROOT/options/L0DAQ.opts')
                importOptions('$HLTSYSROOT/options/HltDAQ.opts')
            else :
                if self.getProp('IncludeL0Emulator') : 
                    importOptions('$HLTSYSROOT/options/L0.opts')
                else : 
                    importOptions('$HLTSYSROOT/options/L0FromRaw.opts')
                importOptions('$HLTSYSROOT/options/HltInit.opts')
                if self.getProp('IncludeHlt1') : importOptions('$HLTSYSROOT/options/Hlt1.opts')
                if self.getProp('IncludeHlt2') : importOptions('$HLTSYSROOT/options/Hlt2.opts')
            if self.getProp('RunTiming') :
                importOptions('$HLTSYSROOT/options/HltAlleysTime.opts')
                importOptions('$HLTSYSROOT/options/HltAlleysHistos.opts')
        if self.getProp("GenerateConfig") :
            gen = HltGenConfig( ConfigTop = [ i.rsplit('/')[-1] for i in ApplicationMgr().TopAlg ]
                              , ConfigSvc = [ 'ToolSvc','HltDataSvc','HltANNSvc' ]
                              , ConfigAccessSvc = self.getConfigAccessSvc().getName() )
            # make sure gen is the very first Top algorithm...
            ApplicationMgr().TopAlg = [ gen.getFullName() ] + ApplicationMgr().TopAlg
        LHCbApp().applyConf()
