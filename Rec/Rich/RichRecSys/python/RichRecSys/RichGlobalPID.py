
## @package RichGlobalPID
#  High level configuration tools for RICH Global PID
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: RichGlobalPID.py,v 1.6 2008-08-26 19:50:32 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from Configurables import ( GaudiSequencer,
                            Rich__Rec__GlobalPID__Initialize,
                            Rich__Rec__GlobalPID__DigitSel,
                            Rich__Rec__GlobalPID__TrackSel,
                            Rich__Rec__PixelBackgroundAlg,
                            Rich__Rec__GlobalPID__Likelihood,
                            Rich__Rec__GlobalPID__LikelihoodTool,
                            Rich__Rec__GlobalPID__Finalize,
                            Rich__Rec__GlobalPID__MultiStepTool )

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
        "initAlgorithms": True,
        "initTools": True,
        "nIterations": 0,
        "maxUsedPixels": 8000,
        "finalDLLCheck": [ False, True, True, True ],
        "trackFreezeOutDLL":    None,
        "trackForceChangeDLL":  None,
        "likelihoodThreshold":  None,
        "maxTrackChangesPerIt": None,
        "minSignalForNoLLCalc": None
        }

    ## Initialise 
    def initialise(self):
        # default values
        self.setRichDefaults("nIterations",{"Offline":2,"HLT":2})
        self.setRichDefaults("trackFreezeOutDLL",
                             { "Offline" : [ 2, 4, 5, 6 ],
                               "HLT"     : [ 2, 4, 5, 6 ] } )
        self.setRichDefaults("trackForceChangeDLL",
                             { "Offline" : [ -1, -2, -3, -4 ],
                               "HLT"     : [ -1, -2, -3, -4 ] } )
        self.setRichDefaults("likelihoodThreshold",
                             { "Offline" : [ -1e-2, -1e-3, -1e-4, -1e-5 ],
                               "HLT"     : [ -1e-2, -1e-3, -1e-4, -1e-5 ] } )
        self.setRichDefaults("maxTrackChangesPerIt",
                             { "Offline" : [ 5, 5, 4, 3 ],
                               "HLT"     : [ 5, 5, 5, 5 ] } )
        self.setRichDefaults("minSignalForNoLLCalc",
                             { "Offline" : [ 1e-3, 1e-3, 1e-3, 1e-3 ],
                               "HLT"     : [ 1e-3, 1e-3, 1e-3, 1e-3 ] } )

    ## @brief Apply the configuration
    #  @param sequence The sequencer to add the PID algorithms to
    def applyConf(self,sequence):

        # Setup the tools and algs
        if self.getProp("initAlgorithms") : self.applyConfAlgs(sequence)
        if self.getProp("initTools")      : self.applyConfTools()

    ## @brief Configure the algorithms, adding them to the supplied sequencer
    #  @param sequence The sequencer to add the PID algorithms to
    def applyConfAlgs(self,sequence):
        
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

    ## @brief Configure the tools
    #  @return The Configurable for the PID tool
    def applyConfTools(self):

        # Context
        cont = self.getProp("context")

        # PID tool
        tool = Rich__Rec__GlobalPID__MultiStepTool("ToolSvc.Rich"+cont+"PIDTool")
        tool.Context = cont
        tool.NSteps  = self.getProp("nIterations")

        # Configure each iteration
        for it in range(0,tool.NSteps):

            # configure the likelihood minimisation
            pidTool = Rich__Rec__GlobalPID__LikelihoodTool( "Likelihood" + `it` )
            pidTool.TrackFreezeOutDLL    = self.getProp("trackFreezeOutDLL")[it]
            pidTool.FinalDLLCheck        = self.getProp("finalDLLCheck")[it]
            pidTool.TrackForceChangeDLL  = self.getProp("trackForceChangeDLL")[it]
            pidTool.LikelihoodThreshold  = self.getProp("likelihoodThreshold")[it]
            pidTool.MaxTrackChangesPerIt = self.getProp("maxTrackChangesPerIt")[it]
            pidTool.MinSignalForNoLLCalc = self.getProp("minSignalForNoLLCalc")[it]

            # add to main tool
            tool.addTool(pidTool)

        # Return the PID tool
        return tool
    
