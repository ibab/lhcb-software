#!/usr/bin/env python
# =============================================================================
# $Id: CheckOverlap.py,v 1.1 2009-11-15 16:46:00 ibelyaev Exp $
# =============================================================================
## @file DetDescChecks/CheckOverlap.py
#  Helper file to illustrate the usage of Check-Overlap algorithm
#  @see DetDesc::CheckOverlap 
#  @author Vanya BELYAEV Icvan.Belyaev@nikhef.nl
# =============================================================================
"""
Helper file to illustrate the usage of Check-Overlap algorithm
"""
# =============================================================================
__author__  = 'Vanya BELYAEV Icvan.Belyaev@nikhef.nl'
__version__ = 'CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $'
# =============================================================================

from Gaudi.Configuration import *

from Configurables  import DDDBConf
DDDBConf ( DataType = '2015' )

from Configurables import DetDesc__CheckOverlap

check = DetDesc__CheckOverlap (
    'Overlap' ,
    #
    #Shots = 1000 ,
    Shots = 50 ,
    #
    Volume = "/dd/Geometry/LHCb/lvLHCb" 
    #Volume = "/dd/Geometry/MagnetRegion/Magnet/Magnet"
    )

from Configurables import TransportSvc

TransportSvc (
    Recovery = True 
    )

from Configurables import ApplicationMgr

ApplicationMgr  (
    TopAlg = [ check ] ,
    EvtSel = 'NONE'    , 
    EvtMax = 1 
    )

# =============================================================================
if "__main__" == __name__  :
    
    print __doc__
    print __author__
    print __version__
    
    from GaudiPython.Bindings import AppMgr
    
    gaudi = AppMgr()
    
    gaudi.run(1) 
    
# =============================================================================
# The END 
# =============================================================================

