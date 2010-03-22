
## @package RichRecQC
#  High level Configuration tools for RICH Data Quality monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.47 2010-02-12 14:12:24 ryoung Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from Alignment import RichAlignmentConf
from Configurables import ( GaudiSequencer, MessageSvc )
from DDDB.Configuration import DDDBConf

# -------------------------------------------------------------------------------------------

## @class RichRecQCConf
#  High level Configuration tools for RICH Data Quality monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichRecQCConf(RichConfigurableUser):

    ## Possible used Configurables
    __used_configurables__ = [ (RichAlignmentConf,None) ]

    ## Default Histogram set
    __default_histo_set__ = "OfflineFull"

    ## List of all known monitors. For sanity checks
    __known_monitors__ = [ "L1SizeMonitoring", "DBConsistencyCheck",
                           "HotPixelFinder", "PidMonitoring",
                           "PixelMonitoring", "TrackMonitoring",
                           "PhotonMonitoring", "TracklessRingAngles",
                           "TracklessRingPeakSearch",
                           "AlignmentMonitoring", "HPDIFBMonitoring",
                           "HPDImageShifts",
                           "RichPixelPositions", "HPDHitPlots",
                           "RichTrackGeometry","RichGhostTracks",
                           "RichCKThetaResolution","RichTrackResolution",
                           "RichPhotonSignal","RichTrackCKResolutions",
                           "RichPhotonGeometry","PhotonRecoEfficiency",
                           "RichPhotonTrajectory","RichStereoFitterTests",
                           "RichRayTracingTests","RichDataObjectChecks",
                           "RichRecoTiming" ]

    ## Added monitors
    __added_monitors__ = [ ]

    ## Removed monitors
    __removed_monitors__ = [ ]

    ## Steering options
    __slots__ = {
        "Context": "Offline"  # The context within which to run
       ,"DataType" : "2008" # Data type, can be ['DC06','2008',2009','2010']
       ,"MoniSequencer" : None # The sequencer to add the RICH monitoring algorithms to
       ,"Monitors" : { "Expert"         : [ "L1SizeMonitoring", "DBConsistencyCheck",
                                            "HotPixelFinder", "PidMonitoring",
                                            "PixelMonitoring", "TrackMonitoring",
                                            "PhotonMonitoring", "TracklessRingAngles",
                                            "TracklessRingPeakSearch",
                                            "AlignmentMonitoring", "HPDIFBMonitoring",
                                            "HPDImageShifts",
                                            "RichPixelPositions", "HPDHitPlots",
                                            "RichTrackGeometry","RichGhostTracks",
                                            "RichCKThetaResolution","RichTrackResolution",
                                            "RichPhotonSignal","RichTrackCKResolutions",
                                            "RichPhotonGeometry","PhotonRecoEfficiency",
                                            "RichPhotonTrajectory","RichStereoFitterTests"
                                            #,"RichRayTracingTests","RichDataObjectChecks","RichRecoTiming"
                                            ],
                       "OfflineFull"    : [ "L1SizeMonitoring", "DBConsistencyCheck",
                                            "HotPixelFinder", "PidMonitoring",
                                            "PixelMonitoring", "TrackMonitoring",
                                            "PhotonMonitoring", "TracklessRingAngles",
                                            "HPDImageShifts",
                                            "AlignmentMonitoring", "HPDIFBMonitoring" ],
                       "OfflineExpress" : [ "L1SizeMonitoring", "DBConsistencyCheck",
                                            "HotPixelFinder", "PidMonitoring",
                                            "PixelMonitoring", "TrackMonitoring",
                                            "PhotonMonitoring", "TracklessRingAngles",
                                            "HPDImageShifts",
                                            "AlignmentMonitoring", "HPDIFBMonitoring" ],
                       "Online"         : [ "DBConsistencyCheck",
                                            "PhotonMonitoring", "TracklessRingAngles",
                                            "AlignmentMonitoring" ],
                       "None"           : [ ]
                       }
       ,"PidMomentumRanges": { "Expert"         : [ [2,100], [2,10], [10,70], [70,100] ],
                               "OfflineFull"    : [ [2,100], [2,10], [10,70], [70,100] ],
                               "OfflineExpress" : [ [2,100], [2,10], [10,70], [70,100] ],
                               "Online"         : [ [2,100], [2,10], [10,70], [70,100] ]
                               }
       ,"MinTrackBeta"     : [ 0.9999, 0.9999, 0.9999 ]
       ,"PidTrackTypes":  { "Expert"         : [ ["All"],["Forward","Match"],
                                                 ["Forward"],["Match"],["KsTrack"],["VeloTT"],["Seed"] ],
                            "OfflineFull"    : [ ["Forward","Match"] ],
                            "OfflineExpress" : [ ["Forward","Match"] ],
                            "Online"         : [ ["Forward","Match"] ]
                            }
       ,"RecoTrackTypes": { "Expert"         : [ ["All"],["Forward","Match"],
                                                 ["Forward"],["Match"],
                                                 ["KsTrack"],["VeloTT"],["Seed"] ],
                            "OfflineFull"    : [ ["Forward","Match"],
                                                 ["Forward"],["Match"],
                                                 ["KsTrack"],["VeloTT"],["Seed"] ],
                            "OfflineExpress" : [ ["Forward","Match"],
                                                 ["Forward"],["Match"],
                                                 ["KsTrack"],["VeloTT"],["Seed"] ],
                            "Online"         : [ ["Forward","Match"] ]
                            }
       ,"EffTrackTypes": { "Expert"         : [ ["Forward","Match"],
                                                ["Forward"],["Match"],["KsTrack"],
                                                ["VeloTT"],["Seed"] ],
                           "OfflineFull"    : [ ["Forward","Match"],
                                                ["Forward"],["Match"],["KsTrack"],
                                                ["VeloTT"],["Seed"] ],
                           "OfflineExpress" : [ ["Forward","Match"],
                                                ["Forward"],["Match"],["KsTrack"],
                                                ["VeloTT"],["Seed"] ],
                           "Online"         : [ ["Forward","Match"] ]
                           }
       ,"TracklessRingClasses": { "Expert"         : [ "All","Best","Isolated" ],
                                  "OfflineFull"    : [ "All","Best","Isolated" ],
                                  "OfflineExpress" : [ "All","Best","Isolated" ],
                                  "Online"         : [ "Isolated" ],
                            }
       ,"Histograms"    : "OfflineFull"
       ,"NTupleProduce" : True
       ,"WithMC"        : False # set to True to use MC truth
       ,"OutputLevel"   : INFO  # The output level to set all algorithms and tools to use
       ,"EventCuts"     : { }   # Event selection cuts for monitoring. Default no cuts
       ,"RichPIDLocation" : "Rec/Rich/PIDs" # Location of RichPID data objects to monitor
        }

    ## Get the given option
    def getHistoOptions(self,optionname):
        histoset = self.getProp("Histograms")
        option = self.getProp(optionname)
        if option.has_key(histoset) :
            return option[histoset]
        else :
            return option[ self.__default_histo_set__ ]

    ## Add a monitor to a given histo set
    def addMonitor(self,monitor):
        if monitor not in self.__added_monitors__ : self.__added_monitors__.append(monitor)

    ## Remove a monitor to a given histo set
    def removeMonitor(self,monitor):
        if monitor not in self.__removed_monitors__ : self.__removed_monitors__.append(monitor)
                
    ## Set the histogram and ntuple producing options
    def setHistosTupleOpts(self,mon):
        if "NTupleProduce" in mon.properties() :
            mon.NTupleProduce = self.getProp("NTupleProduce")
        if "HistoProduce" in mon.properties() :
            mon.HistoProduce  = self.getProp("Histograms") != "None"

    ## Configure a default monitor algorithm of given type
    def createMonitor(self,type,name,trackType=None,typeSelOnly=False):
        mon = type(name)
        self.setHistosTupleOpts(mon)
        if self.isPropertySet("OutputLevel") : mon.OutputLevel = self.getProp("OutputLevel")
        if trackType != None :
            mon.addTool( self.richTools().trackSelector(nickname="TrackSelector",private=True), name="TrackSelector" )
            if trackType != ["All"] : mon.TrackSelector.TrackAlgs = trackType
        if typeSelOnly :
            bigvalue = 1e+30
            mon.TrackSelector.MinPCut    = 0
            mon.TrackSelector.MaxPCut    = bigvalue
            mon.TrackSelector.MinPtCut   = 0
            mon.TrackSelector.MaxPtCut   = bigvalue
            mon.TrackSelector.MinChi2Cut = 0
            mon.TrackSelector.MaxChi2Cut = bigvalue
            mon.TrackSelector.MinLikelihood   = -bigvalue
            mon.TrackSelector.MaxLikelihood   =  bigvalue
            mon.TrackSelector.MinCloneDistCut = -bigvalue
            mon.TrackSelector.MaxCloneDistCut =  bigvalue
            mon.TrackSelector.MinGhostProbCut = -bigvalue
            mon.TrackSelector.MaxGhostProbCut =  bigvalue
            mon.TrackSelector.AcceptClones    = True
        return mon

    ## Check a new sequence and add to main sequence
    def newSeq(self,sequence,name):
        seq = GaudiSequencer(name)
        seq.MeasureTime = True
        if self.isPropertySet("OutputLevel") : seq.OutputLevel = self.getProp("OutputLevel")
        sequence.Members += [seq]
        return seq

    ## Create the track selection name from a list of track alg types
    def trackSelName(self,types) :
        name = ""
        for type in types : name += type
        if name == "ForwardMatch" or name == "MatchForward" : name = "Long"
        return name

    ## Sanity checks
    def sanityChecks(self):

        histoset = self.getProp("Histograms")

        # Add additionsl monitors to the list for this histo set
        for monitor in self.__added_monitors__ :
            if monitor not in self.Monitors[histoset] : self.Monitors[histoset].append(monitor)

        # Disable any monitors for this histo set
        for monitor in self.__removed_monitors__ :
            if monitor in self.Monitors[histoset] : self.Monitors[histoset].remove(monitor)

        # Check sequencer is set
        if not self.isPropertySet("MoniSequencer") :
            raise RuntimeError("ERROR : Monitor Sequencer not set")

        # Check the monitor list contains only known monitors
        for mon in self.getHistoOptions("Monitors"):
            if mon not in self.__known_monitors__ :
                raise RuntimeError("Unknown monitor '%s'"%mon)

    ## Apply the configuration to the given sequence
    def applyConf(self):

        ## Sanity checks
        self.sanityChecks()

        # Sequence to add minotrs to
        sequence = self.getProp("MoniSequencer")

        # Set context
        sequence.Context = self.getProp("Context")

        # Suppress errors from Linker
        msgSvc = getConfigurable("MessageSvc")
        msgSvc.setFatal += [ "LinkedTo::MC/Rich/Hits2MCRichOpticalPhotons",
                             "LinkedTo::MC/Particles2MCRichTracks" ]

        # Event selection ?
        if self.isPropertySet("EventCuts") :
            from Configurables import Rich__Rec__EventSelectionAlg
            evtSel = Rich__Rec__EventSelectionAlg("RichMoniEventSel")
            sequence.Members += [evtSel]
            for name,cut in self.getProp("EventCuts").iteritems() :
                evtSel.setProp("Min"+name,cut[0])
                evtSel.setProp("Max"+name,cut[1])

        # Do we need MC access or not
        if not self.getProp("WithMC") :
            self.toolRegistry().Tools += [ "Rich::MC::NULLMCTruthTool/RichMCTruthTool",
                                           "Rich::Rec::MC::NULLMCTruthTool/RichRecMCTruthTool" ]

        # The list of monitors to run
        monitors = self.getHistoOptions("Monitors")
        
        # Some monitoring of raw information
        if self.getProp("DataType") not in ["DC06"]:

            rawSeq = self.newSeq(sequence,"RichRawMoni")
            
            if "L1SizeMonitoring" in monitors :
                
                # Data Sizes
                from Configurables import Rich__DAQ__RawDataSize
                dataSize = self.createMonitor(Rich__DAQ__RawDataSize,"RichRawDataSize")
                rawSeq.Members += [dataSize]

            if "DBConsistencyCheck" in monitors :

                # Data base consistency
                from Configurables import Rich__DAQ__DataDBCheck
                dbCheck = self.createMonitor(Rich__DAQ__DataDBCheck,"RichRawDataDBCheck")
                rawSeq.Members += [dbCheck]

            if "HotPixelFinder" in monitors :

                # Hot pixel finder
                from Configurables import Rich__HPDAnalysisAlg
                hotpix = self.createMonitor(Rich__HPDAnalysisAlg,"RichHotPixels")
                hotpix.HPDAnalysisTools = ["RichHotPixelFinder"]
                rawSeq.Members += [hotpix]

        # RICH data monitoring
        if "PixelMonitoring" in monitors :
            self.pixelPerf(self.newSeq(sequence,"RichPixelMoni"))

        # Track monitoring
        if "TrackMonitoring" in monitors :
            self.trackMoni(self.newSeq(sequence,"RichTrackMoni"))
            
        # Reconstruction monitoring
        if "PhotonMonitoring" in monitors :
            self.recPerf(self.newSeq(sequence,"RichRecoMoni"))

        # PID Performance
        if "PidMonitoring" in monitors :
            self.pidPerf(self.newSeq(sequence,"RichPIDMoni"))

        # Trackless rings angles
        if "TracklessRingAngles" in monitors :
            from Configurables import RichRecSysConf
            types = RichRecSysConf().TracklessRingAlgs
            for type in types :
                self.ringsMoni(type,self.newSeq(sequence,"Rich"+type+"TracklessRingsMoni"))

        # Trackless rings peak search
        if "TracklessRingPeakSearch" in monitors :
            from Configurables import RichRecSysConf
            types = RichRecSysConf().TracklessRingAlgs
            for type in types :
                self.ringsPeakSearch(type,self.newSeq(sequence,"Rich"+type+"TracklessRingsPeaks"))

        # Alignment monitor
        if "AlignmentMonitoring" in monitors :
            conf = self.getRichCU(RichAlignmentConf)
            self.setOtherProps(conf,["Histograms","Context","NTupleProduce","WithMC"])
            conf.AlignmentSequencer = self.newSeq(sequence,"RichMirrAlignMoni")

        # HPD IFB
        if "HPDIFBMonitoring" in monitors :
            self.ionfeedbackMoni(self.newSeq(sequence,"RichHPDIonFeedback"))

        # HPD Image shifts
        if "HPDImageShifts" in monitors :
            self.hpdImageShifts(self.newSeq(sequence,"RichHPDImageShifts"))

        # Expert Monitoring
        if self.getProp("Histograms") == "Expert" :
            self.expertMonitoring( self.newSeq(sequence,"RichExpertChecks") )

    ## standalone ring finder monitors
    def ringsMoni(self,type,sequence):

        from Configurables import ( Rich__Rec__MC__TracklessRingMoni )
        
        # Activate histos in the finder algs themselves
        if "Expert" == self.getProp("Histograms"):
            if type == "Markov" :
                from Configurables import RichMarkovRingFinderConf
                conf = RichMarkovRingFinderConf()
            elif type == "ENN" :
                from Configurables import RichENNRingFinderConf
                conf = RichENNRingFinderConf()
            elif type == "Template" :
                from Configurables import RichTemplateRingFinderConf
                conf = RichTemplateRingFinderConf()
            else :
                raise RuntimeError("ERROR : Unknown trackless ring finder type")
            conf.enableHistos( True )

        histoSet = self.getProp("Histograms")
 
        # Add monitors
        for ringclass in self.getHistoOptions("TracklessRingClasses") :
            moni = self.createMonitor(Rich__Rec__MC__TracklessRingMoni,type+"RingMoni"+ringclass)
            moni.RingLocation = "Rec/Rich/"+type+"/Rings"+ringclass
            sequence.Members += [moni]

    ## standalone ring finder peak searches
    def ringsPeakSearch(self,type,sequence):

        from Configurables import ( Rich__Rec__RingPeakSearch )

        # Ring peak search
        for ringclass in self.getHistoOptions("TracklessRingClasses") :
            search = self.createMonitor(Rich__Rec__RingPeakSearch,type+"RingPeakSearch"+ringclass)
            search.RingLocation = "Rec/Rich/"+type+"/Rings"+ringclass
            sequence.Members += [search]
        
    ## Pixel performance monitors
    def pixelPerf(self,sequence):

        from Configurables import ( Rich__Rec__MC__PixelQC,
                                    Rich__Rec__PixelClusterMoni )

        sequence.Members += [ self.createMonitor(Rich__Rec__MC__PixelQC,"RichRecPixelQC") ]
        sequence.Members += [ self.createMonitor(Rich__Rec__PixelClusterMoni,"RichRecPixelClusters") ]

    ## Run the PID Performance monitors
    def pidPerf(self,sequence):

        from Configurables import ( Rich__Rec__MC__PIDQC )

        # Track Types
        for trackType in self.getHistoOptions("PidTrackTypes") :

            # Momentum ranges
            for pRange in self.getHistoOptions("PidMomentumRanges") :

                # Construct the name for this monitor out of the track types
                # and momentum range
                tkName = self.trackSelName(trackType)
                name = "RiPIDMon" + tkName + `pRange[0]` + "To" + `pRange[1]`

                # Make a monitor alg
                pidMon = self.createMonitor(Rich__Rec__MC__PIDQC,name,trackType)

                # Trackselector momentum cuts
                pidMon.TrackSelector.MinPCut = pRange[0]
                pidMon.TrackSelector.MaxPCut = pRange[1]

                # RichPID location
                pidMon.InputPIDs = self.getProp("RichPIDLocation")

                # Add to sequence
                sequence.Members += [pidMon]

    ## Track monitoring
    def trackMoni(self,sequence):

        from Configurables import ( Rich__Rec__MC__TrackSelEff )

        # Track Types
        for trackType in self.getHistoOptions("EffTrackTypes") :

            # Construct the name for this monitor
            name = "Ri" + self.trackSelName(trackType) + "TrkEff"

            # Make a monitor alg
            mon = self.createMonitor(Rich__Rec__MC__TrackSelEff,name,trackType,True)
            #mon.HistoPrint = True

            # Add to sequence
            sequence.Members += [mon]

    ## Reconstruction performance
    def recPerf(self,sequence):

        from Configurables import ( Rich__Rec__MC__RecoQC )

        # Track Types
        for trackType in self.getHistoOptions("RecoTrackTypes") :

            # Construct the name for this monitor
            name = "RiCKRes" + self.trackSelName(trackType)

            # Make a monitor alg
            mon = self.createMonitor(Rich__Rec__MC__RecoQC,name,trackType)
            mon.HistoPrint = False

            # cuts
            mon.MinBeta = self.getProp("MinTrackBeta")

            # Add to sequence
            sequence.Members += [mon]

    ## Ion Feedback monitor
    def ionfeedbackMoni(self, sequence):
        from Configurables import Rich__Mon__HPDIonFeedbackMoni
        RichIFBMon                       = Rich__Mon__HPDIonFeedbackMoni("RichHPDIFBMoni")
        RichIFBMon.IsDark                = False
        RichIFBMon.MonitorRate           = 1
        RichIFBMon.EventSize             = 3000000
        RichIFBMon.IonFeedbackALICE      = 5
        RichIFBMon.IonFeedbackLHCB       = 5
        RichIFBMon.WantIFB               = True
        RichIFBMon.WantHitmaps           = False
        RichIFBMon.WantQuickHitmap       = False
        sequence.Members      += [RichIFBMon]

    ## HPD image shift monitoring
    def hpdImageShifts(self,sequence):
        from Configurables import Rich__Mon__RichHPDImageSummary
        imageSummary = Rich__Mon__RichHPDImageSummary("RichHPDImageSummary")
        imageSummary.HistoProduce = False
        imageSummary.StatEntityList +=  [ ".*HPD.*" ]
        imageSummary.StatEntityList +=  [ ".*EventTime.*" ]
        sequence.Members += [imageSummary]

    ## Expert monitoring options
    def expertMonitoring(self,sequence):

        # Define the RICH ntuple file
        if self.getProp("NTupleProduce") :
            from Configurables import NTupleSvc
            NTupleSvc().Output += ["RICHTUPLE1 DATAFILE='rich.tuples.root' TYP='ROOT' OPT='NEW'"]

        checks  = self.getHistoOptions("Monitors")
        tkTypes = self.getHistoOptions("RecoTrackTypes")

        # Turn on/off histos in CK resolution tool
        if "HistoProduce" in self.richTools().ckResolution().properties() :
            self.richTools().ckResolution().HistoProduce = self.getProp("Histograms") != "None"

        check = "RichPixelPositions"
        if check in checks :
            from Configurables import Rich__Rec__MC__PixelPositionMonitor
            seq = self.newSeq(sequence,check)
            seq.Members += [Rich__Rec__MC__PixelPositionMonitor("RiRecPixelPosMoni")]

        check = "HPDHitPlots"
        if check in checks :
            from Configurables import Rich__Rec__HPDHitsMoni
            seq = self.newSeq(sequence,check)
            seq.Members += [Rich__Rec__HPDHitsMoni("HPDHitsMoni")]

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

            self.richTools().toolRegistry().Tools += [ "Rich::Rec::PhotonCreator/ForcedRichPhotonCreator" ]
            self.richTools().toolRegistry().Tools += [ "Rich::Rec::PhotonRecoUsingQuarticSoln/ForcedPhotonReco" ]
            self.richTools().toolRegistry().Tools += [ "Rich::Rec::SimplePhotonPredictor/ForcedRichPhotonPredictor" ]

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

