from Gaudi.Configuration import *

ApplicationMgr().ExtSvc + [ "AuditorSvc" ]
AuditorSvc().Auditors += [ "FPEAuditor" ]
from Configurables import FPEAuditor
#FPEAuditor().TrapOn = [ "DivByZero", "Overflow", "Underflow" ]
FPEAuditor().ActivateAt = ["Execute"]
