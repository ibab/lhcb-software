
## @package RichRecSys
#  High level Configuration tools for RICH Reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.3 2008-08-15 14:41:23 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from RichRecSys.TrackCreator  import *
from RichRecSys.PixelCreator  import *
from RichRecSys.PhotonCreator import *
from RichRecSys.CKThetaResolution import *
from Configurables import ( GaudiSequencer,
                            Rich__DAQ__HPDPixelClusteringTool,
                            Rich__ParticleProperties )
from RichRecSys.RichGlobalPID import *

# ----------------------------------------------------------------------------------

## @class RichRecSysConf
#  Configurable for RICH reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichRecSysConf(RichConfigurableUser):

    ## Known contexts
    KnownContexts      = [ "Offline", "HLT" ]
    ## Known RICH radiators
    KnownRadiators     = [ "Aerogel", "Rich1Gas", "Rich2Gas" ]
    ## Default Particle Types
    KnownParticleTypes = { "Offline" : [ "electron","muon","pion","kaon","proton" ],
                           "HLT"     : [ "pion","kaon" ] }
    
    ## Steering options
    __slots__ = {
        "fieldOff":       False # set to True for magnetic field off data
       ,"veloOpen":       False # set to True for Velo open data
       ,"context":    "Offline" # The context within which to run
       ,"radiators": []         # The radiators to use
       ,"particles": []
       ,"configureTools": True
       ,"configureAlgs": True
       ,"preloadRawEvent": False
       ,"preloadTracks": False
       ,"initPixels": True
       ,"initTracks": True
       ,"initPhotons": True
       ,"checkProcStatus": True
       ,"pidConfig": "FullGlobal"
       ,"makeSummaryObjects": False
       ,"testOldOpts": False
        }

    ## @brief Check the configuration is sensible
    #
    def checkConfiguration(self):
        context = self.getProp("context")
        if context not in self.KnownContexts:
            raise RuntimeError("ERROR : Unknown RichRecSys context '%s'"%context)
        if len(self.getProp("radiators")) == 0 :
            self.setProp("radiators",self.KnownRadiators)
        if len(self.getProp("particles")) == 0 :
            self.setProp("particles",self.KnownParticleTypes[context])

    ## @brief The RICH radiators to use
    #  @return a vector of bools indicating if (Aero,Rich1Gas,Rich2Gas) should be used
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
    def applyConf(self,sequence):
        
        # Check configuration is sane
        self.checkConfiguration()

        # Tool mapping
        self.setOtherProp(RichTools(),"context")

        # Temporary option to use old options, for testing
        if self.getProp("testOldOpts") :
            importOptions( "$RICHRECSYSOPTS/RecoOfflineToolPara.opts" )
            importOptions( "$RICHRECSYSOPTS/RecoOfflineAlgs.opts")
        else:
            if self.getProp("configureAlgs") :
                self.configureAlgorithms(sequence)
            if self.getProp("configureTools") :
                self.configureTools()
        
    ## @brief Configure the RICH algorithms
    #  @param sequence The GaudiSequencer to add the RICH reconstruction to      
    def configureAlgorithms(self,sequence):

        from Configurables import (Rich__DAQ__RawBufferToRichDigitsAlg,
                                   Rich__DAQ__LoadRawEvent,
                                   Rich__Rec__Initialise,
                                   Rich__Rec__HierarchicalPIDMerge )

        # Set the context
        cont = self.getProp("context")
        sequence.Context = cont

        #-----------------------------------------------------------------------------
        # Enable these to make sure data (tracks and raw event) are in memory
        # Useful for timing studies
        #-----------------------------------------------------------------------------
        if self.getProp("preloadRawEvent"):
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
        # PID
        #-----------------------------------------------------------------------------
        pidConf = self.getProp("pidConfig")
        if pidConf != "None" :

            pidSeq = GaudiSequencer("Rich"+cont+"PIDSeq")
            pidSeq.MeasureTime = True
            sequence.Members += [ pidSeq ]
            
            if pidConf == "FullGlobal" or pidConf == "FastGlobal":
                gpidConf = RichGlobalPIDConfig()
                if pidConf == "FastGlobal": gpidConf.mode = "Fast"
                self.setOtherProps(gpidConf,["context"])
                gpidSeq = GaudiSequencer("Rich"+cont+"GPIDSeq")
                pidSeq.Members += [ gpidSeq ]
                gpidConf.applyConf(gpidSeq)
                del gpidConf
            else:
                raise RuntimeError("ERROR : Unknown PID config '%s'"%pidConf)

        #-----------------------------------------------------------------------------
        # Finalise
        #-----------------------------------------------------------------------------
        pidSeq.Members += [ Rich__Rec__HierarchicalPIDMerge("Merge"+cont+"RichPIDs") ]
        
        #-----------------------------------------------------------------------------
        # Summary objects
        #-----------------------------------------------------------------------------
        if self.getProp("makeSummaryObjects"):
            from Configurables import Rich__Rec__SummaryAlg
            sequence.Members += [Rich__Rec__SummaryAlg("RichRec"+cont+"Summary")]
        
    ## @brief Configure the RICH tools
    #
    def configureTools(self):

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
        
          
