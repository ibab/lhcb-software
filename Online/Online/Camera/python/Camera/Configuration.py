# ==========================================================================================
# @package Camera
#   Contains the configurable for the CAMERA package.
#
# @author Gareth Rogers (gareth.rogers@cern.ch)
# @date   2009/01/29
#

# ==========================================================================================
# Import everything from in Gaudi configuration and the configurables that we wish to use.
#
from Gaudi.Configuration  import *
from GaudiConf.Configuration import *

# Tools.
from Configurables import CameraTool

# ==========================================================================================
# @class Camera
#   CAMERA configurable.
#
# @author Gareth Rogers (gareth.rogers@cern.ch)
# @date   26/09/2008
#
class Camera(LHCbConfigurableUser):
    # Steering options
    __slots__ = {
        'cameraServer' : 'localhost'
        }

    _propertyDocDct = {
        'cameraServer' : """This is the server that is running CAMERA."""
        }
    
    def __apply_configuration__(self):
        """Applies all the configuration settings for the class."""
        # We are only exposing the server name and will leave the other
        # properties at their default values.
        camera = CameraTool("ToolSvc.CameraTool")
        camera.ServerName = self.getProp("cameraServer")
    # def __apply_configuration__()
# def Camera()
