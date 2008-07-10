"""
High level configuration tools for Moore
"""
__version__ = "$Id: Configuration.py,v 1.3 2008-07-10 16:16:01 graven Exp $"
__author__  = "Gerhard Raven <Gerhard.Raven@nikhef.nl>"

from os import environ
from Gaudi.Configuration import *
from GaudiConf.Configuration import *

import GaudiKernel.ProcessJobOptions

class Moore(ConfigurableUser):
    __slots__ = {
          "EvtMax":          -1 # Maximum number of events to process
        , "DAQStudies":    False# use DAQ study version of options
        , "IncludeL0":     True # include L0 options
        , "IncludeHlt1":   True # include Hlt1 options
        , "IncludeHlt2":   True # include Hlt2 options
        , "RunTiming"  :   False # include Hlt1 options
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

    def applyConf(self):
        GaudiKernel.ProcessJobOptions.printing_level += 1
        importOptions('$STDOPTS/LHCbApplication.opts')
        importOptions('$STDOPTS/DstDicts.opts')
        importOptions('$STDOPTS/DC06Conditions.opts')
        if self.getProp("DAQStudies") :
            if self.getProp('IncludeL0') : importOptions('$HLTSYSROOT/options/L0DAQ.opts')
            importOptions('$HLTSYSROOT/options/HltDAQ.opts')
        else :
            if self.getProp('IncludeL0') : 
                importOptions('$HLTSYSROOT/options/L0.opts')
            importOptions('$HLTSYSROOT/options/HltInit.opts')
            if self.getProp('IncludeHlt1') : 
                importOptions('$HLTSYSROOT/options/Hlt1.opts')
            if self.getProp('IncludeHlt2') : 
                importOptions('$HLTSYSROOT/options/Hlt2.opts')
        if self.getProp('RunTiming') :
            importOptions('$HLTSYSROOT/options/HltAlleysTime.opts')
            importOptions('$HLTSYSROOT/options/HltAlleysHistos.opts')
        LHCbApp().applyConf()
