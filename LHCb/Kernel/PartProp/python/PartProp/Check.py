#!/usr/bin/env python
# =============================================================================
# $Id: Check.py,v 1.2 2009-07-18 13:01:49 ibelyaev Exp $ 
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
__verison__ = "CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.2 $ "
# =============================================================================

from Gaudi.Configuration import *
from Configurables       import LHCbApp, LHCb__ParticlePropertySvc, ApplicationMgr


## access to DBASE: 
LHCbApp ( DDDBtag   = '' ,   # HEAD
          CondDBtag = '' )   # HEAD

## let application manager to know about the service:
ApplicationMgr().ExtSvc    += [ LHCb__ParticlePropertySvc() ]


## switch to GaudiPython
from   GaudiPython.Bindings      import AppMgr
from   GaudiKernel.SystemOfUnits import micrometer 
import PartProp.Service

# instantiate the actual (C++) application manager
gaudi=AppMgr()

# switch off all algorithms 
gaudi.TopAlg = []

# initialize everything properly
gaudi.run(1) 

# get the actual (C++) particle properties service
ppsvc = gaudi.ppSvc()

# finally: play with the service

# get all a self-conjugated particles 
print ' Self-charge conjugated particles '
selfcc = ppsvc.get ( lambda x : x.selfcc() )
print selfcc

# set self-charge conjugated conjugated mesons:
print ' CC-neutral mesons'
mesonscc = ppsvc.get ( lambda x : ( x.selfcc() and x.isMeson() ) ) 
print mesonscc

# set self-charge conjugated conjugated mesons:
print ' CC-neutral beauty mesons'
mesonscc = ppsvc.get ( lambda x : ( x.isMeson() and x.hasBottom() ) ) 
print mesonscc

# get "stable" particles ( lifetime in excess of 1 micrometer
print ' STABLE particles (c*tau> 1 um)'
stable = ppsvc.get ( lambda x : ( x.ctau() > micrometer ) ) 
print stable


