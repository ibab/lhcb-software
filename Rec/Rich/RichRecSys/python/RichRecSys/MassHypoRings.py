
"""
Mass Hypo rings
"""
__version__ = "$Id: MassHypoRings.py,v 1.1 2008-08-07 20:56:54 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from Configurables import Rich__Rec__MassHypothesisRingCreator

class RichMassHypoRingsConf(RichConfigurableUser):

    DefaultMaxRingPoints = [ 100,  100,  100 ]
    DefaultMinRingPoints = [ 100,  100,  100 ]
    
    __slots__ = {
        "context":    "Offline"
       ,"maxRingPoints": []
       ,"minRingPoints": []
       ,"checkBeamPipe": False
        }

    def applyConf(self):

        self.checkRadData("maxRingPoints",self.DefaultMaxRingPoints)
        self.checkRadData("minRingPoints",self.DefaultMinRingPoints)

        nickname = "RichMassHypoRings"

        rings = Rich__Rec__MassHypothesisRingCreator( "ToolSvc."+self.getProp("context")+"_"+nickname )

        rings.MaxRingPoints = self.getProp("maxRingPoints")
        rings.MinRingPoints = self.getProp("minRingPoints")
        rings.CheckBeamPipe = self.getProp("checkBeamPipe")
        
