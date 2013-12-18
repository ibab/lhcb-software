
## @package PixelCreator
#  Pixel creator options for RICH Reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: PixelCreator.py,v 1.8 2009-09-16 13:37:28 jonrob Exp $"
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
        "Context"                : "Offline"   # The context within which to run
       ,"DataType"               : ""          # Type of data, propagated from application
       ,"SpecialData"            : []   # Various special data processing options. See KnownSpecialData in RecSys for all options
       ,"Detectors"              : [ True, True ] # Which RICH detectors to use (RICH1/RICH2)
       ,"PixelCleaning"          : "HotHPDs" # Turn on RICH pixel cleaning (hot HPDs etc.)
       ,"FindClusters"           : True # Find clusters in the HPD data
       ,"UseClustersAsPixels"    : [ False, False ] # Use clusters as the raw pixel objects
       ,"MaxHotPixelClusterSize" : [ 10, 8 ] # Max size of clusters for HotClusters pixel cleaning (RICH1,RICH2)
       ,"MaxPixels"              : 30000 # Max pixels per event
       ,"MaxHPDOccupancy"        : [ 200,  200  ] # Max HPD occupancy for HPD cleaning
       ,"HPDOccScaleFactor"      : [ 9999, 9999 ]   # Scale factor for HPD occupancy fluctuations (was [14,14])
       ,"BookKeeping"            : False
       ,"OutputLevel"            : INFO # The output level to set all algorithms and tools to use
        }

    ## Initialize 
    def initialize(self):
        # default values
        self.setRichDefaults ( "MaxPixels", { "Offline" : 30000, 
                                              "HLT"     : 30000 } )
    ## @brief Check the configuration is OK
    #
    def checkConfiguration(self):
        dets = self.getProp("Detectors")
        if len(dets) != 2 : raise RuntimeError("ERROR : Invalid Detectors '%s'"%config)

    ## @brief Apply any tweaks to the default configuration that vary by DataType
    def dataTypeTweaks(self):

        # Relaxed GECs for pA data, tuning for 2013 Reco14r1 
        if "pA" in self.getProp("SpecialData"):
            if not self.isPropertySet("MaxPixels") : self.setProp("MaxPixels", 90000)

    ## @brief Apply the configuration
    #
    def applyConf(self):

        # Check the configuration
        self.checkConfiguration()

        # DataType specific tweaks
        self.dataTypeTweaks()

        context = self.getProp("Context")

        # Pixel Creator
        pixmaker = self.richTools().pixelCreator()
        pixmaker.UseDetectors  = self.getProp("Detectors")
        pixmaker.DoBookKeeping = self.getProp("BookKeeping")

        # Max pixel limit
        pixmaker.MaxPixels = self.getProp("MaxPixels")

        # OutputLevel
        self.setOutputLevel(pixmaker)

        # pixel cleaning
        # -------------------------------------------------------------------------------------------
        from Configurables import Rich__DAQ__HPDPixelClusteringTool
        self.toolRegistry().Tools += ["Rich::DAQ::HPDPixelClusteringTool/ClusteringTool"]
 
        # Cuts
        maxClusterSize = self.getProp("MaxHotPixelClusterSize")
        maxHPDOcc      = self.getProp("MaxHPDOccupancy")
        hpdOccScale    = self.getProp("HPDOccScaleFactor")

        r1CleanerName = "PixelSuppressRich1"
        r2CleanerName = "PixelSuppressRich2"
               
        pixCleanMode = self.getProp("PixelCleaning")
        
        if pixCleanMode == "HotClusters" :
            
            from Configurables import Rich__DAQ__HPDPixelClusterSuppressionTool

            pixmaker.ApplyPixelSuppression = True
            
            # RICH1 cleaning
            pixmaker.addTool(Rich__DAQ__HPDPixelClusterSuppressionTool,name=r1CleanerName)
            r1Cleaner = pixmaker.PixelSuppressRich1
            self.toolRegistry().Tools += [r1Cleaner]
            r1Cleaner.addTool( Rich__DAQ__HPDPixelClusteringTool, name="ClusteringTool" )
            r1Cleaner.ClusteringTool.AllowDiagonalsInClusters = True
            r1Cleaner.WhichRich = "RICH1"
            r1Cleaner.AbsoluteMaxHPDOcc      = maxHPDOcc[0]
            r1Cleaner.OccCutScaleFactor      = hpdOccScale[0]
            r1Cleaner.MaxPixelClusterSize    = maxClusterSize[0]
            r1Cleaner.MinHPDOccForClustering = maxClusterSize[0]
            pixmaker.addTool( r1Cleaner )
                               
            # RICH2 cleaning
            pixmaker.addTool(Rich__DAQ__HPDPixelClusterSuppressionTool,name=r2CleanerName)
            r2Cleaner = pixmaker.PixelSuppressRich2
            self.toolRegistry().Tools += [r2Cleaner]
            r2Cleaner.addTool( Rich__DAQ__HPDPixelClusteringTool, name="ClusteringTool" )
            r2Cleaner.ClusteringTool.AllowDiagonalsInClusters = True
            r2Cleaner.WhichRich = "RICH2"
            r2Cleaner.AbsoluteMaxHPDOcc      = maxHPDOcc[1]
            r2Cleaner.OccCutScaleFactor      = hpdOccScale[1]
            r2Cleaner.MaxPixelClusterSize    = maxClusterSize[1]
            r2Cleaner.MinHPDOccForClustering = maxClusterSize[1]
            pixmaker.addTool( r2Cleaner )
  
        elif pixCleanMode == "HotHPDs" :

            from Configurables import Rich__DAQ__HighOccHPDSuppressionTool

            pixmaker.ApplyPixelSuppression = True

            # RICH1 cleaning
            pixmaker.addTool(Rich__DAQ__HighOccHPDSuppressionTool,name=r1CleanerName)
            r1Cleaner = pixmaker.PixelSuppressRich1
            r1Cleaner.WhichRich = "RICH1"
            self.toolRegistry().Tools += [r1Cleaner]
            r1Cleaner.AbsoluteMaxHPDOcc      = maxHPDOcc[0]
            r1Cleaner.OccCutScaleFactor      = hpdOccScale[0]
            
            # RICH2 cleaning
            pixmaker.addTool(Rich__DAQ__HighOccHPDSuppressionTool,name=r2CleanerName)
            r2Cleaner = pixmaker.PixelSuppressRich2
            r2Cleaner.WhichRich = "RICH2"
            self.toolRegistry().Tools += [r2Cleaner]
            r2Cleaner.AbsoluteMaxHPDOcc      = maxHPDOcc[1]
            r2Cleaner.OccCutScaleFactor      = hpdOccScale[1]
            
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
        
