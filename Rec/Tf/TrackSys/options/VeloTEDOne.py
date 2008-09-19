# This file is for reconstructing the VELO TED data between August and mid September 2008
# load the configurables I'll need
from Gaudi.Configuration import *
from Configurables import ( UpdateManagerSvc,
                            DecodeVeloRawBuffer,
                            Tf__PatVeloGeneric,
                            Tf__PatVeloGeneralTracking)

# Setup TELL1->Sensor list for August->Mid-Sept VELO data
UpdateManagerSvc().ConditionsOverride += ["Conditions/ReadoutConf/Velo/TELL1ToSensors := int_v SensorID = 128 129 130 131  0  64  1  65 66   2  67   3  4  68  5  69  70   6  71   7  8  72  9  73  74  10  75  11 12  76 13  77  78  14  79  15 16  80 17  81  82  18  83  1 9 20  84 21  85  86  22  87  23 24  88 25  89  90  26  91  27 28  92 29  93  94  30  95  31 32  96 33  97  98  34  99  35 36 100 37 101 102  38 103  39 40 104 41 105; int_v  Tell1Id  =  21  22  23  26  49  235  147  186  173  217 400 200 247 61 196 204 228  216 146 180 263 240 151 132 131 252 142 145 69 158 199 149 234 236 202 201 205 30 143 198 220 218 214 177 192 203 232 176 225 253 183 197 264  118 161 166 231 175 185 152 242 50 194 184 244 62 29 170 207 120 195 181 174 245 54 188 121 128 190 249 162 229 261 241 51 122 259 222 ;"]

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
