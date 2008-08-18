
## @package RichGlobalPID
#  High level configuration tools for RICH Global PID
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: RichGlobalPID.py,v 1.2 2008-08-18 19:33:00 jonrob Exp $"
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

    ## Default number of iterations
    DefaultNiterations          = { "Offline" : 2, "HLT" : 2 }
    ## Default trackFreezeOutDLL options per iteration
    DefaultTrackFreezeOutDLL    = { "Offline" : [ 2, 4, 5, 6 ],
                                    "HLT"     : [ 2, 4, 5, 6 ] }
    ## Default trackForceChangeDLL options per iteration
    DefaultTrackForceChangeDLL  = { "Offline" : [ -1, -2, -3, -4 ],
                                    "HLT"     : [ -1, -2, -3, -4 ] }
    ## Default likelihoodThreshold options per iteration
    DefaultLikelihoodThreshold  = { "Offline" : [ -1e-2, -1e-3, -1e-4, -1e-5 ],
                                    "HLT"     : [ -1e-2, -1e-3, -1e-4, -1e-5 ] }
    ## Default maxTrackChangesPerIt options per iteration
    DefaultMaxTrackChangesPerIt = { "Offline" : [ 5, 5, 4, 3 ],
                                    "HLT"     : [ 5, 5, 5, 5 ] }
    ## Default minSignalForNoLLCalc options per iteration
    DefaultMinSignalForNoLLCalc = { "Offline" : [ 1e-3, 1e-3, 1e-3, 1e-3 ],
                                    "HLT"     : [ 1e-3, 1e-3, 1e-3, 1e-3 ] }

    ## The default options
    __slots__ = {
        "context":  "Offline",
        "mode": "Full",
        "nIterations": 0,
        "maxUsedPixels": 8000,
        "finalDLLCheck": [ False, True, True, True ],
        "trackFreezeOutDLL":    [ ],
        "trackForceChangeDLL":  [ ],
        "likelihoodThreshold":  [ ],
        "maxTrackChangesPerIt": [ ],
        "minSignalForNoLLCalc": [ 1e-3, 1e-3, 1e-3, 1e-3 ]
        }

    ## @brief Check the configuration
    def checkConf(self):
        cont = self.getProp("context")
        if self.getProp("nIterations") == 0 :
            self.setProp("nIterations",self.DefaultNiterations[cont])
        if len(self.getProp("trackFreezeOutDLL")) == 0 :
            self.setProp("trackFreezeOutDLL",self.DefaultTrackFreezeOutDLL[cont])
        if len(self.getProp("trackForceChangeDLL")) == 0 :
            self.setProp("trackForceChangeDLL",self.DefaultTrackForceChangeDLL[cont])
        if len(self.getProp("likelihoodThreshold")) == 0 :
            self.setProp("likelihoodThreshold",self.DefaultLikelihoodThreshold[cont])
        if len(self.getProp("maxTrackChangesPerIt")) == 0 :
            self.setProp("maxTrackChangesPerIt",self.DefaultMaxTrackChangesPerIt[cont])
        if len(self.getProp("minSignalForNoLLCalc")) == 0 :
            self.setProp("minSignalForNoLLCalc",self.DefaultMinSignalForNoLLCalc[cont])
            
    ## @brief Configure the algorithms, adding them to the supplied sequencer
    #  @param sequence The sequencer to add the PID algorithms to
    def applyConf(self,sequence):

        # Context
        cont = self.getProp("context")

        # Check configuration
        self.checkConf()

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

            # Likelihood minimisation
            lik = Rich__Rec__GlobalPID__Likelihood( "Rich"+cont+"GPIDLLIt" + `it` )

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

            # Add the algs to the sequence
            likSeq.Members += [ bckEsti, lik ]
        
        # Finalise Global PID results
        sequence.Members += [ Rich__Rec__GlobalPID__Finalize("Rich"+cont+"GPIDFin") ]
