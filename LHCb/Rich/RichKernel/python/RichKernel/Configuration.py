
## @package RichKernel
#  Base configurables for the RICH software
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.13 2009-09-03 16:51:02 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from LHCbKernel.Configuration import *

# ----------------------------------------------------------------------------------

## @class RichConfigurableUser
#  Base configurable for RICH software
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichConfigurableUser(LHCbConfigurableUser):
    
    ## Context specific default values for each property
    __slots__ = { }

    ## Context specific defaults
    _context_defaults_ = { }

    ## Get the context defaults 
    def contextDefaults(self):
        return self._context_defaults_

    ## @brief Create and return an instance of the RichTools() Configurable for the current Context
    #  @return the RichTools object
    def richTools(self):
        from Configurables import RichTools
        name = LHCbConfigurableUser.getProp(self,"Context")+"ToolRegistry"
        tools = RichTools(name)
        return tools

    ## @brief Returns the Rich Tool registry configuration object for the current context
    #  @return The tool registry configuration object
    def toolRegistry(self,common=False) :
        from Configurables import RichTools
        return self.richTools().toolRegistry(common)

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
        key = self.name() + ":" + context + ":" + option
        self.contextDefaults()[key] = value

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
            context = LHCbConfigurableUser.getProp(self,"Context").upper()
            if context.find("HLT")     != -1 : key = "HLT"
            if context.find("OFFLINE") != -1 : key = "Offline"
            key = self.name() + ":" + key + ":" + option
            if self.contextDefaults().has_key(key) :
                value = self.contextDefaults()[key]
            else:
                value = LHCbConfigurableUser.getProp(self,option)
        return value

    ## @brief Access to an instance of a CU with the correct name
    def getRichCU(self,type,shared=False):
        if shared :
            cu = type()
        else:
            name = self._instanceName(type)
            cu = type(name)
        return cu

    ## Set the output level for the given component
    def setOutputLevel(self,component):
        if self.isPropertySet("OutputLevel") :
            level =  self.getProp("OutputLevel")
            component.OutputLevel = level

    ## Make an instance of an algortithm
    def makeRichAlg(self,type,name):
        alg = type(name)
        self.setOutputLevel(alg)
        return alg

    ## Printout Configuration
    def printInfo(self,conf):
        import GaudiKernel.ProcessJobOptions
        GaudiKernel.ProcessJobOptions.PrintOn()
        log.debug(conf)
        GaudiKernel.ProcessJobOptions.PrintOff()


# ----------------------------------------------------------------------------------

## @class RichTools
#  Configuration object that gives access to the various tool types for
#  each processing context
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichTools(RichConfigurableUser):

    ## The default settings
    __slots__ = {
        "Context"              : "Offline",
        "PixelCreatorType"     : None,  
        "PhotonCreatorType"    : None,
        "PhotonRecoType"       : None,
        "PhotonPredictorType"  : None,
        "TrackCreatorType"     : None,
        "TrackSelectorType"    : None,
        "TrSegMakerType"       : None,
        "GeomEffType"          : None,
        "CkResType"            : None,
        "SignalDetEffType"     : None,
        "OutputLevel"          : INFO
        }

    ## Initialize 
    def initialize(self):
        # default values
        self.setRichDefaults( "PixelCreatorType",    { "Offline" : "RawBuffer",      "HLT" : "RawBuffer"   } )
        self.setRichDefaults( "PhotonCreatorType",   { "Offline" : "RecoPhotons",    "HLT" : "RecoPhotons" } )
        self.setRichDefaults( "PhotonRecoType",      { "Offline" : "Quartic",        "HLT" : "Quartic" } )
        #self.setRichDefaults( "PhotonRecoType",      { "Offline" : "Adaptive",        "HLT" : "Adaptive" } )
        self.setRichDefaults( "PhotonPredictorType", { "Offline" : "CKthetaBands",   "HLT" : "CKthetaBands" } )
        self.setRichDefaults( "TrackCreatorType",    { "Offline" : "RecoTracks",     "HLT" : "RecoTracks" } )
        self.setRichDefaults( "TrackSelectorType",   { "Offline" : "Reco",           "HLT" : "Reco" } )
        self.setRichDefaults( "TrSegMakerType",      { "Offline" : "Detailed",       "HLT" : "Detailed" } )
        self.setRichDefaults( "GeomEffType",         { "Offline" : "CKMassRing",     "HLT" : "CKMassRing" } )
        self.setRichDefaults( "CkResType",           { "Offline" : "Functional",     "HLT" : "Functional" } )
        self.setRichDefaults( "SignalDetEffType",    { "Offline" : "Tabulated",      "HLT" : "Tabulated" } )

    ## @brief Make an instance of the given configurable and configure this
    #         tool in the RichToolRegistry
    #  @param tooltype The tool type to instanciate
    #  @param nickname The tool name to use
    #  @param private  Make a private or public tool instance
    #  @return The newly created tool configurable
    def __makeRichTool(self,tooltype,nickname,private=False):
        context = self.getProp("Context")
        if private:
            tool = tooltype(context+nickname)
        else:
            tool = tooltype("ToolSvc."+context+"_"+nickname)
        self.toolRegistry().Tools += [tool.getType()+"/"+nickname]
        self.setOutputLevel(tool)
        return tool

    ## @brief Returns the Rich Tool registry configuration object for the current context
    #  @return The tool registry configuration object
    def toolRegistry(self,common=False):
        from Configurables import Rich__ToolRegistry
        if common :
            reg = Rich__ToolRegistry("RichToolRegistry")
        else :
            context = self.getProp("Context")
            reg = Rich__ToolRegistry(context+"_RichToolRegistry")
            reg.Context = context
        self.setOutputLevel(reg)
        return reg
    
    ## @brief Returns the appropriate ray tracing tool
    #  @param nickname The tool nickname
    #  @return The tool configurable
    def rayTracing(self,nickname="RichRayTracing",private=False) :
        from Configurables import Rich__RayTracingEigen
        tool = self.__makeRichTool( Rich__RayTracingEigen, nickname, private )
        #from Configurables import Rich__RayTracing
        #tool = self.__makeRichTool( Rich__RayTracing, nickname, private )
        return tool
    
    ## @brief Returns the appropriate Cherenkov resolution tool
    #  @param nickname The tool nickname
    #  @return The tool configurable
    def sigDetEff(self,nickname="RichSignalDetectionEff",private=False):
        type = self.getProp("SignalDetEffType")
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
        type = self.getProp("CkResType")
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
        type = self.getProp("GeomEffType")
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
        type = self.getProp("TrSegMakerType")
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
        selType = self.getProp("TrackSelectorType")
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
        trackType = self.getProp("TrackCreatorType")
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
        predType = self.getProp("PhotonPredictorType")
        if predType == "CKthetaBands" :
            from Configurables import Rich__Rec__CKthetaBandsPhotonPredictor
            tool = self.__makeRichTool( Rich__Rec__CKthetaBandsPhotonPredictor, nickname, private )
        elif predType == "Simple" :
            from Configurables import Rich__Rec__SimplePhotonPredictor
            tool = self.__makeRichTool( Rich__Rec__SimplePhotonPredictor, nickname, private )
        elif predType == "SelectAll" :
            from Configurables import Rich__Rec__SimplePhotonPredictor
            tool = self.__makeRichTool( Rich__Rec__SimplePhotonPredictor, nickname, private )
            tool.MaxTrackROI = [ 999, 999, 999 ]
            tool.MinTrackROI = [ 000, 000, 000 ]
        elif predType == "MCTrueCKPhotons" :
            #from Configurables import Rich__Rec__MC__PhotonPredictorUsingMCRichOpticalPhotons
            #tool = self.__makeRichTool( Rich__Rec__MC__PhotonPredictorUsingMCRichOpticalPhotons, nickname, private )
            from Configurables import Rich__Rec__MC__PhotonPredictorUsingMCTruth
            tool = self.__makeRichTool( Rich__Rec__MC__PhotonPredictorUsingMCTruth, nickname, private )
        else:
            raise RuntimeError("Unknown Photon Predictor '%s'"%predType)
        return tool

    ## @brief Returns the appropriate RichRecPixel creator tool
    #  @param nickname The tool nickname
    #  @return The tool configurable
    def pixelCreator(self,nickname="RichPixelCreator",private=False):
        pixtype = self.getProp("PixelCreatorType")
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
            tool = self.__makeRichTool( Rich__Rec__PixelCreatorWithForcedIneffic, nickname, private )
        elif pixtype == "RawBufferWithPanelFlip":
            from Configurables import Rich__Rec__PixelCreatorWithPanelFlip
            tool = self.__makeRichTool( Rich__Rec__PixelCreatorWithPanelFlip, nickname, private )
        elif pixtype == "AllMCRichHits":
            from Configurables import Rich__Rec__MC__PixelCreatorFromAllMCRichHits
            tool = self.__makeRichTool( Rich__Rec__MC__PixelCreatorFromAllMCRichHits, nickname, private ) 
        else:
            raise RuntimeError("Unknown Pixel Creator '%s'"%pixtype)
        return tool

    ## @brief Returns the appropriate RichRecPhoton creator tool
    #  @param nickname The tool nickname
    #  @return The tool configurable
    def photonCreator(self,nickname="RichPhotonCreator",private=False):
        phottype = self.getProp("PhotonCreatorType")
        if phottype == "RecoPhotons" :
            from Configurables import Rich__Rec__PhotonCreator 
            tool = self.__makeRichTool( Rich__Rec__PhotonCreator, nickname, private )
        elif phottype == "CheatedTrackDir" :
            from Configurables import ( Rich__Rec__PhotonCreator,
                                        Rich__Rec__MC__PhotonCreatorCheatedTrackDir )
            tool = self.__makeRichTool( Rich__Rec__MC__PhotonCreatorCheatedTrackDir, nickname, private )
            self.__makeRichTool( Rich__Rec__PhotonCreator, "RichDelPhotonCreator", False )
        elif phottype == "SmearCKTheta" :
            from Configurables import ( Rich__Rec__PhotonCreator,
                                        Rich__Rec__MC__PhotonCreatorWithGaussianCKSmear )
            tool = self.__makeRichTool( Rich__Rec__MC__PhotonCreatorWithGaussianCKSmear, nickname, private )
            self.__makeRichTool( Rich__Rec__PhotonCreator, "RichDelegatedPhotonCreator", False )
        else:
            raise RuntimeError("Unknown Photon Creator '%s'"%phottype)
        return tool

    ## @brief Returns the appropriate RichGeomPhoton reconstruction tool
    #  @param nickname The tool nickname
    #  @return The tool configurable
    def photonReco(self,nickname="PhotonParams",private=True):
        recoType = self.getProp("PhotonRecoType")
        if recoType == "Quartic" :
            from Configurables import Rich__Rec__PhotonRecoUsingQuarticSoln
            tool = self.__makeRichTool( Rich__Rec__PhotonRecoUsingQuarticSoln, nickname, private )
        elif recoType == "EstiFromRadius" :
            from Configurables import Rich__Rec__PhotonRecoUsingCKEstiFromRadius
            tool = self.__makeRichTool( Rich__Rec__PhotonRecoUsingCKEstiFromRadius, nickname, private )
        elif recoType == "Adaptive" :
            from Configurables import ( Rich__Rec__AdaptivePhotonReco,
                                        Rich__Rec__PhotonRecoUsingCKEstiFromRadius )
            slowName = "AdaptiveSlowPhotonReco"
            fastName = "AdaptiveFastPhotonReco"
            tool = self.__makeRichTool( Rich__Rec__AdaptivePhotonReco, nickname, private )
            self.toolRegistry().Tools += [ "Rich::Rec::PhotonRecoUsingQuarticSoln/"+slowName,
                                           "Rich::Rec::PhotonRecoUsingCKEstiFromRadius/"+fastName ]
            tool.addTool( Rich__Rec__PhotonRecoUsingCKEstiFromRadius, fastName )
            getattr(tool,fastName).RejectAmbiguousPhotons = True
        else:
            raise RuntimeError("Unknown Photon Reco '%s'"%recoType)
        return tool

    ## @brief Raw Event encode/decode tool
    def smartIDTool(self):
        from Configurables import ( Rich__DAQ__RawDataFormatTool,
                                    Rich__DAQ__RawBufferToSmartIDsTool )
        smarts  = Rich__DAQ__RawBufferToSmartIDsTool("ToolSvc.RichSmartIDDecoder")
        self.setOutputLevel(smarts)
        smarts.addTool( Rich__DAQ__RawDataFormatTool, name = "RawDecoder" )
        return smarts

    ## @brief Raw Event encode/decode tool
    def rawDecoder(self):
        return self.smartIDTool().RawDecoder

    ## @brief Expected track signal tool
    def expectedTrackSignal(self,nickname="RichExpectedTrackSignal",private=False):
        from Configurables import Rich__Rec__ExpectedTrackSignal
        return self.__makeRichTool( Rich__Rec__ExpectedTrackSignal, nickname, private )

    ## @brief CK Mass Hypothesis Ring creator
    def massHypoRingCreator(self,nickname="RichMassHypoRings",private=False):
        from Configurables import Rich__Rec__MassHypothesisRingCreator
        return self.__makeRichTool( Rich__Rec__MassHypothesisRingCreator, nickname, private )
    
# ----------------------------------------------------------------------------------

## @class RichTools
#  Configuration object that stores the TAE structure
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   01/09/2009
class RichTAEStruture(RichConfigurableUser):

    ## The default settings
    __slots__ = { "RawEventLocations" : [ "" ] }

