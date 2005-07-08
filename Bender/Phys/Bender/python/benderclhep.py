#!/usr/bin/env python
# =============================================================================
# $Id: benderclhep.py,v 1.2 2005-07-08 21:00:05 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.2 $
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
HepGeom    = PyLCGDict.makeNamespace('HepGeom')
CLHEP      = PyLCGDict.makeNamespace('CLHEP'  )

# get classes 
Point3D    = HepGeom.Point3D('double')
HepPoint3D = Point3D 


HepVector      = CLHEP.HepVector
HepMatrix      = CLHEP.HepMatrix
HepSymMatrix   = CLHEP.HepSymMatrix
HepDiagMatrix  = CLHEP.HepDiagMatrix

Vector         =       HepVector
Matrix         =       HepMatrix
SymMatrix      =       HepSymMatrix
DiagMatrix     =       HepDiagMatrix

# make matrix operators '()'
def _make_round_ ( klass ) :
    if not hasattr ( klass , '()'       ) : return False
    if     hasattr ( klass , '__call__' ) : return False
    setattr ( klass , '__call__' , getattr ( klass , '()' ) )
    return True

_types = [ CLHEP.HepVector     ,
           CLHEP.HepMatrix     ,
           CLHEP.HepSymMatrix  ,
           CLHEP.HepDiagMatrix ]

for _type in _types :
    _make_round_ ( _type )
    
# make matrix operators []
def _make_square_ ( klass ) :
    if not hasattr ( klass , '[]'          ) : return False
    if     hasattr ( klass , '__getitem__' ) : return False 
    setattr ( klass , '__getitem__' , getattr ( klass , '[]' ) )
    return True

# for access to rows, the temporary ojects need to be created first :-((
def _make_objects_ () :
    a1 = CLHEP.HepMatrix(2,2)
    getattr( a1 , '[]' )(1)
    a1 = CLHEP.HepSymMatrix(2,1)
    getattr( a1 , '[]' )(1)
    a1 = CLHEP.HepDiagMatrix(2,1)
    getattr( a1 , '[]' )(1)
    
_make_objects_()

_types += [ CLHEP.HepMatrix.HepMatrix_row         ,
            CLHEP.HepSymMatrix.HepSymMatrix_row   ,
            CLHEP.HepDiagMatrix.HepDiagMatrix_row ]

for _type in _types :
    _make_square_ ( _type )
