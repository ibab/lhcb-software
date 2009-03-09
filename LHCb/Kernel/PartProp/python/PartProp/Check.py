#!/usr/bin/env python
# =============================================================================
# $Id: Check.py,v 1.1 2009-03-09 17:29:39 ibelyaev Exp $ 
# =============================================================================
## @file PartProp/Check.py
#  the trivial example to inspect the particle data table from the data base
#  @author Vanya BELYAEV vanya@nikhef.nl
#  @date  2009-03-06
# =============================================================================
"""
The trivial example to inspect the particle data table from the data base
"""
# =============================================================================
__author__  = "Vanya BELYAEV vanya@nikhef.nl"
__verison__ = "CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1 $ "
# =============================================================================

from Gaudi.Configuration import *

from Configurables import LHCbApp, LHCb__ParticlePropertySvc, ApplicationMgr

import GaudiKernel.SystemOfUnits as Units


## access to DBASE: 
LHCbApp ( DDDBtag   = '' ,   # HEAD
          CondDBtag = '' )   # HEAD

## the service (Configurable)
ppsvc = LHCb__ParticlePropertySvc() 

## let applictaion manager to know about the service:
ApplicationMgr().ExtSvc += [ ppsvc ] 

from GaudiPython.Bindings import AppMgr
import PartProp.Service

# instantiate the actual (C++) application manager
gaudi=AppMgr()

# switch off all algporithms 
gaudi.TopAlg = []

# initialize everything proeprly 
gaudi.run(1) 

# get the actual (C++) particle properties service
ppsvc = gaudi.ppSvc()

# finally: play with the service

# get all a self-conjugated particles 
print ' Self-chargeconjugated particles '
selfcc = ppsvc.get ( lambda x : x.selfcc() )
print selfcc

# set self-chareg conjugated mesons:
print ' CC-neutral mesons'
mesonscc = ppsvc.get ( lambda x : ( x.selfcc() and x.isMeson() ) ) 
print mesonscc

# get "stable" particles ( lifetime in excess of 1 micrometer
print ' STABLE particles (c*tau> 1 um)'
stable = ppsvc.get ( lambda x : ( x.ctau() > Units * micrometer ) ) 
print stable

