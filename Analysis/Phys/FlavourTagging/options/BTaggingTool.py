##############################################################################
# $Id: BTaggingTool.py,v 1.3 2009-04-21 19:20:22 pkoppenb Exp $
#
# Tagging options
#
#
##############################################################################
from GaudiConf.Configuration import *

from Configurables import CombinedParticleMaker, ProtoParticleMUONFilter, ProtoParticleCALOFilter, ProtoParticleRICHFilter

algName = "TaggerParticles"

taggerParticles = CombinedParticleMaker(algName)
taggerParticles.addTool(ProtoParticleMUONFilter("Muon"))
taggerParticles.Muon.Selection = [ "RequiresDet='MUON' CombDLL(mu-pi)>'0.0'" ]
taggerParticles.addTool(ProtoParticleCALOFilter("Electron"))
taggerParticles.Electron.Selection = [ "RequiresDet='CALO' CombDLL(e-pi)>'5.0'" ]
taggerParticles.addTool(ProtoParticleRICHFilter("Kaon"))
taggerParticles.Kaon.Selection = [ "RequiresDet='RICH' CombDLL(k-pi)>'8.0' CombDLL(k-p)>'-4.0'"]

hat = '/Event/Phys/'

dod = DataOnDemandSvc()
dod.AlgMap.update(
    { hat + algName + '/Particles' : 'CombinedParticleMaker/'+algName ,
      hat + algName + '/Vertices'  : 'CombinedParticleMaker/'+algName }
    )


##############################################################################
