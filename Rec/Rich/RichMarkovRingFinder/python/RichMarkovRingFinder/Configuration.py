
## @package RichMarkovRingFinder
#  High level Configuration tools for RICH Markov ring finding
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.4 2009-01-07 13:31:19 ukerzel Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from Configurables import ( Rich__Rec__TracklessRingFilterAlg,
                            Rich__Rec__TracklessRingIsolationAlg )

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
       ,"AssociateToSegments" : True
       ,"Sequencer"           : GaudiSequencer("RichMarkovDefaultSequencer")     # The sequencer to add the RICH MCMC algorithms to
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

        sequence = self.getProp("Sequencer")
        if sequence.name() == "RichMarkovDefaultSequencer" :
            raise RuntimeError("ERROR : Rich Markov Ring Finder Sequence not set")
        else :
            panels = self.getProp("Panels")
            if len(panels) == 0 :
                raise RuntimeError("ERROR : No panels set to find Markov Rings")
            else:
                # maximum hits in each panel for finding
                maxhits = self.getProp("MaxHitsInPanel")

                # context
                cont = self.getProp("Context")

                # The finders, for each HPD panel
                if "Rich1Top" in panels :
                    alg                = self.rich1TopFinder()
                    alg.MaxHitsInEvent = maxhits
                    sequence.Members  += [alg]
                    if "Rich1Bottom" in panels :
                        alg                = self.rich1BottomFinder()
                        alg.MaxHitsInEvent = maxhits
                        sequence.Members  += [alg]
                    if "Rich2Left" in panels :
                        alg                = self.rich2LeftFinder()
                        alg.MaxHitsInEvent = maxhits
                        sequence.Members  += [alg]
                    if "Rich2Right" in panels :
                        alg                = self.rich2RightFinder()
                        alg.MaxHitsInEvent = maxhits
                        sequence.Members  += [alg]

                # Attempt to associated rings to segments
                if self.getProp("AssociateToSegments"):
                    from Configurables import Rich__Rec__TracklessRingSegmentAssociationAlg
                    alg               = Rich__Rec__TracklessRingSegmentAssociationAlg(cont+"MRingsSegAssoc")
                    sequence.Members += [alg]

                # Post finding cleaning and ring selection
                filter            = Rich__Rec__TracklessRingFilterAlg(cont+"BestMRings")
                iso               = Rich__Rec__TracklessRingIsolationAlg(cont+"IsolatedMRings")
                iso.InputRings    = "Rec/Rich/Markov/RingsBest"
                sequence.Members += [filter,iso]
            
