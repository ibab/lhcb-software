
## @package RichRecQC
#  High level Configuration tools for RICH Data Quality monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.7 2008-11-19 17:50:10 cattanem Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from Configurables import ( GaudiSequencer, MessageSvc )
from DDDB.Configuration import DDDBConf
    
# ----------------------------------------------------------------------------------

## @class RichRecQCConf
#  High level Configuration tools for RICH Data Quality monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichRecQCConf(RichConfigurableUser):

    ## Steering options
    __slots__ = {
        "Context": "Offline"  # The context within which to run
       ,"DataType"   : "2008" # Data type, can be ['DC06','2008']
       ,"RawMonitoring": True
       ,"PidMonitoring": True
       ,"PixelMonitoring": True
       ,"PhotonMonitoring": True
       ,"TracklessRingMonitoring": False
       ,"MirrorAlignmentMonitoring": False
       ,"PidMomentumRanges": [ [2,100], [2,10], [10,70], [70,100] ]
       ,"PidTrackTypes":  [ ["All"] ]
       ,"RecoTrackTypes": [ ["All"],
                            ["Forward","Match"],
                            ["Forward"],["Match"],["KsTrack"],["VeloTT"],["Seed"] ]
       ,"MoniSequencer" : None     # The sequencer to add the RICH monitoringalgorithms to
       ,"ExpertHistos": False # set to True to write out expert histos
        }

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

        # Some monitoring of raw information
        if self.getProp("RawMonitoring") :
            pixSeq = GaudiSequencer("RichRawMoni")
            pixSeq.MeasureTime = True
            sequence.Members += [pixSeq]
            if self.getProp("DataType") not in ["DC06"]:
                from Configurables import Rich__DAQ__DataDBCheck
                dbCheck = Rich__DAQ__DataDBCheck("RichRawDataDBCheck")
                pixSeq.Members += [dbCheck]

        # RICH data monitoring
        if self.getProp("PixelMonitoring") :
            pixSeq = GaudiSequencer("RichPixelMoni")
            pixSeq.MeasureTime = True
            sequence.Members += [pixSeq]
            self.pixelPerf(pixSeq)
         
        # Reconstruction monitoring
        if self.getProp("PhotonMonitoring") :
            recSeq = GaudiSequencer("RichRecoMoni")
            recSeq.MeasureTime = True
            sequence.Members += [recSeq]
            self.recPerf(recSeq)

        # PID Performance
        if self.getProp("PidMonitoring") :
            pidPerf = GaudiSequencer("RichPIDMoni")
            pidPerf.MeasureTime = True
            sequence.Members += [pidPerf]
            self.pidPerf(pidPerf)

        # Trackless rings
        if self.getProp("TracklessRingMonitoring") :
            ringsMoni = GaudiSequencer("RichTracklessRingsMoni")
            ringsMoni.MeasureTime = True
            sequence.Members += [ringsMoni]
            self.ringsMoni(ringsMoni)

        # Alignment monitor
        if self.getProp("MirrorAlignmentMonitoring"):
            alignSeq = GaudiSequencer("RichMirrAlignMoni")
            alignSeq.MeasureTime = True
            sequence.Members += [alignSeq]
            from RichRecQC.Alignment import RichAlignmentConf
            self.setOtherProp(RichAlignmentConf(),"context")
            RichAlignmentConf().alignmentSequncer = alignSeq

    ## standalone ring finder monitors
    def ringsMoni(self,sequence):
 
        from RichMarkovRingFinder.Configuration import RichMarkovRingFinderConf
        from Configurables import Rich__Rec__MarkovRingFinder__MC__Moni
        
        conf = RichMarkovRingFinderConf()

        # Activate histos in the finder algs themselves
        conf.rich1TopFinder().HistoProduce    = True
        conf.rich1BottomFinder().HistoProduce = True
        conf.rich2LeftFinder().HistoProduce   = True
        conf.rich2RightFinder().HistoProduce  = True

        # Add monitors
        allMoni  = Rich__Rec__MarkovRingFinder__MC__Moni("MarkovRingMoniAll")
        sequence.Members += [allMoni]
        bestMoni = Rich__Rec__MarkovRingFinder__MC__Moni("MarkovRingMoniBest")
        bestMoni.RingLocation = "Rec/Rich/Markov/RingsBest"
        sequence.Members += [bestMoni]
        isoMoni = Rich__Rec__MarkovRingFinder__MC__Moni("MarkovRingMoniIsolated")
        isoMoni.RingLocation = "Rec/Rich/Markov/RingsIsolated"
        sequence.Members += [isoMoni]
        
    ## Pixel performance monitors
    def pixelPerf(self,sequence):
        
        from Configurables import ( Rich__Rec__MC__PixelQC )

        sequence.Members += [ Rich__Rec__MC__PixelQC("RichRecPixelQC") ]
        
    ## Run the PID Performance monitors
    def pidPerf(self,sequence):

        from Configurables import ( Rich__Rec__MC__PIDQC )

        # Track Types
        for trackType in self.getProp("PidTrackTypes") :

            # Momentum ranges
            for pRange in self.getProp("PidMomentumRanges") :

                # Construct the name for this monitor out of the track types
                # and momentum range
                tkName = ""
                for tkT in trackType : tkName += tkT
                name = "RiPIDMon" + tkName + `pRange[0]` + "To" + `pRange[1]`

                # Make a monitor alg
                pidMon = Rich__Rec__MC__PIDQC(name)

                # Trackselector
                tkSel = RichTools().trackSelector(private=True)
                tkSel.MinPCut = pRange[0]
                tkSel.MaxPCut = pRange[1]
                if trackType != ["All"] : tkSel.TrackAlgs = trackType
                pidMon.addTool( tkSel )

                # Add to sequence
                sequence.Members += [pidMon]

    ## Reconstruction performance
    def recPerf(self,sequence):

        from Configurables import ( Rich__Rec__MC__RecoQC )
                
        # Track Types
        for trackType in self.getProp("RecoTrackTypes") : 

            # Construct the name for this monitor
            name = "RiCKRes"
            for tkT in trackType : name += tkT

            # Make a monitor alg
            mon = Rich__Rec__MC__RecoQC(name)
            mon.HistoPrint = False
          
            # Trackselector
            tkSel = RichTools().trackSelector(private=True)
            if trackType != ["All"] : tkSel.TrackAlgs = trackType
            if trackType == ["All"] : mon.MinBeta = [ 0.0, 0.0, 0.0 ]
            mon.addTool( tkSel )

            # Add to sequence
            sequence.Members += [mon]

