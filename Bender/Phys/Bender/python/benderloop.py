#!/usr/bin/env python
# =============================================================================
# $Id: benderloop.py,v 1.2 2004-08-26 19:34:36 ibelyaev Exp $ 
# =============================================================================
# CVS tag $NAme:$ 
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================

# =============================================================================
# @file
#
# defintion of all Lo?ki/Bender functions/cuts 
#
# @date   2004-07-11
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# =============================================================================

# import the global namespace 
from gaudimodule import gbl

# load the minimal set of dictionaries
gaudi.loaddict('BenderDict')

# load 'intermediate object
BenderLoop = gbl.Bender.Loop

# extend the intermediate object
class Loop(BenderLoop):
    """
    The basic object for looping over particle combinatios
    Usage:
    for D0 in loop( formula = 'K- pi+' , pid = 'D0' ) :
    \tp=D0.particle()
    \tprint ' D0 mass ' , M( p ) 
    """
    def __init__ ( self , loop ) :
        """
        Constructor from C++ Bender::Loop class
        """
        BenderLoop.__init__( self , loop )
        
    def save     ( self , **args ) :
        """
        Save the particle ans assosiate it with a certain tag
        Usage:
        for D0 in loop( formula = 'K- pi+' , pid = 'D0' ) :
        \t...
        \tD0.save( tag = 'D0' )
        """
        if not args.has_key('tag') :
            raise TypeError, "  'tag'  is not specified "
        return BenderLoop.save( self , args.get('tag') )

    def fit       ( self , **args ) :
        """
        Perforn kinematical/topological constraint according the fitting strategy
        Usage:
        for D0 in loop( formula = 'K- pi+' , pid = 'D0' ) :
        \t...
        \tD0.fit( strategy = FitMassVertex ) 
        """
        if args.has_key ( 'stragegy' ) :
            return BenderLoop.fit( self , args.get ( 'strategy' ) )
        if args.has_key ( 'policy'   ) :
            return BenderLoop.fit( self , args.get ( 'policy'   ) )
        if args.has_key ( 'fit'      ) :
            return BenderLoop.fit( self , args.get ( 'fit'      ) )
        raise TypeError, "  Neither 'strategy' nor 'policy' are specified "
