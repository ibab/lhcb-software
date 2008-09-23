
## @package RichKernel
#  Base configurables for the RICH software
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.5 2008-09-23 14:31:40 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from LHCbKernel.Configuration import *
from Configurables import ( Rich__ToolRegistry )

# ----------------------------------------------------------------------------------

## @class RichConfigurableUser
#  Base configurable for RICH software
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichConfigurableUser(LHCbConfigurableUser):

    ## Context specific defaultvalues for each property
    _context_defaults_ = { }

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

    ## @brief Sets the default value for the given property, in the given context
    #  @param option  The option name to set
    #  @param context The processig context
    #  @param value   The default value to set
    def setRichDefault(self,option,context,value):
        self._context_defaults_[context+":"+option] = value

    ## @brief Sets the default value for the given properties and contexts
    #  @param option  The option name to set
    #  @param values  The default values to set
    def setRichDefaults(self,option,values):
        for context in values.keys() :
            self.setRichDefault(option,context,values[context])

    ## @brief Get the value for the given property
    #  @param option The property name to get
    #  @return The property value
    def getProp(self,option):
        foundValue = False
        if hasattr(self,option) :
            value = getattr(self,option)
            foundValue = value != None
        if False == foundValue:
            key = LHCbConfigurableUser.getProp(self,"context")+":"+option
            if self._context_defaults_.has_key(key) :
                value = self._context_defaults_[key]
            else:
                value = LHCbConfigurableUser.getProp(self,option)
            #else:
            #    value = LHCbConfigurableUser.getProp(self,option)
        #print self.name,"[",option,"] =",value
        return value

# ----------------------------------------------------------------------------------

## @class RichTools
#  Configuration object that gives access to the various tool types for
#  each processing context
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichTools(RichConfigurableUser):

    ## The default settings
    __slots__ = {
        "context"              : "Offline",
        "pixelCreatorType"     : None,  
        "photonCreatorType"    : None,
        "photonRecoType"       : None,
        "photonPredictorType"  : None,
        "trackCreatorType"     : None,
        "trackSelectorType"    : None,
        "trSegMakerType"       : None,
        "geomEffType"          : None,
        "ckResType"            : None,
        "signalDetEffType"     : None
        }

    ## Initialise 
    def initialise(self):
        # default values
        self.setRichDefaults( "pixelCreatorType",    { "Offline" : "RawBuffer",    "HLT" : "RawBuffer"   } )
        self.setRichDefaults( "photonCreatorType",   { "Offline" : "RecoPhotons",  "HLT" : "RecoPhotons" } )
        self.setRichDefaults( "photonRecoType",      { "Offline" : "Quartic",      "HLT" : "EstiFromRadius" } )
        self.setRichDefaults( "photonPredictorType", { "Offline" : "CKthetaBands", "HLT" : "CKthetaBands" } )
        self.setRichDefaults( "trackCreatorType",    { "Offline" : "RecoTracks",   "HLT" : "RecoTracks" } )
        self.setRichDefaults( "trackSelectorType",   { "Offline" : "Reco",         "HLT" : "Reco" } )
        self.setRichDefaults( "trSegMakerType",      { "Offline" : "Detailed",     "HLT" : "Detailed" } )
        self.setRichDefaults( "geomEffType",         { "Offline" : "CKMassRing",   "HLT" : "CKMassRing" } )
        self.setRichDefaults( "ckResType",           { "Offline" : "Binned",       "HLT" : "Binned" } )
        self.setRichDefaults( "signalDetEffType",    { "Offline" : "Tabulated",    "HLT" : "NominalTabulated" } )

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
    
    ## @brief Returns the appropriate Cherenkov resolution tool
    #  @param nickname The tool nickname
    #  @return The tool configurable
    def sigDetEff(self,nickname="RichSignalDetectionEff",private=False):
        type = self.getProp("signalDetEffType")
        if type == "Tabulated" :
            from Configurables import Rich__Rec__TabulatedSignalDetectionEff
            tool = self.__makeRichTool( Rich__Rec__TabulatedSignalDetectionEff, nickname, private )
        elif type == "NominalTabulated" :
            from Configurables import Rich__Rec__NominalTabulatedSignalDetectionEff
            tool = self.__makeRichTool( Rich__Rec__NominalTabulatedSignalDetectionEff, nickname, private )
        else:
            raise RuntimeError("Unknown signal detection efficiency tool '%s'"%type)
        return tool
    
    ## @brief Returns the appropriate Cherenkov resolution tool
    #  @param nickname The tool nickname
    #  @return The tool configurable
    def ckResolution(self,nickname="RichCherenkovResolution",private=False):
        type = self.getProp("ckResType")
        if type == "Binned" :
            from Configurables import Rich__Rec__BinnedCKResVthetaForRecoTracks
            tool = self.__makeRichTool( Rich__Rec__BinnedCKResVthetaForRecoTracks, nickname, private )
        elif type == "Interpolated":
            from Configurables import Rich__Rec__InterpCKResVthetaForRecoTracks
            tool = self.__makeRichTool( Rich__Rec__InterpCKResVthetaForRecoTracks, nickname, private )
        elif type == "Functional" :
            from Configurables import Rich__Rec__FunctionalCKResForRecoTracks
            tool = self.__makeRichTool( Rich__Rec__FunctionalCKResForRecoTracks, nickname, private )
        else:
            raise RuntimeError("Unknown CK resolution tool '%s'"%type)
        return tool

    ## @brief Returns the appropriate geometrical efficiency tool
    #  @param nickname The tool nickname
    #  @return The tool configurable      
    def geomEff(self,nickname="RichGeomEff",private=False):
        type = self.getProp("geomEffType")
        if type == "CKMassRing":
            from Configurables import Rich__Rec__GeomEffCKMassRing
            tool = self.__makeRichTool( Rich__Rec__GeomEffCKMassRing, nickname, private )
        elif type == "PhotonTracing":
            from Configurables import Rich__Rec__GeomEffPhotonTracing
            tool = self.__makeRichTool( Rich__Rec__GeomEffPhotonTracing, nickname, private )
        elif type == "FixedValue":
            from Configurables import Rich__Rec__GeomEffFixedValue
            tool = self.__makeRichTool( Rich__Rec__GeomEffFixedValue, nickname, private )
        else:
            raise RuntimeError("Unknown geom eff tool '%s'"%type)
        return tool
    
    ## @brief Returns the appropriate RichTrackSegment reconstruction tool
    #  @param nickname The tool nickname
    #  @return The tool configurable
    def trSegMaker(self,nickname="TrSegMaker",private=True):
        type = self.getProp("trSegMakerType")
        if type == "Detailed":
            from Configurables import Rich__Rec__DetailedTrSegMakerFromRecoTracks
            tool = self.__makeRichTool( Rich__Rec__DetailedTrSegMakerFromRecoTracks, nickname, private )
        elif type == "Fast":
            from Configurables import Rich__Rec__FastTrSegMakerFromRecoTracks
            tool = self.__makeRichTool( Rich__Rec__FastTrSegMakerFromRecoTracks, nickname, private )
        elif type == "MCRichTracks":
            from Configurables import Rich__Rec__MC__TrSegMakerFromMCRichTracks
            tool = self.__makeRichTool( Rich__Rec__MC__TrSegMakerFromMCRichTracks, nickname, private )
        else:
            raise RuntimeError("Unknown TrSegMaker '%s'"%type)
        return tool
    
    ## @brief Returns the appropriate track selector tool
    #  @param nickname The tool nickname
    #  @return The tool configurable  
    def trackSelector(self,nickname="TrackSelector",private=False):
        selType = self.getProp("trackSelectorType")
        if selType == "Reco":
            from Configurables import Rich__Rec__TrackSelector
            tool = self.__makeRichTool( Rich__Rec__TrackSelector, nickname, private )
        elif selType == "MCTrue":
            from Configurables import Rich__Rec__MC__MCTrueTrackSelector
            tool = self.__makeRichTool( Rich__Rec__MC__MCTrueTrackSelector, nickname, private )
        else:
            raise RuntimeError("Unknown Track Selector '%s'"%selType)
        return tool
    
    ## @brief Returns the appropriate RichRecTrack creator tool
    #  @param nickname The tool nickname
    #  @return The tool configurable
    def trackCreator(self,nickname="RichTrackCreator",private=False):
        trackType = self.getProp("trackCreatorType")
        if trackType == "RecoTracks" :
            from Configurables import Rich__Rec__TrackCreatorFromRecoTracks
            tool = self.__makeRichTool( Rich__Rec__TrackCreatorFromRecoTracks, nickname, private )
        elif trackType == "RecoInefficTracks":
            from Configurables import Rich__Rec__TrackCreatorFromRecoTracksWithForcedIneffic
            tool = self.__makeRichTool( Rich__Rec__TrackCreatorFromRecoTracksWithForcedIneffic, nickname, private )
        elif trackType == "CheatedRecoTracks":
            print "Not Yet Done"
        else:
            raise RuntimeError("Unknown Track Creator '%s'"%trackType)
        return tool
    
    ## @brief Returns the appropriate RichRecPhoton predictor tool
    #  @param nickname The tool nickname
    #  @return The tool configurable
    def photonPredictor(self,nickname="RichPhotonPredictor",private=False):
        predType = self.getProp("photonPredictorType")
        if predType == "CKthetaBands" :
            from Configurables import Rich__Rec__CKthetaBandsPhotonPredictor
            tool = self.__makeRichTool( Rich__Rec__CKthetaBandsPhotonPredictor, nickname, private )
        elif predType == "SelectAll" :
            from Configurables import Rich__Rec__SimplePhotonPredictor
            tool = self.__makeRichTool( Rich__Rec__SimplePhotonPredictor, nickname, private )
        else:
            raise RuntimeError("Unknown Photon Predictor '%s'"%predType)
        return tool
    
    ## @brief Returns the appropriate RichRecPixel creator tool
    #  @param nickname The tool nickname
    #  @return The tool configurable
    def pixelCreator(self,nickname="RichPixelCreator",private=False):
        pixtype = self.getProp("pixelCreatorType")
        if pixtype == "RawBuffer":
            from Configurables import Rich__Rec__PixelCreatorFromRawBuffer
            tool = self.__makeRichTool( Rich__Rec__PixelCreatorFromRawBuffer, nickname, private )
        elif pixtype == "CheatedRawBuffer":
            from Configurables import Rich__Rec__MC__PixelCreatorFromCheatedRawBuffer
            tool = self.__makeRichTool( Rich__Rec__MC__PixelCreatorFromCheatedRawBuffer, nickname, private )
        elif pixtype == "SignalRawBuffer":
            from Configurables import Rich__Rec__MC__PixelCreatorFromSignalRawBuffer
            tool = self.__makeRichTool( Rich__Rec__MC__PixelCreatorFromSignalRawBuffer, nickname, private )
        elif pixtype == "RawBufferWithIneffic":
            from Configurables import Rich__Rec__PixelCreatorWithForcedIneffic
            tool = self.__makeRichTool(Rich__Rec__PixelCreatorWithForcedIneffic,nickname, private)
        else:
            raise RuntimeError("Unknown Pixel Creator '%s'"%pixtype)
        return tool

    ## @brief Returns the appropriate RichRecPhoton creator tool
    #  @param nickname The tool nickname
    #  @return The tool configurable
    def photonCreator(self,nickname="RichPhotonCreator",private=False):
        phottype = self.getProp("photonCreatorType")
        if phottype == "RecoPhotons" :
            from Configurables import Rich__Rec__PhotonCreator 
            tool = self.__makeRichTool( Rich__Rec__PhotonCreator, nickname, private )
        else:
            raise RuntimeError("Unknown Photon Creator '%s'"%phottype)
        return tool
    
    ## @brief Returns the appropriate RichGeomPhoton reconstruction tool
    #  @param nickname The tool nickname
    #  @return The tool configurable
    def photonReco(self,nickname="PhotonParams",private=True):
        recoType = self.getProp("photonRecoType")
        if recoType == "Quartic" :
            from Configurables import Rich__Rec__PhotonRecoUsingQuarticSoln
            tool = self.__makeRichTool( Rich__Rec__PhotonRecoUsingQuarticSoln, nickname, private )
        elif recoType == "EstiFromRadius" :
            from Configurables import Rich__Rec__PhotonRecoUsingCKEstiFromRadius
            tool = self.__makeRichTool( Rich__Rec__PhotonRecoUsingCKEstiFromRadius, nickname, private )
        else:
            raise RuntimeError("Unknown Photon Reco '%s'"%recoType)
        return tool

    ## @brief Raw Event encode/decode tool
    def smartIDTool(self):
        from Configurables import ( Rich__DAQ__RawDataFormatTool,
                                    Rich__DAQ__RawBufferToSmartIDsTool )
        smarts  = Rich__DAQ__RawBufferToSmartIDsTool("ToolSvc.RichSmartIDDecoder")
        decoder = Rich__DAQ__RawDataFormatTool("RawDecoder")
        smarts.addTool(decoder)
        return smarts

    ## @brief Raw Event encode/decode tool
    def rawDecoder(self):
        return self.smartIDTool().RawDecoder
        
