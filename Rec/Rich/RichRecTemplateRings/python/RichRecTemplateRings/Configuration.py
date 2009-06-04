
## @package RichRecTemplateRings
#  High level Configuration tools for RICH Template Ring finder
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @author Sajan Easo   (Sajan.Easo@cern.ch)
#  @date   02/03/2009

__version__ = "$Id: Configuration.py,v 1.4 2009-06-04 16:30:48 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *

# ----------------------------------------------------------------------------------

## @class RichTemplateRingFinderConf
#  High level Configuration tools for RICH Template Ring finder
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichTemplateRingFinderConf(RichConfigurableUser):

        ## Steering options
    __slots__ = {
        "Context"             : "Offline"   # The context within which to run
       ,"Sequencer"           : None        # The sequencer to add the RICH MCMC algorithms to
       ,"MakeNtuple"          : False       # Make development ntuple
       ,"OutputLevel"         : INFO
        }

    ## Turn on histograming for all algs
    def enableHistos( self, enableHistos = False ) : pass

    ## @brief Apply the configuration to the given GaudiSequencer
    def __apply_configuration__(self):

        if not self.isPropertySet("Sequencer") :
            raise RuntimeError("ERROR : Rich Template Ring Finder Sequence not set")
        
        sequence = self.getProp("Sequencer")

        # context
        cont = self.getProp("Context")

        # Ring locations
        AllRingsLoc  = "Rec/Rich/Template/RingsAll"
        BestRingsLoc = "Rec/Rich/Template/RingsBest"
        IsoRingsLoc  = "Rec/Rich/Template/RingsIsolated"

        setOutLevel = self.isPropertySet("OutputLevel")
        level = self.getProp("OutputLevel")

        # Ring finding algs
        from Configurables import ( RichRingRecAlgBase,
                                    RichTargetDataConfigAlg,
                                    RichRingMasterAlg )
        base   = RichRingRecAlgBase()
        config = RichTargetDataConfigAlg()
        master = RichRingMasterAlg()
        master.RingLocation = AllRingsLoc
        sequence.Members += [ base, config, master ]

        # selection and isolation algs
        from Configurables import ( Rich__Rec__TracklessRingFilterAlg,
                                    Rich__Rec__TracklessRingIsolationAlg )
        filter             = Rich__Rec__TracklessRingFilterAlg(cont+"BestTemplateRings")
        filter.InputRings  = AllRingsLoc
        filter.OutputRings = BestRingsLoc
        if setOutLevel : filter.OutputLevel = level
        iso                = Rich__Rec__TracklessRingIsolationAlg(cont+"IsolatedTemplateRings")
        iso.InputRings     = BestRingsLoc
        iso.OutputRings    = IsoRingsLoc
        if setOutLevel : iso.OutputLevel = level
        sequence.Members  += [filter,iso]

        # Development ntuple
        if self.getProp("MakeNtuple") :

            from Configurables import NTupleSvc
            NTupleSvc().Output = ["RICHTUPLE1 DATAFILE='Ntuple_RichTemplateRings.root' TYP='ROOT' OPT='NEW'"]

            master.RichTemplateRingNtupProduce = True
            
