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
        'cameraServer' : ''
        }

    _propertyDocDct = {
        'cameraServer' : """This is the server that is running CAMERA.
                            If left empty then no messages will be sent to CAMERA
                            and it is effectively disabled. Otherwise it must be
                            set to the name of the CAMERA server to use. Defaults
                            to empty string and CAMERA is disabled.
                         """
        }
    
    def __apply_configuration__(self):
        """Applies all the configuration settings for the class."""
        # We are only exposing the server name and will leave the other
        # properties at their default values.
        from Configurables import CameraTool
        camera = CameraTool("ToolSvc.CameraTool")
        serverName = self.getProp("cameraServer")
        if '' != serverName:
            camera.Enabled = True
            camera.ServerName = serverName
        else:
            camera.Enabled = False
        
    # def __apply_configuration__()
# def Camera()
