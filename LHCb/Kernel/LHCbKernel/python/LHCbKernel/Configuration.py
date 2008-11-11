
## @package LHCbKernel
#  Base configurables for the LHCb software
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.6 2008-11-11 16:26:11 cattanem Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from Gaudi.Configuration import *

# ----------------------------------------------------------------------------------

## @class LHCbConfigurableUser
#  Base configurable for LHCb software
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class LHCbConfigurableUser(ConfigurableUser):

    ## Default options are empty
    __slots__ = { }

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

    ## @brief Set the given property in another configurable object
    #  @param other The other configurable to set the property for
    #  @param name  The property name
    def setOtherProp(self,other,name):
        # If self property is set, use it
        if hasattr(self,name):
            if hasattr(other,name) :
                log.warning("Both %s().%s and  %s().%s are defined, using %s().%s"%
                            (other.name(),name, self.name(), name, self.name(), name)  )
            other.setProp(name,self.getProp(name))
        # If not, and other property also not set, propagate the default
        elif not hasattr(other,name) :
            if isinstance(other,ConfigurableUser):
                other._properties[name].setDefault(self.getProp(name))
            else:
                other.setProp(name,self.getProp(name))
    

    ## @brief Set the given properties in another configurable object
    #  @param other The other configurable to set the property for
    #  @param names The property names
    def setOtherProps(self,other,names):
        for name in names:
            self.setOtherProp(other,name)
            
