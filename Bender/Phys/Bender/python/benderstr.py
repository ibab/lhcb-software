#!/usr/bin/env python
# =============================================================================
# $Id: benderstr.py,v 1.1 2004-07-24 15:03:33 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================
# $Log: not supported by cvs2svn $
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
g = gaudi.AppMgr()
g.loaddict ( 'EventDict'     )
g.loaddict ( 'PhysEventDict' )
g.loaddict ( 'TrEventDict'   )
g.loaddict ( 'OTEventDict'   )
g.loaddict ( 'ITEventDict'   )
g.loaddict ( 'VeloEventDict' )
g.loaddict ( 'RichEventDict' )
g.loaddict ( 'MuonEventDict' )
g.loaddict ( 'CaloEventDict' )
g.loaddict ( 'L1EventDict'   )
g.loaddict ( 'L0EventDict'   )

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
          gbl.CaloDigit     ,
          gbl.L0DUReport    ,
          gbl.L1Report      ]

for t in types :
    t.__repr__ = _fillStream_
    
# =============================================================================
# The END 
# =============================================================================
