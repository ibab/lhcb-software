## @package RichRecSys
#  High level Configuration tools for RICH Reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.25 2010-01-25 16:38:24 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from TrackCreator  import *
from PixelCreator  import *
from PhotonCreator import *
from CKThetaResolution import *
from Configurables import RichENNRingFinderConf
from Configurables import RichTemplateRingFinderConf
from RichGlobalPID_ import RichGlobalPIDConfig
from Configurables import GaudiSequencer

# ----------------------------------------------------------------------------------

## @class RichRecSysConf
#  Configurable for RICH reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichRecSysConf(RichConfigurableUser):

    ## Possible used Configurables
    __used_configurables__ = [ (RichENNRingFinderConf,None),
                               (RichTemplateRingFinderConf,None),
                               (RichGlobalPIDConfig,None),
                               (RichTrackCreatorConfig,None),
                               (RichPixelCreatorConfig,None),
                               (RichPhotonCreatorConfig,None),
                               (CKThetaResolutionConfig,None)
                               ]
    
    ## Steering options
    __slots__ = {
        "UseCaloMomentumTracks" : False # Use Tracks cloned from originals with P updated using the CALO
       ,"Context":    "Offline"   # The context within which to run
       ,"Radiators": None         # The radiators to use
       ,"Particles": None         # The particle species to consider. Default is (el,mu,pi,ka,pr,bt)
       ,"ConfigureTools":  True   # Configure the general RICH reconstruction tools
       ,"ConfigureAlgs":   True   # Configure the reconstruction algorithms
       ,"PreloadRawEvent": False  # Preload the RawEvent prior to the RICH algorithms
       ,"PreloadTracks":   False  # Preload the input tracks prior to the RICH algorithms
       ,"InitPixels":      True   # Run an initialisation algorithm to create the pixels
       ,"InitTracks":      True   # Run an initialisation algorithm to create the tracks
       ,"InitPhotons":     True   # Run an initialisation algorithm to create the photons
       ,"TracklessRingAlgs": ["ENN"] # Run the given Trackless ring finding algorithms
       ,"CheckProcStatus": True   # Check the status of the ProcStatus object
       ,"PidConfig": "FullGlobal" # The PID algorithm configuration
       ,"MakeSummaryObjects": False # Make the reconstruction summary TES data objects
       ,"HpdRandomBackgroundProb" : -1.0 # If positive, will add additional random background to the data at the given pixel percentage
       ,"SpecialData"   : []       # Various special data processing options. See KnownSpecialData in RecSys for all options
       ,"RecoSequencer" : None    # The sequencer to add the RICH reconstruction algorithms to
       ,"Simulation"    : False   # Simulated data
       ,"DataType"      : ""      # Type of data, propagated from application
       ,"OutputLevel"   : INFO    # The output level to set all algorithms and tools to use
       ,"RichPIDLocation" : ""    # Output RichPID Location
       ,"PIDVersion"      : 1     # Default PID version
        }

    ## Initialize 
    def initialize(self):
        
        # default values
        self.setRichDefault("Particles","Offline",["electron","muon","pion","kaon",
                                                   "proton","belowThreshold"])
        self.setRichDefault("Particles","HLT",    ["pion","kaon"] )
        self.setRichDefault("Radiators","Offline", ["Aerogel","Rich1Gas","Rich2Gas"] )
        self.setRichDefault("Radiators","HLT",     ["Rich1Gas","Rich2Gas"] )

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

    ## Access the Track Configurable
    def trackConfig(self) : return self.getRichCU(RichTrackCreatorConfig)

    ## Access the Pixel Configurable
    def pixelConfig(self) : return self.getRichCU(RichPixelCreatorConfig)

    ## Access the Photon Configurable
    def photonConfig(self) : return self.getRichCU(RichPhotonCreatorConfig)

    ## Access the global PID configurable
    def gpidConfig(self) : return self.getRichCU(RichGlobalPIDConfig)

    ## @brief Apply any tweaks to the default configuration that vary by DataType
    def dataTypeTweaks(self):

        # Get the DataType
        dataType = self.getProp("DataType")

        # Tweaks for the Upgrade
        if dataType == "Upgrade" :

            # No Aerogel in the upgrade
            if not self.isPropertySet("Radiators") :
                self.setProp( "Radiators", ["Rich1Gas","Rich2Gas"] )

            # Fudge factors for the photon reconstruction.
            # Need to understand why these are required, as they indicate
            # some small biases probably exist in the reconstruction.
            self.richTools().photonReco().CKThetaQuartzRefractCorrections = [ -0.00625, 0.0001, 2.9e-5 ]

            # Scale factors for the Cherenkov resolution tool.
            # Needed as the upgrade has better resolutions.
            # Longer term, the tools needs a proper retuning for the upgrade
            self.richTools().ckResolution().ScaleFactor = [ 0.57, 0.57, 0.7 ]

    ## @brief Apply the configuration to the configured GaudiSequencer
    def applyConf(self) :

        # DataType specific tweaks
        self.dataTypeTweaks()

        # Check the sequencer is set
        if not self.isPropertySet("RecoSequencer") :
            raise RuntimeError("ERROR : Reconstruction Sequence not set")
        recoSequencer = self.getProp("RecoSequencer")

        # Pass some settings to used configurables
        self.setOtherProps( self.photonConfig(), ["DataType"] )
        self.setOtherProps( self.pixelConfig(),  ["DataType"] )
        self.setOtherProps( self.trackConfig(),  ["DataType"] )
        self.setOtherProps( self.gpidConfig(),   ["DataType"] )
         
        # Tools. (Should make this automatic in the base class somewhere)
        self.setOtherProps(self.richTools(),["Context","OutputLevel"])

        # Do the configuration
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
            raw = self.makeRichAlg( Rich__DAQ__LoadRawEvent, "LoadRawRichEvent"+cont )
            sequence.Members += [ raw ]
        if self.getProp("PreloadTracks"):
            trackInit = self.makeRichAlg( Rich__Rec__Initialise, "LoadRawTracks"+cont )
            sequence.Members += [ trackInit ]

        #-----------------------------------------------------------------------------
        # Initialisation
        #-----------------------------------------------------------------------------
        initSeq = self.makeRichAlg(GaudiSequencer,"RichRecInit"+cont+"Seq")
        sequence.Members += [ initSeq ]

        if self.getProp("CheckProcStatus") :
            procStat = self.makeRichAlg( Rich__Rec__Initialise, "CheckProc"+cont+"Status" )
            initSeq.Members += [ procStat ]

        if self.getProp("InitPixels") :
            # Make a pixel sequence
            pixelSeq = self.makeRichAlg(GaudiSequencer,"Rich"+cont+"PixelsSeq")
            initSeq.Members += [ pixelSeq ]
            # Raw decoding algorithm
            decodeRich = self.makeRichAlg( Rich__DAQ__RawBufferToRichDigitsAlg, "DecodeRawRich"+cont )
            decodeRich.DecodeBufferOnly = True
            pixelSeq.Members += [ decodeRich ]
            # Add random background ?
            pixBackPercent = self.getProp("HpdRandomBackgroundProb")
            if pixBackPercent > 0 :
                from Configurables import ( Rich__AddBackground,
                                            Rich__RandomPixelBackgroundTool )
                bkgAlg  = self.makeRichAlg( Rich__AddBackground, "AddRichBackgrounds"+cont )
                bkgAlg.addTool( Rich__RandomPixelBackgroundTool, name="RichAddBackground" )
                bkgAlg.RichAddBackground.PixelBackgroundProb = pixBackPercent
                pixelSeq.Members += [ bkgAlg ]
            # Make RichRecPixels
            recoPixs = self.makeRichAlg( Rich__Rec__Initialise, "Create"+cont+"Pixels" )
            recoPixs.CheckProcStatus   = False
            recoPixs.CreatePixels      = True
            # Add algs to sequence
            pixelSeq.Members += [ recoPixs ]

        if self.getProp("InitTracks") :
            # Make a track sequence
            trackSeq = self.makeRichAlg(GaudiSequencer,"Rich"+cont+"TracksSeq")
            initSeq.Members += [ trackSeq ]
            
            # Calo tracks
            if self.getProp("UseCaloMomentumTracks") :
                from Configurables import TrackUseCaloMomentumAlg
                caloTrackAlg = self.makeRichAlg( TrackUseCaloMomentumAlg, cont+"TracksWithCALOP" )
                caloTrackAlg.OutputTracks = "Rec/Track/BestWithCALOInfo"
                trackSeq.Members += [ caloTrackAlg ]

            # Make RichRecTracks
            recoTracks = self.makeRichAlg(Rich__Rec__Initialise,"Create"+cont+"Tracks")
            recoTracks.CheckProcStatus   = False
            recoTracks.CreateTracks      = True
            trackSeq.Members += [ recoTracks ]

        if self.getProp("InitPhotons") :
            # Make a photon sequence
            photonSeq = self.makeRichAlg(GaudiSequencer,"Rich"+cont+"PhotonsSeq")
            initSeq.Members += [ photonSeq ]

             # Make RichRecPhotons
            recoPhotons = self.makeRichAlg(Rich__Rec__Initialise,"Create"+cont+"Photons")
            recoPhotons.CheckProcStatus   = False
            recoPhotons.CreatePhotons     = True
            photonSeq.Members += [ recoPhotons ]

        #-----------------------------------------------------------------------------
        # Trackless rings
        #-----------------------------------------------------------------------------
        ringalgs = self.getProp("TracklessRingAlgs")

        if "ENN" in ringalgs :
            ennfinderSeq = self.makeRichAlg(GaudiSequencer,"Rich"+cont+"ENNRingFinderSeq")
            sequence.Members                      += [ ennfinderSeq ]
            ennConf = self.getRichCU(RichENNRingFinderConf)
            self.setOtherProps(ennConf,["Context","OutputLevel"])
            ennConf.setProp("Sequencer",ennfinderSeq)
            self.printInfo(ennConf)
           
        if "Template" in ringalgs :
            tfinderSeq = self.makeRichAlg(GaudiSequencer,"Rich"+cont+"TemplateRingFinderSeq")
            sequence.Members                    += [ tfinderSeq ]
            tempConf = self.getRichCU(RichTemplateRingFinderConf)
            self.setOtherProps(tempConf,["Context","OutputLevel"])
            tempConf.setProp("Sequencer",tfinderSeq)
            self.printInfo(tempConf)
            
        #-----------------------------------------------------------------------------
        # PID
        #-----------------------------------------------------------------------------
        pidMode = self.getProp("PidConfig")
        if pidMode != "None" :

            pidSeq = self.makeRichAlg(GaudiSequencer,"Rich"+cont+"PIDSeq")
            sequence.Members += [ pidSeq ]
            
            if pidMode == "FullGlobal" or pidMode == "FastGlobal":
                pidConf = self.gpidConfig()
                if pidMode == "FastGlobal": pidConf.Mode = "Fast"
                gpidSeq = self.makeRichAlg(GaudiSequencer,"Rich"+cont+"GPIDSeq")
                pidSeq.Members += [ gpidSeq ]
                pidConf.PidSequencer = gpidSeq
            else:
                raise RuntimeError("ERROR : Unknown PID config '%s'"%pidConf)

            pidConf.setProp("Radiators",self.usedRadiators())
            self.setOtherProps(pidConf,["Context","OutputLevel"])
            
            self.printInfo(pidConf)

            #-------------------------------------------------------------------------
            # Finalise (merge results from various algorithms)
            #-------------------------------------------------------------------------
            from Configurables import Rich__Rec__HierarchicalPIDMerge
            pidMerge = self.makeRichAlg(Rich__Rec__HierarchicalPIDMerge,"Merge"+cont+"RichPIDs")
            pidMerge.OutputPIDLocation = self.getProp("RichPIDLocation")
            pidMerge.PIDVersion = self.getProp("PIDVersion")
            pidSeq.Members += [pidMerge]
        
        #-----------------------------------------------------------------------------
        # Summary objects
        #-----------------------------------------------------------------------------
        if self.getProp("MakeSummaryObjects"):
            from Configurables import Rich__Rec__SummaryAlg
            summary = self.makeRichAlg(Rich__Rec__SummaryAlg,"RichRec"+cont+"Summary")
            sequence.Members += [summary]
        
    ## @brief Configure the RICH tools
    #
    def configTools(self):

        # The context
        context = self.getProp("Context")

        #--------------------------------------------------------------------
                
        # Tracks and segments
        tkConf = self.trackConfig()
        tkConf.setProp("Radiators",self.usedRadiators())
        self.setOtherProps(tkConf,["OutputLevel","Context","SpecialData",
                                   "UseCaloMomentumTracks"])

        # Pixels
        pixConf = self.pixelConfig()
        pixConf.setProp("Detectors",self.usedDetectors())
        self.setOtherProps(pixConf,["OutputLevel","Context"])
 
        # Photons
        photConf = self.photonConfig()
        photConf.setProp("Radiators",self.usedRadiators())
        self.setOtherProps(photConf,["OutputLevel","SpecialData","Context","Simulation"])

        #--------------------------------------------------------------------

        # Misc. items

        # NB : Need to instanciate the configurables, even if we do nothing with
        #    : them in order to get properties (like output levels) set correctly.
        
        # geometrical efficiency
        self.richTools().geomEff()

        # CK theta resolution
        ckResConf = self.getRichCU(CKThetaResolutionConfig)
        self.setOtherProps(ckResConf,["OutputLevel","Context"])

        # Particle Types
        from Configurables import Rich__ParticleProperties
        partProps = self.makeRichAlg(Rich__ParticleProperties,
                                     "ToolSvc."+context+"_RichParticleProperties")
        partProps.ParticleTypes = self.getProp("Particles")

        # clustering
        from Configurables import Rich__DAQ__HPDPixelClusteringTool
        self.toolRegistry().Tools += [self.makeRichAlg(Rich__DAQ__HPDPixelClusteringTool,
                                                       "BackgroundClustering")]

        # Signal detection eff.
        self.richTools().sigDetEff()

        # Ray tracing
        self.richTools().rayTracing()

        # Tool registory
        self.richTools().toolRegistry()
        self.richTools().toolRegistry(common=True)

        # Decoding
        self.richTools().smartIDTool()

        # Messages from RichDet
        msgSvc = getConfigurable("MessageSvc")
        if self.isPropertySet("OutputLevel") :
            dets = [ "DeRichSystem" ]
            if self.getProp("OutputLevel") == WARNING :
                msgSvc.setWarning += dets
            elif self.getProp("OutputLevel") == ERROR :
                msgSvc.setError += dets
            elif self.getProp("OutputLevel") == FATAL :
                msgSvc.setFatal += dets
            elif self.getProp("OutputLevel") == DEBUG :
                msgSvc.setDebug += dets
            elif self.getProp("OutputLevel") == VERBOSE :
                msgSvc.setVerbose += dets

        # Print Config
        self.printInfo(tkConf)
        self.printInfo(pixConf)
        self.printInfo(photConf)
        self.printInfo(ckResConf)
        self.printInfo(self.richTools())
               
        #--------------------------------------------------------------------
