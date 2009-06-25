
## @package RichGlobalPID
#  High level configuration tools for RICH Global PID
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: RichGlobalPID_.py,v 1.5 2009-06-25 10:19:09 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
from Configurables import ( GaudiSequencer,
                            Rich__Rec__GlobalPID__Initialize,
                            Rich__Rec__GlobalPID__DigitSel,
                            Rich__Rec__GlobalPID__TrackSel,
                            Rich__Rec__PixelBackgroundAlg,
                            Rich__Rec__GlobalPID__TrackCreator,
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
        "Context":  "Offline",
        "Mode": "Full",
        "InitAlgorithms": True,
        "InitTools": True,
        "NIterations": 0,
        "MaxUsedPixels": 12000,
        "FinalDLLCheck": [ False, True, True, True ],
        "TrackFreezeOutDLL":    None,
        "TrackForceChangeDLL":  None,
        "LikelihoodThreshold":  None,
        "MaxTrackChangesPerIt": None,
        "MinSignalForNoLLCalc": None
       ,"PidSequencer" : None     # The sequencer to add the RICH reconstruction algorithms to
       ,"OutputLevel"   : INFO    # The output level to set all algorithms and tools to use
        }

    ## Make an instance of an algortithm
    def makeRichAlg(self,type,name):
        alg = type(name)
        if self.isPropertySet("OutputLevel") : alg.OutputLevel = self.getProp("OutputLevel")
        return alg 

    ## Initialize 
    def initialize(self):
        # default values
        self.setRichDefaults("NIterations",{"Offline":2,"HLT":2})
        self.setRichDefaults("TrackFreezeOutDLL",
                             { "Offline" : [ 2, 4, 5, 6 ],
                               "HLT"     : [ 2, 4, 5, 6 ] } )
        self.setRichDefaults("TrackForceChangeDLL",
                             { "Offline" : [ -1, -2, -3, -4 ],
                               "HLT"     : [ -1, -2, -3, -4 ] } )
        self.setRichDefaults("LikelihoodThreshold",
                             { "Offline" : [ -1e-2, -1e-3, -1e-4, -1e-5 ],
                               "HLT"     : [ -1e-2, -1e-3, -1e-4, -1e-5 ] } )
        self.setRichDefaults("MaxTrackChangesPerIt",
                             { "Offline" : [ 5, 5, 4, 3 ],
                               "HLT"     : [ 5, 5, 5, 5 ] } )
        self.setRichDefaults("MinSignalForNoLLCalc",
                             { "Offline" : [ 1e-3, 1e-3, 1e-3, 1e-3 ],
                               "HLT"     : [ 1e-3, 1e-3, 1e-3, 1e-3 ] } )
    
    ## @brief Apply the configuration
    #  @param sequence The sequencer to add the PID algorithms to
    def applyConf(self):

        # Are we properly configured
        if not self.isPropertySet("PidSequencer") :
            raise RuntimeError("ERROR : PID Sequence not set")
        sequence = self.getProp("PidSequencer")
                
        # Setup the tools and algs
        if self.getProp("InitAlgorithms") : self.applyConfAlgs(sequence)
        if self.getProp("InitTools")      : self.applyConfTools()

    ## @brief Configure the algorithms, adding them to the supplied sequencer
    #  @param sequence The sequencer to add the PID algorithms to
    def applyConfAlgs(self,sequence):
        
        # Context
        cont = self.getProp("Context")

        # Initialisation
        initSeq = self.makeRichAlg(GaudiSequencer,"Rich"+cont+"GPIDInitSeq")
        initSeq.MeasureTime = True
        sequence.Members += [ initSeq ]
        gInit   = self.makeRichAlg(Rich__Rec__GlobalPID__Initialize,"Rich"+cont+"GPIDInit")
        gPixSel = self.makeRichAlg(Rich__Rec__GlobalPID__DigitSel,"Rich"+cont+"GPIDDigSel")
        gPixSel.MaxUsedPixels = self.getProp("MaxUsedPixels")
        gTrkSel = self.makeRichAlg(Rich__Rec__GlobalPID__TrackSel,"Rich"+cont+"GPIDTrSel")
        initSeq.Members += [ gInit, gPixSel, gTrkSel ]

        # track creator tool
        tool = self.makeRichAlg(Rich__Rec__GlobalPID__TrackCreator,"ToolSvc."+cont+"_GPIDTrackCreator")
        tool.Context = cont

        # Likelihood minimisation
        likSeq = self.makeRichAlg(GaudiSequencer,"Rich"+cont+"GPIDLLSeq")
        likSeq.MeasureTime = True
        sequence.Members += [ likSeq ]
        for it in range(0,self.getProp("NIterations")):

            # background estimation
            bckEsti = self.makeRichAlg( Rich__Rec__PixelBackgroundAlg,
                                        "Rich"+cont+"BckEstIt" + `it` )

            # Likelihood minimisation
            lik = self.makeRichAlg( Rich__Rec__GlobalPID__Likelihood,
                                    "Rich"+cont+"GPIDLLIt" + `it` )

            # configure the likelihood minimisation
            pidTool = self.makeRichAlg(Rich__Rec__GlobalPID__LikelihoodTool,"GPIDLikelihoodTool")
            pidTool.TrackFreezeOutDLL    = self.getProp("TrackFreezeOutDLL")[it]
            pidTool.FinalDLLCheck        = self.getProp("FinalDLLCheck")[it]
            pidTool.TrackForceChangeDLL  = self.getProp("TrackForceChangeDLL")[it]
            pidTool.LikelihoodThreshold  = self.getProp("LikelihoodThreshold")[it]
            pidTool.MaxTrackChangesPerIt = self.getProp("MaxTrackChangesPerIt")[it]
            pidTool.MinSignalForNoLLCalc = self.getProp("MinSignalForNoLLCalc")[it]

            # Add to alg
            lik.addTool( pidTool )

            # Add the algs to the sequence
            likSeq.Members += [ bckEsti, lik ]

        # Finalise Global PID results
        sequence.Members += [ self.makeRichAlg(Rich__Rec__GlobalPID__Finalize,"Rich"+cont+"GPIDFin") ]

    ## @brief Configure the tools
    #  @return The Configurable for the PID tool
    def applyConfTools(self):

        # Context
        cont = self.getProp("Context")

        # PID tool
        tool = self.makeRichAlg(Rich__Rec__GlobalPID__MultiStepTool,"ToolSvc.Rich"+cont+"PIDTool")
        tool.Context = cont
        tool.NSteps  = self.getProp("NIterations")

        # Configure each iteration
        for it in range(0,tool.NSteps):

            # configure the likelihood minimisation
            pidTool = self.makeRichAlg(Rich__Rec__GlobalPID__LikelihoodTool, "Likelihood" + `it` )
            pidTool.TrackFreezeOutDLL    = self.getProp("TrackFreezeOutDLL")[it]
            pidTool.FinalDLLCheck        = self.getProp("FinalDLLCheck")[it]
            pidTool.TrackForceChangeDLL  = self.getProp("TrackForceChangeDLL")[it]
            pidTool.LikelihoodThreshold  = self.getProp("LikelihoodThreshold")[it]
            pidTool.MaxTrackChangesPerIt = self.getProp("MaxTrackChangesPerIt")[it]
            pidTool.MinSignalForNoLLCalc = self.getProp("MinSignalForNoLLCalc")[it]

            # add to main tool
            tool.addTool(pidTool)

        # Return the PID tool
        return tool
    
