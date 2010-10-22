#!/usr/bin/env python
# =============================================================================
# $Id: PartPropSvc.py,v 1.3 2008-12-03 17:35:54 ibelyaev Exp $ 
# =============================================================================
## @file PartProp/PartPropSvc.py
#  Demo-file for interactive work with new Particle Property Service
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-10-22
# =============================================================================
"""

Trivial script to dump the table of Particle Properties 

Last modification $Date$
               by $Author$
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "version $Revision: 1.3 $" 
# =============================================================================
import PartProp.PartPropAlg
import PartProp.Service
from   GaudiPython.Bindings import AppMgr
# =============================================================================

gaudi = AppMgr()

gaudi.initialize()

pps   = gaudi.ppSvc()

print pps.all()


# =============================================================================
# The END 
# =============================================================================
