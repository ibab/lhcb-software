"""
Options to run simple ChargedCombineParticleMaker configuraiton test.
Different configuraiton parameter combinaitons should give
cetrain Output, Input values.

Author Juan Palacios palacios@phisik.uzh.ch

"""
from Configurables import ( ApplicationMgr,
                            ChargedProtoParticleMaker,
                            MessageSvc)

#MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

# This should work fine
# Inputs = [Rec/Track/Special0]
# Output = Rec/ChargedPP/ProtoParticles0
cp0 = ChargedProtoParticleMaker('CPP0',
                                Inputs=['Rec/Track/Special0'],
                                Output='Rec/ChargedPP/ProtoParticles0',
                                OutputLevel=1)
ApplicationMgr().TopAlg +=[cp0]

# This should work fine
# Inputs = [Rec/Track/Special1]
# Output = Rec/ChargedPP/ProtoParticles1
cp1 = ChargedProtoParticleMaker('CPP1',
                                Inputs=['Rec/Track/Special1'],
                                Output='Rec/ChargedPP/ProtoParticles1',
                                Context='HLT',
                                OutputLevel=1)
ApplicationMgr().TopAlg +=[cp1]
