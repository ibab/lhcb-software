
## @package LHCbKernel
#  Base configurables for the LHCb software
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.8 2008-12-01 10:56:45 cattanem Exp $"
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
    def __init__(self, name = Configurable.DefaultName, **kwargs):
        kwargs["name"] = name
        apply(super(LHCbConfigurableUser, self).__init__, (), kwargs)
        self.initialize()
        
    ## @brief Default initialize method
    #  Derived classes can reimplement this method, to run any object
    #  initialisation tasks they wish
    def initialize(self) : pass


    ## @brief Set the given property in another configurable object
    #  @param other The other configurable to set the property for
    #  @param name  The property name
    def setOtherProp(self,other,name):
        # If self property is set, use it
        if hasattr(self,name) and self.getProp(name) != self.getDefaultProperties()[name]:
            if hasattr(other,name) and other.getProp(name) != other.getDefaultProperties()[name]:
                log.warning("Both %s().%s and %s().%s are defined, using %s().%s"%
                            (other.name(),name, self.name(), name, self.name(), name)  )
            other.setProp(name,self.getProp(name))
        # If not, and other property also not set, propagate the default
        elif not hasattr(other,name) or other.getProp(name) != other.getDefaultProperties()[name]:
            if isinstance(other,ConfigurableUser):
                other._properties[name].setDefault(self.getProp(name))
            else:
                other.setProp(name,self.getProp(name))

    ## @brief Set the given properties in another configurable object
    #  @param other The other configurable to set the property for
    #  @param names The property names
    def setOtherProps(self,other,names):
        for name in names : self.setOtherProp(other,name)
