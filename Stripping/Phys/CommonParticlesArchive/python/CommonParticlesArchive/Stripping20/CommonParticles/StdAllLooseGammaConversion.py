#!/usr/bin/env python
# =============================================================================
# $Id: StdNoPIDsDownElectrons.py,v 1.1 2010-08-11 12:43:05 ukerzel Exp $ 
# =============================================================================
## @file  CommonParticles/StdNoPIDsElectrons.py
#  configuration file for 'Standard NoPIDs Pions' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard DiElectrons from gamma conversion'
"""
__author__  = "Olivier Deschamps : Olivier.Deschamps @cern.ch"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'StdAllLooseGammaLL' ,
    'StdAllLooseGammaDD' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration   import *
from CommonParticles.Utils import *
from GaudiKernel.SystemOfUnits import *


###--- Long pair
from Configurables import DiElectronMaker,ProtoParticleCALOFilter
dieLL = DiElectronMaker('StdAllLooseGammaLL')
dieLL.DecayDescriptor = "gamma -> e+ e-"
selector = trackSelector ( dieLL , trackTypes = [ "Long"]) 
dieLL.addTool( ProtoParticleCALOFilter('Electron'))
dieLL.Electron.Selection = ["RequiresDet='CALO' CombDLL(e-pi)>'-2.0'"]
dieLL.DeltaY = 3.
dieLL.DeltaYmax = 200 * mm
dieLL.DiElectronMassMax = 100.*MeV
dieLL.DiElectronPtMin = 200.*MeV
locations = updateDoD ( dieLL )
StdAllLooseGammaLL=dieLL

###--- Downstream pair
dieDD = DiElectronMaker('StdAllLooseGammaDD')
dieDD.DecayDescriptor = "gamma -> e+ e-"
selector = trackSelector ( dieDD , trackTypes = [ "Downstream"]) 
dieDD.addTool( ProtoParticleCALOFilter('Electron'))
dieDD.Electron.Selection = ["RequiresDet='CALO' CombDLL(e-pi)>'-2.0'"]
dieDD.DeltaY = 3.
dieDD.DeltaYmax = 200 * mm
dieDD.DiElectronMassMax = 100.*MeV
dieDD.DiElectronPtMin = 200.*MeV
locations.update( updateDoD ( dieDD ))
StdAllLooseGammaDD=dieDD



## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
