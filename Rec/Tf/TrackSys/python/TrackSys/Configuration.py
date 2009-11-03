
## @package TrackSys
#  High level configuration for LHCb Tracking software
#  @author Marco Cattaneo <Marco.Cattaneo@cern.ch>
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.12 2009-11-03 12:26:28 smenzeme Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from LHCbKernel.Configuration import *

## @class TrackSys
#  High level configuration for LHCb Tracking software
#  @author Marco Cattaneo <Marco.Cattaneo@cern.ch>
#  @date   15/08/2008
class TrackSys(LHCbConfigurableUser):

    # Steering options
    __slots__ = {
        "ExpertHistos": False # set to True to write out expert histos
       ,"SpecialData"  : []   # Various special data processing options.
       ,"ExpertTracking":  []  # list of expert Tracking options, see KnownExpertTracking
       ,"TrackPatRecAlgorithms": []  # List of track pattern recognition algorithms to run
       ,"TrackExtraInfoAlgorithms": []  # List of track 'extra info' algorithms to run
       ,"WithMC":       False # set to True to use MC truth
       ,"OutputType": "" # set to "RDST" for special RDST sequence
        }
    
    ## Possible expert options
    KnownExpertTracking        = ["noDrifttimes", "simplifiedGeometry", "kalmanSmoother"]
    ## Default track pattern recognition algorithms to run
    DefaultPatRecAlgorithms    = ["Velo","Forward","TsaSeed","Match","Downstream","VeloTT"]
    ## Default track 'extra info' algorithms to run
    DefaultExtraInfoAlgorithms = ["CloneFlagging","TrackLikelihood","GhostProbability"]
    ## Cosmic track pattern recognition algorithms to run
    CosmicPatRecAlgorithms    = ["PatSeed"]
    ## Cosmic expert swithces
    CosmicExpertTracking      = ["noDrifttimes"] 
            
    ## @brief Check the options are sane etc.
    def defineOptions(self):
      if "cosmics" not in self.getProp("SpecialData"):
           if len(self.getProp("TrackPatRecAlgorithms")) == 0 :
               self.setProp("TrackPatRecAlgorithms",self.DefaultPatRecAlgorithms)
           if len(self.getProp("TrackExtraInfoAlgorithms")) == 0 :
               self.setProp("TrackExtraInfoAlgorithms",self.DefaultExtraInfoAlgorithms)
           for prop in self.getProp("ExpertTracking"):
               if prop not in self.KnownExpertTracking:
                   raise RuntimeError("Unknown expertTracking option '%s'"%prop)
      else:         
           if len(self.getProp("TrackPatRecAlgorithms")) == 0 :
               self.setProp("TrackPatRecAlgorithms",self.CosmicPatRecAlgorithms)
           if len(self.getProp("ExpertTracking")) == 0 :
               self.setProp("ExpertTracking",self.CosmicExpertTracking)            
                                                               

    ## @brief Shortcut to the fieldOff option
    def fieldOff(self) : return "fieldOff" in self.getProp("SpecialData")
    ## @brief Shortcut to the veloOpen option
    def veloOpen(self) : return "veloOpen" in self.getProp("SpecialData")
    ## @brief Shortcut to the cosmics option
    def cosmics(self)  : return "cosmics"  in self.getProp("SpecialData")
    ## @brief Shortcut to the noDrifttimes option
    def noDrifttimes(self) : return "noDrifttimes" in self.getProp("ExpertTracking")
    ## @brief Shortcut to the simplifiedGeometry option
    def simplifiedGeometry(self) : return "simplifiedGeometry" in self.getProp("ExpertTracking")
    ## @brief Shortcut to the kalmanSmoother option
    def kalmanSmoother(self) : return "kalmanSmoother" in self.getProp("ExpertTracking")

    ## @brief Option to enable setting the special data options easily
    def setSpecialDataOption(self,option,value):
        optSet = option in self.getProp("SpecialData")
        if   True  == value and False == optSet : self.getProp("SpecialData").append(option)
        elif False == value and True  == optSet : self.getProp("SpecialData").remove(option)

    ## @brief Apply the configuration
    def __apply_configuration__(self):
        self.defineOptions()
        importOptions( "$TRACKSYSROOT/options/RecoTracking.py" )
        if self.getProp( "WithMC" ):
            importOptions("$TRACKSYSROOT/options/PatChecking.py")
   
