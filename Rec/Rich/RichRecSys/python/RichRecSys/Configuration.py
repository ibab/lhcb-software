
"""
High level configuration tools for RICH Reconstruction
"""
__version__ = "$Id: Configuration.py,v 1.2 2008-08-07 20:56:54 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from RichRecSys.TrackCreator  import *
from RichRecSys.PixelCreator  import *
from RichRecSys.PhotonCreator import *
from RichRecSys.RayTracing    import *
from RichRecSys.MassHypoRings import *
from RichRecSys.GeomEff       import *
from RichRecSys.CKThetaResolution import *
from Configurables import ( GaudiSequencer,
                            Rich__DAQ__RawBufferToRichDigitsAlg,
                            Rich__Rec__Initialise )

class RichRecSys(RichConfigurableUser):

    # Known Reconstruction Configurations
    KnownRecoConfigs = [ "FullOfflineGlobal", "FastOfflineGlobal" ]
    # Known contexts
    KnownContexts    = [ "Offline", "HLT" ]
    # Known radiators
    KnownRadiators   = [ "Aerogel", "Rich1Gas", "Rich2Gas" ]
    
    # Steering options
    __slots__ = {
        "configuration": "FullOfflineGlobal"
       ,"fieldOff":       False # set to True for magnetic field off data
       ,"veloOpen":       False # set to True for Velo open data
       ,"context":    "Offline" # The context within which to run
       ,"radiators": []         # The radiators to use
       ,"preloadRawEvent": False
       ,"preloadTracks": False
       ,"initPixels": True
       ,"initTracks": True
       ,"initPhotons": True
       ,"checkProcStatus": True
       ,"pidAlg": "Global"
        }
        
    def checkConfiguration(self):
        config = self.getProp("configuration")
        if config not in self.KnownRecoConfigs:
            raise RuntimeError("ERROR : Unknown RichRecSys configuration '%s'"%config)
        context = self.getProp("context")
        if context not in self.KnownContexts:
            raise RuntimeError("ERROR : Unknown RichRecSys context '%s'"%context)
        if len(self.getProp("radiators")) == 0 :
            self.setProp("radiators",self.KnownRadiators)

    def usedRadiators(self):
        usedRads = [ False, False, False ]
        for rad in self.getProp("radiators"):
            if rad == "Aerogel"  : usedRads[0] = True
            if rad == "Rich1Gas" : usedRads[1] = True
            if rad == "Rich2Gas" : usedRads[2] = True
        return usedRads

    def usedDetectors(self):
        usedRads = self.usedRadiators()
        usedDets = [ False, False ]
        if usedRads[0] : usedDets[0] = True
        if usedRads[1] : usedDets[0] = True
        if usedRads[2] : usedDets[1] = True
        return usedDets
                    
    def applyConf(self,sequence):
        self.configureAlgorithms(sequence)
        self.configureTools()
        
    #--------------------------------------------------------------------
    def configureAlgorithms(self,sequence):

        # Set the context
        sequence.Context = self.getProp("context")

        #-----------------------------------------------------------------------------
        # Enable these to make sure data (tracks and raw event) are in memory
        # Useful for timing studies
        #-----------------------------------------------------------------------------
        if self.getProp("preloadRawEvent"):
            sequence.Members += [ "Rich::DAQ::LoadRawEvent/LoadRawRichEvent" ]
        if self.getProp("preloadTracks"):
            sequence.Members += [ "Rich::Rec::Initialise/LoadRawTracks" ]

        #-----------------------------------------------------------------------------
        # Initialisation
        #-----------------------------------------------------------------------------
        initSeq = GaudiSequencer("RichRecInitSeq")
        initSeq.MeasureTime = True
        sequence.Members += [ initSeq ]

        if self.getProp("checkProcStatus") :
            initSeq.Members += [ Rich__Rec__Initialise("CheckProcStatus") ]

        if self.getProp("initPixels") :
            # Make a pixel sequence
            pixelSeq = GaudiSequencer("RichPixelsSeq")
            pixelSeq.MeasureTime = True
            initSeq.Members += [ pixelSeq ]
            # Raw decoding algorithm
            decodeRich = Rich__DAQ__RawBufferToRichDigitsAlg("DecodeRawRich")
            decodeRich.DecodeBufferOnly = True
            # Make RichRecPixels
            recoPixs = Rich__Rec__Initialise("CreatePixels")
            recoPixs.CheckProcStatus   = False
            recoPixs.CreatePixels      = True
            # Add algs to sequence
            pixelSeq.Members += [ decodeRich, recoPixs ]

        if self.getProp("initTracks") :
            # Make a track sequence
            trackSeq = GaudiSequencer("RichTracksSeq")
            trackSeq.MeasureTime = True
            initSeq.Members += [ trackSeq ]
            
            # Field Off ?
            if self.getProp("fieldOff") :
                from Configurables import TrackUseCaloMomentumAlg
                caloTrackAlg = TrackUseCaloMomentumAlg("TracksWithCALOP")
                caloTrackAlg.OutputTracks = "Rec/Track/BestWithCALOInfo"
                trackSeq.Members += [ caloTrackAlg ]

            # Make RichRecTracks
            recoTracks = Rich__Rec__Initialise("CreateTracks")
            recoTracks.CheckProcStatus   = False
            recoTracks.CreateTracks      = True
            trackSeq.Members += [ recoTracks ]

        if self.getProp("initPhotons") :
            # Make a photon sequence
            photonSeq = GaudiSequencer("RichPhotonsSeq")
            photonSeq.MeasureTime = True
            initSeq.Members += [ photonSeq ]

             # Make RichRecPhotons
            recoPhotons = Rich__Rec__Initialise("CreatePhotons")
            recoPhotons.CheckProcStatus   = False
            recoPhotons.CreatePhotons     = True
            photonSeq.Members += [ recoPhotons ]

        #-----------------------------------------------------------------------------
        # PID
        #-----------------------------------------------------------------------------
        
    #--------------------------------------------------------------------
    def configureTools(self):

        # Check configuration is sane
        self.checkConfiguration()

        # configuration
        config = self.getProp("configuration")

        #--------------------------------------------------------------------
                
        # Tracks and segments
        RichTrackCreatorConf().setProp("radiators",self.usedRadiators())
        self.setOtherProps(RichTrackCreatorConf(),["context","fieldOff"])
        RichTrackCreatorConf().applyConf()

        # Pixels
        RichPixelCreator().setProp("detectors",self.usedDetectors())
        self.setOtherProps(RichPixelCreator(),["context"])
        RichPixelCreator().applyConf()

        # Photons
        RichPhotonCreatorConf().setProp("radiators",self.usedRadiators())
        self.setOtherProps(RichPhotonCreatorConf(),["context"])
        RichPhotonCreatorConf().applyConf()

        #--------------------------------------------------------------------

        # Misc. items
        
        self.setOtherProp(RichRayTracingConf(),"context")
        self.setOtherProp(RichMassHypoRingsConf(),"context")
        RichRayTracingConf().applyConf()
        RichMassHypoRingsConf().applyConf()

        self.setOtherProp(GeomEffConf(),"context")
        GeomEffConf().applyConf()

        self.setOtherProp(CKThetaResolutionConf(),"context")
        CKThetaResolutionConf().applyConf()

        self.toolRegistry().Tools += ["Rich::DAQ::HPDPixelClusteringTool/BackgroundClustering"]
        
        #--------------------------------------------------------------------
        
          
