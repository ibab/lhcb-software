#!/usr/bin/env python
# =============================================================================
# $Id: benderstr.py,v 1.3 2004-08-26 19:34:36 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================
# $Log: not supported by cvs2svn $
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


import gaudimodule as     gaudi
from   benderaux   import fillStream

# load modules
gaudi.loaddict ( 'EventDict'     )
gaudi.loaddict ( 'PhysEventDict' )
gaudi.loaddict ( 'TrEventDict'   )
gaudi.loaddict ( 'OTEventDict'   )
gaudi.loaddict ( 'ITEventDict'   )
gaudi.loaddict ( 'VeloEventDict' )
gaudi.loaddict ( 'RichEventDict' )
gaudi.loaddict ( 'MuonEventDict' )
gaudi.loaddict ( 'CaloEventDict' )

def _fillStream_ ( self ) :
    _obj = self 
    return fillStream ( _obj )

gbl = gaudi.gbl
types = [ gbl.MCParticle    ,
          gbl.MCVertex      ,
          gbl.SelResult     ,
          gbl.Particle      ,
          gbl.Vertex        ,
          gbl.ProtoParticle ,
          gbl.TrStoredTrack ,
          gbl.RichPID       ,
          gbl.MuonID        ,
          gbl.CaloParticle  ,
          gbl.CaloHypo      ,
          gbl.CaloCluster   ,
          gbl.CaloDigit     ]

for t in types :
    t.__repr__ = _fillStream_
    
# =============================================================================
# The END 
# =============================================================================
