"""
High level configuration for ray tracing
"""
__version__ = "$Id: RayTracing.py,v 1.1 2008-08-07 20:56:54 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from Configurables import Rich__Rec__RayTraceCherenkovCone

class RichRayTracingConf(RichConfigurableUser):

    DefaultBailoutTries = [ 50,  50,  50 ]
    
    __slots__ = {
        "context":    "Offline" # The context within which to run
       ,"bailoutTries": [ ]
        }
    
    def applyConf(self):

        self.checkRadData("bailoutTries",self.DefaultBailoutTries)
          
        nickname = "RichRayTraceCKCone"
        
        rayT = Rich__Rec__RayTraceCherenkovCone( "ToolSvc."+self.getProp("context")+"_"+nickname )

        rayT.BailoutTries = self.getProp("bailoutTries")
