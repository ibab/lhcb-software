"""
Geometrical Efficiency options for RICH Reconstruction
"""
__version__ = "$Id: GeomEff.py,v 1.1 2008-08-07 20:56:54 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from Configurables import ( Rich__Rec__GeomEffCKMassRing,
                            Rich__Rec__GeomEffPhotonTracing )

# ----------------------------------------------------------------------------------

class GeomEffCKMassRing(RichConfigurableUser):

    __slots__ = { "context": "Offline" }

    def createInstance(self,parent,nickname):
        type = "Rich::Rec::GeomEffCKMassRing"
        context = parent.getProp("context")
        self.setProp("context",context)
        gEff = Rich__Rec__GeomEffCKMassRing("ToolSvc."+context+"_"+nickname)
        self.toolRegistry().Tools += [type+"/"+nickname]
        return gEff

# ----------------------------------------------------------------------------------
        
class GeomEffPhotonTracing(RichConfigurableUser):

    __slots__ = {
        "context":    "Offline"
        }
    
    def createInstance(self,parent,nickname):
        type = "Rich::Rec::GeomEffPhotonTracing"
        context = parent.getProp("context")
        self.setProp("context",context)
        gEff = Rich__Rec__GeomEffPhotonTracing("ToolSvc."+context+"_"+nickname)
        self.toolRegistry().Tools += [type+"/"+nickname]
        return gEff

# ----------------------------------------------------------------------------------

class GeomEffConf(RichConfigurableUser):
    
    __slots__ = {
        "context":    "Offline"
       ,"type": "CKMassRing"
        }
    
    def applyConf(self):
        
        type = self.getProp("type")
        nickname = "RichGeomEff"

        if type == "CKMassRing":
            gEff = GeomEffCKMassRing().createInstance(self,nickname)
        elif type == "PhotonTracing":
            gEff = GeomEffPhotonTracing().createInstance(self,nickname)
        elif type == "FixedValue":
            gEff = GeomEffFixedValue().createInstance(self,nickname)
        else:
            raise RuntimeError("Unknown geom eff tool '%s'"%type)
        
