#!/usr/bin/env
# =============================================================================
# $Id: bendermatrix.py,v 1.3 2005-07-24 14:04:41 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.3 $
# =============================================================================
"""
Trivial module to make a some adaptation for CLHEP 'Matrix'-like classes
"""
# =============================================================================
__author__ = 'Vanya BELYAEV'
# =============================================================================

import gaudimodule

# load 
gaudimodule.loaddict('SealCLHEPDict')

_gbl = gaudimodule.gbl

def _get_round_  ( self , *arg ) :
    return self._round_( *arg )

def _get_square_ ( self , *arg ) :
    return self._square_( *arg )

def _make_operators_ ( klass ) :
    
    if hasattr( klass , '()' ) :
        klass._round_     = getattr( klass , '()' )
        klass.__call__    = _get_round_
        
    #if hasattr( klass , '[]' ) and not hasattr( klass , '__getitem__' ) :
    #    klass._square_    = getattr( klass , '[]' )
    #    klass.__getitem__ = _get_square_
        
def _getClass_ ( name , space = gaudimodule.gbl ) :
    klass = getattr( space , name )
    if not klass :
        raise TypeError , 'unknow class name "%s" ' % name
    return klass

_types_ = [ 'HepVector'     ,
            'HepDiagMatrix' ,
            'HepSymMatrix'  ,
            'HepMatrix'     ,
            'HepGenMatrix'  ]


for _type_ in _types_ :
    klass = gaudimodule.getClass( _type_ )
    _make_operators_( klass ) 

# =============================================================================
# $Log:@ 
# =============================================================================
# The END 
# =============================================================================

