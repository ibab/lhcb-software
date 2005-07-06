#!/usr/bin/env python
# =============================================================================
# $Id: benderclhep.py,v 1.1 2005-07-06 18:28:02 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.1 $
# =============================================================================
"""
Trivial module to make a some adaptation for CLHEP 'Point'-like classes
"""
# =============================================================================
__author__ = 'Vanya BELYAEV'
# =============================================================================

import gaudimodule, PyLCGDict

# load dictionary
gaudimodule.loaddict('SealCLHEPDict')

# namespace 
_HepGeom   = PyLCGDict.makeNamespace('HepGeom')

# get classes 
Point3D    = _HepGeom.Point3D('double')
HepPoint3D = Point3D 
