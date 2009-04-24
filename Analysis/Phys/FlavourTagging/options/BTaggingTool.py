##############################################################################
# $Id: BTaggingTool.py,v 1.4 2009-04-24 09:45:13 pkoppenb Exp $
#
# Tagging options
#
#
##############################################################################
from GaudiConf.Configuration import *

from Configurables import CombinedParticleMaker, ProtoParticleMUONFilter, ProtoParticleCALOFilter, ProtoParticleRICHFilter, FilterDesktop

kName = "TaggingKaons"
mName = "TaggingMuons"
eName = "TaggingElectrons"

taggerKaons = CombinedParticleMaker(kName)
taggerKaons.Particle = 'Kaon'
taggerKaons.addTool(ProtoParticleRICHFilter("Kaon"))
taggerKaons.Kaon.Selection = [ "RequiresDet='RICH' CombDLL(k-pi)>'8.0' CombDLL(k-p)>'-4.0'"]

taggerMuons = CombinedParticleMaker(mName)
taggerMuons.Particle = 'Muon'
taggerMuons.addTool(ProtoParticleMUONFilter("Muon"))
taggerMuons.Muon.Selection = [ "RequiresDet='MUON' CombDLL(mu-pi)>'0.0'" ]

taggerElectrons = CombinedParticleMaker(eName)
taggerElectrons.Particle = 'Electron'
taggerElectrons.addTool(ProtoParticleCALOFilter("Electron"))
taggerElectrons.Electron.Selection = [ "RequiresDet='CALO' CombDLL(e-pi)>'5.0'" ]

tagName = 'TaggingParticles'
taggingParticles = FilterDesktop(tagName)
taggingParticles.InputLocations = [ kName, eName, mName ]
taggingParticles.Code = "(P>2.0*GeV)"    ## @todo Add more cuts here

hat = '/Event/Phys/'

dod = DataOnDemandSvc()
dod.AlgMap.update( { hat + kName + '/Particles' : 'CombinedParticleMaker/'+kName ,
                     hat + kName + '/Vertices'  : 'CombinedParticleMaker/'+kName } )
dod.AlgMap.update( { hat + mName + '/Particles' : 'CombinedParticleMaker/'+mName ,
                     hat + mName + '/Vertices'  : 'CombinedParticleMaker/'+mName } )
dod.AlgMap.update( { hat + eName + '/Particles' : 'CombinedParticleMaker/'+eName ,
                     hat + eName + '/Vertices'  : 'CombinedParticleMaker/'+eName } )
dod.AlgMap.update( { hat + tagName + '/Particles' : 'FilterDesktop/'+tagName ,
                     hat + tagName + '/Vertices'  : 'FilterDesktop/'+tagName } )


##############################################################################
