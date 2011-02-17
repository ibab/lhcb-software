

__version__ = "$Id: Configuration.py,v 1.15 2010-02-04 17:14:02 papanest Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
# ----------------------------------------------------------------------------------

## @class RichparticleSearchConf
#  High level Configuration tools for RICH Data Quality monitoring
#  @author MAtt Coombes
#  @date   22/09/2010
class RichParticleSearchConf(RichConfigurableUser):



    ## Steering options
    __slots__ = {
        "Context"           : "Offline"  # The context within which to run
        ,"Sequencer"        :  None
        ,"OutputLevel"      : INFO 
        ,"Radiators"        : [ "Aerogel", "Rich1Gas", "Rich2Gas" ] # The RICH detectors to monitor
        ,"NTupleProduce"    : False
        ,"R1NTupleProduce"  : False
        ,"R2NTupleProduce"  : False
        ,"Histograms"       : "OfflineFull"
        ,"WithMC"           : False     # set to True to use MC truth
        ,"MinTrackMomentum" : [ 0.0, 0.0, 0.0 ] # momentum cut in GeV, use -1 for default
       # ,"HPDList"          : [ [], [], [] ] # list of HPDs for histograms
       # ,"MinIsolationCuts"  : [20,20,20] #Track isolation Cut
        }


    ## Apply the configuration
    def __apply_configuration__(self):

        if not self.isPropertySet("Sequencer") :
            raise RuntimeError("ERROR : Sequencer is not set")
         #   print "ERROR : Sequencer is not set"
            
        sequence = self.getProp("Sequencer")

        # context
        cont = self.getProp("Context")
        histoLevel = { "Online":0, "OfflineExpress":3, "OfflineFull":2, "Expert":5 }
        setOutLevel = self.isPropertySet("OutputLevel")
        level = self.getProp("OutputLevel")

        from Configurables import (Rich__Rec__MC__RichParticleSearchMain)
   
        # Mirror Alignment monitor for Aerogel
        #-------------------------------------------------------------------------------
        if "Aerogel" in self.getProp("Radiators") :
            RichAlignMoniAerogel = Rich__Rec__MC__RichParticleSearchMain("RichParticleSearchAerogel")
            sequence.Members += [RichAlignMoniAerogel]

            RichAlignMoniAerogel.UseMCTruth      = self.getProp("WithMC")
            RichAlignMoniAerogel.Radiator        = 0   # Aerogel = 0, Rich1Gas = 1, Rich2Gas = 2
        

            # Track selector
            trselname = "TrackSelector"
            RichAlignMoniAerogel.addTool( self.richTools().trackSelector(trselname), name=trselname )

            RichAlignMoniAerogel.TrackSelector.TrackAlgs = [ "Match","Forward" ]
            aeroMinPCut = self.getProp("MinTrackMomentum")[0]
            RichAlignMoniAerogel.TrackSelector.MinPCut   = aeroMinPCut

            RichAlignMoniAerogel.NTupleProduce = self.getProp("NTupleProduce") and self.getProp("R1NTupleProduce")
            RichAlignMoniAerogel.HistoProduce  = self.getProp("Histograms") != "None"

            RichAlignMoniAerogel.HistoOutputLevel = histoLevel[self.getProp("Histograms")]

            # This list is of "popular" mirrors. A longer list is required for full alignment
            
          

  
        # Mirror Alignment monitor for Rich1
        #-------------------------------------------------------------------------------
        if "Rich1Gas" in self.getProp("Radiators") :

            RichAlignMoniR1 = Rich__Rec__MC__RichParticleSearchMain("RichParticleSearchR1Gas")
            sequence.Members += [RichAlignMoniR1]

            RichAlignMoniR1.UseMCTruth      = self.getProp("WithMC")
            RichAlignMoniR1.Radiator        = 1   # Aerogel = 0, Rich1Gas = 1, Rich2Gas = 2

            # Track selector
            trselname = "TrackSelector"
            RichAlignMoniR1.addTool( self.richTools().trackSelector(trselname), name=trselname )

            RichAlignMoniR1.TrackSelector.TrackAlgs = [ "Match","Forward" ]
            r1MinPCut = self.getProp("MinTrackMomentum")[1]
            RichAlignMoniR1.TrackSelector.MinPCut   = r1MinPCut

            RichAlignMoniR1.NTupleProduce = self.getProp("NTupleProduce") and self.getProp("R1NTupleProduce")
            RichAlignMoniR1.HistoProduce  = self.getProp("Histograms") != "None"

            RichAlignMoniR1.HistoOutputLevel = histoLevel[self.getProp("Histograms")]

            # This list is of "popular" mirrors. A longer list is required for full alignment
           
        #    RichAlignMoniR1.HPDList = self.getProp("HPDList")[1]
       #     RichAlignMoniR1.IsolationCut = self.getProp("IsolationCut")[1]

        # Mirror Alignment monitor for Rich2
        #-------------------------------------------------------------------------------
        if "Rich2Gas" in self.getProp("Radiators") :

            RichAlignMoniR2 = Rich__Rec__MC__RichParticleSearchMain("RichParticleSearchR2Gas")
            sequence.Members += [RichAlignMoniR2]

            RichAlignMoniR2.UseMCTruth      = self.getProp("WithMC")
            RichAlignMoniR2.Radiator        = 2   # Aerogel = 0, Rich1Gas = 1, Rich2Gas = 2

            # Track selector
            trselname = "TrackSelector"
            RichAlignMoniR2.addTool( self.richTools().trackSelector(trselname), name=trselname )

            RichAlignMoniR2.TrackSelector.TrackAlgs = [ "Match","Forward" ]
            r2MinPCut = self.getProp("MinTrackMomentum")[2]
            RichAlignMoniR2.TrackSelector.MinPCut   = r2MinPCut

            RichAlignMoniR2.NTupleProduce = self.getProp("NTupleProduce") and self.getProp("R2NTupleProduce")
            RichAlignMoniR2.HistoProduce  = self.getProp("Histograms") != "None"

            RichAlignMoniR2.HistoOutputLevel = histoLevel[self.getProp("Histograms")]

      #      RichAlignMoniR2.HPDList = self.getProp("HPDList")[2]

   #         RichAlignMoniR2.IsolationCut = self.getProp("IsolationCut")[2]

