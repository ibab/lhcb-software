

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
        ,"MinIsolationCuts" : [20.0,20.0,20.0] #Track isolation Cut
        ,"MaxCK_Sig"        : [1.0,1.0,1.0]
        ,"CKDevCut"         : [0.5, 0.5, 0.5]
        ,"UseMuonInfo"      : False
        ,"PlotPhotons"      : False
        ,"FindYields"       : False
        ,"MinTrackMass"     : [0,0,0] # lower limit of track mass in Mev
        ,"MaxPhotons"       : [10,50,50]
        ,"MinPhotons"       : [0, 2, 2]
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
            RichPartSearchAerogel = Rich__Rec__MC__RichParticleSearchMain("RichParticleSearchAerogel")
            sequence.Members += [RichPartSearchAerogel]

            RichPartSearchAerogel.UseMCTruth      = self.getProp("WithMC")
            RichPartSearchAerogel.Radiator        = 0   # Aerogel = 0, Rich1Gas = 1, Rich2Gas = 2
        

            # Track selector
            trselname = "TrackSelector"
            RichPartSearchAerogel.addTool( self.richTools().trackSelector(trselname), name=trselname )

            RichPartSearchAerogel.TrackSelector.TrackAlgs = [ "Match","Forward" ]
            aeroMinPCut = self.getProp("MinTrackMomentum")[0]
            RichPartSearchAerogel.TrackSelector.MinPCut   = aeroMinPCut

            RichPartSearchAerogel.NTupleProduce = self.getProp("NTupleProduce") and self.getProp("R1NTupleProduce")
            RichPartSearchAerogel.HistoProduce  = self.getProp("Histograms") != "None"

            RichPartSearchAerogel.HistoOutputLevel = histoLevel[self.getProp("Histograms")]
            
            RichPartSearchAerogel.MinIsolationCut = self.getProp("MinIsolationCuts")[0]
            RichPartSearchAerogel.MaxCK_Sig = self.getProp("MaxCK_Sig")[0] 
            RichPartSearchAerogel.CKDevCut = self.getProp("CKDevCut")[0] 
            RichPartSearchAerogel.UseMuonInfo = self.getProp("UseMuonInfo")
            RichPartSearchAerogel.PlotPhotons = self.getProp("PlotPhotons")
            RichPartSearchAerogel.MinTrackMass = self.getProp("MinTrackMass")[0]
            RichPartSearchAerogel.findYields = self.getProp("FindYields")
            RichPartSearchAerogel.MaxPhotons = self.getProp("MaxPhotons")[0]
            RichPartSearchAerogel.MinPhotons = self.getProp("MinPhotons")[0]


            # This list is of "popular" mirrors. A longer list is required for full alignment
            
          

  
        # Mirror Alignment monitor for Rich1
        #-------------------------------------------------------------------------------
        if "Rich1Gas" in self.getProp("Radiators") :

            RichPartSearchR1 = Rich__Rec__MC__RichParticleSearchMain("RichParticleSearchR1Gas")
            sequence.Members += [RichPartSearchR1]

            RichPartSearchR1.UseMCTruth      = self.getProp("WithMC")
            RichPartSearchR1.Radiator        = 1   # Aerogel = 0, Rich1Gas = 1, Rich2Gas = 2

            # Track selector
            trselname = "TrackSelector"
            RichPartSearchR1.addTool( self.richTools().trackSelector(trselname), name=trselname )

            RichPartSearchR1.TrackSelector.TrackAlgs = [ "Match","Forward" ]
            r1MinPCut = self.getProp("MinTrackMomentum")[1]
            RichPartSearchR1.TrackSelector.MinPCut   = r1MinPCut

            RichPartSearchR1.NTupleProduce = self.getProp("NTupleProduce") and self.getProp("R1NTupleProduce")
            RichPartSearchR1.HistoProduce  = self.getProp("Histograms") != "None"

            RichPartSearchR1.HistoOutputLevel = histoLevel[self.getProp("Histograms")]
            
            RichPartSearchR1.MinIsolationCut = self.getProp("MinIsolationCuts")[1]
            RichPartSearchR1.MaxCK_Sig = self.getProp("MaxCK_Sig")[1] 
            RichPartSearchR1.CKDevCut = self.getProp("CKDevCut")[1] 
            
            RichPartSearchR1.UseMuonInfo = self.getProp("UseMuonInfo")
            RichPartSearchR1.PlotPhotons = self.getProp("PlotPhotons")
            RichPartSearchR1.MinTrackMass = self.getProp("MinTrackMass")[1]
            RichPartSearchR1.findYields = self.getProp("FindYields")

            RichPartSearchR1.MaxPhotons = self.getProp("MaxPhotons")[1]
            RichPartSearchR1.MinPhotons = self.getProp("MinPhotons")[1]

            # This list is of "popular" mirrors. A longer list is required for full alignment
           
        #    RichAlignMoniR1.HPDList = self.getProp("HPDList")[1]
       #     RichAlignMoniR1.IsolationCut = self.getProp("IsolationCut")[1]

        # Mirror Alignment monitor for Rich2
        #-------------------------------------------------------------------------------
        if "Rich2Gas" in self.getProp("Radiators") :

            RichPartSearchR2 = Rich__Rec__MC__RichParticleSearchMain("RichParticleSearchR2Gas")
            sequence.Members += [RichPartSearchR2]

            RichPartSearchR2.UseMCTruth      = self.getProp("WithMC")
            RichPartSearchR2.Radiator        = 2   # Aerogel = 0, Rich1Gas = 1, Rich2Gas = 2

            # Track selector
            trselname = "TrackSelector"
            RichPartSearchR2.addTool( self.richTools().trackSelector(trselname), name=trselname )

            RichPartSearchR2.TrackSelector.TrackAlgs = [ "Match","Forward" ]
            r2MinPCut = self.getProp("MinTrackMomentum")[2]
            RichPartSearchR2.TrackSelector.MinPCut   = r2MinPCut

            RichPartSearchR2.NTupleProduce = self.getProp("NTupleProduce") and self.getProp("R2NTupleProduce")
            RichPartSearchR2.HistoProduce  = self.getProp("Histograms") != "None"

            RichPartSearchR2.HistoOutputLevel = histoLevel[self.getProp("Histograms")]
            
            RichPartSearchR2.MinIsolationCut = self.getProp("MinIsolationCuts")[2]
            RichPartSearchR2.MaxCK_Sig = self.getProp("MaxCK_Sig")[2] 
            RichPartSearchR2.CKDevCut = self.getProp("CKDevCut")[2] 

            RichPartSearchR2.UseMuonInfo = self.getProp("UseMuonInfo")
            RichPartSearchR2.PlotPhotons = self.getProp("PlotPhotons")
            RichPartSearchR2.MinTrackMass = self.getProp("MinTrackMass")[2]
            RichPartSearchR2.findYields = self.getProp("FindYields")


            RichPartSearchR2.MaxPhotons = self.getProp("MaxPhotons")[2]
            RichPartSearchR2.MinPhotons = self.getProp("MinPhotons")[2]

      #      RichAlignMoniR2.HPDList = self.getProp("HPDList")[2]

   #         RichAlignMoniR2.IsolationCut = self.getProp("IsolationCut")[2]

