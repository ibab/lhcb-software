
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
        "Context"                 : "Offline",
        "DataType"                : "", # Type of data, propagated from application
        "Mode"                    : "Full",
        "Radiators"               : [], # The radiators to use (Aerogel/Rich1Gas/Rich2Gas)
        "InitAlgorithms"          : True,
        "InitTools"               : True,
        "NIterations"             : 0,
        "MaxUsedPixels"           : 30000,
        "MaxUsedTracks"           : 1000,
        "FinalDLLCheck"           : [ False, True, True, True ],
        "TrackFreezeOutDLL"       : None,
        "TrackForceChangeDLL"     : None,
        "LikelihoodThreshold"     : None,
        "MaxTrackChangesPerIt"    : None,
        "MinSignalForNoLLCalc"    : None,
        "PidSequencer"            : None, # The sequencer to add the RICH reconstruction algorithms to
        "OutputLevel"             : INFO, # The output level to set all algorithms and tools to use
        "TrackCuts"               : None,
        "SingleTrackMode"         : None,
        "HPDBackgroundTool"       : "AvHPD",
        "HPDBackIgnoreExpSignals" : None,
        "HPDBackMinPixBackground" : None,
        "HPDBackMinForInc"        : None
        }

    ## Initialize 
    def initialize(self):
        # default values
        self.setRichDefaults( "SingleTrackMode", { "Offline" : False, "HLT" : False } )
        self.setRichDefaults( "NIterations", { "Offline" : 2, "HLT" : 2 } )
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
        self.setRichDefault ( "TrackCuts", "Offline",
                              { "Forward" : { "Chi2Cut" : [0,5], "PCut" : [0,9999999] },
                                "Match"   : { "Chi2Cut" : [0,5], "PCut" : [0,9999999] },
                                "Seed"    : { "Chi2Cut" : [0,5], "PCut" : [1,9999999] },
                                "VeloTT"  : { "Chi2Cut" : [0,5], "PCut" : [1,9999999] },
                                "KsTrack" : { "Chi2Cut" : [0,5], "PCut" : [1,9999999] } } )
        self.setRichDefault ( "TrackCuts", "HLT",
                              { "Forward" : { "Chi2Cut" : [0,4], "PCut" : [1,9999999] },
                                "Match"   : { "Chi2Cut" : [0,4], "PCut" : [1,9999999] },
                                "Seed"    : { "Chi2Cut" : [0,4], "PCut" : [1,9999999] },
                                "VeloTT"  : { "Chi2Cut" : [0,4], "PCut" : [1,9999999] },
                                "KsTrack" : { "Chi2Cut" : [0,4], "PCut" : [1,9999999] } } )
        self.setRichDefaults("HPDBackIgnoreExpSignals",
                             { "Offline" : [ True, False, False, False ],
                               "HLT"     : [ True, False, False, False ] } )
        self.setRichDefaults("HPDBackMinPixBackground",
                             { "Offline" : [ 0,0,0,0 ],
                               "HLT"     : [ 0,0,0,0 ] } )
        self.setRichDefaults("HPDBackMinForInc",
                             { "Offline" : [ 0,0,0,0 ],
                               "HLT"     : [ 0,0,0,0 ] } )
        
    ## @brief Apply the configuration
    #  @param sequence The sequencer to add the PID algorithms to
    def applyConf(self):

        # DataType specific tweeks
        self.dataTypeTweeks()

        # Are we properly configured
        if not self.isPropertySet("PidSequencer") :
            raise RuntimeError("ERROR : PID Sequence not set")
        sequence = self.getProp("PidSequencer")
                
        # Setup the tools and algs
        if self.getProp("InitAlgorithms") : self.applyConfAlgs(sequence)
        if self.getProp("InitTools")      : self.applyConfTools()

    ## @brief Apply any tweeks to the default configuration that vary by DataType
    def dataTypeTweeks(self):

        # Get the DataType
        dataType = self.getProp("DataType")

        # Different cuts for early data
        if dataType == "2009" or dataType == "2010" :
            if not self.isPropertySet("MaxUsedPixels") :
                self.setProp( "MaxUsedPixels", 20000 )

    ## @brief Configure the algorithms, adding them to the supplied sequencer
    #  @param sequence The sequencer to add the PID algorithms to
    def applyConfAlgs(self,sequence):
        
        # Context
        cont = self.getProp("Context")

        # Initialisation
        initSeq = self.makeRichAlg(GaudiSequencer,"Rich"+cont+"GPIDInitSeq")
        sequence.Members += [ initSeq ]
        gInit   = self.makeRichAlg(Rich__Rec__GlobalPID__Initialize,"Rich"+cont+"GPIDInit")
        gPixSel = self.makeRichAlg(Rich__Rec__GlobalPID__DigitSel,"Rich"+cont+"GPIDDigSel")
        gPixSel.MaxUsedPixels = self.getProp("MaxUsedPixels")
        gTrkSel = self.makeRichAlg(Rich__Rec__GlobalPID__TrackSel,"Rich"+cont+"GPIDTrSel")
        gTrkSel.MaxUsedTracks = self.getProp("MaxUsedTracks")
        initSeq.Members += [ gInit, gPixSel, gTrkSel ]

        # track creator tool
        tool = self.makeRichAlg(Rich__Rec__GlobalPID__TrackCreator,"ToolSvc."+cont+"_GPIDTrackCreator")
        tool.Context = cont

        # Track Selection cuts
        trselname = "TrackSelector"
        gTrkSel.addTool( self.richTools().trackSelector(trselname), name=trselname )
        import TrackSelectorTools
        TrackSelectorTools.configureTrackSelectorCuts(gTrkSel.TrackSelector,self.getProp("TrackCuts"))
        
        # Likelihood minimisation
        likSeq = self.makeRichAlg(GaudiSequencer,"Rich"+cont+"GPIDLLSeq")
        sequence.Members += [ likSeq ]
        for it in range(0,self.getProp("NIterations")):

            # background estimation
            bckEsti = self.makeRichAlg( Rich__Rec__PixelBackgroundAlg,
                                        "Rich"+cont+"BckEstIt" + `it` )
            bkgToolT = self.getProp("HPDBackgroundTool")
            if bkgToolT == "AvHPD":
                from Configurables import Rich__Rec__BackgroundEstiAvHPD
                self.toolRegistry().Tools += ["Rich::Rec::BackgroundEstiAvHPD/PixelBackgroundTool"]
                bckEsti.addTool( Rich__Rec__BackgroundEstiAvHPD, "PixelBackgroundTool" )
            else:
                raise RuntimeError("ERROR : HPD background tool "+bkgToolT+" not known")
            bckEsti.PixelBackgroundTool.IgnoreExpectedSignals = self.getProp("HPDBackIgnoreExpSignals")[it]
            bckEsti.PixelBackgroundTool.MinPixelBackground    = self.getProp("HPDBackMinPixBackground")[it]
            bckEsti.PixelBackgroundTool.MinHPDBckForInclusion = self.getProp("HPDBackMinForInc")[it]

            # Likelihood minimisation
            lik = self.makeRichAlg( Rich__Rec__GlobalPID__Likelihood,
                                    "Rich"+cont+"GPIDLLIt" + `it` )
            # Likelihood minimisation mode
            lik.SingleTrackMode = self.getProp("SingleTrackMode")

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
            likSeq.Members += [ bckEsti , lik ]

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
    
