
## @package RichRecQC
#  High level Configuration tools for RICH Data Quality monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.47 2010-02-12 14:12:24 ryoung Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from Configurables import ( RichAlignmentConf, RichRecSysConf,
                            GaudiSequencer, MessageSvc )

# -------------------------------------------------------------------------------------------
# Workaround for Configurables problem
#import Configurables
#if "RichPIDQCConf" in Configurables.__all__:
#    from Configurables import RichPIDQCConf
#else:
#    from Gaudi.Configuration import ConfigurableUser
#    class RichPIDQCConf(ConfigurableUser):
#        pass
# -------------------------------------------------------------------------------------------

#__all__ = [ "RichRecQCConf" ]

## @class RichRecQCConf
#  High level Configuration tools for RICH Data Quality monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichRecQCConf(RichConfigurableUser):

    ## Possible used Configurables
    __used_configurables__ = [ (RichAlignmentConf,None), (RichRecSysConf,'RichOfflineRec') ]

    ## Default Histogram set
    __default_histo_set__ = "OfflineFull"

    ## List of all known monitors. For sanity checks
    __known_monitors__ = [ "ODIN", "DBConsistencyCheck", "L1SizeMonitoring",
                           "HotPixelFinder", "PidMonitoring",
                           "PixelMonitoring", "TrackMonitoring",
                           "PhotonMonitoring", "TracklessRingAngles",
                           "TracklessRingPeakSearch",
                           "AlignmentMonitoring", "HPDIFBMonitoring",
                           "HPDImageShifts", "DataDecodingErrors",
                           "RichPixelPositions", "HPDHitPlots",
                           "RichTrackGeometry", "RichGhostTracks",
                           "RichCKThetaResolution", "RichTrackResolution",
                           "RichPhotonSignal", "RichTrackCKResolutions",
                           "RichPhotonGeometry", "PhotonRecoEfficiency",
                           "RichPhotonTrajectory", "RichStereoFitterTests",
                           "RichRayTracingTests", "RichDataObjectChecks",
                           "RichRecoTiming", "RichFuncCKResPlots",
                           "RichPerfFromData","AerogelMonitoring",
                           "RichTracksToTextFile","RichHitsToTextFile"
                           ]

    ## Added monitors
    __added_monitors__ = [ ]

    ## Removed monitors
    __removed_monitors__ = [ ]

    ## Steering options
    __slots__ = {
        "Context"  : "Offline"  # The context within which to run
       ,"DataType" : "2012"     # Data type, can be ['2008',2009','2010','2011','2012','Upgrade']
       ,"OnlineMode"   : False  # Online mode
       ,"MoniSequencer" : None  # The sequencer to add the RICH monitoring algorithms to
       ,"Monitors" : { "Expert"         : [ "DBConsistencyCheck", "L1SizeMonitoring",
                                            "DataDecodingErrors", "ODIN",
                                            "HotPixelFinder", "PidMonitoring",
                                            "PixelMonitoring", "TrackMonitoring",
                                            "PhotonMonitoring", "TracklessRingAngles",
                                            "TracklessRingPeakSearch",
                                            "AlignmentMonitoring", "HPDIFBMonitoring",
                                            "HPDImageShifts", "RichFuncCKResPlots",
                                            "RichPixelPositions", "HPDHitPlots",
                                            "RichTrackGeometry", "RichGhostTracks",
                                            "RichCKThetaResolution", "RichTrackResolution",
                                            "RichPhotonSignal", "RichTrackCKResolutions",
                                            "RichPhotonGeometry", "PhotonRecoEfficiency",
                                            "RichPhotonTrajectory", "RichStereoFitterTests"
                                            #,"RichRayTracingTests","RichDataObjectChecks",
                                            #"RichRecoTiming"
                                            ],
                       "OfflineFull"    : [ "DBConsistencyCheck", "L1SizeMonitoring",
                                            "HotPixelFinder", "PidMonitoring",
                                            "PixelMonitoring", "TrackMonitoring",
                                            "PhotonMonitoring", "TracklessRingAngles",
                                            "DataDecodingErrors",
                                            "AlignmentMonitoring", "HPDIFBMonitoring" ],
                       "OfflineExpress" : [ "DBConsistencyCheck", "L1SizeMonitoring",
                                            "HotPixelFinder", "PidMonitoring",
                                            "PixelMonitoring", "TrackMonitoring",
                                            "PhotonMonitoring", "TracklessRingAngles",
                                            "DataDecodingErrors",
                                            "AlignmentMonitoring", "HPDIFBMonitoring" ],
                       "Online"         : [ "L1SizeMonitoring","HPDHitPlots",
                                            "DataDecodingErrors", "TrackMonitoring",
                                            "PhotonMonitoring", "TracklessRingAngles",
                                            "AlignmentMonitoring", "HPDImageShifts" ],
                       "None"           : [ ]
                       }
       ,"PidMomentumRanges": { "Expert"         : [ [2,100], [2,10], [10,70], [70,100] ],
                               "OfflineFull"    : [ [2,100], [2,10], [10,70], [70,100] ],
                               "OfflineExpress" : [ [2,100], [2,10], [10,70], [70,100] ],
                               "Online"         : [ [2,100] ]
                               }
       ,"MinTrackBeta"     : [ 0.9999, 0.9999, 0.9999 ]
       ,"PidTrackTypes":  { "Expert"         : [ ["Forward","Match"],
                                                 ["Forward"],["Match"],["KsTrack"],
                                                 ["VeloTT"],["Seed"] ],
                            "OfflineFull"    : [ ["Forward","Match"] ],
                            "OfflineExpress" : [ ["Forward","Match"] ],
                            "Online"         : [ ["Forward","Match"] ]
                            }
       ,"RecoTrackTypes": { "Expert"         : [ ["Forward","Match"],
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
       ,"NTupleProduce" : False
       ,"WithMC"        : False # set to True to use MC truth
       ,'Simulation'    : False # MC or real data (default)
       ,"OutputLevel"   : INFO  # The output level to set all algorithms and tools to use
       ,"EventCuts"     : { }   # Event selection cuts for monitoring. Default no cuts
       ,"RichPIDLocation" : "Rec/Rich/PIDs" # Location of RichPID data objects to monitor
       ,"Radiators"       : None         # The radiators to use
       ,"CKThetaResRange" : [ ] # The CK theta range to use in resolution plots. Empty means defaults
       ,"EventFilter"     : "" # Event filters for the monitoring. Default none
        }

    ## Initialize 
    def initialize(self):
        self.setRichDefault("Radiators","Offline", ["Aerogel","Rich1Gas","Rich2Gas"] )
        self.setRichDefault("Radiators","HLT",     ["Rich1Gas","Rich2Gas"] )

    ## @brief The RICH radiators to use
    #  @return a vector of bools indicating if (Aerogel,Rich1Gas,Rich2Gas) should be used
    def usedRadiators(self):
        usedRads = [ False, False, False ]
        for rad in self.getProp("Radiators"):
            if rad == "Aerogel"  : usedRads[0] = True
            if rad == "Rich1Gas" : usedRads[1] = True
            if rad == "Rich2Gas" : usedRads[2] = True
        return usedRads

    ## Access the alignment Configurable
    def alignmentConf(self):
        return self.getRichCU(RichAlignmentConf)

    ## Access the Reconstruction configurable
    def recoConf(self):
        # Hardcoded name for the moment
        return RichRecSysConf('RichOfflineRec')

    ## Get the given option
    def getHistoOptions(self,optionname):
        histoset = self.getProp("Histograms")
        option = self.getProp(optionname)
        
        if option.has_key(histoset) :
            return option[histoset]
        else :
            return option[ self.__default_histo_set__ ]

    ## Get track type list
    def getTrackTypes(self,optionname):
        types = self.getHistoOptions(optionname)
        return [ tk for tk in types if self.trackTypeIsActive(tk) ]

    ## Add a monitor to a given histo set
    def addMonitor(self,monitor):
        if monitor not in self.__added_monitors__ : self.__added_monitors__.append(monitor)

    ## Add a list of monitors to a given histo set
    def addMonitors(self,monitors):
        for monitor in monitors : self.addMonitor(monitor)
    
    ## Remove a monitor from a given histo set
    def removeMonitor(self,monitor):
        if monitor not in self.__removed_monitors__ : self.__removed_monitors__.append(monitor)

    ## Remove a list of monitors from a given histo set
    def removeMonitors(self,monitors):
        for monitor in monitors : self.removeMonitor(monitor)

    ## Set the histogram and ntuple producing options
    def setHistosTupleOpts(self,mon):
        if "NTupleProduce" in mon.properties() :
            mon.NTupleProduce = self.getProp("NTupleProduce")
        if "HistoProduce" in mon.properties() :
            mon.HistoProduce  = self.getProp("Histograms") != "None"

    ## Configure a default monitor algorithm of given type
    def createMonitor(self,type,name,trackType=None,tkCuts="None"):
        
        mon = type(name)
        self.setHistosTupleOpts(mon)
        
        if self.isPropertySet("OutputLevel") : mon.OutputLevel = self.getProp("OutputLevel")
        
        if trackType != None :
            mon.addTool( self.richTools().trackSelector(nickname="TrackSelector",private=True),
                         name="TrackSelector" )
            if trackType != ["All"] : mon.TrackSelector.TrackAlgs = trackType
            mon.Context = self.getTrackContext( trackType )
        else:
            mon.Context = self.getBestContext()
            
        if tkCuts == "None" :
            pass
        elif tkCuts == "All" :
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
        elif tkCuts == "Tight" :
            bigvalue = 1e+30
            mon.TrackSelector.MinPCut    = 10
            mon.TrackSelector.MaxPCut    = bigvalue
            mon.TrackSelector.MinPtCut   = 0.5
            mon.TrackSelector.MaxPtCut   = bigvalue
            mon.TrackSelector.MinChi2Cut = 0
            mon.TrackSelector.MaxChi2Cut = 2
            mon.TrackSelector.MinLikelihood   = -40
            mon.TrackSelector.MaxLikelihood   =  bigvalue
            mon.TrackSelector.MinCloneDistCut = 5000
            mon.TrackSelector.MaxCloneDistCut =  bigvalue
            mon.TrackSelector.MinGhostProbCut = -bigvalue
            mon.TrackSelector.MaxGhostProbCut =  0.8
            mon.TrackSelector.AcceptClones    = False
        else:
            raise RuntimeError("ERROR : Unknown track selection "+tkCuts)
        
        return mon

    ## Returns a configured list of L0 filters
    def l0Filters(self):
        ## L0Report and HltDecReport missing in 2008 and 2009 data
        if ( self.getProp("DataType") in ["2008","2009"] ) : return []

        from Configurables import ( LoKi__HDRFilter,
                                    LoKi__L0Filter,
                                    GaudiSequencer )
        from DAQSys.Decoders import DecoderDB 
        from DAQSys.DecoderClass import decodersForBank
        l0dec = [ d.setup() for d in decodersForBank(DecoderDB,'L0DU') ]
        hltdec=DecoderDB["HltDecReportsDecoder/Hlt1DecReportsDecoder"].setup()

        return l0dec+ [
                 LoKi__L0Filter("RichL0Filter", Code='L0_DECISION_PHYSICS'),
                 hltdec, 
                 LoKi__HDRFilter("RichHlt1Filter",
                                 Code="HLT_PASS('Hlt1L0AnyDecision','Hlt1MBNoBiasDecision')",
                                 Location = hltdec.OutputHltDecReportsLocation
                                 )
               ]

    ## Check a new sequence and add to main sequence
    def newSeq(self,sequence,name):
        seq = GaudiSequencer(name)
        if self.isPropertySet("OutputLevel") : seq.OutputLevel = self.getProp("OutputLevel")
        sequence.Members += [seq]
        return seq

    ## Create the track selection name from a list of track alg types
    def trackSelName(self,types) :
        return self.recoConf().trackGroupName(types)

    ## Sanity checks
    def sanityChecks(self):

        histoset = self.getProp("Histograms")

        # Add additional monitors to the list for this histo set
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

    ## @brief Apply any tweaks to the default configuration that vary by DataType
    def dataTypeTweaks(self):

        # Get the DataType
        dataType = self.getProp("DataType")

        # For Run II
        if ( dataType == "2015" or
             dataType == "2016" or
             dataType == "2017" ) :
            
            # No Aerogel in Run II
            if not self.isPropertySet("Radiators") :
                self.setProp( "Radiators", ["Rich1Gas","Rich2Gas"] )

        if dataType == "Upgrade" :

            # No Aerogel in the upgrade
            if not self.isPropertySet("Radiators") :
                self.setProp( "Radiators", ["Rich1Gas","Rich2Gas"] )

    ## @brief Online mode 
    def setupOnlineMode(self) :

        # Open up the CK res plot range, for the Wide photon selection
        self.setProp("CKThetaResRange", [ 0.05, 0.008, 0.004 ] )

        # Save all histograms from image analysis
        from Configurables import Rich__HPDImage__Summary
        imageSummary = Rich__HPDImage__Summary("RichHPDImageSummary")
        imageSummary.Keep2DHistograms = True
        imageSummary.FinalHPDFit = False

    ## Get the context to use for a monitoring algorithm using a given track location
    def getTrackContext(self,tkType):
        cont = self.getProp("Context")
        if self.recoConf().usingTrackGroups() :
            cont = self.recoConf().getContextForTrackType( tkType )
        return cont

    ## Get the 'best' context to use for a monitor without a clear track type (so pixel based etc.)
    def getBestContext(self):
        return self.recoConf().getBestContext()

    ## Is the given track type active
    def trackTypeIsActive(self,type):
        return self.recoConf().trackTypeIsActive(type)

    ## Apply the configuration to the given sequence
    def applyConf(self):

        # Configure dependency on Reco conf
        self.recoConf()

        # DataType specific tweaks
        self.dataTypeTweaks()

        # Online mode ?
        if self.getProp("OnlineMode") : self.setupOnlineMode()

        ## Sanity checks
        self.sanityChecks()

        # Sequence to add mintors to
        sequence = self.getProp("MoniSequencer")
        sequence.IgnoreFilterPassed = True

        # Set context
        sequence.Context = self.getProp("Context")

        # Suppress errors from Linker
        msgSvc = getConfigurable("MessageSvc")
        msgSvc.setFatal += [ "LinkedTo::MC/Rich/Hits2MCRichOpticalPhotons",
                             "LinkedTo::MC/Particles2MCRichTracks" ]

        # Event filters
        filter = self.getProp("EventFilter")
        if filter != "" :
            from Configurables import LoKi__VoidFilter
            rFilter = LoKi__VoidFilter('RichMoniEventFilter',Code=filter)
            sequence.Members += [rFilter]

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
            nullMCtools = [ "Rich::MC::NULLMCTruthTool/RichMCTruthTool",
                            "Rich::Rec::MC::NULLMCTruthTool/RichRecMCTruthTool" ]
            self.toolRegistry().Tools += nullMCtools

        # The list of monitors to run
        monitors = self.getHistoOptions("Monitors")
  
        # Some monitoring of raw information
        rawSeq = self.newSeq(sequence,"RichRawMoni")
        rawSeq.IgnoreFilterPassed = True
            
        if "L1SizeMonitoring" in monitors :

            detailedPlots = self.getProp("Histograms") == "Expert"

            # Data Sizes - all events
            from Configurables import Rich__DAQ__RawDataSize
            dataSize = self.createMonitor(Rich__DAQ__RawDataSize,"RichRawDataSize")
            dataSize.FillDetailedHPDPlots        = detailedPlots
            dataSize.FillDetailedL1IngressPlots  = detailedPlots
            rawSeq.Members += [dataSize]

            # Data size plots, L0 unbiased events
            if not self.getProp("Simulation") :
                from Configurables import GaudiSequencer
                lSeq = self.newSeq(rawSeq,"RichRawDataSizeL0Seq")
                lSeq.ReturnOK = True
                dataSizeL0 = self.createMonitor(Rich__DAQ__RawDataSize,"RichRawDataSizeL0")
                dataSizeL0.FillDetailedHPDPlots        = detailedPlots
                dataSizeL0.FillDetailedL1IngressPlots  = detailedPlots
                lSeq.Members = self.l0Filters() + [ dataSizeL0 ]

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

        if "DataDecodingErrors" in monitors :

            # Decoding error summaries
            from Configurables import Rich__DAQ__DataDecodingErrorMoni
            daqErrs = self.createMonitor(Rich__DAQ__DataDecodingErrorMoni,"RichDecodingErrors")
            rawSeq.Members += [daqErrs]

        # ODIN
        if "ODIN" in monitors :
            self.odinMoni(self.newSeq(sequence,"RichODINMoni"))

        # RICH data monitoring
        if "PixelMonitoring" in monitors :
            self.pixelPerf(self.newSeq(sequence,"RichPixelMoni"))

        # Track monitoring
        if "TrackMonitoring" in monitors :
            self.trackMoni(self.newSeq(sequence,"RichTrackMoni"))
            
        # Reconstruction monitoring
        if "PhotonMonitoring" in monitors :
            self.recPerf(self.newSeq(sequence,"RichRecoMoni"),tkCuts="None")
            self.recPerf(self.newSeq(sequence,"RichRecoMoniTight"),tkCuts="Tight")
            self.recPerf(self.newSeq(sequence,"RichRecoMoniTightUnambig"),
                         tkCuts="Tight",photCuts="UnambPhots")

        # Aerogel specific monitoring
        if "AerogelMonitoring" in monitors :
            self.aeroMoni(self.newSeq(sequence,"RichAeroMoni"))    

        # PID Performance
        if "PidMonitoring" in monitors :
            self.pidPerf(self.newSeq(sequence,"RichPIDMoni"),False,"RiPIDMon")
            #self.pidPerf(self.newSeq(sequence,"RichPIDMoniDLL"),True,"RiPIDMonDLL")

        # Trackless rings angles
        if "TracklessRingAngles" in monitors :
            types = self.recoConf().TracklessRingAlgs
            for type in types :
                self.ringsMoni(type,self.newSeq(sequence,"Rich"+type+"TracklessRingsMoni"))

        # Trackless rings peak search
        if "TracklessRingPeakSearch" in monitors :
            types = self.recoConf().TracklessRingAlgs
            for type in types :
                self.ringsPeakSearch(type,self.newSeq(sequence,"Rich"+type+"TracklessRingsPeaks"))

        # Alignment monitor
        if "AlignmentMonitoring" in monitors :
            conf = self.alignmentConf()
            self.setOtherProps(conf,["Histograms","Context","NTupleProduce","WithMC"])
            conf.AlignmentSequencer = self.newSeq(sequence,"RichMirrAlignMoni")

        # HPD IFB
        if "HPDIFBMonitoring" in monitors :
            self.ionfeedbackMoni(self.newSeq(sequence,"RichHPDIonFeedback"))

        # HPD Image shifts
        if "HPDImageShifts" in monitors :
            self.hpdImageShifts(self.newSeq(sequence,"RichHPDImageShifts"))

        # Expert Monitoring
        self.expertMonitoring( sequence )

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

        context = self.getBestContext()

        histoSet = self.getProp("Histograms")
 
        # Add monitors
        for ringclass in self.getHistoOptions("TracklessRingClasses") :
            moni = self.createMonitor(Rich__Rec__MC__TracklessRingMoni,type+"RingMoni"+ringclass)
            moni.RingLocation = "Rec/Rich/"+type+"/"+context+"/Rings"+ringclass
            sequence.Members += [moni]

    ## standalone ring finder peak searches
    def ringsPeakSearch(self,type,sequence):

        from Configurables import ( Rich__Rec__RingPeakSearch )

        # Ring peak search
        for ringclass in self.getHistoOptions("TracklessRingClasses") :
            search = self.createMonitor(Rich__Rec__RingPeakSearch,type+"RingPeakSearch"+ringclass)
            search.RingLocation = "Rec/Rich/"+type+"/Rings"+ringclass
            sequence.Members += [search]

    ## ODIN monitors
    def odinMoni(self,sequence):
        from Configurables import Rich__DAQ__ODINMoni
        sequence.Members += [ self.createMonitor(Rich__DAQ__ODINMoni,"RichODIN") ]
        
    ## Pixel performance monitors
    def pixelPerf(self,sequence):

        from Configurables import ( Rich__Rec__MC__PixelQC,
                                    Rich__Rec__PixelClusterMoni )

        sequence.Members += [ self.createMonitor(Rich__Rec__MC__PixelQC,"RichRecPixelQC") ]
        sequence.Members += [ self.createMonitor(Rich__Rec__PixelClusterMoni,"RichRecPixelClusters") ]

    ## Run the PID Performance monitors
    def pidPerf(self,sequence,useDLLcut=False,basename="RiPIDMon"):

        from Configurables import ( Rich__Rec__MC__PIDQC )

        # Track Types
        for trackType in self.getTrackTypes("PidTrackTypes") :

            # Momentum ranges
            for pRange in self.getHistoOptions("PidMomentumRanges") :

                # Construct the name for this monitor out of the track types
                # and momentum range
                tkName = self.trackSelName(trackType)
                name = basename + tkName + `pRange[0]` + "To" + `pRange[1]`
                
                # Make a monitor alg
                pidMon = self.createMonitor(Rich__Rec__MC__PIDQC,name,trackType)
                
                # Trackselector momentum cuts
                pidMon.TrackSelector.MinPCut = pRange[0]
                pidMon.TrackSelector.MaxPCut = pRange[1]
                
                if useDLLcut : pidMon.KaonDLLCut = 0;
                
                # RichPID location
                pidMon.InputPIDs = self.getProp("RichPIDLocation")
                
                # Extra plots in Expert mode
                if "Expert" == self.getProp("Histograms"):
                    pidMon.ExpertPlots = True
                    
                # MC mode
                pidMon.MCTruth = self.getProp("WithMC")
                
                # Add to sequence
                sequence.Members += [pidMon]

    ## Track monitoring
    def trackMoni(self,sequence):

        from Configurables import Rich__Rec__MC__TrackSelEff

        # Track Types
        for trackType in self.getTrackTypes("EffTrackTypes") :

            # Construct the name for this monitor
            name = "Ri" + self.trackSelName(trackType) + "TrkEff"

            # Make a monitor alg
            mon = self.createMonitor(Rich__Rec__MC__TrackSelEff,name,trackType,tkCuts="All")
            #mon.HistoPrint = True

            # Add to sequence
            sequence.Members += [mon]

    ## Reconstruction performance
    def recPerf(self,sequence,tkCuts="None",photCuts="None"):

        from Configurables import Rich__Rec__MC__RecoQC
        
        # Track Types
        for trackType in self.getTrackTypes("RecoTrackTypes") :
            
            # Construct the name for this monitor
            tkShortName = self.trackSelName(trackType)
            name = "RiCKRes" + tkShortName
            if tkCuts   != "None" : name += tkCuts
            if photCuts != "None" : name += photCuts
            
            # Make a monitor alg
            mon = self.createMonitor(Rich__Rec__MC__RecoQC,name,trackType,tkCuts)
            mon.HistoPrint = False
            
            # cuts
            mon.MinBeta = self.getProp("MinTrackBeta")
            if photCuts == "UnambPhots" : mon.RejectAmbiguousPhotons = True

            # Radiators
            mon.Radiators = self.usedRadiators()

            # Make aerogel tile plots only for Long monitor
            if tkShortName == "Long" : mon.EnableAerogelTilePlots = True

            # Expert Mode histos
            if self.getProp("Histograms") == "Expert" :
                mon.EnablePerPDPlots    = True
                mon.EnablePerPDColPlots = True

            # Enable plots only for Long monitors
            if tkShortName == "Long" :
                mon.EnablePerPDColPlots       = True
                mon.EnablePerPDFittedResPlots = True
                # If online, enable periodic histogram fitting
                # Also have to enable the saving of individual HPD plots, so they get reset
                if self.getProp("Histograms") == "Online" :
                    mon.HistoFitFreq     = 100
                    mon.EnablePerPDPlots = True

            # Histogram ranges
            ckRange = self.getProp("CKThetaResRange")
            if len(ckRange) > 0 :
                if len(ckRange) != 3 :
                    raise RuntimeError("ERROR : CK Theta resolution must have 3 entries")
                mon.CKResHistoRange = ckRange
        
            # Add to sequence
            sequence.Members += [mon]

    ## Specially tuned aerogel monitor
    def aeroMoni(self,sequence):
        
        from Configurables import Rich__Rec__MC__RecoQC
                
        trackType = ['Forward','Match']
        name = "RiAeroMoni" + self.trackSelName(trackType)

        aeroMoni = self.createMonitor(Rich__Rec__MC__RecoQC,name,trackType)
        aeroMoni.HistoPrint = False

        # Set options
        aeroMoni.Radiators  = [ True, False, False ] # Only run on aerogel segments
        aeroMoni.EnableAerogelTilePlots = True
        #aeroMoni.MaxRadSegs = [ 5, 0, 0, ]
        aeroMoni.TrackSelector.MinPCut    = 10
        aeroMoni.TrackSelector.MaxChi2Cut = 3
        aeroMoni.MinBeta = self.getProp("MinTrackBeta")

        # Add to sequence
        sequence.Members += [aeroMoni]
        # ----------------------------------------------------------------
            
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
        sequence.Members += [RichIFBMon]

    ## HPD image shift monitoring
    def hpdImageShifts(self,sequence):
        from Configurables import Rich__HPDImage__Summary
        imageSummary = Rich__HPDImage__Summary("RichHPDImageSummary")
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
        tkTypes = self.getTrackTypes("RecoTrackTypes")

        check = "RichFuncCKResPlots"
        if check in checks :
            if "HistoProduce" in self.richTools().ckResolution().properties() :
                self.richTools().ckResolution().HistoProduce = self.getProp("Histograms") != "None"

        check = "RichPixelPositions"
        if check in checks :
            from Configurables import Rich__Rec__MC__PixelPositionMonitor
            seq = self.newSeq(sequence,check)
            seq.Members += [Rich__Rec__MC__PixelPositionMonitor("RiRecPixelPosMoni")]

        check = "HPDHitPlots"
        if check in checks :
            from Configurables import ( Rich__Rec__HPDHitsMoni, GaudiSequencer )
            seq = self.newSeq(sequence,check)
            seq.IgnoreFilterPassed = True
            seq.Members += [ Rich__Rec__HPDHitsMoni("HPDHitsMoni") ]
            if not self.getProp("Simulation") :
                lSeq = self.newSeq(seq,check+"L0")
                lSeq.ReturnOK = True
                lSeq.Members = self.l0Filters() + [ Rich__Rec__HPDHitsMoni("HPDL0HitsMoni") ]

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

            recotools = [ "Rich::Rec::PhotonCreator/ForcedRichPhotonCreator",
                          "Rich::Rec::PhotonRecoUsingQuarticSoln/ForcedPhotonReco",
                          "Rich::Rec::SimplePhotonPredictor/ForcedRichPhotonPredictor" ]
            self.toolRegistry().Tools += recotools
            self.recoConf().toolRegistry().Tools += recotools

            seq = self.newSeq(sequence,check)

            for trackType in tkTypes :
                
                context = self.getTrackContext(trackType)

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
                name = "RiRecPhotEff" + self.trackSelName(trackType) + "TkMoni"
                mon = self.createMonitor(Rich__Rec__MC__PhotonRecoEffMonitor,name,trackType)
                seq.Members += [ mon ]

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

            # Enable ChronoAuditor
            chronoAuditor = "ChronoAuditor"
            if chronoAuditor not in AuditorSvc().Auditors :
                AuditorSvc().Auditors += [ chronoAuditor ]
            ChronoAuditor().Enable = True

            seq = self.newSeq(sequence,check)

            cont = self.getProp("Context")

            # RICH reconstruction only (tracks,pixels,photons)
            moni = self.createMonitor(Rich__Rec__TimeMonitor,"RichRecoTime")
            moni.TimingName = "RichRecInit"+cont+"Seq"
            moni.Algorithms = [ moni.TimingName ]
            seq.Members += [moni]

            # RICH PID only
            moni = self.createMonitor(Rich__Rec__TimeMonitor,"RichPIDTime")
            moni.TimingName = "Rich"+cont+"PIDSeq"
            moni.Algorithms = [ moni.TimingName ]
            seq.Members += [moni]

            # overall RICH reconstruction
            moni = self.createMonitor(Rich__Rec__TimeMonitor,"OverallRichTime")
            moni.TimingName = "RecoRICHSeq"
            moni.Algorithms = [ moni.TimingName ]
            seq.Members += [moni]

            # overall track reconstruction
            moni = self.createMonitor(Rich__Rec__TimeMonitor,"OverallTrackTime")
            moni.TimingName = "RecoTrSeq"
            moni.Algorithms = [ moni.TimingName ]
            seq.Members += [moni]

        check = "RichTracksToTextFile"
        if check in checks:

            from Configurables import Rich__Rec__MC__DumpRichTracksToTextFileAlg as RichTracksToText
            moni = self.createMonitor(RichTracksToText,"RichTracksToText")
            sequence.Members += [moni]

        check = "RichHitsToTextFile"
        if check in checks:

            from Configurables import Rich__Rec__MC__DumpRichHitsToTextFileAlg as RichHitsToText
            moni = self.createMonitor(RichHitsToText,"RichHitsToText")
            sequence.Members += [moni]
            
        check = "RichPerfFromData"
        if check in checks:
            pass
        
            # Warning : This option requires access to some items from the Analysis software stack
            #         : Will only work in a Brunel + DaVinci software environment
            #         : Use 'SetupProject Brunel vXrY --runtime DaVinci vNrM --use DaVinciSys'
            
            #from Configurables import ( GaudiSequencer, ChargedProtoParticleAddRichInfo,
            #                            ChargedProtoCombineDLLsAlg, RichPIDQCConf )
 
            # General sequence
            #seq = self.newSeq(sequence,check)

            # make sure ProtoParticles are all up to date with the RICH info
            #seq.Members += [ ChargedProtoParticleAddRichInfo("ChargedProtoPAddRich"),
            #                 ChargedProtoCombineDLLsAlg("ChargedProtoPCombDLL") ]

            # Set up PID monitoring sequence
            #pidSeq = GaudiSequencer("RichPIDSelections")
            #seq.Members += [pidSeq]

            # Get the PID QC configurable
            #pidqcConf = self.getRichCU(RichPIDQCConf)

            # Set options
            #self.setOtherProps(pidqcConf,['OutputLevel','Context'])
            #pidqcConf.setProp("CalibSequencer",pidSeq)
            
