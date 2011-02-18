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

# This should give warning
# Inputs = [Rec/Track/Best]
# Output = Rec/ChargedPP/ProtoParticles1
cp1 = ChargedProtoParticleMaker('CPP1',
                                OutputProtoParticleLocation='Rec/ChargedPP/ProtoParticles1',
                                OutputLevel=1)
ApplicationMgr().TopAlg +=[cp1]

# This should give warning
# Inputs = [Rec/Track/Special2]
# Output = Rec/ProtoP/Charged
cp2 = ChargedProtoParticleMaker('CPP2',
                                InputTrackLocation=['Rec/Track/Special2'],
                                OutputLevel=1)
ApplicationMgr().TopAlg +=[cp2]

# This should work fine
# Inputs = [Rec/Track/Special3]
# Output = Rec/ChargedPP/ProtoParticles3
cp3 = ChargedProtoParticleMaker('CPP3',
                                Inputs=['Rec/Track/Special3'],
                                Output='Rec/ChargedPP/ProtoParticles3',
                                Context='HLT',
                                OutputLevel=1)
ApplicationMgr().TopAlg +=[cp3]

# This should give warning
# Inputs = [Rec/Track/Special4]
# Output = 
cp4 = ChargedProtoParticleMaker('CPP4',
                                InputTrackLocation=['Rec/Track/Special4'],
                                Context='HLT',
                                OutputLevel=1)
ApplicationMgr().TopAlg +=[cp4]

# This should give warning
# Inputs = []
# Output = Rec/ChargedPP/ProtoParticles1
cp5 = ChargedProtoParticleMaker('CPP5',
                                OutputProtoParticleLocation='Rec/ChargedPP/ProtoParticles1',
                                Context='HLT',
                                OutputLevel=1)
ApplicationMgr().TopAlg +=[cp5]
