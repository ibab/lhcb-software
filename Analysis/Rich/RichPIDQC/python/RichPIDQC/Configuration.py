
## @package RichPIDQC
#  RICH PID Calibration and Monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   10/02/2009

__version__ = "$Id: Configuration.py,v 1.7 2009-10-28 10:17:57 jpalac Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from RichKernel.Configuration import *
       
## @class RichPIDQCConf
#  Configurable for RICH reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichPIDQCConf(RichConfigurableUser):

    ## Possible used Configurables
    __used_configurables__ = [ ('DsToPhiPiConf',None),
                               ('JPsiMuMuConf',None),
                               ('KshortPiPiConf',None),
                               ('LambdaToProtonPionConf',None),
                               ('DstarToDzeroPiConf',None) ]
    
    ## Steering options
    __slots__ = {
         "Context"         : "Offline"  # The context within which to run
        ,"OutputLevel"     : INFO  # The output level to set all algorithms and tools to use
        ,"CalibSequencer"  : None  # The sequencer to add the calibration algorithms too
        ,"PIDCalibrations" : [ "DsPhiPi","DstarD0Pi","JPsiMuMu","LambdaPrPi","KshortPiPi" ]
        ,"RunSelection"    : True
        ,"RunMonitors"     : True
        ,"MCChecks"        : False
        ,"MakeNTuple"      : False
        ,"MakeSelDST"      : False
        ,"PlotTools"       : [ "MassPlotTool","MomentumPlotTool","RichPlotTool","CombinedPidPlotTool" ]
        }

    ## Full list of 'PlotTools' = "MassPlotTool","MomentumPlotTool", "CombinedPidPlotTool",
    ##                            "RichPlotTool","CaloPlotTool","MuonPlotTool"

    ## Check a new sequence and add to main sequence
    def newSeq(self,sequence,name):
        from Configurables import GaudiSequencer
        seq = GaudiSequencer(name)
        seq.MeasureTime = True
        sequence.Members += [seq]
        return seq

    ## get the calibration sequence
    def calibSeq(self):
        if not self.isPropertySet("CalibSequencer") :
            raise RuntimeError("ERROR : Calibration Sequence not set") 
        sequence = self.getProp("CalibSequencer")
        return sequence

    ## Propagate the options
    def propagateOptions(self,conf):
        self.setOtherProps(conf,["MCChecks","MakeNTuple","MakeSelDST","Context",
                                 "RunSelection","RunMonitors","OutputLevel","PlotTools"])

    ## @brief Apply the configuration to the configured GaudiSequencer
    def __apply_configuration__(self) :

        # Which calibrations to run
        calibs = self.getProp("PIDCalibrations")

        calibSeq = self.calibSeq()
        calibSeq.IgnoreFilterPassed = True

        # Sanity checks
        if not self.getProp("RunSelection") :
            if self.getProp("MCChecks")   : raise RuntimeError("ERROR : Must run selections in order to run MC checks")
            if self.getProp("MakeNTuple") : raise RuntimeError("ERROR : Must run selections in order to make an NTuple")
            if self.getProp("MakeSelDST") : raise RuntimeError("ERROR : Must run selections in order to write a DST")

        # Run Conor's Ds -> Phi Pi selection and calibration
        if "DsPhiPi" in calibs :
            from DsToPhiPi import DsToPhiPiConf
            conf = self.getRichCU(DsToPhiPiConf)
            self.propagateOptions(conf)
            conf.setProp("Sequencer",self.newSeq( calibSeq, "RichDsToPhiPiPIDMoni"))
            #if forceInit : conf.__apply_configuration__()

        # Andrew's D* -> D0(KPi) Pi selection and calibration
        if "DstarD0Pi" in calibs :
            from DstarToDzeroPi import DstarToDzeroPiConf
            conf = self.getRichCU(DstarToDzeroPiConf)
            self.propagateOptions(conf)
            conf.setProp("Sequencer",self.newSeq( calibSeq, "RichDstarToD0PiPIDMoni"))

        # Andrew's Lambda -> Proton Pion selection
        if "LambdaPrPi" in calibs :
            from LambdaToProtonPion import LambdaToProtonPionConf
            conf = self.getRichCU(LambdaToProtonPionConf)
            self.propagateOptions(conf)
            conf.setProp("Sequencer",self.newSeq( calibSeq, "RichLambdaToPrPiPIDMoni"))

        # Andrew's Kshort -> Pion Pion selection
        if "KshortPiPi" in calibs :
            from KshortPiPi import KshortPiPiConf
            conf = self.getRichCU(KshortPiPiConf)
            self.propagateOptions(conf)
            conf.setProp("Sequencer",self.newSeq( calibSeq, "RichKsToPiPiPIDMoni"))

        # Nicola's J/Psi -> Mu Mu selection
        if "JPsiMuMu" in calibs :
            from JPsiMuMu import JPsiMuMuConf
            conf = self.getRichCU(JPsiMuMuConf)
            self.propagateOptions(conf)
            conf.setProp("Sequencer",self.newSeq( calibSeq, "RichJPsiToMuMuPIDMoni"))
