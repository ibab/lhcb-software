
## @package RichKernel
#  Base configurables for the RICH software
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.2 2008-08-15 14:19:23 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from LHCbKernel.Configuration import *
from Configurables import ( Rich__ToolRegistry )

# ----------------------------------------------------------------------------------

## @class RichConfigurableUser
#  Base configurable for RICH software
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichConfigurableUser(LHCbConfigurableUser):

    ## @brief Returns the Rich Tool registry configuration object for the current context
    #  @return The tool registry configuration object
    def toolRegistry(self):
        context = self.getProp("context")
        reg = Rich__ToolRegistry(context+"_RichToolRegistry")
        reg.Context = context
        return reg

    ## @brief Checks in a given property is properly configured for all three RICH radiators
    #  @param option The option name to check
    #  @param default Incase the property is not configured, the default values to use
    def checkRadData(self,option,default):
        if len(self.getProp(option)) == 0 : self.setProp(option,default)
        if len(self.getProp(option)) != 3:
            raise RuntimeError("Invalid radiator option '%s'=%s"%option%self.getProp(option))

# ----------------------------------------------------------------------------------

## @class RichTools
#  Configuration object that gives access to the various tool types for
#  each processing context
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichTools(RichConfigurableUser):

    ## The default RichRecPixel creator tools
    DefaultPixelCreator    = { "Offline" : "RawBuffer",    "HLT" : "RawBuffer"   }
    ## The default RichRecPhoton creator tools
    DefaultPhotonCreator   = { "Offline" : "RecoPhotons",  "HLT" : "RecoPhotons" }
    ## The default Cherenkov photon (RichGeomPhoton) reconstruction tools
    DefaultPhotonReco      = { "Offline" : "Quartic",      "HLT" : "EstiFromRadius" }
    ## The default photon predictor (pixel/segment pre-selection) tools
    DefaultPhotonPredictor = { "Offline" : "CKthetaBands", "HLT" : "CKthetaBands" }
    ## The default RichRecTrack creator tools
    DefaultTrackCreator    = { "Offline" : "RecoTracks",   "HLT" : "RecoTracks" }
    ## The default track selector tools
    DefaultTrackSelector   = { "Offline" : "Reco",         "HLT" : "Reco" }
    ## The default RichTrackSegment reconstruction tools
    DefaultTrSegMaker      = { "Offline" : "Detailed",     "HLT" : "Detailed" }
    ## The default geometrical efficiency tools
    DefaultGeomEff         = { "Offline" : "CKMassRing",   "HLT" : "CKMassRing" }
    ## The default CK theta track resolution tools
    DefaultCKRes           = { "Offline" : "Binned",       "HLT" : "Binned" }
    ## Default signal detection efficiency tools
    DefaultSigDetEff       = { "Offline" : "Tabulated",    "HLT" : "NominalTabulated" }

    ## The default settings
    __slots__ = {
        "context"              : "Offline",
        "pixelCreatorType"     : "UNDEFINED",  
        "photonCreatorType"    : "UNDEFINED",
        "photonRecoType"       : "UNDEFINED",
        "photonPredictorType"  : "UNDEFINED",
        "trackCreatorType"     : "UNDEFINED",
        "trackSelectorType"    : "UNDEFINED",
        "trSegMakerType"       : "UNDEFINED",
        "geomEffType"          : "UNDEFINED",
        "ckResType"            : "UNDEFINED",
        "signalDetEffType"     : "UNDEFINED"
        }

    ## Flag to indicate if the object has been properly configured
    _RichConfChecked_ = False

    ## @brief Make an instance of the given configurable and configure this
    #         tool in the RichToolRegistry
    #  @param tooltype The tool type to instanciate
    #  @param nickname The tool name to use
    #  @param private  Make a private or public tool instance
    #  @return The newly created tool configurable
    def __makeRichTool(self,tooltype,nickname,private=False):
        if private:
            tool = tooltype(nickname)
        else:
            context = self.getProp("context")
            tool = tooltype("ToolSvc."+context+"_"+nickname)
        self.toolRegistry().Tools += [tool.getType()+"/"+nickname]
        #print "RICH tool", tool.getType(), tool.name()
        return tool

    ## @brief Check the configuration
    #
    def checkConfiguration(self):
        if False == self._RichConfChecked_ :
            _RichConfChecked_ = True
            context = self.getProp("context")
            if self.getProp("pixelCreatorType") == "UNDEFINED":
                self.setProp("pixelCreatorType",self.DefaultPixelCreator[context])
            if self.getProp("photonCreatorType") == "UNDEFINED":
                self.setProp("photonCreatorType",self.DefaultPhotonCreator[context])
            if self.getProp("photonRecoType") == "UNDEFINED":
                self.setProp("photonRecoType",self.DefaultPhotonReco[context])
            if self.getProp("photonPredictorType") == "UNDEFINED":
                self.setProp("photonPredictorType",self.DefaultPhotonPredictor[context])
            if self.getProp("trackCreatorType") == "UNDEFINED":
                self.setProp("trackCreatorType",self.DefaultTrackCreator[context])
            if self.getProp("trackSelectorType") == "UNDEFINED":
                self.setProp("trackSelectorType",self.DefaultTrackSelector[context])
            if self.getProp("trSegMakerType") == "UNDEFINED":
                self.setProp("trSegMakerType",self.DefaultTrSegMaker[context])
            if self.getProp("geomEffType") == "UNDEFINED":
                self.setProp("geomEffType",self.DefaultGeomEff[context])
            if self.getProp("ckResType") == "UNDEFINED":
                self.setProp("ckResType",self.DefaultCKRes[context])
            if self.getProp("signalDetEffType") == "UNDEFINED":
                self.setProp("signalDetEffType",self.DefaultSigDetEff[context])
    
    ## @brief Returns the appropriate Cherenkov resolution tool
    #  @param nickname The tool nickname
    #  @return The tool configurable
    def sigDetEff(self,nickname="RichSignalDetectionEff"):
        type = self.getProp("signalDetEffType")
        if type == "Tabulated" :
            from Configurables import Rich__Rec__TabulatedSignalDetectionEff
            tool = self.__makeRichTool( Rich__Rec__TabulatedSignalDetectionEff, nickname )
        elif type == "NominalTabulated" :
            from Configurables import Rich__Rec__NominalTabulatedSignalDetectionEff
            tool = self.__makeRichTool( Rich__Rec__NominalTabulatedSignalDetectionEff, nickname )
        else:
            raise RuntimeError("Unknown signal detection efficiency tool '%s'"%type)
        return tool
    
    ## @brief Returns the appropriate Cherenkov resolution tool
    #  @param nickname The tool nickname
    #  @return The tool configurable
    def ckResolution(self,nickname="RichCherenkovResolution"):
        type = self.getProp("ckResType")
        if type == "Binned" :
            from Configurables import Rich__Rec__BinnedCKResVthetaForRecoTracks
            tool = self.__makeRichTool( Rich__Rec__BinnedCKResVthetaForRecoTracks, nickname )
        elif type == "Interpolated":
            from Configurables import Rich__Rec__InterpCKResVthetaForRecoTracks
            tool = self.__makeRichTool( Rich__Rec__InterpCKResVthetaForRecoTracks, nickname )
        elif type == "Functional" :
            from Configurables import Rich__Rec__FunctionalCKResForRecoTracks
            tool = self.__makeRichTool( Rich__Rec__FunctionalCKResForRecoTracks, nickname )
        else:
            raise RuntimeError("Unknown CK resolution tool '%s'"%type)
        return tool

    ## @brief Returns the appropriate geometrical efficiency tool
    #  @param nickname The tool nickname
    #  @return The tool configurable      
    def geomEff(self,nickname="RichGeomEff"):
        type = self.getProp("geomEffType")
        if type == "CKMassRing":
            from Configurables import Rich__Rec__GeomEffCKMassRing
            tool = self.__makeRichTool( Rich__Rec__GeomEffCKMassRing, nickname )
        elif type == "PhotonTracing":
            from Configurables import Rich__Rec__GeomEffPhotonTracing
            tool = self.__makeRichTool( Rich__Rec__GeomEffPhotonTracing, nickname )
        elif type == "FixedValue":
            from Configurables import Rich__Rec__GeomEffFixedValue
            tool = self.__makeRichTool( Rich__Rec__GeomEffFixedValue, nickname )
        else:
            raise RuntimeError("Unknown geom eff tool '%s'"%type)
        return tool
    
    ## @brief Returns the appropriate RichTrackSegment reconstruction tool
    #  @param nickname The tool nickname
    #  @return The tool configurable
    def trSegMaker(self,nickname="TrSegMaker"):
        self.checkConfiguration()
        type = self.getProp("trSegMakerType")
        if type == "Detailed":
            from Configurables import Rich__Rec__DetailedTrSegMakerFromRecoTracks
            tool = self.__makeRichTool( Rich__Rec__DetailedTrSegMakerFromRecoTracks, nickname, True )
        elif type == "Fast":
            from Configurables import Rich__Rec__FastTrSegMakerFromRecoTracks
            tool = self.__makeRichTool( Rich__Rec__FastTrSegMakerFromRecoTracks, nickname, True )
        elif type == "MCRichTracks":
            from Configurables import Rich__Rec__MC__TrSegMakerFromMCRichTracks
            tool = self.__makeRichTool( Rich__Rec__MC__TrSegMakerFromMCRichTracks, nickname, True )
        else:
            raise RuntimeError("Unknown TrSegMaker '%s'"%type)
        return tool
    
    ## @brief Returns the appropriate track selector tool
    #  @param nickname The tool nickname
    #  @return The tool configurable  
    def trackSelector(self,nickname="TrackSelector"):
        self.checkConfiguration()
        selType = self.getProp("trackSelectorType")
        if selType == "Reco":
            from Configurables import Rich__Rec__TrackSelector
            tool = self.__makeRichTool( Rich__Rec__TrackSelector, nickname )
        elif selType == "MCTrue":
            from Configurables import Rich__Rec__MC__MCTrueTrackSelector
            tool = self.__makeRichTool( Rich__Rec__MC__MCTrueTrackSelector, nickname )
        else:
            raise RuntimeError("Unknown Track Selector '%s'"%selType)
        return tool
    
    ## @brief Returns the appropriate RichRecTrack creator tool
    #  @param nickname The tool nickname
    #  @return The tool configurable
    def trackCreator(self,nickname="RichTrackCreator"):
        self.checkConfiguration()
        trackType = self.getProp("trackCreatorType")
        if trackType == "RecoTracks" :
            from Configurables import Rich__Rec__TrackCreatorFromRecoTracks
            tool = self.__makeRichTool( Rich__Rec__TrackCreatorFromRecoTracks, nickname )
        elif trackType == "RecoInefficTracks":
            from Configurables import Rich__Rec__TrackCreatorFromRecoTracksWithForcedIneffic
            tool = self.__makeRichTool( Rich__Rec__TrackCreatorFromRecoTracksWithForcedIneffic, nickname )
        elif trackType == "CheatedRecoTracks":
            print "Not Yet Done"
        else:
            raise RuntimeError("Unknown Track Creator '%s'"%trackType)
        return tool
    
    ## @brief Returns the appropriate RichRecPhoton predictor tool
    #  @param nickname The tool nickname
    #  @return The tool configurable
    def photonPredictor(self,nickname="RichPhotonPredictor"):
        self.checkConfiguration()
        predType = self.getProp("photonPredictorType")
        if predType == "CKthetaBands" :
            from Configurables import Rich__Rec__CKthetaBandsPhotonPredictor
            tool = self.__makeRichTool( Rich__Rec__CKthetaBandsPhotonPredictor, nickname )
        elif predType == "SelectAll" :
            from Configurables import Rich__Rec__SimplePhotonPredictor
            tool = self.__makeRichTool( Rich__Rec__SimplePhotonPredictor, nickname )
        else:
            raise RuntimeError("Unknown Photon Predictor '%s'"%predType)
        return tool
    
    ## @brief Returns the appropriate RichRecPixel creator tool
    #  @param nickname The tool nickname
    #  @return The tool configurable
    def pixelCreator(self,nickname="RichPixelCreator"):
        self.checkConfiguration()
        pixtype = self.getProp("pixelCreatorType")
        if pixtype == "RawBuffer":
            from Configurables import Rich__Rec__PixelCreatorFromRawBuffer
            tool = self.__makeRichTool( Rich__Rec__PixelCreatorFromRawBuffer, nickname )
        elif pixtype == "CheatedRawBuffer":
            from Configurables import Rich__Rec__MC__PixelCreatorFromCheatedRawBuffer
            tool = self.__makeRichTool( Rich__Rec__MC__PixelCreatorFromCheatedRawBuffer, nickname )
        elif pixtype == "SignalRawBuffer":
            from Configurables import Rich__Rec__MC__PixelCreatorFromSignalRawBuffer
            tool = self.__makeRichTool( Rich__Rec__MC__PixelCreatorFromSignalRawBuffer, nickname )
        elif pixtype == "RawBufferWithIneffic":
            from Configurables import Rich__Rec__PixelCreatorWithForcedIneffic
            tool = self.__makeRichTool(Rich__Rec__PixelCreatorWithForcedIneffic,nickname)
        else:
            raise RuntimeError("Unknown Pixel Creator '%s'"%pixtype)
        return tool

    ## @brief Returns the appropriate RichRecPhoton creator tool
    #  @param nickname The tool nickname
    #  @return The tool configurable
    def photonCreator(self,nickname="RichPhotonCreator"):
        self.checkConfiguration()
        phottype = self.getProp("photonCreatorType")
        if phottype == "RecoPhotons" :
            from Configurables import Rich__Rec__PhotonCreator 
            tool = self.__makeRichTool( Rich__Rec__PhotonCreator, nickname )
        else:
            raise RuntimeError("Unknown Photon Creator '%s'"%phottype)
        return tool
    
    ## @brief Returns the appropriate RichGeomPhoton reconstruction tool
    #  @param nickname The tool nickname
    #  @return The tool configurable
    def photonReco(self,nickname="PhotonParams"):
        self.checkConfiguration()
        recoType = self.getProp("photonRecoType")
        if recoType == "Quartic" :
            from Configurables import Rich__Rec__PhotonRecoUsingQuarticSoln
            tool = self.__makeRichTool( Rich__Rec__PhotonRecoUsingQuarticSoln, nickname, True )
        elif recoType == "EstiFromRadius" :
            from Configurables import Rich__Rec__PhotonRecoUsingCKEstiFromRadius
            tool = self.__makeRichTool( Rich__Rec__PhotonRecoUsingCKEstiFromRadius, nickname, True )
        else:
            raise RuntimeError("Unknown Photon Reco '%s'"%recoType)
        return tool
