#!/usr/bin/env python
# =============================================================================
# $Id: benderstr.py,v 1.9 2005-07-06 18:28:02 ibelyaev Exp $ 
# =============================================================================
# CVS version $Revision: 1.9 $ 
# =============================================================================
# CVS tag     $Name: not supported by cvs2svn $ 
# =============================================================================
# $Log: not supported by cvs2svn $
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
_dicts_ = [ 'LHCbKernelDict',
            'EventDict'     ,
            'PhysEventDict' ,
            'TrEventDict'   ,
            'OTEventDict'   ,
            'ITEventDict'   ,
            'TrgEventDict'  ,
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
           'TrgTrack'      ,
           'RichPID'       ,
           'MuonID'        ,
           'CaloParticle'  ,
           'CaloHypo'      ,
           'TrState'       ,
           'TrStateP'      ,
           'TrStateL'      ,
           'CaloCluster'   ,
           'CaloDigit'     ,
           #
           'CaloCellID'    ,
           'VeloChannelID' ,
           'ITChannelID'   ,
           'OTChannelID'   ,
           'RichSmartID'   ,
           'LHCbID'        ]

for t in _types :
    tt = gaudimodule.getClass( t )
    if not tt :
        print 'invalid class %s' % t 
    else:
        tt.__repr__ = _fillStream_
    
# =============================================================================
# The END 
# =============================================================================
