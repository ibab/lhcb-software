
## @package RichENNRingFinder
#  High level Configuration tools for RICH ENN ring finding
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.6 2009-06-19 22:12:53 jonrob Exp $"
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
       ,"Panels"              : [ "Rich1GasTop","Rich1GasBottom"
                                 ,"Rich2GasLeft","Rich2GasRight"
                                 #,"AerogelTop","AerogelBottom", # Very experimental. Not really working.
                                  ]
       ,"MaxHitsInPanel"      : [ 200, 400, 400 ]
       ,"MaxHitsInHPD"        : [ 30,  50,  50  ]
       ,"AssociateToSegments" : True
       ,"Sequencer"           : None # The sequencer to add the RICH ENN ring finder algorithms to
       ,"OutputLevel"         : INFO # The output level to set all algorithms and tools to use
        }

    ## Access the finder for RICH1 top panel Aerogel
    def rich1AeroTopFinder(self):
        from Configurables import Rich__Rec__ENNRingFinder__Rich1AerogelTopPanel
        cont = self.getProp("Context")
        return Rich__Rec__ENNRingFinder__Rich1AerogelTopPanel(cont+"ENNFinderR1AeroTop")

    ## Access the finder for RICH1 bottom panel Aerogel
    def rich1AeroBottomFinder(self):
        from Configurables import Rich__Rec__ENNRingFinder__Rich1AerogelBottomPanel
        cont = self.getProp("Context")
        return Rich__Rec__ENNRingFinder__Rich1AerogelBottomPanel(cont+"ENNFinderR1AeroBottom")

    ## Access the finder for RICH1 top panel C4F10
    def rich1GasTopFinder(self):
        from Configurables import Rich__Rec__ENNRingFinder__Rich1GasTopPanel
        cont = self.getProp("Context")
        return Rich__Rec__ENNRingFinder__Rich1GasTopPanel(cont+"ENNFinderR1GasTop")

    ## Access the finder for RICH1 bottom panel C4F10
    def rich1GasBottomFinder(self):
        from Configurables import Rich__Rec__ENNRingFinder__Rich1GasBottomPanel
        cont = self.getProp("Context")
        return Rich__Rec__ENNRingFinder__Rich1GasBottomPanel(cont+"ENNFinderR1GasBottom")

    ## Access the finder for RICH2 left panel CF4
    def rich2GasLeftFinder(self):
        from Configurables import Rich__Rec__ENNRingFinder__Rich2GasLeftPanel
        cont = self.getProp("Context")
        return Rich__Rec__ENNRingFinder__Rich2GasLeftPanel(cont+"ENNFinderR2GasLeft")

    ## Access the finder for RICH2 right panel CF4
    def rich2GasRightFinder(self):
        from Configurables import Rich__Rec__ENNRingFinder__Rich2GasRightPanel
        cont = self.getProp("Context")
        return Rich__Rec__ENNRingFinder__Rich2GasRightPanel(cont+"ENNFinderR2GasRight")

    ## Turn on histograming for all algs
    def enableHistos( self, enableHistos = False ) :
        self.rich1AeroTopFinder().HistoProduce    = enableHistos
        self.rich1AeroBottomFinder().HistoProduce = enableHistos
        self.rich1GasTopFinder().HistoProduce     = enableHistos
        self.rich1GasBottomFinder().HistoProduce  = enableHistos
        self.rich2GasLeftFinder().HistoProduce    = enableHistos
        self.rich2GasRightFinder().HistoProduce   = enableHistos

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
                maxhits    = self.getProp("MaxHitsInPanel")
                maxhpdhits = self.getProp("MaxHitsInHPD")

                # Ring locations
                AllRingsLoc  = "Rec/Rich/ENN/RingsAll"
                BestRingsLoc = "Rec/Rich/ENN/RingsBest"
                IsoRingsLoc  = "Rec/Rich/ENN/RingsIsolated"

                # context
                cont = self.getProp("Context")

                setOutLevel = self.isPropertySet("OutputLevel")
                level = self.getProp("OutputLevel")

                # The finders, for each HPD panel and radiator
                if "AerogelTop"in panels :
                    alg                = self.rich1AeroTopFinder()
                    alg.MaxHitsInEvent = maxhits[0]
                    alg.MaxHitsInHPD   = maxhpdhits[0]
                    alg.RingLocation   = AllRingsLoc
                    if setOutLevel : alg.OutputLevel = level
                    sequence.Members  += [alg]
                if "AerogelBottom" in panels :
                    alg                = self.rich1AeroBottomFinder()
                    alg.MaxHitsInEvent = maxhits[0]
                    alg.MaxHitsInHPD   = maxhpdhits[0]
                    alg.RingLocation   = AllRingsLoc
                    if setOutLevel : alg.OutputLevel = level
                    sequence.Members  += [alg]
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
                    alg               = Rich__Rec__TracklessRingSegmentAssociationAlg(cont+"ENNRingsSegAssoc")
                    alg.InputRings    = AllRingsLoc
                    if setOutLevel : alg.OutputLevel = level
                    sequence.Members += [alg]

                # Post finding cleaning and ring selection
                filter             = Rich__Rec__TracklessRingFilterAlg(cont+"BestENNRings")
                filter.InputRings  = AllRingsLoc
                filter.OutputRings = BestRingsLoc
                if setOutLevel : filter.OutputLevel = level
                iso                = Rich__Rec__TracklessRingIsolationAlg(cont+"IsolatedENNRings")
                iso.InputRings     = BestRingsLoc
                iso.OutputRings    = IsoRingsLoc
                if setOutLevel : iso.OutputLevel = level
                sequence.Members  += [filter,iso]
            
