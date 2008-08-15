
## @package RichGlobalPID
#  High level configuration tools for RICH Global PID
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: RichGlobalPID.py,v 1.1 2008-08-15 14:41:23 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from Configurables import ( GaudiSequencer,
                            Rich__Rec__GlobalPID__Initialize,
                            Rich__Rec__GlobalPID__DigitSel,
                            Rich__Rec__GlobalPID__TrackSel,
                            Rich__Rec__PixelBackgroundAlg,
                            Rich__Rec__GlobalPID__Likelihood,
                            Rich__Rec__GlobalPID__LikelihoodTool,
                            Rich__Rec__GlobalPID__Finalize )

# ----------------------------------------------------------------------------------

## @class RichGlobalPIDConfig
#  High level configuration tools for RICH Global PID
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichGlobalPIDConfig(RichConfigurableUser):

    ## The default options
    __slots__ = {
        "context":  "Offline",
        "mode": "Full",
        "nIterations": 2,
        "maxUsedPixels": 8000,
        "finalDLLCheck": [ False, True, True, True ],
        "trackFreezeOutDLL":    [ 2, 4, 5, 6 ],
        "trackForceChangeDLL":  [ -1, -2, -3, -4 ],
        "likelihoodThreshold":  [ -1e-2, -1e-3, -1e-4, -1e-5 ],
        "maxTrackChangesPerIt": [ 5, 5, 5, 5 ],
        "minSignalForNoLLCalc": [ 1e-3, 1e-3, 1e-3, 1e-3 ]
        }

    ## @brief Apply the configuration
    #
    def applyConf(self,sequence):

        # Context
        cont = self.getProp("context")

        # Initialisation
        initSeq = GaudiSequencer("Rich"+cont+"GPIDInitSeq")
        initSeq.MeasureTime = True
        sequence.Members += [ initSeq ]
        gInit   = Rich__Rec__GlobalPID__Initialize("Rich"+cont+"GPIDInit")
        gPixSel = Rich__Rec__GlobalPID__DigitSel("Rich"+cont+"GPIDDigSel")
        gPixSel.MaxUsedPixels = self.getProp("maxUsedPixels")
        gTrkSel = Rich__Rec__GlobalPID__TrackSel("Rich"+cont+"GPIDTrSel")
        initSeq.Members += [ gInit, gPixSel, gTrkSel ]

        # Likelihood minimisation
        likSeq = GaudiSequencer("Rich"+cont+"GPIDLLSeq")
        likSeq.MeasureTime = True
        sequence.Members += [ likSeq ]
        for it in range(0,self.getProp("nIterations")):

            # background estimation
            bckEsti = Rich__Rec__PixelBackgroundAlg( "Rich"+cont+"BckEstIt" + `it` )
            likSeq.Members += [bckEsti]

            # Likelihood minimisation
            lik = Rich__Rec__GlobalPID__Likelihood( "Rich"+cont+"GPIDLLIt" + `it` )
            likSeq.Members += [lik]

            # configure the likelihood minimisation
            pidTool = Rich__Rec__GlobalPID__LikelihoodTool("GPIDLikelihoodTool")
            pidTool.TrackFreezeOutDLL    = self.getProp("trackFreezeOutDLL")[it]
            pidTool.FinalDLLCheck        = self.getProp("finalDLLCheck")[it]
            pidTool.TrackForceChangeDLL  = self.getProp("trackForceChangeDLL")[it]
            pidTool.LikelihoodThreshold  = self.getProp("likelihoodThreshold")[it]
            pidTool.MaxTrackChangesPerIt = self.getProp("maxTrackChangesPerIt")[it]
            pidTool.MinSignalForNoLLCalc = self.getProp("minSignalForNoLLCalc")[it]

            # Add to alg
            lik.addTool( pidTool )            
        
        # Finalise Global PID algorithm each event
        sequence.Members += [ Rich__Rec__GlobalPID__Finalize("Rich"+cont+"GPIDFin") ]
