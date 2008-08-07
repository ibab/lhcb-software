
"""
Base configurable for the RICH
"""
__version__ = "$Id: Configuration.py,v 1.1 2008-08-07 15:51:48 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from Gaudi.Configuration import *
from Configurables import ( Rich__ToolRegistry )

class RichConfigurableUser(ConfigurableUser):
    
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
                print "# %s().%s already defined, ignoring %s().%s"%(other.name(),name,
                                                                     self.name(), name )
            else:
                other.setProp(name,self.getProp(name))

    def setOtherProps(self,other,names):
        for name in names: self.setOtherProp(other,name)
        

    def toolRegistry(self):
        context = self.getProp("context")
        reg = Rich__ToolRegistry(context+"_RichToolRegistry")
        reg.Context = context
        return reg

    def checkRadData(self,option,default):
        if len(self.getProp(option)) == 0 : self.setProp(option,default)
        if len(self.getProp(option)) != 3:
            raise RuntimeError("Invalid option '%s'=%s"%option%self.getProp(option))
