#!/usr/bin/env python
# =============================================================================
# $Id: benderstr.py,v 1.6 2005-03-04 19:30:43 ibelyaev Exp $ 
# =============================================================================
# CVS version $Revision: 1.6 $ 
# =============================================================================
# CVS tag     $Name: not supported by cvs2svn $ 
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.5  2005/01/24 17:44:39  ibelyaev
#  v4r5
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
_dicts_ = [ 'EventDict'     ,
            'PhysEventDict' ,
            'TrEventDict'   ,
            'OTEventDict'   ,
            'ITEventDict'   ,
            'VeloEventDict' ,
            'RichEventDict' ,
            'MuonEventDict' ,
            'CaloEventDict' ]
for dct in _dicts_ : gaudimodule.loaddict( dct ) 

def _fillStream_ ( self ) :
    _obj = self 
    return fillStream ( _obj )

_types = [ 'MCParticle'    ,
           'MCVertex'      ,
           'Collision'     ,
           'SelResult'     ,
           'Particle'      ,
           'Vertex'        ,
           'ProtoParticle' ,
           'TrStoredTrack' ,
           'RichPID'       ,
           'MuonID'        ,
           'CaloParticle'  ,
           'CaloHypo'      ,
           'TrState'       ,
           'TrStateP'      ,
           'TrStateL'      ,
           'CaloCluster'   ,
           'CaloDigit'     ]

for t in _types :
    t = gaudimodule.getClass( t ) 
    t.__repr__ = _fillStream_
    
# =============================================================================
# The END 
# =============================================================================
