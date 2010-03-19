from Gaudi.Configuration import *
from Configurables import ( TAlignment, AlignTrTools )
AlignTrTools().Degrees_of_Freedom[0] = 1
TAlignment().Constraints += [
"X_S1_L1_Q0_M0",
"X_S1_L2_Q0_M0",
"X_S3_L1_Q0_M0",
"X_S3_L2_Q0_M0"
]
