
## @package RichRecSys
#  High level Configuration tools for RICH Reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.5 2008-08-26 19:50:32 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from RichRecSys.TrackCreator  import *
from RichRecSys.PixelCreator  import *
from RichRecSys.PhotonCreator import *
from RichRecSys.CKThetaResolution import *
from RichMarkovRingFinder.Configuration import *
from Configurables import ( GaudiSequencer,
                            Rich__DAQ__HPDPixelClusteringTool,
                            Rich__ParticleProperties )

# ----------------------------------------------------------------------------------

## @class RichRecSysConf
#  Configurable for RICH reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichRecSysConf(RichConfigurableUser):

    ## Known contexts
    KnownContexts      = [ "Offline", "HLT" ]
    
    ## Steering options
    __slots__ = {
        "fieldOff":       False   # set to True for magnetic field off data
       ,"veloOpen":       False   # set to True for Velo open data
       ,"context":    "Offline"   # The context within which to run
       ,"radiators": None         # The radiators to use
       ,"particles": None         # The particle species to consider. Default is (el,mu,pi,ka,pr)
       ,"configureTools":  True   # Configure the general RICH reconstruction tools
       ,"configureAlgs":   True   # Configure the reconstruction algorithms
       ,"preloadRawEvent": False  # Preload the RawEvent prior to the RICH algorithms
       ,"preloadTracks":   False  # Preload the input tracks prior to the RICH algorithms
       ,"initPixels":      True   # Run an initialisation algorithm to create the pixels
       ,"initTracks":      True   # Run an initialisation algorithm to create the tracks
       ,"initPhotons":     True   # Run an initialisation algorithm to create the photons
       ,"tracklessRings":  False  # Run the trackless ring finding algorithms
       ,"checkProcStatus": True   # Check the status of the ProcStatus object
       ,"pidConfig": "FullGlobal" # The PID algorithm configuration
       ,"makeSummaryObjects": False # Make the reconstruction summary TES data objects
       ,"testOldOpts": False
        }

    ## Initialise 
    def initialise(self):
        # default values
        self.setRichDefault("particles","Offline",["electron","muon","pion","kaon","proton"])
        self.setRichDefault("particles","HLT",    ["pion","kaon"])
        self.setRichDefault("radiators","Offline",[ "Aerogel", "Rich1Gas", "Rich2Gas" ])
        self.setRichDefault("radiators","HLT",    [ "Aerogel", "Rich1Gas", "Rich2Gas" ])

    ## @brief Check the configuration is sensible
    #
    def checkConfiguration(self):
        context = self.getProp("context")
        if context not in self.KnownContexts:
            raise RuntimeError("ERROR : Unknown RichRecSys context '%s'"%context)

    ## @brief The RICH radiators to use
    #  @return a vector of bools indicating if (Aerogel,Rich1Gas,Rich2Gas) should be used
    def usedRadiators(self):
        usedRads = [ False, False, False ]
        for rad in self.getProp("radiators"):
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

    ## @brief Apply the configuration to the given GaudiSequencer
    #  @param sequence The GaudiSequencer to add the RICH reconstruction to
    def applyConf(self,sequence=None):
        
        # Check configuration is sane
        self.checkConfiguration()

        # Tool mapping
        self.setOtherProp(RichTools(),"context")

        # Temporary option to use old options, for testing
        if self.getProp("testOldOpts") :
            importOptions( "$RICHRECSYSOPTS/RecoOfflineToolPara.opts" )
            importOptions( "$RICHRECSYSOPTS/RecoOfflineAlgs.opts")
        else:
            if self.getProp("configureTools") :
                self.configTools()
            if sequence != None and self.getProp("configureAlgs") :
                self.configAlgorithms(sequence)
        
    ## @brief Configure the RICH algorithms
    #  @param sequence The GaudiSequencer to add the RICH reconstruction to      
    def configAlgorithms(self,sequence):

        from Configurables import ( Rich__DAQ__RawBufferToRichDigitsAlg,
                                    Rich__Rec__Initialise )

        # Set the context
        cont = self.getProp("context")
        sequence.Context = cont

        #-----------------------------------------------------------------------------
        # Enable these to make sure data (tracks and raw event) are in memory
        # Useful for timing studies
        #-----------------------------------------------------------------------------
        if self.getProp("preloadRawEvent"):
            from Configurables import Rich__DAQ__LoadRawEvent
            sequence.Members += [ Rich__DAQ__LoadRawEvent("LoadRawRichEvent"+cont) ]
        if self.getProp("preloadTracks"):
            sequence.Members += [ Rich__Rec__Initialise("LoadRawTracks"+cont) ]

        #-----------------------------------------------------------------------------
        # Initialisation
        #-----------------------------------------------------------------------------
        initSeq = GaudiSequencer("RichRecInit"+cont+"Seq")
        initSeq.MeasureTime = True
        sequence.Members += [ initSeq ]

        if self.getProp("checkProcStatus") :
            initSeq.Members += [ Rich__Rec__Initialise("CheckProc"+cont+"Status") ]

        if self.getProp("initPixels") :
            # Make a pixel sequence
            pixelSeq = GaudiSequencer("Rich"+cont+"PixelsSeq")
            pixelSeq.MeasureTime = True
            initSeq.Members += [ pixelSeq ]
            # Raw decoding algorithm
            decodeRich = Rich__DAQ__RawBufferToRichDigitsAlg("DecodeRawRich"+cont)
            decodeRich.DecodeBufferOnly = True
            # Make RichRecPixels
            recoPixs = Rich__Rec__Initialise("Create"+cont+"Pixels")
            recoPixs.CheckProcStatus   = False
            recoPixs.CreatePixels      = True
            # Add algs to sequence
            pixelSeq.Members += [ decodeRich, recoPixs ]

        if self.getProp("initTracks") :
            # Make a track sequence
            trackSeq = GaudiSequencer("Rich"+cont+"TracksSeq")
            trackSeq.MeasureTime = True
            initSeq.Members += [ trackSeq ]
            
            # Field Off ?
            if self.getProp("fieldOff") :
                from Configurables import TrackUseCaloMomentumAlg
                caloTrackAlg = TrackUseCaloMomentumAlg(cont+"TracksWithCALOP")
                caloTrackAlg.OutputTracks = "Rec/Track/BestWithCALOInfo"
                trackSeq.Members += [ caloTrackAlg ]

            # Make RichRecTracks
            recoTracks = Rich__Rec__Initialise("Create"+cont+"Tracks")
            recoTracks.CheckProcStatus   = False
            recoTracks.CreateTracks      = True
            trackSeq.Members += [ recoTracks ]

        if self.getProp("initPhotons") :
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
        if self.getProp("tracklessRings"):
            finderSeq = GaudiSequencer("Rich"+cont+"RingFinderSeq")
            finderSeq.MeasureTime = True
            sequence.Members += [ finderSeq ]
            self.setOtherProp(RichMarkovRingFinderConf(),"context")
            RichMarkovRingFinderConf().applyConf(finderSeq)
            
        #-----------------------------------------------------------------------------
        # PID
        #-----------------------------------------------------------------------------
        pidMode = self.getProp("pidConfig")
        if pidMode != "None" :

            pidSeq = GaudiSequencer("Rich"+cont+"PIDSeq")
            pidSeq.MeasureTime = True
            sequence.Members += [ pidSeq ]
            
            if pidMode == "FullGlobal" or pidMode == "FastGlobal":
                from RichRecSys.RichGlobalPID import RichGlobalPIDConfig
                pidConf = RichGlobalPIDConfig()
                if pidMode == "FastGlobal": pidConf.mode = "Fast"
            elif pidMode == "FastLocal" :
                from RichRecSys.RichHLTLocalPID import RichHLTLocalPIDConfig
                pidConf = RichHLTLocalPIDConfig()
            else:
                raise RuntimeError("ERROR : Unknown PID config '%s'"%pidConf)

            self.setOtherProps(pidConf,["context"])
            pidConf.applyConf(pidSeq)
            del pidConf

            #-------------------------------------------------------------------------
            # Finalise (merge results from various algorithms)
            #-------------------------------------------------------------------------
            if pidMode != "FastLocal" :
                from Configurables import Rich__Rec__HierarchicalPIDMerge
                pidSeq.Members += [Rich__Rec__HierarchicalPIDMerge("Merge"+cont+"RichPIDs")]
        
        #-----------------------------------------------------------------------------
        # Summary objects
        #-----------------------------------------------------------------------------
        if self.getProp("makeSummaryObjects"):
            from Configurables import Rich__Rec__SummaryAlg
            sequence.Members += [Rich__Rec__SummaryAlg("RichRec"+cont+"Summary")]
        
    ## @brief Configure the RICH tools
    #
    def configTools(self):

        # The context
        context = self.getProp("context")
        
        #--------------------------------------------------------------------
                
        # Tracks and segments
        tkConf = RichTrackCreatorConfig()
        tkConf.setProp("radiators",self.usedRadiators())
        self.setOtherProps(tkConf,["context","fieldOff"])
        tkConf.applyConf()
        del tkConf

        # Pixels
        pixConf = RichPixelCreatorConfig()
        pixConf.setProp("detectors",self.usedDetectors())
        self.setOtherProps(pixConf,["context"])
        pixConf.applyConf()
        del pixConf

        # Photons
        photConf = RichPhotonCreatorConfig()
        photConf.setProp("radiators",self.usedRadiators())
        self.setOtherProps(photConf,["context"])
        photConf.applyConf()
        del photConf

        #--------------------------------------------------------------------

        # Misc. items
        
        # geometrical efficiency
        RichTools().geomEff()

        # CK theta resolution
        ckResConf = CKThetaResolutionConfig()
        self.setOtherProp(ckResConf,"context")
        ckResConf.applyConf()
        del ckResConf

        # Particle Types
        partProps = Rich__ParticleProperties("ToolSvc."+context+"_RichParticleProperties")
        partProps.ParticleTypes = self.getProp("particles")

        # clustering
        self.toolRegistry().Tools += [Rich__DAQ__HPDPixelClusteringTool("BackgroundClustering")]

        # Signal detection eff.
        RichTools().sigDetEff()
  
        #--------------------------------------------------------------------
        
          
