
## @package RichRecSys
#  High level Configuration tools for RICH Reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.16 2009-03-10 16:17:47 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from TrackCreator  import *
from PixelCreator  import *
from PhotonCreator import *
from CKThetaResolution import *
from RichMarkovRingFinder.Configuration import *
#from Configurables import RichTemplateRingFinderConf
from RichGlobalPID_ import RichGlobalPIDConfig
from RichHLTLocalPID_ import RichHLTLocalPIDConfig
from Configurables import GaudiSequencer

# ----------------------------------------------------------------------------------

## @class RichRecSysConf
#  Configurable for RICH reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichRecSysConf(RichConfigurableUser):

    ## Possible used Configurables
    __used_configurables__ = [ RichHLTLocalPIDConfig,
                               RichMarkovRingFinderConf,
                               #RichTemplateRingFinderConf,
                               RichGlobalPIDConfig,
                               RichTrackCreatorConfig,
                               RichPixelCreatorConfig,
                               RichPhotonCreatorConfig,
                               CKThetaResolutionConfig
                               ]
    
    ## Known contexts
    KnownContexts = [ "Offline", "HLT" ]
    
    ## Steering options
    __slots__ = {
        "UseCaloMomentumTracks" : False # Use Tracks cloned from originals with P updated using the CALO
       ,"Context":    "Offline"   # The context within which to run
       ,"Radiators": None         # The radiators to use
       ,"Particles": None         # The particle species to consider. Default is (el,mu,pi,ka,pr)
       ,"ConfigureTools":  True   # Configure the general RICH reconstruction tools
       ,"ConfigureAlgs":   True   # Configure the reconstruction algorithms
       ,"PreloadRawEvent": False  # Preload the RawEvent prior to the RICH algorithms
       ,"PreloadTracks":   False  # Preload the input tracks prior to the RICH algorithms
       ,"InitPixels":      True   # Run an initialisation algorithm to create the pixels
       ,"InitTracks":      True   # Run an initialisation algorithm to create the tracks
       ,"InitPhotons":     True   # Run an initialisation algorithm to create the photons
       ,"TracklessRingAlgs": [""]   # Run the given Trackless ring finding algorithms
       ,"CheckProcStatus": True   # Check the status of the ProcStatus object
       ,"PidConfig": "FullGlobal" # The PID algorithm configuration
       ,"MakeSummaryObjects": False # Make the reconstruction summary TES data objects
       ,"HpdRandomBackgroundProb" : -1.0 # If positive, will add additional random background to the data at the given pixel percentage
       ,"SpecialData"  : []       # Various special data processing options. See KnownSpecialData in RecSys for all options
       ,"RecoSequencer" : None    # The sequencer to add the RICH reconstruction algorithms to
        }

    ## Initialize 
    def initialize(self):
        # default values
        self.setRichDefault("Particles","Offline",["electron","muon","pion","kaon","proton"])
        self.setRichDefault("Particles","HLT",    ["pion","kaon"])
        self.setRichDefault("Radiators","Offline",[ "Aerogel", "Rich1Gas", "Rich2Gas" ])
        self.setRichDefault("Radiators","HLT",    [ "Aerogel", "Rich1Gas", "Rich2Gas" ])

    ## @brief Check the configuration is sensible
    #
    def checkConfiguration(self):
        context = self.getProp("Context")
        if context not in self.KnownContexts:
            raise RuntimeError("ERROR : Unknown RichRecSys context '%s'"%context)

    ## Shortcut to the cosmics option
    def cosmics(self) : return "cosmics" in self.getProp("SpecialData")

    ## @brief The RICH radiators to use
    #  @return a vector of bools indicating if (Aerogel,Rich1Gas,Rich2Gas) should be used
    def usedRadiators(self):
        usedRads = [ False, False, False ]
        for rad in self.getProp("Radiators"):
            if rad == "Aerogel"  : usedRads[0] = True
            if rad == "Rich1Gas" : usedRads[1] = True
            if rad == "Rich2Gas" : usedRads[2] = True
        return usedRads
    
    ## @brief The RICH detectors to use
    #  @return a vector of bools indicating if (Rich1,Rich2) should be used
    def usedDetectors(self):
        usedRads = self.usedRadiators()
        usedDets = [ False, False ]
        if usedRads[0] : usedDets[0] = True
        if usedRads[1] : usedDets[0] = True
        if usedRads[2] : usedDets[1] = True
        return usedDets

    ## @brief Apply the configuration to the configured GaudiSequencer
    def applyConf(self) :

        # Are we properly configured
        recoSequencer = self.getProp("RecoSequencer")
        if recoSequencer == None : raise RuntimeError("ERROR : Reconstruction Sequence not set")
        
        # Check configuration is sane
        self.checkConfiguration()

        # Tool mapping
        self.setOtherProp(RichTools(),"Context")

        if self.getProp("ConfigureTools") : self.configTools()
        if self.getProp("ConfigureAlgs")  : self.configAlgorithms(recoSequencer)
        
    ## @brief Configure the RICH algorithms
    #  @param sequence The GaudiSequencer to add the RICH reconstruction to      
    def configAlgorithms(self,sequence):

        from Configurables import ( Rich__DAQ__RawBufferToRichDigitsAlg,
                                    Rich__Rec__Initialise )

        # Set the context
        cont = self.getProp("Context")
        sequence.Context = cont

        #-----------------------------------------------------------------------------
        # Enable these to make sure data (tracks and raw event) are in memory
        # Useful for timing studies
        #-----------------------------------------------------------------------------
        if self.getProp("PreloadRawEvent"):
            from Configurables import Rich__DAQ__LoadRawEvent
            sequence.Members += [ Rich__DAQ__LoadRawEvent("LoadRawRichEvent"+cont) ]
        if self.getProp("PreloadTracks"):
            sequence.Members += [ Rich__Rec__Initialise("LoadRawTracks"+cont) ]

        #-----------------------------------------------------------------------------
        # Initialisation
        #-----------------------------------------------------------------------------
        initSeq = GaudiSequencer("RichRecInit"+cont+"Seq")
        initSeq.MeasureTime = True
        sequence.Members += [ initSeq ]

        if self.getProp("CheckProcStatus") :
            initSeq.Members += [ Rich__Rec__Initialise("CheckProc"+cont+"Status") ]

        if self.getProp("InitPixels") :
            # Make a pixel sequence
            pixelSeq = GaudiSequencer("Rich"+cont+"PixelsSeq")
            pixelSeq.MeasureTime = True
            initSeq.Members += [ pixelSeq ]
            # Raw decoding algorithm
            decodeRich = Rich__DAQ__RawBufferToRichDigitsAlg("DecodeRawRich"+cont)
            decodeRich.DecodeBufferOnly = True
            pixelSeq.Members += [ decodeRich ]
            # Add random background ?
            pixBackPercent = self.getProp("HpdRandomBackgroundProb")
            if pixBackPercent > 0 :
                from Configurables import ( Rich__AddBackground,
                                            Rich__RandomPixelBackgroundTool )
                bkgAlg  = Rich__AddBackground("AddRichBackgrounds")
                bkgTool = Rich__RandomPixelBackgroundTool("RichAddBackground")
                bkgTool.PixelBackgroundProb = pixBackPercent
                bkgAlg.addTool(bkgTool)
                pixelSeq.Members += [ bkgAlg ]
            # Make RichRecPixels
            recoPixs = Rich__Rec__Initialise("Create"+cont+"Pixels")
            recoPixs.CheckProcStatus   = False
            recoPixs.CreatePixels      = True
            # Add algs to sequence
            pixelSeq.Members += [ recoPixs ]

        if self.getProp("InitTracks") :
            # Make a track sequence
            trackSeq = GaudiSequencer("Rich"+cont+"TracksSeq")
            trackSeq.MeasureTime = True
            initSeq.Members += [ trackSeq ]
            
            # Calo tracks
            if self.getProp("UseCaloMomentumTracks") :
                from Configurables import TrackUseCaloMomentumAlg
                caloTrackAlg = TrackUseCaloMomentumAlg(cont+"TracksWithCALOP")
                caloTrackAlg.OutputTracks = "Rec/Track/BestWithCALOInfo"
                trackSeq.Members += [ caloTrackAlg ]

            # Make RichRecTracks
            recoTracks = Rich__Rec__Initialise("Create"+cont+"Tracks")
            recoTracks.CheckProcStatus   = False
            recoTracks.CreateTracks      = True
            trackSeq.Members += [ recoTracks ]

        if self.getProp("InitPhotons") :
            # Make a photon sequence
            photonSeq = GaudiSequencer("Rich"+cont+"PhotonsSeq")
            photonSeq.MeasureTime = True
            initSeq.Members += [ photonSeq ]

             # Make RichRecPhotons
            recoPhotons = Rich__Rec__Initialise("Create"+cont+"Photons")
            recoPhotons.CheckProcStatus   = False
            recoPhotons.CreatePhotons     = True
            photonSeq.Members += [ recoPhotons ]

        #-----------------------------------------------------------------------------
        # Trackless rings
        #-----------------------------------------------------------------------------
        ringalgs = self.getProp("TracklessRingAlgs")

        if "Markov" in ringalgs :
            mfinderSeq                           = GaudiSequencer("Rich"+cont+"MarkovRingFinderSeq")
            mfinderSeq.MeasureTime               = True
            sequence.Members                    += [ mfinderSeq ]
            self.setOtherProp(RichMarkovRingFinderConf(),"Context")
            RichMarkovRingFinderConf().setProp("Sequencer",mfinderSeq)
            
        #if "Template" in ringalgs :
        #    tfinderSeq = GaudiSequencer("Rich"+cont+"TemplateRingFinderSeq")
        #    tfinderSeq.MeasureTime               = True
        #    sequence.Members                    += [ tfinderSeq ]
        #    self.setOtherProp(RichTemplateRingFinderConf(),"Context")
        #    RichTemplateRingFinderConf().setProp("Sequencer",tfinderSeq)
           
        #-----------------------------------------------------------------------------
        # PID
        #-----------------------------------------------------------------------------
        pidMode = self.getProp("PidConfig")
        if pidMode != "None" :

            pidSeq = GaudiSequencer("Rich"+cont+"PIDSeq")
            pidSeq.MeasureTime = True
            sequence.Members += [ pidSeq ]
            
            if pidMode == "FullGlobal" or pidMode == "FastGlobal":
                pidConf = RichGlobalPIDConfig()
                if pidMode == "FastGlobal": pidConf.Mode = "Fast"
            elif pidMode == "FastLocal" :
                pidConf = RichHLTLocalPIDConfig()
            else:
                raise RuntimeError("ERROR : Unknown PID config '%s'"%pidConf)

            self.setOtherProp(pidConf,"Context")
            pidConf.PidSequencer = pidSeq
            pidConf.applyConf()

            #-------------------------------------------------------------------------
            # Finalise (merge results from various algorithms)
            #-------------------------------------------------------------------------
            if pidMode != "FastLocal" :
                from Configurables import Rich__Rec__HierarchicalPIDMerge
                pidSeq.Members += [Rich__Rec__HierarchicalPIDMerge("Merge"+cont+"RichPIDs")]
        
        #-----------------------------------------------------------------------------
        # Summary objects
        #-----------------------------------------------------------------------------
        if self.getProp("MakeSummaryObjects"):
            from Configurables import Rich__Rec__SummaryAlg
            sequence.Members += [Rich__Rec__SummaryAlg("RichRec"+cont+"Summary")]
        
    ## @brief Configure the RICH tools
    #
    def configTools(self):

        # The context
        context = self.getProp("Context")

        # Special options for cosmics
        #if self.cosmics() :
        #    RichPixelCreatorConfig().pixelCleaning  = "None"
        #    RichTools().photonPredictorType         = "SelectAll"
        #    RichPhotonCreatorConfig().selectionMode = "All"
        
        #--------------------------------------------------------------------
                
        # Tracks and segments
        tkConf = RichTrackCreatorConfig()
        tkConf.setProp("Radiators",self.usedRadiators())
        self.setOtherProps(tkConf,["Context","SpecialData","UseCaloMomentumTracks"])

        # Pixels
        pixConf = RichPixelCreatorConfig()
        pixConf.setProp("Detectors",self.usedDetectors())
        self.setOtherProps(pixConf,["Context"])
 
        # Photons
        photConf = RichPhotonCreatorConfig()
        photConf.setProp("Radiators",self.usedRadiators())
        self.setOtherProps(photConf,["Context"])

        #--------------------------------------------------------------------

        # Misc. items
        
        # geometrical efficiency
        RichTools().geomEff()

        # CK theta resolution
        ckResConf = CKThetaResolutionConfig()
        self.setOtherProp(ckResConf,"Context")

        # Particle Types
        from Configurables import Rich__ParticleProperties
        partProps = Rich__ParticleProperties("ToolSvc."+context+"_RichParticleProperties")
        partProps.ParticleTypes = self.getProp("Particles")

        # clustering
        from Configurables import Rich__DAQ__HPDPixelClusteringTool
        self.toolRegistry().Tools += [Rich__DAQ__HPDPixelClusteringTool("BackgroundClustering")]

        # Signal detection eff.
        RichTools().sigDetEff()
  
        #--------------------------------------------------------------------

