
"""
Track creator options for RICH Reconstruction
"""
__version__ = "$Id: TrackCreator.py,v 1.1 2008-08-07 20:56:54 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration  import *
from GaudiKernel.SystemOfUnits import mm
from RichRecSys.SegmentCreator import *
from Configurables import ( Rich__Rec__TrackCreatorFromRecoTracks,
                            Rich__Rec__DetailedTrSegMakerFromRecoTracks,
                            Rich__Rec__FastTrSegMakerFromRecoTracks,
                            Rich__Rec__TrackCreatorFromRecoTracksWithForcedIneffic,
                            Rich__Rec__TrackSelector,
                            Rich__Rec__MC__MCTrueTrackSelector )

class RichTrackCreatorConf(RichConfigurableUser):

    # Known track creator configurations
    KnownConfigs   = [ "RecoTracks","CheatedRecoTracks" ]
    # Known track algorithms
    KnownTrackAlgs = ["Seed","Match","Forward","KsTrack","VeloTT"]
    # Known track segment makers
    KnownTrSegmakers = ["Detailed","Fast"]
    # Known track selectors
    KnownTrackSelectors = ["Reco","MCTrue"]
       
    # Steering options
    __slots__ = {
        "configuration": "RecoTracks" # The track handling configuration
       ,"context":       "Offline"  # The context within which to run
       ,"fieldOff":       False # set to True for magnetic field off data
       ,"trackAlgs":     []         # The Track algorithm types to use
       ,"radiators":     [True,True,True] # The radiators to use (Aerogel/Rich1Gas/Rich2Gas)
       ,"trSegMaker":    "Detailed" # The track segment maker to use
       ,"minRadPathLength": [0*mm,500*mm,1500*mm] # Min pathlengths (Aerogel/Rich1Gas/Rich2Gas)
       ,"checkBeamPipe": [False,True,False] # Check for intersections with the beam pipe
       ,"zTolerances":   [ 800*mm, 2000*mm, 3000*mm ] # Z tolerances (Aerogel/Rich1Gas/Rich2Gas)
       ,"primaryExtrapolator": "TrackHerabExtrapolator" # The main track state extrapolator
       ,"backupExtrapolator":  "TrackParabolicExtrapolator" # The backup track state extrapolator
       ,"segmentType": "AllStateVectors" # The RichTrackSegment type
       ,"trackSelector": "Reco" # The track selector to use
       ,"forceTrackInefficiency" : -1 # Simulate a tracking inefficiency of fraction X
        }
        
    def checkConfiguration(self):
        # Track Algs
        if len(self.getProp("trackAlgs")) == 0 :
            self.setProp("trackAlgs",self.KnownTrackAlgs)
        # Config
        config = self.getProp("configuration")
        if config not in self.KnownConfigs:
            raise RuntimeError("ERROR : Unknown TrackCreator configuration '%s'"%config)

    def defaultTrSegMaker(self,nickname):
        
        context = self.getProp("context")
        type    = self.getProp("trSegMaker")
        
        if type == "Detailed":
            self.toolRegistry().Tools += ["Rich::Rec::DetailedTrSegMakerFromRecoTracks/"+nickname]
            trSeg = Rich__Rec__DetailedTrSegMakerFromRecoTracks("ToolSvc."+context+"_"+nickname)
            trSeg.SegmentType              = self.getProp("segmentType")
            trSeg.PrimaryTrackExtrapolator = self.getProp("primaryExtrapolator")
            trSeg.BackupTrackExtrapolator  = self.getProp("backupExtrapolator")
            trSeg.UseRadiators             = self.getProp("radiators")
            trSeg.MinRadiatorPathLength    = self.getProp("minRadPathLength")
            trSeg.CheckBeamPipe            = self.getProp("checkBeamPipe")
            trSeg.ZTolerances              = self.getProp("zTolerances")
        
        elif type == "Fast":
            self.toolRegistry().Tools += ["Rich::Rec::FastTrSegMakerFromRecoTracks/"+nickname]
            trSeg = Rich__Rec__FastTrSegMakerFromRecoTracks(context+"_"+nickname)
            
        else:
            raise RuntimeError("ERROR : Unknown TrSegMaker '%s'"%type)

        return trSeg

    def configureTrackCreator(self,nickname):
        
        config = self.getProp("configuration")
        # RichTrackCreator using full reconstructed information : Use full detailed information
        if config == "RecoTracks":
            ineff = self.getProp("forceTrackInefficiency")
            context = self.getProp("context")
            if ineff < 0:
                type = "Rich::Rec::TrackCreatorFromRecoTracks"
                # Make an instance of the tool
                trackCr = Rich__Rec__TrackCreatorFromRecoTracks("ToolSvc."+context+"_"+nickname)
            else:
                type = "Rich::Rec::TrackCreatorFromRecoTracksWithForcedIneffic"
                # Make an instance of the tool
                trackCr = Rich__Rec__TrackCreatorFromRecoTracksWithForcedIneffic("ToolSvc."+context+"_"+nickname)
            self.toolRegistry().Tools += [ type+"/"+"RichTrackCreator" ]

        if self.getProp("fieldOff") :
            trackCr.TracksLocation = "Rec/Track/BestWithCALOInfo"
            
        # Track selector
        trsel = self.getProp("trackSelector")
        if trsel == "Reco":
            trackCr.addTool( Rich__Rec__TrackSelector, name = "TrackSelector" )
        elif trsel == "MCTrue":
            trackCr.addTool( Rich__Rec__MC__MCTrueTrackSelector, name = "TrackSelector" )
        else:
            raise RuntimeError("ERROR : Unknown TrackSelector '%s'"%trsel)
        
        # Track algs to select
        trackCr.TrackSelector.TrackAlgs = self.getProp("trackAlgs")
        
        # Segment maker
        trSegNickName = "TrSegMaker"
        trackCr.TrackSegmentTool = trSegNickName
        trSeg = self.defaultTrSegMaker(trSegNickName)
        
    def applyConf(self):

        # Check the configuration
        self.checkConfiguration()

        # segments
        self.setOtherProps(RichSegmentCreatorConf(),["context"])

        # segment creator
        RichSegmentCreatorConf().applyConf()

        # Configure the tracking tools
        self.configureTrackCreator("RichTrackCreator")
