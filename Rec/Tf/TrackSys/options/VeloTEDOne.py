# This file is for reconstructing the VELO TED data between August and mid September 2008
# load the configurables I'll need
from Gaudi.Configuration import *
from Configurables import ( UpdateManagerSvc,
                            DecodeVeloRawBuffer,
                            Tf__PatVeloGeneric,
                            Tf__PatVeloGeneralTracking)

# Fully open the detector
UpdateManagerSvc().ConditionsOverride +=  ["Conditions/Online/Velo/MotionSystem := double ResolPosRC =-29. ; double ResolPosLA = 29.;"]

# delete and reset the RecVELOSeq to decode (to lite and full), Generic then General PR
GaudiSequencer("RecoVELOSeq").Members =[
    DecodeVeloRawBuffer(),
    Tf__PatVeloGeneric("PatVeloGeneric"),
    Tf__PatVeloGeneralTracking("PatVeloGeneralTracking")]

# Save having the algorithm run twice
DecodeVeloRawBuffer("DecodeVeloRawBuffer").DecodeToVeloClusters     = True
# Fix for bug in TELL1 scripts (fixed mid Sept)
DecodeVeloRawBuffer('DecodeVeloRawBuffer').ForceBankVersion=3
# Move to SPD timed in BEAM2 "main" BX bin
DecodeVeloRawBuffer('DecodeVeloRawBuffer').RawEventLocation='Prev2/DAQ/RawEvent'
