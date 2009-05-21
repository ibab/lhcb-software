
## @package RichENNRingFinder
#  High level Configuration tools for RICH ENN ring finding
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.1.1.1 2009-05-21 17:34:14 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *

# ----------------------------------------------------------------------------------

## @class RichENNRingFinderConf
#  High level Configuration tools for RICH ENN ring finding
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichENNRingFinderConf(RichConfigurableUser):

        ## Steering options
    __slots__ = {
        "Context"             :    "Offline"   # The context within which to run
       ,"Panels"              : ["Rich1Top","Rich1Bottom","Rich2Left","Rich2Right"]
       ,"MaxHitsInPanel"      : 300
       ,"AssociateToSegments" : True
       ,"Sequencer"           : None # The sequencer to add the RICH MCMC algorithms to
        }

    ## Access the finder for RICH1 top panel
    def rich1TopFinder(self):
        from Configurables import Rich__Rec__ENNRingFinder__Rich1TopPanel
        cont = self.getProp("Context")
        return Rich__Rec__ENNRingFinder__Rich1TopPanel(cont+"ENNFinderR1Top")

    ## Access the finder for RICH1 bottom panel
    def rich1BottomFinder(self):
        from Configurables import Rich__Rec__ENNRingFinder__Rich1BottomPanel
        cont = self.getProp("Context")
        return Rich__Rec__ENNRingFinder__Rich1BottomPanel(cont+"ENNFinderR1Bottom")

    ## Access the finder for RICH2 left panel
    def rich2LeftFinder(self):
        from Configurables import Rich__Rec__ENNRingFinder__Rich2LeftPanel
        cont = self.getProp("Context")
        return Rich__Rec__ENNRingFinder__Rich2LeftPanel(cont+"ENNFinderR2Left")

    ## Access the finder for RICH2 right panel
    def rich2RightFinder(self):
        from Configurables import Rich__Rec__ENNRingFinder__Rich2RightPanel
        cont = self.getProp("Context")
        return Rich__Rec__ENNRingFinder__Rich2RightPanel(cont+"ENNFinderR2Right")

    ## @brief Apply the configuration to the given GaudiSequencer
    def applyConf(self):

        from Configurables import ( Rich__Rec__TracklessRingFilterAlg,
                                    Rich__Rec__TracklessRingIsolationAlg )

        if not self.isPropertySet("Sequencer") :
            raise RuntimeError("ERROR : Rich ENN Ring Finder Sequence not set")
        else :
            sequence = self.getProp("Sequencer")
            panels   = self.getProp("Panels")
            if len(panels) == 0 :
                raise RuntimeError("ERROR : No panels set to find ENN Rings")
            else:
                # maximum hits in each panel for finding
                maxhits = self.getProp("MaxHitsInPanel")

                # context
                cont = self.getProp("Context")

                # The finders, for each HPD panel
                if "Rich1Top" in panels :
                    alg                = self.rich1TopFinder()
                    alg.MaxHitsInEvent = maxhits
                    alg.RingLocation   = "Rec/Rich/ENN/RingsAll"
                    alg.OutputLevel = DEBUG
                    sequence.Members  += [alg]
                if "Rich1Bottom" in panels :
                    alg                = self.rich1BottomFinder()
                    alg.MaxHitsInEvent = maxhits
                    alg.RingLocation   = "Rec/Rich/ENN/RingsAll"
                    alg.OutputLevel = DEBUG
                    sequence.Members  += [alg]
                if "Rich2Left" in panels :
                    alg                = self.rich2LeftFinder()
                    alg.MaxHitsInEvent = maxhits
                    alg.RingLocation   = "Rec/Rich/ENN/RingsAll"
                    alg.OutputLevel = DEBUG
                    sequence.Members  += [alg]
                if "Rich2Right" in panels :
                    alg                = self.rich2RightFinder()
                    alg.MaxHitsInEvent = maxhits
                    alg.RingLocation   = "Rec/Rich/ENN/RingsAll"
                    alg.OutputLevel = DEBUG
                    sequence.Members  += [alg]

                # Attempt to associated rings to segments
                if self.getProp("AssociateToSegments"):
                    from Configurables import Rich__Rec__TracklessRingSegmentAssociationAlg
                    alg               = Rich__Rec__TracklessRingSegmentAssociationAlg(cont+"ENNRingsSegAssoc")
                    alg.InputRings    = "Rec/Rich/ENN/RingsAll"
                    sequence.Members += [alg]

                # Post finding cleaning and ring selection
                filter             = Rich__Rec__TracklessRingFilterAlg(cont+"BestENNRings")
                filter.InputRings  = "Rec/Rich/ENN/RingsAll"
                filter.OutputRings = "Rec/Rich/ENN/RingsBest"
                iso                = Rich__Rec__TracklessRingIsolationAlg(cont+"IsolatedENNRings")
                iso.InputRings     = "Rec/Rich/ENN/RingsBest"
                iso.OutputRings    = "Rec/Rich/ENN/RingsIsolated"
                sequence.Members  += [filter,iso]
            
