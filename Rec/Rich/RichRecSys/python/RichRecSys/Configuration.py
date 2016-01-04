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

## @class RichRecSysBaseConf
#  Configurable for RICH reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichRecSysBaseConf(RichConfigurableUser):

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
       ,"ConfigurePrelims": True  # Configure some 'preliminary' items
       ,"ConfigureTools":  True   # Configure the general RICH reconstruction tools
       ,"ConfigureAlgs":   True   # Configure the reconstruction algorithms
       ,"PreloadRawEvent": False  # Preload the RawEvent prior to the RICH algorithms
       ,"PreloadTracks":   False  # Preload the input tracks prior to the RICH algorithms
       ,"RefitTracks":     False  # Refit the tracks first. Useful for running on DSTs.
       ,"InitDecoding":    True   # Run an initialisation algorithm to decode the Rich RawBuffer
       ,"InitPixels":      True   # Run an initialisation algorithm to create the pixels
       ,"InitTracks":      True   # Run an initialisation algorithm to create the tracks
       ,"InitPhotons":     True   # Run an initialisation algorithm to create the photons
       ,"TrackTypeGroups": [ ] # Track type groupings. Empty list means all at once.
       ,"TracklessRingAlgs": ["ENN"] # Run the given Trackless ring finding algorithms
       ,"CheckProcStatus": True   # Check the status of the ProcStatus object
       ,"PidConfig": "FullGlobal" # The PID algorithm configuration
       ,"MakeSummaryObjects": False # Make the reconstruction summary TES data objects
       ,"HpdRandomBackgroundProb" : -1.0 # If positive, will add additional random background to the data at the given pixel percentage
       ,"SpecialData"   : []      # Various special data processing options. See KnownSpecialData in RecSys for all options
       ,"RecoSequencer" : None    # The sequencer to add the RICH reconstruction algorithms to
       ,"Simulation"    : False   # Simulated data
       ,"DataType"      : ""      # Type of data, propagated from application
       ,"OnlineMode"    : False   # Online mode
       ,"OutputLevel"   : INFO    # The output level to set all algorithms and tools to use
       ,"RichPIDLocation" : ""    # Output RichPID Location
       ,"PIDVersion"      : 1     # Default PID version
        }

    ## Initialize 
    def initialize(self):
        
        # default values
        self.setRichDefault("Particles","Offline", ["electron","muon","pion","kaon",
                                                    "proton","belowThreshold"] )
        self.setRichDefault("Particles","HLT",     ["electron","muon","pion","kaon",
                                                    "proton","belowThreshold"] )
        self.setRichDefault("Radiators","Offline", ["Rich1Gas","Rich2Gas"] )
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

    ## Access the CK theta resolution configurable
    def ckResConfig(self) : return self.getRichCU(CKThetaResolutionConfig)

    ## @brief Apply any tweaks to the default configuration that vary by DataType
    def dataTypeTweaks(self):

        # Get the DataType
        dataType = self.getProp("DataType")

        # For Run I and eariler use Aerogel as well as gas radiators
        if ( dataType == "2008" or
             dataType == "2009" or
             dataType == "2010" or
             dataType == "2011" or
             dataType == "2012" or
             dataType == "2013" or
             dataType == "MC09" ) :
            if not self.isPropertySet("Radiators") and self.getProp("Context") != "HLT" :
                self.setProp( "Radiators", ["Aerogel","Rich1Gas","Rich2Gas"] )

        # Tweaks for the Upgrade
        if dataType == "Upgrade" :

            # Fudge factors for the photon reconstruction.
            # Need to understand why these are required, as they indicate
            # some small biases probably exist in the reconstruction.
            self.richTools().photonReco().CKThetaQuartzRefractCorrections = [ -0.00625, 0.0001, 2.9e-5 ]

            # Scale factors for the Cherenkov resolution tool.
            # Needed as the upgrade has better resolutions.
            # Longer term, the tools needs a proper retuning for the upgrade
            self.richTools().ckResolution().ScaleFactor = [ 0.57, 0.57, 0.7 ]

    ## @brief Online mode 
    def setupOnlineMode(self) :

        # Use wide photon selection mode for larger side-bands
        self.photonConfig().SelectionMode = "Wide"

        # Set scale factors to 1 for n-1 corrections
        from Configurables import UpdateManagerSvc
        UpdateManagerSvc().ConditionsOverride += [
            "Conditions/Environment/Rich1/RefractivityScaleFactor := double CurrentScaleFactor = 1.0;",
            "Conditions/Environment/Rich2/RefractivityScaleFactor := double CurrentScaleFactor = 1.0;"
            ]

    ## Common initialisation stuff
    def applyCommonConf(self):

        # DataType specific tweaks
        self.dataTypeTweaks()

        # Online mode ?
        if self.getProp("OnlineMode") : self.setupOnlineMode()
         
        # Tools. (Should make this automatic in the base class somewhere)
        for prop in ["OutputLevel","Context"]:
            self.richTools().setProp( prop, self.getProp(prop) )

        # Set the context
        cont = self.getProp("Context")
        sequence = self.getRecoSeq()
        sequence.Context = cont

        # Setup tools
        if self.getProp("ConfigureTools") : self.configTools()

    ## Get the sequence to fill
    def getRecoSeq(self):
        # Check the sequencer is set
        seq = None
        if not self.isPropertySet("RecoSequencer") :
            log.warning("RICH Reconstruction Sequence not set. Will do nothing.")
            #raise RuntimeError("ERROR : Reconstruction Sequence not set")
        else:
            seq = self.getProp("RecoSequencer")
        return seq

    ## @brief Apply the configuration to the configured GaudiSequencer
    def applyConf(self) :

        self.applyCommonConf()
        if self.getProp("ConfigureAlgs") : self.configAlgorithms()

    ## Propagate options
    def passOption(self,other,name):
        if not other.isPropertySet(name) :
            #print "PassOption", self.name(), other.name(), name
            self.propagateProperty(name,other)

    ## Propagate options
    def passOptions(self,other,names):
        for name in names : self.passOption(other,name)
        
    ## Propagate select options to used configurables
    def setupToolOptions(self) :
        
        # Tracks and segments
        tkConf = self.trackConfig()
        tkConf.setProp("Radiators",self.usedRadiators())
        self.passOptions(tkConf,["DataType","OutputLevel","Context","SpecialData"])
        
        # Pixels
        pixConf = self.pixelConfig()
        pixConf.setProp("Detectors",self.usedDetectors())
        self.passOptions(pixConf,["DataType","OutputLevel","Context","SpecialData"])
        
        # Photons
        photConf = self.photonConfig()
        photConf.setProp("Radiators",self.usedRadiators())
        self.passOptions(photConf,["DataType","OutputLevel","SpecialData","Context","Simulation"])
        
        # CK theta resolution
        ckResConf = self.ckResConfig()
        self.passOptions(ckResConf,["OutputLevel","Context"])
        
        # Print Config
        self.printInfo(tkConf)
        self.printInfo(pixConf)
        self.printInfo(photConf)
        self.printInfo(ckResConf)
  
    ## Copy all options from A to B
    def copyOptions(self,a,b):
        for option in a.__slots__.keys() :
            if a.isPropertySet(option) :
                value = a.getProp(option)
                if not b.isPropertySet(option):
                    #print "Cloning option", option, "=", value, "from", a.name(), "to", b.name()
                    b.setProp(option,value)

    ## Construct the track group name for a given list of types
    def trackGroupName(self,tkTypeGroup) :
        name = ""
        for tk in tkTypeGroup : name = name + tk
        if name == "ForwardMatch" or name == "MatchForward" : name = "Long"
        return name
    
    ## Is this track group selected by the TrackCuts ?
    def trackGroupSelected(self,tkTypeGroup) :
        ok = False
        for tk in tkTypeGroup :
            if tk in self.trackConfig().getProp("TrackCuts").keys() :
                ok = True
        return ok

    ## Gets the configurable name for a given track group
    def getConfName(self,tkTypeGroup):
        return self.name() + self.trackGroupName(tkTypeGroup)

    ## Gets the configurable context for a given track group
    def getConfContext(self,tkTypeGroup):
        return self.getProp("Context") + self.trackGroupName(tkTypeGroup)

    ## Returns the context name to use for a given track type
    def getContextForTrackType(self,tktype):
        if type(tktype) is list : tktype = self.trackGroupName(tktype)
        # Loop over the groups to find the one this track type is in
        cont = ""
        for tkGroup in self.getProp("TrackTypeGroups"):
            if tktype in tkGroup or tktype == self.trackGroupName(tkGroup) :
                cont = self.getConfContext(tkGroup)
        return cont

    ## Returns true if the given track group is active
    def trackTypeIsActive(self,types) :
        groupName = self.trackGroupName(types)
        return ( self.getContextForTrackType(groupName) != "" and
                 self.trackGroupSelected(types) )

    ## Are we running with split track groupings ?
    def usingTrackGroups(self) :
        return 0 != len( self.getProp("TrackTypeGroups") )
            
    ## @brief Configure the RICH algorithms
    #  @param sequence The GaudiSequencer to add the RICH reconstruction to      
    def configAlgorithms(self):

        # Get the sequence to fill. If not set just return.
        sequence = self.getRecoSeq()
        if None == sequence : return

        # Context
        cont = self.getProp("Context")

        from Configurables import Rich__Rec__Initialise 

        #-----------------------------------------------------------------------------
        # Initialisation
        #-----------------------------------------------------------------------------
        initSeq = self.makeRichAlg(GaudiSequencer,"RichRecInit"+cont+"Seq")
        sequence.Members += [ initSeq ]

        #-----------------------------------------------------------------------------
        # Check processing status
        #-----------------------------------------------------------------------------
        if self.getProp("CheckProcStatus") :
            procStat = self.makeRichAlg( Rich__Rec__Initialise, "CheckProc"+cont+"Status" )
            initSeq.Members += [ procStat ]

        #-----------------------------------------------------------------------------
        # Preload the tracks
        #-----------------------------------------------------------------------------
        if self.getProp("PreloadTracks"):
            trackInit = self.makeRichAlg( Rich__Rec__Initialise, "LoadRawTracks"+cont )
            trackInit.LoadRawTracks = True
            initSeq.Members += [ trackInit ]

        if self.getProp("InitPixels") :
            
            # Make a pixel sequence
            pixelSeq = self.makeRichAlg(GaudiSequencer,"Rich"+cont+"PixelsSeq")
            initSeq.Members += [ pixelSeq ]
            
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
            sequence.Members += [ ennfinderSeq ]
            ennConf = self.getRichCU(RichENNRingFinderConf)
            self.setOtherProps(ennConf,["Context","OutputLevel"])
            ennConf.setProp("Sequencer",ennfinderSeq)
            self.printInfo(ennConf)
        if "Template" in ringalgs :
            tfinderSeq = self.makeRichAlg(GaudiSequencer,"Rich"+cont+"TemplateRingFinderSeq")
            sequence.Members += [ tfinderSeq ]
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
                if pidMode == "FastGlobal" : pidConf.Mode = "Fast"
                gpidSeq = self.makeRichAlg(GaudiSequencer,"Rich"+cont+"GPIDSeq")
                pidSeq.Members += [ gpidSeq ]
                pidConf.PidSequencer = gpidSeq
                if self.trackConfig().isPropertySet("TrackCuts") :
                    self.trackConfig().setOtherProps(pidConf,["TrackCuts"])
            else:
                raise RuntimeError("ERROR : Unknown PID config '%s'"%pidConf)

            pidConf.setProp("Radiators",self.usedRadiators())
            self.setOtherProps(pidConf,["DataType","Context","OutputLevel","SpecialData"])
            
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

        # NB : Need to instanciate the configurables, even if we do nothing with
        #    : them in order to get properties (like output levels) set correctly.

        self.setupToolOptions()

        # Tool registory
        self.richTools().toolRegistry()
        self.richTools().toolRegistry(common=True)
        
        # geometrical efficiency
        self.richTools().geomEff()

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

        self.printInfo(self.richTools())
               
        #--------------------------------------------------------------------

## @class RichRecSysConf
#  Configurable for RICH reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichRecSysConf(RichRecSysBaseConf) :

    # Define the track group configurables
    def setTrackGroups( self, tkGroups ):
        # Set the property
        self.setProp("TrackTypeGroups",tkGroups)
        # Configure the used configurables
        for tkTypeGroup in tkGroups:
            # Group name
            name = self.getConfName(tkTypeGroup)
            self._ConfigurableUser__addActiveUseOf( RichRecSysBaseConf(name,_enabled=False) )

    ## Set context correctly in all configurables
    def propagateContext(self,groupConfig,context) :
        # Main configurable
        groupConfig.setProp("Context",context)
        # sub configurables
        groupConfig.trackConfig().setProp("Context",context)
        groupConfig.pixelConfig().setProp("Context",context)
        groupConfig.photonConfig().setProp("Context",context)
        groupConfig.gpidConfig().setProp("Context",context)
        groupConfig.ckResConfig().setProp("Context",context)
        # Main configurable RichTools
        groupConfig.richTools().setProp("Context",context)
        # sub configurables RichTools
        groupConfig.trackConfig().richTools().setProp("Context",context)
        groupConfig.pixelConfig().richTools().setProp("Context",context)
        groupConfig.photonConfig().richTools().setProp("Context",context)
        groupConfig.gpidConfig().richTools().setProp("Context",context)
        groupConfig.ckResConfig().richTools().setProp("Context",context)
        
    ## Propagate all options to used configurables
    def propagateAllOptions(self,groupConfig) :
        
        # Main options
        self.copyOptions( self, groupConfig )

        # RichTools
        self.copyOptions( self.richTools(), groupConfig.richTools() )
        
        # clone options for used configurables
        self.copyOptions( self.trackConfig(),  groupConfig.trackConfig()  )
        self.copyOptions( self.pixelConfig(),  groupConfig.pixelConfig()  )
        self.copyOptions( self.photonConfig(), groupConfig.photonConfig() )
        self.copyOptions( self.gpidConfig(),   groupConfig.gpidConfig()   )
        self.copyOptions( self.ckResConfig(),  groupConfig.ckResConfig()  )

        # Misc. tools
        self.copyOptions( self.richTools().photonReco(), groupConfig.richTools().photonReco() )

    ## Get the configurable for a given track type
    def getTrackGroupConf(self,tktype):
        conf = None
        if type(tktype) is list : tktype = self.trackGroupName(tktype)
        for tkGroup in self.getProp("TrackTypeGroups"):
            if tktype in tkGroup or tktype == self.trackGroupName(tkGroup) :
                conf = RichRecSysBaseConf( self.getConfName(tkGroup) )
        return conf

    ## Get the 'best' context to use for an algorithm without a clear track type (so pixel based etc.)
    def getBestContext(self):
        cont = self.getProp("Context")
        groups = self.getProp("TrackTypeGroups")
        if len(groups) > 0 : cont += self.trackGroupName(groups[0])
        return cont

    ## @brief Apply the configuration to the configured GaudiSequencer
    def applyConf(self) :

        # Get the sequence to fill
        sequence = self.getRecoSeq()
        if None == sequence : return

        # Common stuff
        self.applyCommonConf()

        # Get the context
        cont = self.getProp("Context")

        # Some preliminary stuff
        if self.getProp("ConfigurePrelims") :

            #-----------------------------------------------------------------------------
            # Preload the raw event
            #-----------------------------------------------------------------------------
            if self.getProp("PreloadRawEvent"):
                from Configurables import Rich__DAQ__LoadRawEvent
                raw = self.makeRichAlg( Rich__DAQ__LoadRawEvent, "LoadRawRichEvent"+cont )
                sequence.Members += [ raw ]

            #-----------------------------------------------------------------------------
            # Enable this to run a full track refit prior to processing
            # Useful for running on DSTs.
            #-----------------------------------------------------------------------------
            if self.getProp("RefitTracks") :
                from DAQSys.Decoders import DecoderDB
                from DAQSys.DecoderClass import decodersForBank
                for det in ["Velo","TT","IT"] :
                    sequence.Members += [ d.setup() for d in decodersForBank(DecoderDB,det) ]
                from TrackFitter.ConfiguredFitters import ConfiguredFit
                from Configurables import TrackStateInitAlg
                tracksLoc = self.trackConfig().getProp("InputTracksLocation")
                init = TrackStateInitAlg( name = "InitRichTrackStates",
                                          TrackLocation = tracksLoc )
                fit = ConfiguredFit("RefitRichTracks",tracksLoc)
                #init.OutputLevel = 1
                #fit.OutputLevel  = 1
                sequence.Members += [ init, fit ]

            #-----------------------------------------------------------------------------
            # RICH RawEvent decoding
            #-----------------------------------------------------------------------------
            if self.getProp("InitDecoding") :

                # Raw decoding algorithm
                # Parasite in on this with my database so that the
                # automatic reconfiguration of locations is spawned
                from DAQSys.Decoders import DecoderDB
                from DAQSys.DecoderClass import decodersForBank
                aname="DecodeRawRich"+cont
                # clone the pre-existing alg in the database first...
                decodeRich =DecoderDB["Rich::DAQ::RawBufferToRichDigitsAlg/RichRawEventToDigits"].clone("Rich::DAQ::RawBufferToRichDigitsAlg/"+aname)
                decodeRich.Properties["DecodeBufferOnly"]=True
                # with ["DecodeBufferOnly"]=True, alg produces no output ...
                decodeRich.Outputs=[]
                # set "Active" to auto reconfigure the input location if needed
                decodeRich.Active=True 
                decodeRich=decodeRich.setup()
                # call the makeRichAlg anyway, even though it is already "made..."
                # just in case it is adapted to do other things in the future
                from Configurables import Rich__DAQ__RawBufferToRichDigitsAlg
                decodeRich = self.makeRichAlg( Rich__DAQ__RawBufferToRichDigitsAlg, aname )
                sequence.Members += [ decodeRich ]

        # Configure the RICH processing
        if not self.usingTrackGroups() :

            # Run a single sequence for all track types
            if self.getProp("ConfigureAlgs")  : self.configAlgorithms()

        else:

            # Run a seperate sequence for each track grouping

            # Get the track groupings
            tkGroups = self.getProp("TrackTypeGroups")

            # Global tracking cuts
            globalTkCuts = self.trackConfig().getProp("TrackCuts")

            # Seperated PID location s
            pidLocs = [ ]

            # Split by track type
            for tkTypeGroup in tkGroups:

                # Is at least one of the track types in this group in the track selection cuts ?
                if self.trackGroupSelected(tkTypeGroup) :

                    # Build the reco sequence for this group
                    groupConfig = RichRecSysBaseConf( self.getConfName(tkTypeGroup) )

                    # Context for this group
                    groupcont = self.getConfContext(tkTypeGroup)

                    # Set context with group name
                    self.propagateContext( groupConfig, groupcont )

                    # Propagate all options
                    self.propagateAllOptions( groupConfig )
                
                    # Then tweak things for this track group
                    # Turn off stuff run once for all
                    groupConfig.setProp("CheckProcStatus",False)
                    groupConfig.setProp("PreloadRawEvent",False)
                    groupConfig.setProp("PreloadTracks",False)
                    groupConfig.setProp("RefitTracks",False)
                    groupConfig.setProp("InitDecoding",False)
                    # Turn off splitting again
                    groupConfig.setProp("TrackTypeGroups",[])
                    # Proper context for this group
                    self.propagateContext( groupConfig, groupcont )
  
                    # Clone tool registry tool list
                    groupConfig.toolRegistry().Tools = self.toolRegistry().Tools

                    # PID Output location
                    pidLoc = self.getProp("RichPIDLocation")+self.trackGroupName(tkTypeGroup)
                    groupConfig.setProp("RichPIDLocation",pidLoc)
                    pidLocs += [pidLoc]

                    # Construct the track selection cuts for this group
                    tkCuts = { }
                    for tkType in tkTypeGroup : tkCuts[tkType] = globalTkCuts[tkType]

                    # Set the track selection for this group
                    groupConfig.trackConfig().setProp("TrackCuts",tkCuts)

                    # Make a sequence for this track group
                    tkGroupSeq = self.makeRichAlg(GaudiSequencer,"RichRec"+groupcont+"Seq")
                    sequence.Members += [tkGroupSeq]
                    groupConfig.setProp("RecoSequencer",tkGroupSeq)

            #-------------------------------------------------------------------------
            # Finalise (merge results from various algorithms)
            #-------------------------------------------------------------------------
            pidMode = self.getProp("PidConfig")
            if pidMode != "None" :
                from Configurables import Rich__Rec__PIDMerge
                pidMerge = self.makeRichAlg(Rich__Rec__PIDMerge,"Merge"+cont+"RichPIDs")
                pidMerge.OutputLocation = self.getProp("RichPIDLocation")
                pidMerge.InputLocations = pidLocs
                sequence.Members += [pidMerge]


