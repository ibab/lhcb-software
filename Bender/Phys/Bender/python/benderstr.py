#!/usr/bin/env python
# =============================================================================
# $Id: benderstr.py,v 1.4 2004-11-08 17:06:52 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.3  2004/08/26 19:34:36  ibelyaev
#  remove explict PyLCGDict
#
# Revision 1.2  2004/08/06 12:07:09  ibelyaev
#  minor improvements
#
# Revision 1.1  2004/07/24 15:03:33  ibelyaev
#  add benderstr
#
# =============================================================================


# =============================================================================
# @file
# Helper file to define the __str__ for soem "basic" classes 
# @date   2004-07-11
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# =============================================================================


import gaudimodule 
from   benderaux   import fillStream

# load modules
gaudimodule.loaddict ( 'EventDict'     )
gaudimodule.loaddict ( 'PhysEventDict' )
gaudimodule.loaddict ( 'TrEventDict'   )
gaudimodule.loaddict ( 'OTEventDict'   )
gaudimodule.loaddict ( 'ITEventDict'   )
gaudimodule.loaddict ( 'VeloEventDict' )
gaudimodule.loaddict ( 'RichEventDict' )
gaudimodule.loaddict ( 'MuonEventDict' )
gaudimodule.loaddict ( 'CaloEventDict' )
gaudimodule.loaddict ( 'CaloEventDict' )

def _fillStream_ ( self ) :
    _obj = self 
    return fillStream ( _obj )

_gbl = gaudimodule.gbl
_types = [ _gbl.MCParticle    ,
           _gbl.MCVertex      ,
           _gbl.Collision     ,
           _gbl.SelResult     ,
           _gbl.Particle      ,
           _gbl.Vertex        ,
           _gbl.ProtoParticle ,
           _gbl.TrStoredTrack ,
           _gbl.RichPID       ,
           _gbl.MuonID        ,
           _gbl.CaloParticle  ,
           _gbl.CaloHypo      ,
           _gbl.CaloCluster   ,
           _gbl.CaloDigit     ]

for t in _types :
    t.__repr__ = _fillStream_
    
# =============================================================================
# The END 
# =============================================================================
