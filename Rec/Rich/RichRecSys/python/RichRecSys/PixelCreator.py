
## @package PixelCreator
#  Pixel creator options for RICH Reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: PixelCreator.py,v 1.3 2008-10-15 12:42:19 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from Configurables import ( Rich__DAQ__HPDPixelClusterSuppressionTool,
                            Rich__DAQ__HPDPixelClusteringTool )

# ----------------------------------------------------------------------------------

## @class RichPixelCreatorConfig
#  Pixel creator options for RICH Reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008   
class RichPixelCreatorConfig(RichConfigurableUser):

    ## Known pixel cleaning schemes
    KnownPixelCleaning = [ "HotClusters", "HotHPDs", "None" ]

    ## Default options
    __slots__ = {
        "context":       "Offline"   # The context within which to run
       ,"detectors": [True,True]     # Which RICH detectors to use (RICH1/RICH2)
       ,"pixelCleaning": "HotClusters" # Turn on RICH pixel cleaning (hot HPDs etc.)
       ,"findClusters": True # Find clusters in the HPD data
       ,"useClustersAsPixels": [ False, False ] # Use clusters as the raw pixel objects
       ,"bookKeeping": False
        }

    ## @brief Check the configuration is OK
    #
    def checkConfiguration(self):
        dets = self.getProp("detectors")
        if len(dets) != 2 : raise RuntimeError("ERROR : Invalid Detectors '%s'"%config)

    ## @brief Apply the configuration
    #
    def applyConf(self):

        # Check the configuration
        self.checkConfiguration()
        context = self.getProp("context")

        # Pixel Creator
        pixmaker = RichTools().pixelCreator()
        pixmaker.UseDetectors  = self.getProp("detectors")
        pixmaker.DoBookKeeping = self.getProp("bookKeeping")

        # pixel cleaning
        # -------------------------------------------------------------------------------------------
        self.toolRegistry().Tools += ["Rich::DAQ::HPDPixelClusteringTool/ClusteringTool"]
        pixmaker.ApplyPixelSuppression = True
        pixCleanMode = self.getProp("pixelCleaning")
        if pixCleanMode == "HotClusters" :
 
            # RICH1 cleaning
            r1Cleaner = Rich__DAQ__HPDPixelClusterSuppressionTool("PixelSuppressRich1")
            self.toolRegistry().Tools += [r1Cleaner]
            r1Cleaner.addTool( Rich__DAQ__HPDPixelClusteringTool, name="ClusteringTool" )
            r1Cleaner.ClusteringTool.AllowDiagonalsInClusters = True
            r1Cleaner.WhichRich = "RICH1"
            r1Cleaner.AbsoluteMaxHPDOcc      = 200
            r1Cleaner.OccCutScaleFactor      = 14
            r1Cleaner.MaxPixelClusterSize    = 10
            r1Cleaner.MinHPDOccForClustering = 10
            pixmaker.addTool( r1Cleaner )
                               
            # RICH2 cleaning
            r2Cleaner = Rich__DAQ__HPDPixelClusterSuppressionTool("PixelSuppressRich2")
            self.toolRegistry().Tools += [r2Cleaner]
            r2Cleaner.addTool( Rich__DAQ__HPDPixelClusteringTool, name="ClusteringTool" )
            r2Cleaner.ClusteringTool.AllowDiagonalsInClusters = True
            r2Cleaner.WhichRich = "RICH2"
            r2Cleaner.AbsoluteMaxHPDOcc      = 200
            r2Cleaner.OccCutScaleFactor      = 14
            r2Cleaner.MaxPixelClusterSize    = 8
            r2Cleaner.MinHPDOccForClustering = 8
            pixmaker.addTool( r2Cleaner )
  
        elif pixCleanMode == "HotHPDs" :
            
            cleaner = "Rich::DAQ::HighOccHPDSuppressionTool"
            self.toolRegistry().Tools += [cleaner+"/PixelSuppressRich1"]
            self.toolRegistry().Tools += [cleaner+"/PixelSuppressRich2"]
             
        elif pixCleanMode == "None" :

            pixmaker.ApplyPixelSuppression = False
            
        else :
            
            raise RuntimeError("Unknown pixel cleaning option '%s'"%pixCleanMode)
        # -------------------------------------------------------------------------------------------
        
        # Pixel clustering
        self.toolRegistry().Tools += ["Rich::DAQ::HPDPixelClusteringTool/PixelCreatorClustering"]
        pixmaker.SuppressClusterFinding = self.getProp("findClusters") != True
        pixmaker.ApplyPixelClustering   = self.getProp("useClustersAsPixels")
        pixmaker.addTool( Rich__DAQ__HPDPixelClusteringTool, name="PixelClusteringRich1" )
        pixmaker.addTool( Rich__DAQ__HPDPixelClusteringTool, name="PixelClusteringRich2" )
        pixmaker.PixelClusteringRich1.AllowDiagonalsInClusters = False
        pixmaker.PixelClusteringRich2.AllowDiagonalsInClusters = False
        pixmaker.PixelClusteringRich1.MaxClusterSize = 9999
        pixmaker.PixelClusteringRich2.MaxClusterSize = 9999
        
