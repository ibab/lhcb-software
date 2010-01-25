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
###from GaudiConf.Configuration import *

# ==========================================================================================
# @class Camera
#   CAMERA configurable.
#
# @author Gareth Rogers (gareth.rogers@cern.ch)
# @date   26/09/2008
#
class Camera(ConfigurableUser):
    # Steering options
    __slots__ = {
        'CameraServers' : []
        }

    _propertyDocDct = {
        'CameraServers' : """This is a list of servers that the tool should attempt
                             to send CAMERA messages to. The entry is a colon delimited
                             value of the form host:port e.g. localhost:12345.
                             The first server in the list is considered to be the
                             primary server to use. In the event that the tool
                             cannot connect to the primary server then the remaining
                             servers are tried in order.
                             If this list is empty or None is passed then the CAMERA
                             tool will be disabled.
                             An empty string will connect to the default CAMERA server
                             127.0.0.1:12345
                         """
        }
    
    def __apply_configuration__(self):
        """Applies all the configuration settings for the class."""
        # We are only exposing the server name and will leave the other
        # properties at their default values.
        from Configurables import CameraTool
        camera = CameraTool('ToolSvc.CameraTool')
        servers = self.getProp('CameraServers')
        # Check to see whether we have been given any servers.
        # If the server dict is none then we will throw a TypeError.
        # This indicates that we want to disable the CAMERA server.
        try:
            if 0 is not len(servers):
                # We have been given a list of servers so enable the tool.
                camera.Enabled = True
                # Need to set the list of servers and ports to the
                # appropriate proerties.
                names = []
                ports = []
                for server in servers:
                    try:
                        name, port = server.split(':')
                        names.append(name)
                        ports.append(int(port))
                    except ValueError:
                        log.error("Couldn't determine the server name and port from '"+server+"'. Is it in the form 'server:port'?")
                # Need to define the primary port to use.
                if 0 is not len(names):
                    camera.ServerName = names[0]
                if 0 is not len(ports):
                    camera.ServerPort = ports[0]
                # Append the full list of servers.
                camera.ServerNames = names
                camera.ServerPorts = ports
            else:
                camera.Enabled = False
        except TypeError:
            camera.Enabled = False
    # def __apply_configuration__()
# def Camera()
