from Gaudi.Configuration import *
from Configurables import Escher

Escher().DoF[0] = 1
Escher().Constraints += [
"X_S1_L1_Q0_M0",
"X_S1_L2_Q0_M0",
"X_S3_L3_Q0_M0",
"X_S3_L4_Q0_M0"
]
