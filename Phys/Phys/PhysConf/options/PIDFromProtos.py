### $ ID: $
#
# Make PID classes from protoparticles
#
##
from Gaudi.Configuration import *
DataOnDemandSvc().AlgMap["Rec/Rich/PIDs"] = "RichPIDsFromProtoParticlesAlg/CreateRichPIDs"
DataOnDemandSvc().AlgMap["Rec/Muon/MuonPID"] = "MuonPIDsFromProtoParticlesAlg/CreateMuonPIDs"

