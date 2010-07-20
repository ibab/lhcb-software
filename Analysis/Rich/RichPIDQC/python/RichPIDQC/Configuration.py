
## @package RichPIDQC
#  RICH PID Calibration and Monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   10/02/2009

__version__ = "$Id: Configuration.py,v 1.7 2009-10-28 10:17:57 jpalac Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from LHCbKernel.Configuration import *
        
## @class RichPIDQCConf
#  Configurable for RICH reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichPIDQCConf(LHCbConfigurableUser):

    ## Possible used Configurables
    __used_configurables__ = [ 'DsToPhiPiConf',
                               'JPsiMuMuConf',
                               'KshortPiPiConf',
                               'LambdaToProtonPionConf',
                               'DstarToDzeroPiConf'     ]

    ## Steering options
    __slots__ = {
         "CalibSequencer"  : None  # The sequencer to add the calibration algorithms too
        ,"PIDCalibrations" : [ "DsPhiPi","DstarD0Pi","JPsiMuMu","LambdaPrPi","KshortPiPi" ]
        ,"MCChecks"   : False
        ,"MakeNTuple" : False
        ,"MakeSelDST" : False
        }

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

    ## @brief Apply the configuration to the configured GaudiSequencer
    def __apply_configuration__(self) :

        # Which calibrations to run
        calibs = self.getProp("PIDCalibrations")

        calibSeq = self.calibSeq()
        calibSeq.IgnoreFilterPassed = True

        # Run Conor's Ds -> Phi Pi selection and calibration
        if "DsPhiPi" in calibs :
            from DsToPhiPi          import DsToPhiPiConf
            self.setOtherProps(DsToPhiPiConf(),["MCChecks","MakeNTuple","MakeSelDST"])
            DsToPhiPiConf().setProp("Sequencer",self.newSeq( calibSeq, "RichDsToPhiPiPIDMoni"))

        # Andrew's D* -> D0(KPi) Pi selection and calibration
        if "DstarD0Pi" in calibs :
            from DstarToDzeroPi     import DstarToDzeroPiConf
            self.setOtherProps(DstarToDzeroPiConf(),["MCChecks","MakeNTuple","MakeSelDST"])
            DstarToDzeroPiConf().setProp("Sequencer",self.newSeq( calibSeq, "RichDstarToD0PiPIDMoni"))

        # Andrew's Lambda -> Proton Pion selection
        if "LambdaPrPi" in calibs :
            from LambdaToProtonPion import LambdaToProtonPionConf
            self.setOtherProps(LambdaToProtonPionConf(),["MCChecks","MakeNTuple","MakeSelDST"])
            LambdaToProtonPionConf().setProp("Sequencer",self.newSeq( calibSeq, "RichLambdaToPrPiPIDMoni"))

        # Andrew's Kshort -> Pion Pion selection
        if "KshortPiPi" in calibs :
            from KshortPiPi         import KshortPiPiConf
            self.setOtherProps(KshortPiPiConf(),["MCChecks","MakeNTuple","MakeSelDST"])
            KshortPiPiConf().setProp("Sequencer",self.newSeq( calibSeq, "RichKsToPiPiPIDMoni"))

        # Nicola's J/Psi -> Mu Mu selection
        if "JPsiMuMu" in calibs :
            from JPsiMuMu           import JPsiMuMuConf
            self.setOtherProps(JPsiMuMuConf(),["MCChecks","MakeNTuple","MakeSelDST"])
            JPsiMuMuConf().setProp("Sequencer",self.newSeq( calibSeq, "RichJPsiToMuMuPIDMoni"))
