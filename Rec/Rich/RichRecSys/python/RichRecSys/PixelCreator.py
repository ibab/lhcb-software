
## @package PixelCreator
#  Pixel creator options for RICH Reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: PixelCreator.py,v 1.6 2008-12-16 13:45:36 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *

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
        "Context":       "Offline"   # The context within which to run
       ,"Detectors": [True,True]     # Which RICH detectors to use (RICH1/RICH2)
       ,"PixelCleaning": "HotClusters" # Turn on RICH pixel cleaning (hot HPDs etc.)
       ,"FindClusters": True # Find clusters in the HPD data
       ,"UseClustersAsPixels": [ False, False ] # Use clusters as the raw pixel objects
       ,"MaxHotPixelClusterSize": [ 10, 8 ] # Max size of clusters for HotClusters pixel cleaning (RICH1,RICH2)
       ,"BookKeeping": False
        }

    ## @brief Check the configuration is OK
    #
    def checkConfiguration(self):
        dets = self.getProp("Detectors")
        if len(dets) != 2 : raise RuntimeError("ERROR : Invalid Detectors '%s'"%config)

    ## @brief Apply the configuration
    #
    def applyConf(self):

        # Check the configuration
        self.checkConfiguration()
        context = self.getProp("Context")

        # Pixel Creator
        pixmaker = RichTools().pixelCreator()
        pixmaker.UseDetectors  = self.getProp("Detectors")
        pixmaker.DoBookKeeping = self.getProp("BookKeeping")

        # pixel cleaning
        # -------------------------------------------------------------------------------------------
        from Configurables import Rich__DAQ__HPDPixelClusteringTool
        self.toolRegistry().Tools += ["Rich::DAQ::HPDPixelClusteringTool/ClusteringTool"]
        pixmaker.ApplyPixelSuppression = True
    
        pixCleanMode = self.getProp("PixelCleaning")
        
        if pixCleanMode == "HotClusters" :
            from Configurables import Rich__DAQ__HPDPixelClusterSuppressionTool

            maxClusterSize = self.getProp("MaxHotPixelClusterSize")
            
            # RICH1 cleaning
            r1Cleaner = Rich__DAQ__HPDPixelClusterSuppressionTool("PixelSuppressRich1")
            self.toolRegistry().Tools += [r1Cleaner]
            r1Cleaner.addTool( Rich__DAQ__HPDPixelClusteringTool, name="ClusteringTool" )
            r1Cleaner.ClusteringTool.AllowDiagonalsInClusters = True
            r1Cleaner.WhichRich = "RICH1"
            r1Cleaner.AbsoluteMaxHPDOcc      = 200
            r1Cleaner.OccCutScaleFactor      = 14
            r1Cleaner.MaxPixelClusterSize    = maxClusterSize[0]
            r1Cleaner.MinHPDOccForClustering = maxClusterSize[0]
            pixmaker.addTool( r1Cleaner )
                               
            # RICH2 cleaning
            r2Cleaner = Rich__DAQ__HPDPixelClusterSuppressionTool("PixelSuppressRich2")
            self.toolRegistry().Tools += [r2Cleaner]
            r2Cleaner.addTool( Rich__DAQ__HPDPixelClusteringTool, name="ClusteringTool" )
            r2Cleaner.ClusteringTool.AllowDiagonalsInClusters = True
            r2Cleaner.WhichRich = "RICH2"
            r2Cleaner.AbsoluteMaxHPDOcc      = 200
            r2Cleaner.OccCutScaleFactor      = 14
            r2Cleaner.MaxPixelClusterSize    = maxClusterSize[1]
            r2Cleaner.MinHPDOccForClustering = maxClusterSize[1]
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
        pixmaker.SuppressClusterFinding = self.getProp("FindClusters") != True
        pixmaker.ApplyPixelClustering   = self.getProp("UseClustersAsPixels")
        pixmaker.addTool( Rich__DAQ__HPDPixelClusteringTool, name="PixelClusteringRich1" )
        pixmaker.addTool( Rich__DAQ__HPDPixelClusteringTool, name="PixelClusteringRich2" )
        pixmaker.PixelClusteringRich1.AllowDiagonalsInClusters = False
        pixmaker.PixelClusteringRich2.AllowDiagonalsInClusters = False
        pixmaker.PixelClusteringRich1.MaxClusterSize = 9999
        pixmaker.PixelClusteringRich2.MaxClusterSize = 9999
        
