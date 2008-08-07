"""
Segment creator options for RICH Reconstruction
"""
__version__ = "$Id: SegmentCreator.py,v 1.1 2008-08-07 20:56:54 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from Configurables import ( Rich__Rec__SegmentCreator )

class RichSegmentCreatorConf(RichConfigurableUser):

    DefaultEnergyBins = [ 5, 5, 5 ]

  # Steering options
    __slots__ = {
        "context": "Offline"  # The context within which to run
       ,"energyBins": [ ]
        }
    
    def applyConf(self):

        # Check settings
        self.checkRadData("energyBins",self.DefaultEnergyBins)

        type     = "Rich::Rec::SegmentCreator"
        nickname = "RichSegmentCreator"
        
        segCreator = Rich__Rec__SegmentCreator( "ToolSvc."+self.getProp("context")+"_"+nickname )

        segCreator.EnergyBins = self.getProp("energyBins")
