#!/usr/bin/env python
# =============================================================================
# $Id: VolumeCheck.py,v 1.1 2009-11-15 16:46:00 ibelyaev Exp $
# =============================================================================
## @file DetDescChecks/CheckOverlap.py
#  Helper file to illustrate the usage of Volume-Check algorithm
#  @see DetDesc::CheckOverlap 
#  @author Vanya BELYAEV Icvan.Belyaev@nikhef.nl
# =============================================================================
"""
Helper file to illustrate the usage of Volume-Check algorithm
"""
# =============================================================================
__author__  = 'Vanya BELYAEV Icvan.Belyaev@nikhef.nl'
__version__ = 'CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $'
# =============================================================================

from Gaudi.Configuration import *

from Configurables  import DDDBConf
DDDBConf ( DataType = '2015' )

from Configurables import DetDesc__VolumeCheck

check = DetDesc__VolumeCheck (
    'Check' ,
    #
    Shots3D = 50000 ,
    ShotsXY = 50000 ,
    ShotsYZ = 50000 ,
    ShotsZX = 50000 , 
    #
    Volume   = '/dd/Geometry/MagnetRegion/Magnet/Magnet' 
    )

from Configurables import ApplicationMgr

ApplicationMgr  (
    TopAlg = [ check ] ,
    EvtSel = "NONE"    ,
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

