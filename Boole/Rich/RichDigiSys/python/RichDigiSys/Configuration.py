
## @package RichDigiSys
#  High level Configuration tools for RICH Digitisation
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   24/10/2008

__version__ = "$Id: Configuration.py,v 1.2 2008-11-14 16:29:24 cattanem Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from LHCbKernel.Configuration import *
from Configurables import ( Rich__MC__Digi__Signal,
                            Rich__MC__Digi__CopySummedDepositsToDigits,
                            Rich__MC__Digi__SimpleChargeSharing,
                            Rich__MC__Digi__DetailedFrontEndResponse,
                            Rich__MC__Digi__DetailedFrontEndResponsePMT,
                            Rich__MC__Digi__SimpleFrontEndResponse,
                            Rich__MC__Digi__MCRichDigitsToRawBufferAlg,
                            Rich__MC__Digi__SummedDeposits )

# ----------------------------------------------------------------------------------

## @class RichDigiSysConf
#  Configurable for RICH digitisation
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   24/10/2008
class RichDigiSysConf(LHCbConfigurableUser):

    ## Steering options
    __slots__ = {
        "UseSpillover"         : True
       ,"UseLHCBackground"     : False
       ,"ChargeShareFraction"  : 0.025
       ,"ResponseModel"        : "Detailed"
       ,"ChargeShareModel"     : "Simple"
       ,"RawDataFormatVersion" : 129
       ,"Sequencer"            : None
       ,"OutputLevel"          : INFO
        }

    def makeComponent(self, type, name):
        c = type(name)
        if self.isPropertySet("OutputLevel") : c.OutputLevel = self.getProp("OutputLevel")
        return c

    ## @brief Apply the configuration to the given GaudiSequencer
    #  @param sequence The GaudiSequencer to add the RICH digitisation to
    def applyConf(self) :

        sequence = self.getProp("Sequencer")

        # Process the raw hit signals.
        pdSignals = self.makeComponent(Rich__MC__Digi__Signal,"RichPDSignal")
        pdSignals.UseSpillover     = self.getProp("UseSpillover")
        pdSignals.UseLHCBackground = self.getProp("UseLHCBackground")
        sequence.Members += [pdSignals]

        # Charge Sharing
        chargeShareModel = self.getProp("ChargeShareModel")
        if chargeShareModel == "Simple" :
            chargeShare = self.makeComponent(Rich__MC__Digi__SimpleChargeSharing,"RichChargeShare")
            chargeShare.ChargeShareFraction = self.getProp("ChargeShareFraction")
            sequence.Members += [chargeShare]
        elif chargeShareModel == "None" :
            pass
        else :
            raise RuntimeError("ERROR : Unknown HPD charge sharing model '%s'"%chargeShareModel)

        # deposit summation
        sequence.Members += [ self.makeComponent(Rich__MC__Digi__SummedDeposits,"RichSummedDeposits") ]

        # HPD response
        hpdModel = self.getProp("ResponseModel")
        if hpdModel == "Detailed" :
            response = self.makeComponent(Rich__MC__Digi__DetailedFrontEndResponse,"RichPDResponse")
            response.TimeCalib = [ 10, 6 ]
        elif hpdModel == "Simple" :
            response = self.makeComponent(Rich__MC__Digi__SimpleFrontEndResponse,"RichPDResponse")
        elif hpdModel == "Copy" :
            response = self.makeComponent(Rich__MC__Digi__CopySummedDepositsToDigits,"RichPDResponse")
        elif hpdModel == "DetailedPMT" :
            response = self.makeComponent(Rich__MC__Digi__DetailedFrontEndResponsePMT,"RichPDResponse")
        else :
            raise RuntimeError("ERROR : Unknown HPD Response model '%s'"%hpdModel)
        sequence.Members += [response]

        # Fill the Raw Event
        rawEvtFill = self.makeComponent(Rich__MC__Digi__MCRichDigitsToRawBufferAlg,"RichFillRawBuffer")
        rawEvtFill.DataVersion = self.getProp("RawDataFormatVersion")
        from Configurables import Rich__DAQ__RawDataFormatTool
        Rich__DAQ__RawDataFormatTool("RichRawDataFormatTool").RawEventLocations = ["DAQ/RawEvent"]
        sequence.Members += [rawEvtFill]
