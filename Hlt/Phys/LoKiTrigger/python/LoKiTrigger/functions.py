#!/usr/bin/env python
# =============================================================================
## @file
#  The set of basic objects from LoKiTrigger library
#
#        This file is a part of LoKi project - 
#    "C++ ToolKit  for Smart and Friendly Physics Analysis"
#
#  The package has been designed with the kind help from
#  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
#  contributions and advices from G.Raven, J.van Tilburg, 
#  A.Golutvin, P.Koppenburg have been used in the design.
#
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2007-06-09
# =============================================================================
"""
The set of basic objects from LoKiTrigger library


      This file is a part of LoKi project - 
``C++ ToolKit  for Smart and Friendly Physics Analysis''

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.


"""
# =============================================================================
__author__  = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
__date__    = "2007-06-09"
__version__ = "CVS Tag: $Name:$, version $Revision: $ "
# =============================================================================

import LoKiCore.decorators as _LoKiCore 


# Namespaces:
_global  = _LoKiCore._global 
std      = _global.std
LoKi     = _global.LoKi
LHCb     = _global.LHCb

## helper objects
TrackFunction       = LoKi.Hlt1.TrackFunction
TrackBiFunction     = LoKi.Hlt1.TrackBiFunction
MatchTVelo          = LoKi.Hlt1.MatchTVelo
TrackVertexFunction = LoKi.Hlt1.TrackVertexFunction


# functional stuff


## functional part
_vt       = 'std::vector<LHCb::Track*>'     ## std.vector ( 'LHCb::Track*'     )
_vv       = 'std::vector<LHCb::RecVertex*>' ## std.vector ( 'LHCb::RecVertex*' )
_vd       = 'std::vector<double>'           ## std.vector ( 'double')
#


## ## Hlt1 stuff 
## TrSELECTION = LoKi.Hlt1.TrSelection 
## TrREGISTER  = LoKi.Hlt1.TrRegister
## TrPUT       = LoKi.Hlt1.TrRegister
## TrINPUT     = LoKi.Hlt1.TrTESInput
## RvSELECTION = LoKi.Hlt1.RvSelection 
## RvREGISTER  = LoKi.Hlt1.RvRegister
## RvPUT       = LoKi.Hlt1.RvRegister
## RvINPUT     = LoKi.Hlt1.RvTESInput
## TrUPGRADE   = LoKi.Hlt1.TrUpgrade
## TrMATCH     = LoKi.Hlt1.TrMatch
## TrMATCH2    = LoKi.Hlt1.TrMatch2
## VxMAKER     = LoKi.Hlt1.VxMaker 
## VxUPGRADE   = LoKi.Hlt1.VxUpgrade

## # =============================================================================
## # ## Simple way to create the 'smart' function from the regular function.
## def info ( key , fun , update = False ) :
##     """
##     Simple way to create the 'smart' function from the regular function.
##     Smart function first checks the presence of the information with
##     the given key in 'extra-info' data member, and int he case of missing
##     information it used the supplied function to evaluate the information.
##     optionally the evaluated information is added into  'extra-info'
    
##     >>> fun =  ...
##     >>> key =  ...
##     >>> smartFun = info ( key , fun , true ) 
    
##     """
##     return LoKi.Dicts.TrInfo.info ( key , fun , update )

## info. __doc__ += "\n\n" + LoKi.Dicts.TrInfo.info . __doc__ 
 

## # ## ## Simple function to 'bind' the 2-argument function 
## def bindMin ( a , b , *c ) :
##     """
##     Simple function to 'bind' the 2-argument function to some container
    
##     The function comes in two flavours:

##     1) for the given argument, return the minimal value of 2D-function against all
##     objects from the reference container:

##     Find the minimal distance of the closest approach:
    
##     >>> tracks = ...
##     >>> doca = bindMin ( TTrDOCA , tracks )   ## create the function
    
##     >>> track = ...
##     >>> value = doca ( track )                ## use it !


##     2) for the given argument, seek for the element from the reference container
##     which minimizes 2D-function and return the value of the second function with
##     this minimal element:
    
##     Evaluate of the transverse momentum of the track, which
##     has the the minimal distance of the closest approach to the given track:
    
##     >>> tracks = ...
##     >>> pt = bindMin ( TTrDOCA , TrPT , tracks )   ## create the function
    
##     >>> track = ...
##     >>> value = pt ( track )                ## use it !
    
##     """
##     return LoKi.Dicts.HltBind.bindMin ( a , b , *c  ) 


## # ## Simple function to 'bind' the 2-argument function 
## def bindAbsMin ( a , b , *c ) :
##     """
##     Simple function to 'bind' the 2-argument function to some container

##     The function comes in two flavours:
    
##     1) for the given argument, return the minimal (absolute) value
##     of 2D-function against all objects from the reference container:
    
##     Find the minimal (absolute) distance of the closest approach:
    
##     >>> tracks = ...
##     >>> doca = bindAbsMin ( TTrDOCA , tracks )   ## create the function
    
##     >>> track = ...
##     >>> value = doca ( track )                ## use it !

    
##     2) for the given argument, seek for the element from the reference container
##     which minimizes the absolute value for 2D-function and return the value
##     of the second function with this minimal element:
    
##     Evaluate of the transverse momentum of the track, which
##     has the the minimal distance of the closest approach to the given track:
    
##     >>> tracks = ...
##     >>> pt = bindAbsMin ( TTrDOCA , TrPT , tracks )   ## create the function
    
##     >>> track = ...
##     >>> value = pt ( track )                ## use it !

    
##     Essentially the following expression are equivalent :

##     >>> fun = binAbsMin (       f   , a , ... )
##     >>> fun = bindMin   ( abs ( f ) , a , ... )

##     However the first one a little bit more efficient (less virtual calls) 
    
##     """
##     return LoKi.Dicts.HltBind.bindAbsMin ( a , b , *c  ) 

## # ## Simple function to 'bind' the 2-argument function 
## def bindMax ( a , b , *c ) :
##     """
##     Simple function to 'bind' the 2-argument function to some containers
    
##     The function comes in two flavours:

##     1) for the given argument, return the maximal value of 2D-function against all
##     objects from the reference container:

##     find the maximal distance of the closest approach:
    
##     >>> tracks = ...
##     >>> doca = bindMax ( TTrDOCA , tracks )   ## create the function
    
##     >>> track = ...
##     >>> value = doca ( track )                ## use it !


##     2) for the given argument, seek for the element from the reference container
##     which maximizes 2D-function and return the value of the secodn function with
##     this maximal element:
    
##     Evaluate of the transverse momentum of the track, which
##     has the the maximal distance of the closest approach to the given track:
    
##     >>> tracks = ...
##     >>> pt = bindMax ( TTrDOCA , TrPT , tracks )   ## create the function
    
##     >>> track = ...
##     >>> value = pt ( track )                ## use it !
    
##     """
##     return LoKi.Dicts.HltBind.bindMin ( a , b , *c  ) 


## # ## Simple function to 'bind' the 2-argument function 
## def bindAbsMax ( a , b , *c ) :
##     """
##     Simple function to 'bind' the 2-argument function to some container

##     The function comes in two flavours:

##     1) for the given argument, return the maximal (absolute) value
##     of 2D-function against all objects from the reference container:
    
##     Find the maximal (absolute) distance of the closest approach:
    
##     >>> tracks = ...
##     >>> doca = bindAbsMax ( TTrDOCA , tracks )   ## create the function
    
##     >>> track = ...
##     >>> value = doca ( track )                ## use it !


##     2) for the given argument, seek for the element from the reference container
##     which maximizes the absolute value for 2D-function and return the value of
##     the second function with this maximal element:
    
##     Evaluate of the transverse momentum of the tarck, which
##     has the the maximal (absolute) distance of the closest approach to the given track:
    
##     >>> tracks = ...
##     >>> pt = bindAbsMax ( TTrDOCA , TrPT tracks )   ## create the function
    
##     >>> track = ...
##     >>> value = pt ( track )                ## use it !

    
##     Essenntially the following expression are equivalent :

##     >>> fun = binAbsMax (       f   , a , ... )
##     >>> fun = bindMax   ( abs ( f ) , a , ... )

##     However the first one a little bit more efficient (less virtual calls) 
    
##     """
##     return LoKi.Dicts.HltBind.bindAbsMax ( a , b , *c  ) 


## bindMin    . __doc__ += "\n\n" + LoKi.Dicts.HltBind.bindMin    . __doc__ 
## bindMax    . __doc__ += "\n\n" + LoKi.Dicts.HltBind.bindMax    . __doc__ 
## bindAbsMin . __doc__ += "\n\n" + LoKi.Dicts.HltBind.bindAbsMin . __doc__ 
## bindAbsMax . __doc__ += "\n\n" + LoKi.Dicts.HltBind.bindAbsMax . __doc__ 


import LoKiTrigger.Service

# =============================================================================
if '__main__' == __name__ :
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print 80*'*'

# =============================================================================
# The END 
# =============================================================================
