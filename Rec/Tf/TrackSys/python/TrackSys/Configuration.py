
## @package TrackSys
#  High level configuration for LHCb Tracking software
#  @author Marco Cattaneo <Marco.Cattaneo@cern.ch>
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.17 2009-12-10 19:11:14 smenzeme Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from LHCbKernel.Configuration import *
from Configurables import DecodeRawEvent

## @class TrackSys
#  High level configuration for LHCb Tracking software
#  @author Marco Cattaneo <Marco.Cattaneo@cern.ch>
#  @date   15/08/2008
class TrackSys(LHCbConfigurableUser):
    #not actually used in *this* configurable, but used in the tracking
    #python functions which are sensitive to this configurable
    #some of which change the active decoders, see task #50179
    __used_configurables__=[DecodeRawEvent]
    
    # Steering options
    __slots__ = {
        "ExpertHistos": False # set to True to write out expert histos
       ,"SpecialData"  : []   # Various special data processing options.
       ,"ExpertTracking":  []  # list of expert Tracking options, see KnownExpertTracking
       ,"TrackPatRecAlgorithms": []  # List of track pattern recognition algorithms to run
       ,"TrackExtraInfoAlgorithms": []  # List of track 'extra info' algorithms to run
       ,"WithMC":       False # set to True to use MC truth
       ,"OutputType": "" # DST type, currently not used
       ,"FilterBeforeFit": True  #Clone kill before fit of the Best container only. False = fit before clone killing
       ,"DataType": "2011" # propagated from Brunel(), used to determine which monitors to run
       ,"ForceNewSeq" : False  # use new tracking sequence independent of DataType
       ,"ForceOldSeq" : False  # use old tracking sequence independent of DataType
       ,"GlobalCuts"  : {}     # global event cuts for tracking
       ,"OldCloneKiller" : False # Switch between old and new (in 2012) clone killers
       ,"Simulation" : False # True if using simulated data/SIMCOND
       ,"TrackTypes" : [] 
        }
    
    ## Possible expert options
    KnownExpertTracking        = ["noDrifttimes", "simplifiedGeometry", "kalmanSmoother", "noMaterialCorrections",
                                  "fastSequence", "timing", "disableOTTimeWindow", "fullGeometryHLT1", "fullGeometryHLT2",
                                  "fullClustersHLT1", "fullClustersHLT2" ]

    ## Default track pattern recognition algorithms to run in 2010
    #DefaultPatRecAlgorithms_old    = ["Velo","Forward","TsaSeed","Match","Downstream","VeloTT"]
    DefaultPatRecAlgorithms_old    = ["Velo","Forward","TsaSeed","PatMatch","Downstream","VeloTT"]
    ## Default global cuts before 2011
    DefaultGlobalCuts_old      = {}
    ## Default track pattern recognition algorithms to run in 2011
    DefaultPatRecAlgorithms    = ["FastVelo","Forward","PatSeed","PatMatch","Downstream","VeloTT"]
    ## Default track pattern recognition algorithms to run in Run II
    DefaultPatRecAlgorithmsRun2    = ["FastVelo","ForwardHLT1","ForwardHLT2","PatSeed","PatMatch","Downstream","VeloTT"]
    ## Default global cuts from 2011
    DefaultGlobalCuts          = { 'Velo':6000, 'IT':999999, 'OT':15000 }
    ## Global cuts for pA, from 2013
    DefaultpAGlobalCuts        = { 'Velo':80000, 'IT':999999, 'OT':999999 }
    ## Default track 'extra info' algorithms to run
    DefaultExtraInfoAlgorithms = ["CloneFlagging","TrackLikelihood","GhostProbability"]
    ## Cosmic track pattern recognition algorithms to run
    CosmicPatRecAlgorithms    = ["PatSeed"]
    ## Cosmic expert switches
    CosmicExpertTracking      = ["noDrifttimes"] 
    ## DefaultTrackTypes
    DefaultTrackTypes = ["Velo","Upstream","Forward","Seeding","Match","Downstream"]
        
    ## @brief Check the options are sane etc.
    def defineOptions(self):
        
      if self.getProp( "ForceNewSeq" ) and self.getProp( "ForceOldSeq" ) :
         raise RuntimeError("Cannot force both old and new tracking sequence at once")

      if self.getProp( "ForceNewSeq" ) :
         self.setProp("TrackPatRecAlgorithms",self.DefaultPatRecAlgorithms)

      if self.getProp( "ForceOldSeq" ):
         self.setProp("TrackPatRecAlgorithms",self.DefaultPatRecAlgorithms_old)

      for track in self.getProp( "TrackTypes" ):
          if track not in self.DefaultTrackTypes:
              log.warning("Using non-default track type: '%s'" %track)
              log.warning("Default track types are: %s" %self.DefaultTrackTypes)

                  
      if "cosmics" not in self.getProp("SpecialData"):
          # Defaults changes starting in 2011
          if "MC09" == self.getProp("DataType") or "2008" == self.getProp("DataType") or "2009" == self.getProp("DataType") :
              defaultPatRecAlgorithms = self.DefaultPatRecAlgorithms_old
              defaultGlobalCuts       = self.DefaultGlobalCuts_old
          else:
              defaultPatRecAlgorithms = self.DefaultPatRecAlgorithms
              defaultGlobalCuts       = self.DefaultGlobalCuts
          # for Run II we will use a different sequence
          if "2015" == self.getProp("DataType"):
              defaultPatRecAlgorithms = self.DefaultPatRecAlgorithmsRun2
          
          ##
          if "pA" in  self.getProp("SpecialData"):
              defaultGlobalCuts       = self.DefaultpAGlobalCuts
          
          if len(self.getProp("TrackPatRecAlgorithms")) == 0 :
              self.setProp("TrackPatRecAlgorithms",defaultPatRecAlgorithms)
          if len(self.getProp("TrackExtraInfoAlgorithms")) == 0 :
              self.setProp("TrackExtraInfoAlgorithms",self.DefaultExtraInfoAlgorithms)
          for prop in self.getProp("ExpertTracking"):
              if prop not in self.KnownExpertTracking:
                  raise RuntimeError("Unknown expertTracking option '%s'"%prop)
          if len(self.getProp("GlobalCuts")) == 0 :
              self.setProp("GlobalCuts",defaultGlobalCuts)
          else:
              globalCuts = self.getProp("GlobalCuts")
              for key in globalCuts:
                  if key not in ["Velo","IT","OT"]:
                      raise RuntimeError("Invalid key for TrackSys().GlobalCuts: '%s'"%key)
              for key in defaultGlobalCuts:
                  if key not in globalCuts:
                      globalCuts[key] = defaultGlobalCuts[key]
              log.warning("Using non-default global tracking cuts: '%s'"%globalCuts)
              self.setProp("GlobalCuts",globalCuts)
          
          if len(self.getProp("TrackExtraInfoAlgorithms")) == 0 :
              self.setProp("TrackExtraInfoAlgorithms",self.DefaultExtraInfoAlgorithms)
      else:
          # Cosmics case
          if len(self.getProp("TrackPatRecAlgorithms")) == 0 :
              self.setProp("TrackPatRecAlgorithms",self.CosmicPatRecAlgorithms)
          if len(self.getProp("ExpertTracking")) == 0 :
              self.setProp("ExpertTracking",self.CosmicExpertTracking)

          
    ## @brief Shortcut to the fieldOff option
    def MC09(self)     : return "MC09" == self.getProp( "DataType" )
    ## @brief Shortcut to the fieldOff option
    def fieldOff(self) : return "fieldOff" in self.getProp("SpecialData")
    ## @brief Shortcut to the veloOpen option
    def veloOpen(self) : return "veloOpen" in self.getProp("SpecialData")
    ## @brief Shortcut to the cosmics option
    def cosmics(self)  : return "cosmics"  in self.getProp("SpecialData")
    ## @brief Shortcut to the beamGas option
    def beamGas(self)  : return "beamGas"  in self.getProp("SpecialData")
    ## @brief Shortcut to the noDrifttimes option
    def noDrifttimes(self) : return "noDrifttimes" in self.getProp("ExpertTracking")
    ## @brief Shortcut to the simplifiedGeometry option
    def simplifiedGeometry(self) : return "simplifiedGeometry" in self.getProp("ExpertTracking")
    ## @brief Shortcut to the fullGeometry option for HLT1 in Run 2
    def fullGeometryHLT1(self) : return "fullGeometryHLT1" in self.getProp("ExpertTracking")
    ## @brief Shortcut to the fullGeometry option for HLT2 in Run 2
    def fullGeometryHLT2(self) : return "fullGeometryHLT2" in self.getProp("ExpertTracking")
    ## @brief Shortcut to the fullCluster option for HLT1 (in fitter) in Run 2
    def fullClustersHLT1(self) : return "fullClustersHLT1" in self.getProp("ExpertTracking")
    ## @brief Shortcut to the fullCluster option for HLT2 (in fitter) in Run 2
    def fullClustersHLT2(self) : return "fullClustersHLT2" in self.getProp("ExpertTracking")
    ## @brief Shortcut to the kalmanSmoother option
    def kalmanSmoother(self) : return "kalmanSmoother" in self.getProp("ExpertTracking")
    ## @brief Shortcut to the noMaterialCorrections option
    def noMaterialCorrections(self) : return "noMaterialCorrections" in self.getProp("ExpertTracking")
    ## @brief Shortcut to the timing option
    def timing(self) : return "timing" in self.getProp("ExpertTracking")
    ### @brief Shortcut to the upgrade option
    #def upgrade(self): return "upgrade" in self.getProp("SpecialData")

    

    ## @brief Option to enable setting the special data options easily
    def setSpecialDataOption(self,option,value):
        optSet = option in self.getProp("SpecialData")
        if   True  == value and False == optSet : self.getProp("SpecialData").append(option)
        elif False == value and True  == optSet : self.getProp("SpecialData").remove(option)

    ## @brief Apply the configuration
    def __apply_configuration__(self):
        self.defineOptions()
        useUpgrade = False
        from Configurables import LHCbApp
        upgradeDets = []
        #Test if LHCbApp has this method (same revision as property)
        if hasattr(LHCbApp(),"Detectors"):
            if LHCbApp().isPropertySet("Detectors"):
                upgradeDets = LHCbApp().upgradeDetectors()
                useUpgrade = bool([det for det in upgradeDets if det in ['VP','UT','FT']])
        if not self.isPropertySet("TrackTypes"):
            self.setProp("TrackTypes", self.DefaultTrackTypes)
        
        if useUpgrade :
            from TrackSys import RecoUpgradeTracking
            RecoUpgradeTracking.RecoUpgradeTracking()
        else:
            if self.getProp( "FilterBeforeFit" ):
                if  "2015" == self.getProp("DataType"):
                    from TrackSys import RecoTrackingRun2
                    # per default, run simplified geometry and use liteClusters
                    RecoTrackingRun2.RecoTrackingHLT1( simplifiedGeometryFit = not self.fullGeometryHLT1(), liteClustersFit = not self.fullClustersHLT1() )
                    RecoTrackingRun2.RecoTrackingHLT2( simplifiedGeometryFit = not self.fullGeometryHLT2(), liteClustersFit = not self.fullClustersHLT2() )
                else:
                    from TrackSys import RecoTracking
                    RecoTracking.RecoTracking()
                
                   
                    
            else:
                raise RuntimeError("'FilterBeforeFit' needs to be set...")
                #RecoTracking.RecoTracking()
            #if self.upgrade():
            #    from RecoUpgrade import RecoTrackingUpgrade
            #    RecoTrackingUpgrade.RecoTrackingUpgrade()
            

        if self.getProp( "WithMC" ):
            if useUpgrade :
                from TrackSys import PrUpgradeChecking
                PrUpgradeChecking.PrUpgradeChecking()
            else :
                from TrackSys import PatChecking
                PatChecking.PatChecking()
           
                
            #old upgrade configurable
            #if self.upgrade():
            #    from RecoUpgrade import PatCheckingUpgrade
            #    PatCheckingUpgrade.PatCheckingUpgrade()

