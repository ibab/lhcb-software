#!/usr/bin/env python 
# =============================================================================
## @file Benedr/Fixes_LoKi.py
#  The helper Python module for Bender application 
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  ``C++ ToolKit for Smart and Friendly Physics Analysis''
#
#  By usage of this code one clearly states the disagreement 
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
# 
#  @date   2004-07-11
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$ 
# =============================================================================
"""Apply some last-moment (version-dependent) fixes

oooooooooo.                              .o8                     
`888'   `Y8b                            \"888                     
 888     888  .ooooo.  ooo. .oo.    .oooo888   .ooooo.  oooo d8b 
 888oooo888' d88' `88b `888P\"Y88b  d88' `888  d88' `88b `888\"\"8P 
 888    `88b 888ooo888  888   888  888   888  888ooo888  888     
 888    .88P 888    .o  888   888  888   888  888    .o  888     
o888bood8P'  `Y8bod8P' o888o o888o `Y8bod88P\" `Y8bod8P' d888b    

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
# =============================================================================
__author__  = 'Vanya BELYAEV ibelyaev@physics.syr.edu'
__date__    = "2004-07-11"
__version__ = '$Revision$'
__all__     = ()
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
if '__main__' == __name__ : logger = getLogger ( 'Bender.Fixes_LoKi' )
else                      : logger = getLogger ( __name__ )
# =============================================================================
logger.info ( '*** Fix some LHCb/LoKi features' ) 
# =============================================================================

# =============================================================================
## decorate the ranges
# =============================================================================
from   LoKiCore.basic      import cpp , std, Gaudi, LHCb, cpp 
import LoKiCore.decorators as     LCD
            
# 
## decorate some "packed" objects:
for i in [
    ## RC 
    ( LHCb.PackedCaloHypos        , 'hypos'    ) ,
    ( LHCb.PackedRecVertices      , 'vertices' ) ,
    ( LHCb.PackedTwoProngVertices , 'vertices' ) ,
    ( LHCb.PackedParticles        , 'data'     ) ,
    ( LHCb.PackedVertices         , 'data'     ) ,
    ( LHCb.PackedMuonPIDs         , 'data'     ) ,
    ( LHCb.PackedRichPIDs         , 'data'     ) ,
    ( LHCb.PackedProtoParticles   , 'protos'   ) ,
    ( LHCb.PackedTracks           , 'tracks'   ) ,
    ## MC
    ( LHCb.PackedMCCaloHits       , 'data'     ) ,
    ( LHCb.PackedMCHits           , 'data'     )
    ] :
    
    t = i[0]
    m = i[1]
    
    if not hasattr ( t , 'size' ) :
        logger.debug ( 'decorate packed type: %s ' % t  ) 
        t.get_data    = getattr ( t , m ) 
        t.__len__     = lambda s : s.get_data().size()
        t.size        = lambda s : len  ( s )

        
# =============================================================================
## get iteration over raw banks in raw-event
#  @code
#  raw_event = ...
#  for bank in raw_event :  print bank
#  @endcode
def _re_iter_ ( rawevt ) :
    """Get iteration over raw banks in raw-event
    >>> raw_event = ...
    >>> for bank in raw_event :  print bank
    """
    for _bt in range ( 0 , 100 ) :
        _bn = LHCb.RawBank.typeName ( _bt )
        if 'Undefined_name' == _bn : continue
        _banks = rawevt.banks ( _bt )
        for _b in _banks :
            yield _b
            
# =============================================================================
## Number of banks in RawEvent 
#  @code
#  raw_event = ...
#  print ' total: ', raw_event.nBanks()
#  print ' Muon : ', raw_event.nBanks( LHCb.RawBank.Muon )
#  @endcode
def _re_nbanks_ ( rawevt , bank_type = None ) :
    """Number of banks in RawEvent 
    >>> raw_event = ...
    >>> print ' total: ', raw_event.nBanks()
    >>> print ' Muon : ', raw_event.nBanks( LHCb.RawBank.Muon )
    """
    
    if isinstance ( bank_type , int ) :
        return len( rawevt.banks( bank_type ) )
    
    _nb = 0
    for _bt in range ( 0 , 100 ) :
        _bn = LHCb.RawBank.typeName ( _bt )
        if 'Undefined_name' == _bn : continue
        _banks = revt.banks ( _bt )
        _nb   += _banks
    return _nb

# =============================================================================
## get a list of bank types in raw event
#  @code
#  raw_event = ...
#  for i in raw_event.bank_list() : print i 
#  @endcode
def _re_banks_ ( rawevt ) :
    """Get a list of bank types in raw event
    >>> raw_event = ...
    >>> for i in raw_event.bank_list() : print i 
    """
    _banks = {}
    for _bank in rawevt :
        _nb = _bank.type()  
        _banks[ _nb ] = 1 + _banks.get ( _nb , 0 )
    _keys = _banks.keys()
    _keys.sort()
    return [ '%s/%d' % ( LHCb.RawBank.typeName ( _k ) ,_banks[_k]) for _k in _keys ] 

# =============================================================================
## print rawevent 
def _re_print_ ( rawevt ) :
    return 'LHCb::RawEvent(%s)' % _re_banks_ ( rawevt )

LHCb.RawEvent.__iter__ = _re_iter_
LHCb.RawEvent.__len__  = _re_nbanks_
LHCb.RawEvent.__repr__ = _re_print_

# =============================================================================
if __name__ == '__main__' :
    
    logger.info ( 80*'*'  ) 
    logger.info ( __doc__ ) 
    logger.info ( ' Author  : %s ' %  __author__  ) 
    logger.info ( ' Version : %s ' %  __version__ ) 
    logger.info ( ' Date    : %s ' %  __date__    ) 
    logger.info ( ' Symbols : %s ' %  list ( __all__ ) ) 
    logger.info ( 80*'*'  ) 

# =============================================================================
# The END 
# =============================================================================
