
## @package RichMarkovRingFinder
#  High level Configuration tools for RICH Markov ring finding
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.8 2009-06-03 09:01:41 jonrob Exp $"
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
       ,"Panels"              : [  "Rich1GasTop","Rich1GasBottom"
                                  ,"Rich2GasLeft","Rich2GasRight"
                                  ]
       ,"MaxHitsInPanel"      : [ 400, 400, 400 ]
       ,"MaxHitsInHPD"        : [ 50,  50,  50  ]
       ,"AssociateToSegments" : True
       ,"Sequencer"           : None # The sequencer to add the RICH Markov ring finder algorithms to
       ,"OutputLevel"         : INFO
        }

    ## Access the finder for RICH1 top panel C4F10
    def rich1GasTopFinder(self):
        from Configurables import Rich__Rec__MarkovRingFinder__Rich1TopPanel
        cont = self.getProp("Context")
        return Rich__Rec__MarkovRingFinder__Rich1TopPanel(cont+"MarkovFinderR1GasTop")

    ## Access the finder for RICH1 bottom panel C4F10
    def rich1GasBottomFinder(self):
        from Configurables import Rich__Rec__MarkovRingFinder__Rich1BottomPanel
        cont = self.getProp("Context")
        return Rich__Rec__MarkovRingFinder__Rich1BottomPanel(cont+"MarkovFinderR1GasBottom")

    ## Access the finder for RICH2 left panel CF4
    def rich2GasLeftFinder(self):
        from Configurables import Rich__Rec__MarkovRingFinder__Rich2LeftPanel
        cont = self.getProp("Context")
        return Rich__Rec__MarkovRingFinder__Rich2LeftPanel(cont+"MarkovFinderR2GasLeft")

    ## Access the finder for RICH2 right panel CF4
    def rich2GasRightFinder(self):
        from Configurables import Rich__Rec__MarkovRingFinder__Rich2RightPanel
        cont = self.getProp("Context")
        return Rich__Rec__MarkovRingFinder__Rich2RightPanel(cont+"MarkovFinderR2GasRight")

    ## Turn on histograming for all algs
    def enableHistos( self, enableHistos = False ) :
        self.rich1GasTopFinder().HistoProduce     = enableHistos
        self.rich1GasBottomFinder().HistoProduce  = enableHistos
        self.rich2GasLeftFinder().HistoProduce    = enableHistos
        self.rich2GasRightFinder().HistoProduce   = enableHistos

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
                maxhits    = self.getProp("MaxHitsInPanel")
                maxhpdhits = self.getProp("MaxHitsInHPD")

                # Ring locations
                AllRingsLoc  = "Rec/Rich/Markov/RingsAll"
                BestRingsLoc = "Rec/Rich/Markov/RingsBest"
                IsoRingsLoc  = "Rec/Rich/Markov/RingsIsolated"

                # context
                cont = self.getProp("Context")

                setOutLevel = self.isPropertySet("OutputLevel")
                level = self.getProp("OutputLevel")

                # The finders, for each HPD panel and radiator
                if "Rich1GasTop" in panels :
                    alg                = self.rich1GasTopFinder()
                    alg.MaxHitsInEvent = maxhits[1]
                    alg.MaxHitsInHPD   = maxhpdhits[1]
                    alg.RingLocation   = AllRingsLoc
                    if setOutLevel : alg.OutputLevel = level
                    sequence.Members  += [alg]
                if "Rich1GasBottom" in panels :
                    alg                = self.rich1GasBottomFinder()
                    alg.MaxHitsInEvent = maxhits[1]
                    alg.MaxHitsInHPD   = maxhpdhits[1]
                    alg.RingLocation   = AllRingsLoc
                    if setOutLevel : alg.OutputLevel = level
                    sequence.Members  += [alg]
                if "Rich2GasLeft" in panels :
                    alg                = self.rich2GasLeftFinder()
                    alg.MaxHitsInEvent = maxhits[2]
                    alg.MaxHitsInHPD   = maxhpdhits[2]
                    alg.RingLocation   = AllRingsLoc
                    if setOutLevel : alg.OutputLevel = level
                    sequence.Members  += [alg]
                if "Rich2GasRight" in panels :
                    alg                = self.rich2GasRightFinder()
                    alg.MaxHitsInEvent = maxhits[2]
                    alg.MaxHitsInHPD   = maxhpdhits[2]
                    alg.RingLocation   = AllRingsLoc
                    if setOutLevel : alg.OutputLevel = level
                    sequence.Members  += [alg]

                # Attempt to associated rings to segments
                if self.getProp("AssociateToSegments"):
                    from Configurables import Rich__Rec__TracklessRingSegmentAssociationAlg
                    alg               = Rich__Rec__TracklessRingSegmentAssociationAlg(cont+"MarkovRingsSegAssoc")
                    alg.InputRings    = AllRingsLoc
                    if setOutLevel : alg.OutputLevel = level
                    sequence.Members += [alg]

                # Post finding cleaning and ring selection
                filter             = Rich__Rec__TracklessRingFilterAlg(cont+"BestMarkovRings")
                filter.InputRings  = AllRingsLoc
                filter.OutputRings = BestRingsLoc
                if setOutLevel : filter.OutputLevel = level
                iso                = Rich__Rec__TracklessRingIsolationAlg(cont+"IsolatedMarkovRings")
                iso.InputRings     = BestRingsLoc
                iso.OutputRings    = IsoRingsLoc
                if setOutLevel : iso.OutputLevel = level
                sequence.Members  += [filter,iso]
            
