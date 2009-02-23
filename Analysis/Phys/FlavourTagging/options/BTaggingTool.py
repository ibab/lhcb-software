##############################################################################
# $Id: BTaggingTool.py,v 1.2 2009-02-23 02:16:50 musy Exp $
#
# Tagging options
#
#
##############################################################################
from GaudiConf.Configuration import *


from Configurables import BTaggingTool, PhysDesktop, CombinedParticleMaker, ProtoParticleMUONFilter, ProtoParticleCALOFilter, ProtoParticleRICHFilter

tagtool = BTaggingTool("BTaggingTool")
pd = PhysDesktop()

pd.ParticleMakerType = "CombinedParticleMaker"
pd.addTool( CombinedParticleMaker() )
pd.CombinedParticleMaker.ExclusiveSelection = False;
pd.CombinedParticleMaker.addTool(ProtoParticleMUONFilter("Muon"))
pd.CombinedParticleMaker.Muon.Selection = [ "RequiresDet='MUON' CombDLL(mu-pi)>'0.0'" ]
pd.CombinedParticleMaker.addTool(ProtoParticleCALOFilter("Electron"))
pd.CombinedParticleMaker.Electron.Selection = [ "RequiresDet='CALO' CombDLL(e-pi)>'5.0'" ]
pd.CombinedParticleMaker.addTool(ProtoParticleRICHFilter("Kaon"))
pd.CombinedParticleMaker.Kaon.Selection = [ "RequiresDet='RICH' CombDLL(k-pi)>'8.0' CombDLL(k-p)>'-4.0'"]

tagtool.addTool( pd )

##############################################################################
