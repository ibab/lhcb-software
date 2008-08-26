
## @package LHCbKernel
#  Base configurables for the LHCb software
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.3 2008-08-26 19:25:08 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from Gaudi.Configuration import *

# ----------------------------------------------------------------------------------

## @class LHCbConfigurableUser
#  Base configurable for LHCb software
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class LHCbConfigurableUser(ConfigurableUser):

    ## @brief Object 'Constructor'
    def __init__( self, name = Configurable.DefaultName, **kwargs ):
        # base class
        initVal = ConfigurableUser.__init__(self,name,**kwargs)
        # Call custom initialise method
        self.initialise()
        # return
        return initVal

    ## @brief Default initialise method
    #  Derived classes can reimplement this method, to run any object
    #  initialisation tasks they wish
    def initialise(self): None

    ## @brief Returns the value of the given property
    #  @param name The property name
    #  @return The value of property 'name'
    def getProp(self,name):
        if hasattr(self,name):
            return getattr(self,name)
        else:
            return self.getDefaultProperties()[name]

    ## @brief Set the value of a given property
    #  @param name  The name of the property
    #  @param value The value of the property
    def setProp(self,name,value):
        return setattr(self,name,value)

    ## @brief Set the given property in another configurable object
    #  @param other The other configurable to set the property for
    #  @param name  The property name
    def setOtherProp(self,other,name):
        # Function to propagate properties to other component, if not already set
        if hasattr(self,name):
            if hasattr(other,name) and len(other.getProp(name)) > 0 :
                print "# %s().%s already defined, ignoring %s().%s"%(other.name(),name,
                                                                     self.name(), name )
            else:
                other.setProp(name,self.getProp(name))

    ## @brief Set the given properties in another configurable object
    #  @param other The other configurable to set the property for
    #  @param names The property names
    def setOtherProps(self,other,names):
        for name in names:
            self.setOtherProp(other,name)
            
