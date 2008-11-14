
## @package RichDigiSys
#  High level Configuration tools for RICH Digitisation
#  @author Chris Jones  (Christopher.Rob.Jones@cern.ch)
#  @date   24/10/2008

__version__ = "$Id: Configuration.py,v 1.2 2008-11-14 16:29:24 cattanem Exp $"
__author__  = "Chris Jones <Christopher.Rob.Jones@cern.ch>"

from LHCbKernel.Configuration import *
from Configurables import ( Rich__MC__Digi__Signal,
                            Rich__MC__Digi__SimpleChargeSharing,
                            Rich__MC__Digi__DetailedFrontEndResponse,
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
       ,"HpdResponseModel"     : "Detailed"
       ,"ChargeShareModel"     : "Simple"
       ,"RawDataFormatVersion" : 129
        }

    ## @brief Apply the configuration to the given GaudiSequencer
    #  @param sequence The GaudiSequencer to add the RICH digitisation to
    def applyConf(self,sequence) :

        # Process the raw hit signals.
        pdSignals = Rich__MC__Digi__Signal("RichPDSignal")
        pdSignals.UseSpillover     = self.getProp("UseSpillover")
        pdSignals.UseLHCBackground = self.getProp("UseLHCBackground")
        sequence.Members += [pdSignals]

        # Charge Sharing
        chargeShareModel = self.getProp("ChargeShareModel")
        if chargeShareModel == "Simple" :
            chargeShare = Rich__MC__Digi__SimpleChargeSharing("RichChargeShare")
            chargeShare.ChargeShareFraction = self.getProp("ChargeShareFraction")
        else :
            raise RuntimeError("ERROR : Unknown HPD charge sharing model '%s'"%chargeShareModel)
        sequence.Members += [chargeShare]

        # deposit summation
        sequence.Members += [ Rich__MC__Digi__SummedDeposits("RichSummedDeposits") ]

        # HPD response
        hpdModel = self.getProp("HpdResponseModel")
        if hpdModel == "Detailed" :
            response = Rich__MC__Digi__DetailedFrontEndResponse("RichPDResponse")
            response.TimeCalib = [ 10, 6 ]
        elif hpdModel == "Simple" :
            response = Rich__MC__Digi__SimpleFrontEndResponse("RichPDResponse")
        else :
            raise RuntimeError("ERROR : Unknown HPD Response model '%s'"%hpdModel)
        sequence.Members += [response]

        # Fill the Raw Event
        rawEvtFill = Rich__MC__Digi__MCRichDigitsToRawBufferAlg("RichFillRawBuffer")
        rawEvtFill.DataVersion = self.getProp("RawDataFormatVersion")
        sequence.Members += [rawEvtFill]

