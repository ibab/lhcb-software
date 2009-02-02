
## @package RichRecQC
#  High level Configuration tools for RICH Data Quality monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.16 2009-02-02 10:09:01 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from Alignment import RichAlignmentConf
from RichMarkovRingFinder.Configuration import RichMarkovRingFinderConf
from Configurables import ( GaudiSequencer, MessageSvc )
from DDDB.Configuration import DDDBConf
    
# -------------------------------------------------------------------------------------------

## @class RichRecQCConf
#  High level Configuration tools for RICH Data Quality monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichRecQCConf(RichConfigurableUser):

    ## Possible used Configurables
    __used_configurables__ = [ RichAlignmentConf, RichMarkovRingFinderConf ]

    ## Steering options
    __slots__ = {
        "Context": "Offline"  # The context within which to run
       ,"DataType" : "2008" # Data type, can be ['DC06','2008']
       ,"RawMonitoring"             : True
       ,"PidMonitoring"             : True
       ,"PixelMonitoring"           : True
       ,"PhotonMonitoring"          : True
       ,"TracklessRingMonitoring"   : False
       ,"MirrorAlignmentMonitoring" : False
       ,"PidMomentumRanges": [ [2,100], [2,10], [10,70], [70,100] ]
       ,"PidTrackTypes":  [ ["All"] ]
       ,"RecoTrackTypes": [ ["All"],
                            ["Forward","Match"],
                            ["Forward"],["Match"],["KsTrack"],["VeloTT"],["Seed"] ]
       ,"MoniSequencer" : None # The sequencer to add the RICH monitoring algorithms to
       ,"ExpertHistos" : False  # set to True to write out expert histos
       ,"ExpertTests" : [ "RichPixelPositions",
                          "RichTrackGeometry","RichGhostTracks","RichCKThetaResolution",
                          "RichTrackResolution","RichPhotonSignal","RichTrackCKResolutions",
                          "RichPhotonGeometry","PhotonRecoEfficiency","RichPhotonTrajectory"
                          #,"RichRayTracingTests","RichStereoFitterTests"
                          #,"RichDataObjectChecks"
                          #,"RichRecoTiming"
                          ]
       ,"NTupleProduce" : True
       ,"HistoProduce" : True
        }

    ## Set the histogram and ntuple producing options
    def setHistosTupleOpts(self,mon):
        if "NTupleProduce" in mon.properties() :
            mon.NTupleProduce = self.getProp("NTupleProduce")
        if "HistoProduce" in mon.properties() :
            mon.HistoProduce  = self.getProp("HistoProduce")
            
    ## Configure a default monitor algorithm of given type 
    def createMonitor(self,type,name,trackType=None):
        mon = type(name)
        self.setHistosTupleOpts(mon)
        if trackType != None :
            mon.addTool( RichTools().trackSelector(nickname="TrackSelector",private=True) )
            if trackType != ["All"] : mon.TrackSelector.TrackAlgs = trackType
        return mon

    ## Check a new sequence and add to main sequence
    def newSeq(self,sequence,name):
        seq = GaudiSequencer(name)
        seq.MeasureTime = True
        sequence.Members += [seq]
        return seq

    ## Create the track selection name from a list of track alg types
    def trackSelName(self,types) :
        name = ""
        for type in types : name += type
        if name == "ForwardMatch" or name == "MatchForward" : name = "Long"
        return name

    ## Apply the configuration to the given sequence
    def applyConf(self):

        sequence = self.getProp("MoniSequencer")
        if sequence == None : raise RuntimeError("ERROR : Monitor Sequencer not set")

        # Set context
        sequence.Context = self.getProp("Context")

        # Suppress errors from Linker
        MessageSvc().setFatal += [ "LinkedTo::MC/Rich/Hits2MCRichOpticalPhotons",
                                   "LinkedTo::MC/Rich/MCPartsToMCRichTracks",
                                   "LinkedTo::MC/Rich/MCRichHitsToOpPhotons" ]

        # Expert Monitoring
        if self.getProp("ExpertHistos") :
            # Extend PID performance monitoring types
            self.PidTrackTypes += [ ["Forward","Match"],
                                    ["Forward"],["Match"],["KsTrack"],["VeloTT"],["Seed"] ]

        # Some monitoring of raw information
        if self.getProp("RawMonitoring") :
            if self.getProp("DataType") not in ["DC06"]:
                rawSeq = self.newSeq(sequence,"RichRawMoni")
                from Configurables import Rich__DAQ__DataDBCheck
                dbCheck = self.createMonitor(Rich__DAQ__DataDBCheck,"RichRawDataDBCheck")
                rawSeq.Members += [dbCheck]

        # RICH data monitoring
        if self.getProp("PixelMonitoring") :
            self.pixelPerf(self.newSeq(sequence,"RichPixelMoni"))
         
        # Reconstruction monitoring
        if self.getProp("PhotonMonitoring") :
            self.recPerf(self.newSeq(sequence,"RichRecoMoni"))

        # PID Performance
        if self.getProp("PidMonitoring") :
            self.pidPerf(self.newSeq(sequence,"RichPIDMoni"))

        # Trackless rings
        if self.getProp("TracklessRingMonitoring") :
            self.ringsMoni(self.newSeq(sequence,"RichTracklessRingsMoni"))

        # Alignment monitor
        if self.getProp("MirrorAlignmentMonitoring"):
            self.setOtherProps(RichAlignmentConf(),["Context","NTupleProduce","HistoProduce"])
            RichAlignmentConf().AlignmentSequencer = self.newSeq(sequence,"RichMirrAlignMoni")

        # Expert Monitoring
        if self.getProp("ExpertHistos") :
            self.exportMonitoring(self.newSeq(sequence,"RichExpertChecks"))

    ## standalone ring finder monitors
    def ringsMoni(self,sequence):
 
        from Configurables import Rich__Rec__MarkovRingFinder__MC__Moni
        
        conf = RichMarkovRingFinderConf()

        # Activate histos in the finder algs themselves
        conf.rich1TopFinder().HistoProduce    = self.getProp("HistoProduce")
        conf.rich1BottomFinder().HistoProduce = self.getProp("HistoProduce")
        conf.rich2LeftFinder().HistoProduce   = self.getProp("HistoProduce")
        conf.rich2RightFinder().HistoProduce  = self.getProp("HistoProduce")

        # Add monitors
        allMoni  = self.createMonitor(Rich__Rec__MarkovRingFinder__MC__Moni,"MarkovRingMoniAll")
        sequence.Members += [allMoni]
        bestMoni = self.createMonitor(Rich__Rec__MarkovRingFinder__MC__Moni,"MarkovRingMoniBest")
        bestMoni.RingLocation = "Rec/Rich/Markov/RingsBest"
        sequence.Members += [bestMoni]
        isoMoni = self.createMonitor(Rich__Rec__MarkovRingFinder__MC__Moni,"MarkovRingMoniIsolated")
        isoMoni.RingLocation = "Rec/Rich/Markov/RingsIsolated"
        sequence.Members += [isoMoni]
        
    ## Pixel performance monitors
    def pixelPerf(self,sequence):
        
        from Configurables import ( Rich__Rec__MC__PixelQC )

        sequence.Members += [ self.createMonitor(Rich__Rec__MC__PixelQC,"RichRecPixelQC") ]
        
    ## Run the PID Performance monitors
    def pidPerf(self,sequence):

        from Configurables import ( Rich__Rec__MC__PIDQC )

        # Track Types
        for trackType in self.getProp("PidTrackTypes") :

            # Momentum ranges
            for pRange in self.getProp("PidMomentumRanges") :

                # Construct the name for this monitor out of the track types
                # and momentum range
                tkName = self.trackSelName(trackType)
                name = "RiPIDMon" + tkName + `pRange[0]` + "To" + `pRange[1]`

                # Make a monitor alg
                pidMon = self.createMonitor(Rich__Rec__MC__PIDQC,name,trackType)

                # Trackselector momentum cuts
                pidMon.TrackSelector.MinPCut = pRange[0]
                pidMon.TrackSelector.MaxPCut = pRange[1]
 
                # Add to sequence
                sequence.Members += [pidMon]

    ## Reconstruction performance
    def recPerf(self,sequence):

        from Configurables import ( Rich__Rec__MC__RecoQC )
                
        # Track Types
        for trackType in self.getProp("RecoTrackTypes") : 

            # Construct the name for this monitor
            name = "RiCKRes" + self.trackSelName(trackType)
 
            # Make a monitor alg
            mon = self.createMonitor(Rich__Rec__MC__RecoQC,name,trackType)
            mon.HistoPrint = False
          
            # cuts
            if trackType == ["All"] : mon.MinBeta = [ 0.0, 0.0, 0.0 ]
            
            # Add to sequence
            sequence.Members += [mon]

    ## Expert monitoring options
    def exportMonitoring(self,sequence):

        # Define the RICH ntuple file
        if self.getProp("NTupleProduce") :
            from Configurables import NTupleSvc
            NTupleSvc().Output += ["RICHTUPLE1 DATAFILE='rich.tuples.root' TYP='ROOT' OPT='NEW'"]

        checks  = self.getProp("ExpertTests")
        tkTypes = self.getProp("RecoTrackTypes")

        # Turn on/off histos in CK resolution tool
        if "HistoProduce" in RichTools().ckResolution().properties() :
            RichTools().ckResolution().HistoProduce = self.getProp("HistoProduce")

        check = "RichPixelPositions"
        if check in checks :
            from Configurables import Rich__Rec__MC__PixelPositionMonitor
            seq = self.newSeq(sequence,check)
            seq.Members += [Rich__Rec__MC__PixelPositionMonitor("RiRecPixelPosMoni")]

        check = "RichTrackGeometry"
        if check in checks :
            from Configurables import Rich__Rec__MC__TrackGeomMoni
            seq = self.newSeq(sequence,check)
            for trackType in tkTypes :
                name = "RiRec" + self.trackSelName(trackType) + "GeomMoni"
                seq.Members += [ self.createMonitor(Rich__Rec__MC__TrackGeomMoni,name,trackType) ]

        check = "RichTrackResolution"
        if check in checks :
            from Configurables import Rich__Rec__MC__TrackResolutionMoni
            seq = self.newSeq(sequence,check)
            for trackType in tkTypes :
                name = "RiRec" + self.trackSelName(trackType) + "TkResMoni"
                seq.Members += [ self.createMonitor(Rich__Rec__MC__TrackResolutionMoni,name,trackType) ]

        check = "RichCKThetaResolution"
        if check in checks :
            from Configurables import Rich__Rec__MC__CherenkovAngleMonitor
            seq = self.newSeq(sequence,check)
            for trackType in tkTypes :
                name = "RiRecPhotAng" + self.trackSelName(trackType) + "Moni"
                seq.Members += [ self.createMonitor(Rich__Rec__MC__CherenkovAngleMonitor,name,trackType) ]

        check = "RichTrackCKResolutions"
        if check in checks :
            from Configurables import Rich__Rec__MC__CherenkovResMoni
            seq = self.newSeq(sequence,check)
            for trackType in tkTypes :
                name = "RiRec" + self.trackSelName(trackType) + "CKResMoni"
                seq.Members += [ self.createMonitor(Rich__Rec__MC__CherenkovResMoni,name,trackType) ]

        check = "RichPhotonSignal"
        if check in checks :
            from Configurables import Rich__Rec__MC__PhotonSignalMonitor
            seq = self.newSeq(sequence,check)
            for trackType in tkTypes :
                name = "RiRecPhotSig" + self.trackSelName(trackType) + "TkMoni"
                seq.Members += [ self.createMonitor(Rich__Rec__MC__PhotonSignalMonitor,name,trackType) ]
        
        check = "RichPhotonGeometry"
        if check in checks :
            from Configurables import Rich__Rec__MC__PhotonGeomMonitor
            seq = self.newSeq(sequence,check)
            for trackType in tkTypes :
                name = "RiRecPhotGeom" + self.trackSelName(trackType) + "TkMoni"
                seq.Members += [ self.createMonitor(Rich__Rec__MC__PhotonGeomMonitor,name,trackType) ]

        check = "RichPhotonTrajectory"
        if check in checks :
            from Configurables import Rich__Rec__MC__PhotonTrajectoryMonitor
            seq = self.newSeq(sequence,check)
            for trackType in tkTypes :
                name = "RiRecPhotTraj" + self.trackSelName(trackType) + "TkMoni"
                seq.Members += [ self.createMonitor(Rich__Rec__MC__PhotonTrajectoryMonitor,name,trackType) ]
                
        check = "PhotonRecoEfficiency"
        if check in checks :

            RichTools().toolRegistry().Tools += [ "Rich::Rec::PhotonCreator/ForcedRichPhotonCreator" ]
            RichTools().toolRegistry().Tools += [ "Rich::Rec::PhotonRecoUsingQuarticSoln/ForcedPhotonReco" ]
            RichTools().toolRegistry().Tools += [ "Rich::Rec::SimplePhotonPredictor/ForcedRichPhotonPredictor" ]

            context = self.getProp("Context")
                       
            from Configurables import ( Rich__Rec__PhotonCreator, Rich__Rec__SimplePhotonPredictor )
            forcedCreator = Rich__Rec__PhotonCreator(context+".ForcedRichPhotonCreator")
            forcedCreator.MinAllowedCherenkovTheta = [  0.0,     0.0,     0.0    ]
            forcedCreator.MaxAllowedCherenkovTheta = [  999,     999,     999    ]
            forcedCreator.MinPhotonProbability     = [  1e-99,   1e-99,   1e-99  ]
            forcedCreator.NSigma                   = [  999,    999,      999    ]
            forcedCreator.PhotonPredictor = "ForcedRichPhotonPredictor"
            forcedCreator.PhotonRecoTool  = "ForcedPhotonReco"
            forcedPredictor = Rich__Rec__SimplePhotonPredictor(context+".ForcedRichPhotonPredictor")
            forcedPredictor.MinTrackROI            = [  0.0,   0.0,    0.0   ]
            forcedPredictor.MaxTrackROI            = [  9999,  9999,   9999  ]

            from Configurables import Rich__Rec__MC__PhotonRecoEffMonitor
            seq = self.newSeq(sequence,check)
            for trackType in tkTypes :
                name = "RiRecPhotEff" + self.trackSelName(trackType) + "TkMoni"
                seq.Members += [ self.createMonitor(Rich__Rec__MC__PhotonRecoEffMonitor,name,trackType) ]
                
        check = "RichRayTracingTests"
        if check in checks :
            from Configurables import Rich__Rec__MC__PhotonRecoRayTraceTest
            seq = self.newSeq(sequence,check)
            moniA = Rich__Rec__MC__PhotonRecoRayTraceTest("PhotRayTraceTestSimple")
            moniA.FullHPDsInRayTracing = False
            moniB = Rich__Rec__MC__PhotonRecoRayTraceTest("PhotRayTraceTestFull")
            moniB.FullHPDsInRayTracing = True
            seq.Members += [moniA,moniB]

        check = "RichStereoFitterTests"
        if check in checks :
            from Configurables import Rich__Rec__MC__StereoPhotonFitTest
            seq = self.newSeq(sequence,check)
            for trackType in tkTypes :
                name = "RiRecStereoFitter" + self.trackSelName(trackType) + "TkMoni"
                seq.Members += [ self.createMonitor(Rich__Rec__MC__StereoPhotonFitTest,name,trackType) ]
        
        check = "RichGhostTracks"
        if check in checks:
            from Configurables import Rich__Rec__MC__GhostTrackMoni
            seq = self.newSeq(sequence,check)
            seq.Members += [ self.createMonitor(Rich__Rec__MC__GhostTrackMoni,"RichGhostsMoni") ]

        check = "RichDataObjectChecks"
        if check in checks:
            from Configurables import Rich__Rec__DataObjVerifier
            seq = self.newSeq(sequence,check)
            moni = Rich__Rec__DataObjVerifier("RichRecObjPrint")
            moni.PrintPixels   = True
            moni.PrintTracks   = True
            moni.PrintSegments = True
            moni.PrintPhotons  = True
            seq.Members += [moni]
            
        check = "RichRecoTiming"
        if check in checks:
            from Configurables import ( AuditorSvc, ChronoAuditor, Rich__Rec__TimeMonitor )
           
            AuditorSvc().Auditors += [ "ChronoAuditor" ]
            ChronoAuditor().Enable = True

            seq = self.newSeq(sequence,check)

            # overall rich reconstruction
            moni = self.createMonitor(Rich__Rec__TimeMonitor,"RichTime")
            moni.TimingName = "RecoRICHSeq"
            moni.Algorithms = [ "RecoRICHSeq" ]
            seq.Members += [moni]

            # overall track reconstruction
            moni = self.createMonitor(Rich__Rec__TimeMonitor,"TrackTime")
            moni.TimingName = "RecoTrSeq"
            moni.Algorithms = [ "RecoTrSeq" ]
            seq.Members += [moni]
        
