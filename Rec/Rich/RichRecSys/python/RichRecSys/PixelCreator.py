
"""
Pixel creator options for RICH Reconstruction
"""
__version__ = "$Id: PixelCreator.py,v 1.1 2008-08-07 20:56:54 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from Configurables import ( Rich__Rec__PixelCreatorFromRawBuffer,
                            Rich__DAQ__HPDPixelClusterSuppressionTool,
                            Rich__DAQ__HPDPixelClusteringTool )
    
class RichPixelCreator(RichConfigurableUser):

    # Known pixel cleaning schemes
    KnownPixelCleaning = [ "HotClusters", "HotHPDs", "None" ]
   
    __slots__ = {
        "configuration": "RawBuffer" # The pixel handling
       ,"context":       "Offline"   # The context within which to run
       ,"detectors": [True,True]     # Which RICH detectors to use (RICH1/RICH2)
       ,"pixelCleaning": "HotClusters" # Turn on RICH pixel cleaning (hot HPDs etc.)
       ,"findClusters": True # Find clusters in the HPD data
       ,"useClustersAsPixels": [ False, False ] # Use clusters as the raw pixel objects 
        }

    def checkConfiguration(self):
        dets = self.getProp("detectors")
        if len(dets) != 2 : raise RuntimeError("ERROR : Invalid Detectors '%s'"%config)
    
    def applyConf(self):

        # Check the configuration
        self.checkConfiguration()
        config  = self.getProp("configuration")
        context = self.getProp("context")
        nickname = "RichPixelCreator"

        # basic pixel creator type
        if config == "RawBuffer":
            type = "Rich::Rec::PixelCreatorFromRawBuffer"
            self.toolRegistry().Tools += [type+"/"+nickname]
            pixmaker = Rich__Rec__PixelCreatorFromRawBuffer("ToolSvc."+context+"_"+nickname)
            pixmaker.UseDetectors = self.getProp("detectors")
        else:
            raise RuntimeError("Unknown Pixel configuration '%s'"%config)

        # pixel cleaning
        # -------------------------------------------------------------------------------------------
        self.toolRegistry().Tools += ["Rich::DAQ::HPDPixelClusteringTool/ClusteringTool"]
        pixmaker.ApplyPixelSuppression = True
        pixCleanMode = self.getProp("pixelCleaning")
        if pixCleanMode == "HotClusters" :
            cleaner = "Rich::DAQ::HPDPixelClusterSuppressionTool"

            # RICH1 cleaning
            self.toolRegistry().Tools += [cleaner+"/PixelSuppressRich1"]
            pixmaker.addTool( Rich__DAQ__HPDPixelClusterSuppressionTool, name="PixelSuppressRich1" )
            pixmaker.PixelSuppressRich1.addTool( Rich__DAQ__HPDPixelClusteringTool, name="ClusteringTool" )
            pixmaker.PixelSuppressRich1.ClusteringTool.AllowDiagonalsInClusters = True
            pixmaker.PixelSuppressRich1.WhichRich = "RICH1"
            pixmaker.PixelSuppressRich1.AbsoluteMaxHPDOcc      = 200
            pixmaker.PixelSuppressRich1.OccCutScaleFactor      = 14
            pixmaker.PixelSuppressRich1.MaxPixelClusterSize    = 10
            pixmaker.PixelSuppressRich1.MinHPDOccForClustering = 10
                   
            # RICH2 cleaning
            self.toolRegistry().Tools += [cleaner+"/PixelSuppressRich2"]
            pixmaker.addTool( Rich__DAQ__HPDPixelClusterSuppressionTool, name="PixelSuppressRich2" )
            pixmaker.PixelSuppressRich2.addTool( Rich__DAQ__HPDPixelClusteringTool, name="ClusteringTool" )
            pixmaker.PixelSuppressRich2.ClusteringTool.AllowDiagonalsInClusters = True
            pixmaker.PixelSuppressRich2.WhichRich = "RICH2"
            pixmaker.PixelSuppressRich2.AbsoluteMaxHPDOcc      = 200
            pixmaker.PixelSuppressRich2.OccCutScaleFactor      = 14
            pixmaker.PixelSuppressRich2.MaxPixelClusterSize    = 8
            pixmaker.PixelSuppressRich2.MinHPDOccForClustering = 8

        elif pixCleanMode == "HotHPDs" :
            
            cleaner = "Rich::DAQ::HighOccHPDSuppressionTool"
            self.toolRegistry().Tools += [cleaner+"/PixelSuppressRich1"]
            self.toolRegistry().Tools += [cleaner+"/PixelSuppressRich2"]
             
        elif pixCleanMode == "None" :

            pixmaker.ApplyPixelSuppression = False
            
        else:
            
            raise RuntimeError("Unknown pixel cleaning option '%s'"%pixCleanMode)
        # -------------------------------------------------------------------------------------------
        
        # Pixel clustering
        self.toolRegistry().Tools += ["Rich::DAQ::HPDPixelClusteringTool/PixelCreatorClustering"]
        pixmaker.SuppressClusterFinding = self.getProp("findClusters") != True
        pixmaker.ApplyPixelClustering   = self.getProp("useClustersAsPixels")
