## $Id: StandardOptions.py,v 1.1 2008-12-23 14:28:45 pkoppenb Exp $
## ============================================================================
## CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1 $
## ============================================================================

### @file 
 #
 #  The configuration file for "standard" particles, created "on-demand"
 # 
 #  The configuration includes:
 #  
 #  <b>"No-PID" - particles</b>
 #   These sets of particles are useful for loose stripping and 
 #   PID-studies. NoPIDsParticleMaker is used for creation:
 #     - "/Event/Phys/StdNoPIDsMuons"     
 #     - "/Event/Phys/StdNoPIDsKaons"     
 #     - "/Event/Phys/StdNoPIDsPions"     
 #     - "/Event/Phys/StdNoPIDsProtons"   
 #     - "/Event/Phys/StdNoPIDsElectrons"
 #
 #   Note that the NoPIDsParticlreMaker uses VTT, Long and 
 #   Downstream tracks by default
 #
 #   @see NoPIDsParticleMaker
 # 
 #   <b>"Loose PID" particles</b>
 #  CombinedParticlMaker (with "ExclusiveSelection=false") is used for 
 #  charged particles with the definitions:
 #    - Muon     : det='MUON' DLL(mu/pi) > -15
 #    - Electron : det='CALO' DLL(e/pi)  > -5 
 #    - Kaon     : det='RICH' DLL(K/pi)  > -5 
 #    - Proton   : det='RICH' DLL(p/pi)  > -5 
 #    - Pion     : No requirements
 #    - VTT Pions: Made of VTT tracks
 #    - Downstream Pions: Made of Downstream tracks
 # 
 #  Containers: 
 #    - "/Event/Phys/StdLooseMuons"
 #    - "/Event/Phys/StdLooseElectrons"
 #    - "/Event/Phys/StdLooseLeptons"
 #    - "/Event/Phys/StdLooseKaons"
 #    - "/Event/Phys/StdLoosePions"
 #    - "/Event/Phys/StdLooseVTTPions"
 #
 #  @see CombinedParticleMaker
 # 
 #   <b>"Tight PID" particles</b>
 #  CombinedParticlMaker (with "ExclusiveSelection=false") is used for 
 #  charged particles with the definitions:
 #    - Muon     : det='MUON' DLL(mu/pi) > -8
 #    - Electron : det='CALO' DLL(e/pi)  > 0
 #    - Kaon     : det='RICH' DLL(K/pi)  > 0
 #    - Proton   : det='RICH' DLL(p/pi)  > 0
 #    - Pion     : det='RICH' DLL(pi/K)  > -5
 # 
 #  Containers: 
 #    - "/Event/Phys/StdTightMuons"
 #    - "/Event/Phys/StdTightElectrons"
 #    - "/Event/Phys/StdTightLeptons"
 #    - "/Event/Phys/StdTightKaons"
 #    - "/Event/Phys/StdTightPions"
 #  
 #  <b>Photons</b>
 #  (Cnv)PhotonParticlemaker is used.
 #    - non-converted photons: "/Event/Phys/StdLoosePhotons"
 #    - converted photons:     "/Event/Phys/StdLooseCnvPhotons"
 #    - all photons:           "/Event/Phys/StdLooseAllPhotons"
 #
 #  @see PhotonParticleMaker
 #  @see CnvPhotonParticleMaker
 #
 #  The configuration file for "standard" particles, created "on-demand"
 # 
 #  There are a set of standard basic particle, standard Pi0s and a set
 #  of standard intermediate states: D0, Ds, D*, J/psi, K*, Phi.
 #  
 #  The relevant algorithms are fired on demand when one wants to read
 #  their outputlocation. See the relevant options for a complete list.
 #
 #  The cuts coded in this version are 'just a little tighter' than the 
 #  ones applied in the exclusive HLT (with the big exception that PID is used
 #  here but not in the HLT). So hopefully events selected using these particles 
 #  should also pass HLT. This has not been tested.
 #
 #  The actual definition of the cuts should be defined by the physics WG
 #  for the DC06 stripping. The present first import should be taken as an example
 #  more definitions of 'Tight' 'Looser' or 'NoPID' intermediate states could
 #  be provided if needed.
 #
 #  @see StandardD0.opts : The standard D0 
 #  @see StandardDs.opts : The standard Ds
 #  @see StandardDplus.opts : The standard Dplus
 #  @see StandardDstar.opts : The standard D*
 #  @see StandardJpsi.opts : The standard J/psi -> MuMu
 #  @see StandardKstar.opts : The standard K*
 #  @see StandardPhi.opts : The standard Phi
 #  @see StandardRho0.opts : The standard Rho0
 #  @see StandardKs.opts : The standard Ks
 #  @see StandardLambda.opts : The standard Lambda
 #  @see StandardMergedPi0s.opts : The standard marged Pi0
 #  @see StandardResolvedPi0s.opts : The standard resolved Pi0
 #
 #  @author P. Koppenburg
 #  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 #  @date 2006-01-30
###
from Gaudi.Configuration import *
 #
 # @todo Translate them all to python
 #
importOptions("$COMMONPARTICLESROOT/options/StandardElectrons.opts")
importOptions("$COMMONPARTICLESROOT/options/StandardMuons.opts")
importOptions("$COMMONPARTICLESROOT/options/StandardProtons.opts")
importOptions("$COMMONPARTICLESROOT/options/StandardPions.opts")
importOptions("$COMMONPARTICLESROOT/options/StandardKaons.opts")
importOptions("$COMMONPARTICLESROOT/options/StandardPhotons.opts")


importOptions("$COMMONPARTICLESROOT/options/StandardD0.opts")
importOptions("$COMMONPARTICLESROOT/options/StandardDplus.opts")
importOptions("$COMMONPARTICLESROOT/options/StandardDs.opts")
importOptions("$COMMONPARTICLESROOT/options/StandardDstar.opts")
importOptions("$COMMONPARTICLESROOT/options/StandardJpsi.opts")
importOptions("$COMMONPARTICLESROOT/options/StandardKstar.opts")
importOptions("$COMMONPARTICLESROOT/options/StandardPhi.opts")
importOptions("$COMMONPARTICLESROOT/options/StandardRho0.opts")
importOptions("$COMMONPARTICLESROOT/options/StandardKs.opts")
importOptions("$COMMONPARTICLESROOT/options/StandardLambda.opts")
importOptions("$COMMONPARTICLESROOT/options/StandardMergedPi0s.opts")
importOptions("$COMMONPARTICLESROOT/options/StandardResolvedPi0s.opts")
