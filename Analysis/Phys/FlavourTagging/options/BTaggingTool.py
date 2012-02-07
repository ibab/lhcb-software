##############################################################################
#
# Tagging options
#
#
##############################################################################
from Configurables import ( CombinedParticleMaker, ProtoParticleMUONFilter,
                            ProtoParticleCALOFilter, ProtoParticleRICHFilter,
                            FilterDesktop )

mName = "TaggingMuons"
eName = "TaggingElectrons"
pName = "TaggingPions"
tagName = 'TaggingParticles'

hat = 'Phys/'
leaf = '/Particles'

mLoc, eLoc, pLoc, tagLoc = [ hat+name+leaf for name in mName, eName, pName, tagName]

taggerMuons = CombinedParticleMaker(mName)
taggerMuons.Particle = 'Muon'
taggerMuons.addTool(ProtoParticleMUONFilter, name="Muon")
taggerMuons.Muon.Selection = [ "RequiresDet='MUON' CombDLL(mu-pi)>'0.0'" ]

taggerElectrons = CombinedParticleMaker(eName)
taggerElectrons.Particle = 'Electron'
taggerElectrons.addTool(ProtoParticleCALOFilter, name="Electron")
taggerElectrons.Electron.Selection = [ "RequiresDet='CALO' CombDLL(e-pi)>'4.0'" ]

taggerPions = CombinedParticleMaker(pName)
taggerPions.Particle = 'Pion'


taggingParticles = FilterDesktop(tagName)
taggingParticles.Inputs = [ eLoc, mLoc, pLoc ]
taggingParticles.Code = "(P>2.0*GeV)"    



dod = DataOnDemandSvc()
dod.AlgMap.update( { mLoc   : 'CombinedParticleMaker/'+mName } )
dod.AlgMap.update( { eLoc   : 'CombinedParticleMaker/'+eName } )
dod.AlgMap.update( { pLoc   : 'CombinedParticleMaker/'+pName } )
dod.AlgMap.update( { tagLoc : 'FilterDesktop/'+tagName       } )

##############################################################################
