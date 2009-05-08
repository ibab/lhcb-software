
## @package RichPIDQC
#  RICH PID Calibration and Monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   10/02/2009

__version__ = "$Id: Configuration.py,v 1.6 2009-05-08 14:19:50 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from LHCbKernel.Configuration import *
from DsToPhiPi          import DsToPhiPiConf
from DstarToDzeroPi     import DstarToDzeroPiConf
from JPsiMuMu           import JPsiMuMuConf
from KshortPiPi         import KshortPiPiConf
from LambdaToProtonPion import LambdaToProtonPionConf
        
## @class RichPIDQCConf
#  Configurable for RICH reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichPIDQCConf(LHCbConfigurableUser):

    ## Possible used Configurables
    __used_configurables__ = [ DsToPhiPiConf, JPsiMuMuConf,
                               KshortPiPiConf, LambdaToProtonPionConf,
                               DstarToDzeroPiConf ]

    ## Steering options
    __slots__ = {
         "CalibSequencer"  : None  # The sequencer to add the calibration algorithms too
        ,"PIDCalibrations" : [ "DsPhiPi","DstarD0Pi","JPsiMuMu","LambdaPrPi","KshortPiPi" ]
        ,"MCChecks"   : False
        ,"MakeNTuple" : False
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
            self.setOtherProps(DsToPhiPiConf(),["MCChecks","MakeNTuple"])
            DsToPhiPiConf().setProp("Sequencer",self.newSeq( calibSeq, "RichDsToPhiPiSeq"))

        # Andrew's D* -> D0(KPi) Pi selection and calibration
        if "DstarD0Pi" in calibs :
            self.setOtherProps(DstarToDzeroPiConf(),["MCChecks","MakeNTuple"])
            DstarToDzeroPiConf().setProp("Sequencer",self.newSeq( calibSeq, "RichDstarToD0PiSeq"))

        # Andrew's Lambda -> Proton Pion selection
        if "LambdaPrPi" in calibs :
            self.setOtherProps(LambdaToProtonPionConf(),["MCChecks","MakeNTuple"])
            LambdaToProtonPionConf().setProp("Sequencer",self.newSeq( calibSeq, "RichLambdaToPrPiSeq"))

        # Andrew's Kshort -> Pion Pion selection
        if "KshortPiPi" in calibs :
            self.setOtherProps(KshortPiPiConf(),["MCChecks","MakeNTuple"])
            KshortPiPiConf().setProp("Sequencer",self.newSeq( calibSeq, "RichKsToPiPiSeq"))

        # Nicola's J/Psi -> Mu Mu selection
        if "JPsiMuMu" in calibs :
            self.setOtherProps(JPsiMuMuConf(),["MCChecks","MakeNTuple"])
            JPsiMuMuConf().setProp("Sequencer",self.newSeq( calibSeq, "RichJPsiToMuMuSeq"))
