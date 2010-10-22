#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id$
# =============================================================================
## @file
#  The configurationi file to run QMTest for the package Kernel/PartProp
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date   2008-12-02
# =============================================================================
"""
The configurationi file to run QMTest for the package Kernel/PartProp
"""
# =============================================================================
__author__  =  "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ =  "CVS tag $Name: not supported by cvs2svn $, version $Revision$"
# =============================================================================

from Gaudi.Configuration import *
from Configurables import ApplicationMgr 
from Configurables import LHCb__ParticlePropertySvc  as PPS 
from Configurables import LHCb__Example__PartPropAlg as PPA 

## needed for the proper access to Particle table  from DDDB  
from Configurables import DDDBConf 
DDDBConf       ()  # use the default Configuration of DDDB



ApplicationMgr ( TopAlg = [ PPA() ] ,
                 EvtSel = "NONE"    ,
                 EvtMax = 1         )

# theApp.ExtSvc += [
#     PPS ( ParticlePropertiesFile =
#          "$PARAM_HOME/ParamFiles/v7r3/data/ParticleTable.txt"
#          ) ]


# =============================================================================
# The END 
# =============================================================================
