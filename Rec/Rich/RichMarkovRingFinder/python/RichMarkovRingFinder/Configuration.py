
## @package RichMarkovRingFinder
#  High level Configuration tools for RICH Markov ring finding
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.7 2009-05-22 15:35:45 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *

# ----------------------------------------------------------------------------------

## @class RichMarkovRingFinderConf
#  High level Configuration tools for RICH Markov ring finding
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichMarkovRingFinderConf(RichConfigurableUser):

        ## Steering options
    __slots__ = {
        "Context"             :    "Offline"   # The context within which to run
       ,"Panels"              : ["Rich1Top","Rich1Bottom","Rich2Left","Rich2Right"]
       ,"MaxHitsInPanel"      : 300
       ,"MaxHitsInHPD"        : 30
       ,"AssociateToSegments" : True
       ,"Sequencer"           : None # The sequencer to add the RICH MCMC algorithms to
       ,"OutputLevel"         : INFO
        }

    ## Access the finder for RICH1 top panel
    def rich1TopFinder(self):
        from Configurables import Rich__Rec__MarkovRingFinder__Rich1TopPanel
        cont = self.getProp("Context")
        return Rich__Rec__MarkovRingFinder__Rich1TopPanel(cont+"MFinderR1Top")

    ## Access the finder for RICH1 bottom panel
    def rich1BottomFinder(self):
        from Configurables import Rich__Rec__MarkovRingFinder__Rich1BottomPanel
        cont = self.getProp("Context")
        return Rich__Rec__MarkovRingFinder__Rich1BottomPanel(cont+"MFinderR1Bottom")

    ## Access the finder for RICH2 left panel
    def rich2LeftFinder(self):
        from Configurables import Rich__Rec__MarkovRingFinder__Rich2LeftPanel
        cont = self.getProp("Context")
        return Rich__Rec__MarkovRingFinder__Rich2LeftPanel(cont+"MFinderR2Left")

    ## Access the finder for RICH2 right panel
    def rich2RightFinder(self):
        from Configurables import Rich__Rec__MarkovRingFinder__Rich2RightPanel
        cont = self.getProp("Context")
        return Rich__Rec__MarkovRingFinder__Rich2RightPanel(cont+"MFinderR2Right")

    ## @brief Apply the configuration to the given GaudiSequencer
    def applyConf(self):

        from Configurables import ( Rich__Rec__TracklessRingFilterAlg,
                                    Rich__Rec__TracklessRingIsolationAlg )

        if not self.isPropertySet("Sequencer") :
            raise RuntimeError("ERROR : Rich Markov Ring Finder Sequence not set")
        else :
            sequence = self.getProp("Sequencer")
            panels   = self.getProp("Panels")
            if len(panels) == 0 :
                raise RuntimeError("ERROR : No panels set to find Markov Rings")
            else:
                # maximum hits in each panel for finding
                maxhits = self.getProp("MaxHitsInPanel")
                maxhpdhits = self.getProp("MaxHitsInHPD")
      
                # context
                cont = self.getProp("Context")

                setOutLevel = self.isPropertySet("OutputLevel")
                level = self.getProp("OutputLevel")

                # The finders, for each HPD panel
                if "Rich1Top" in panels :
                    alg                = self.rich1TopFinder()
                    alg.MaxHitsInEvent = maxhits
                    alg.MaxHitsInHPD   = maxhpdhits
                    alg.RingLocation   = "Rec/Rich/Markov/RingsAll"
                    if setOutLevel : alg.OutputLevel = level
                    sequence.Members  += [alg]
                if "Rich1Bottom" in panels :
                    alg                = self.rich1BottomFinder()
                    alg.MaxHitsInEvent = maxhits
                    alg.MaxHitsInHPD   = maxhpdhits
                    alg.RingLocation   = "Rec/Rich/Markov/RingsAll"
                    if setOutLevel : alg.OutputLevel = level
                    sequence.Members  += [alg]
                if "Rich2Left" in panels :
                    alg                = self.rich2LeftFinder()
                    alg.MaxHitsInEvent = maxhits
                    alg.MaxHitsInHPD   = maxhpdhits
                    alg.RingLocation   = "Rec/Rich/Markov/RingsAll"
                    if setOutLevel : alg.OutputLevel = level
                    sequence.Members  += [alg]
                if "Rich2Right" in panels :
                    alg                = self.rich2RightFinder()
                    alg.MaxHitsInEvent = maxhits
                    alg.MaxHitsInHPD   = maxhpdhits
                    alg.RingLocation   = "Rec/Rich/Markov/RingsAll"
                    if setOutLevel : alg.OutputLevel = level
                    sequence.Members  += [alg]

                # Attempt to associated rings to segments
                if self.getProp("AssociateToSegments"):
                    from Configurables import Rich__Rec__TracklessRingSegmentAssociationAlg
                    alg               = Rich__Rec__TracklessRingSegmentAssociationAlg(cont+"MRingsSegAssoc")
                    alg.InputRings    = "Rec/Rich/Markov/RingsAll"
                    if setOutLevel : alg.OutputLevel = level
                    sequence.Members += [alg]

                # Post finding cleaning and ring selection
                filter             = Rich__Rec__TracklessRingFilterAlg(cont+"BestMRings")
                filter.InputRings  = "Rec/Rich/Markov/RingsAll"
                filter.OutputRings = "Rec/Rich/Markov/RingsBest"
                if setOutLevel : filter.OutputLevel = level
                iso                = Rich__Rec__TracklessRingIsolationAlg(cont+"IsolatedMRings")
                iso.InputRings     = "Rec/Rich/Markov/RingsBest"
                iso.OutputRings    = "Rec/Rich/Markov/RingsIsolated"
                if setOutLevel : iso.OutputLevel = level
                sequence.Members  += [filter,iso]
            
