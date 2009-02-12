
## @package RichRecQC
#  RICH PID Calibration and Monitoring
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   10/02/2009

__version__ = "$Id: Configuration.py,v 1.1.1.1 2009-02-12 12:58:14 jonrob Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from LHCbKernel.Configuration import *
#from Configurables import DsToPhiPiConf
from RichPIDQC.DsToPhiPi import DsToPhiPiConf
        
## @class RichRecSysConf
#  Configurable for RICH reconstruction
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   15/08/2008
class RichPIDQCConf(LHCbConfigurableUser):

    ## Possible used Configurables
    __used_configurables__ = [ DsToPhiPiConf ]

    ## Steering options
    __slots__ = {
         "CalibSequencer"  : None  # The sequencer to add the calibration algorithms too
        ,"PIDCalibrations" : [ "DsPhiPi" ]
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
        sequence = self.getProp("CalibSequencer")
        if sequence == None : raise RuntimeError("ERROR : Calibration Sequence not set")
        return sequence

    ## @brief Apply the configuration to the configured GaudiSequencer
    def __apply_configuration__(self) :

        # Which calibrations to run
        calibs = self.getProp("PIDCalibrations")

        # Run Conor's Ds -> Phi Pi selection and calibration
        if "DsPhiPi" in calibs :
            self.setOtherProps(DsToPhiPiConf(),["MCChecks","MakeNTuple"])
            DsToPhiPiConf().Sequencer = self.newSeq( self.calibSeq() ,"RichDsToPhiPiSeq" )


