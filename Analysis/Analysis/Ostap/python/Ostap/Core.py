#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file Core.py
#  Core objects for Ostap 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
"""Core objects for Ostap 
"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-06-07"
__all__     = (
    'cpp'              ,  ## global C++ namespace 
    'ROOTCWD'          ,  ## context manager to keep ROOT current directory
    'rootID'           ,  ## global identified for ROOT objects
    'funcID'           ,  ## global identified for ROOT functions 
    'funID'            ,  ## global identified for ROOT functions 
    'fID'              ,  ## global identified for ROOT functions 
    'histoID'          ,  ## global identified for ROOT histograms 
    'hID'              ,  ## global identified for ROOT histograms 
    'dsID'             ,  ## global identified for ROOT/RooFit datasets
    ##
    'VE'               ,  ## shortcut for Gaudi::Math::ValuewithError
    'SE'               ,  ## shortcut for StatEntity
    'WSE'              ,  ## shortcut for Gaudi::Math::WStatEntity 
    ##
    'binomEff'         ,  ## binomial efficiency  
    'binomEff2'        ,  ## binomial efficiency
    'zechEff'          ,  ## binomial efficiency: Zech's recipe 
    'wilsonEff'        ,  ## binomial efficiency: Wilson 
    'agrestiCoullEff'  ,  ## binomial efficiency: Agresti-Coull
    ##
    'iszero'           ,  ## comparison with zero  for doubles  
    'isequal'          ,  ## comparison for doubles 
    'isint'            ,  ## Is float value actually int  ? 
    'islong'           ,  ## Is float value actually long ?
    ##
    'natural_entry'    ,  ## natual entry?   @see Gaudi::Math::natural_entry 
    'natural_number'   ,  ## natual numnber? @see Gaudi::Math::natural_number 
    )
# =============================================================================
import ROOT, cppyy, math, sys
cpp = cppyy.makeNamespace('')
# =============================================================================
# logging 
# =============================================================================
from Ostap.Logger import getLogger 
if '__main__' ==  __name__ : logger = getLogger( 'Ostap.Core' )
else                       : logger = getLogger( __name__     )
# =============================================================================
logger.debug ( 'Core objects/classes/functions for Ostap')
# =============================================================================
cpp = cppyy.makeNamespace('')
# 
VE              = cpp.Gaudi.Math.ValueWithError
SE              = cpp.StatEntity 
WSE             = cpp.Gaudi.Math.WStatEntity 
ValueWithError  = cpp.Gaudi.Math.ValueWithError
#
binomEff        = cpp.Gaudi.Math.binomEff
binomEff2       = cpp.Gaudi.Math.binomEff2
zechEff         = cpp.Gaudi.Math.zechEff
wilsonEff       = cpp.Gaudi.Math.wilsonEff
agrestiCoullEff = cpp.Gaudi.Math.agrestiCoullEff
iszero          = cpp.LHCb.Math.Zero     ('double')()
isequal         = cpp.LHCb.Math.Equal_To ('double')()
isint           = cpp.LHCb.Math.isint 
islong          = cpp.LHCb.Math.islong
#Gaudi           = cpp.Gaudi
#3 
import LHCbMath.Types
## natural number ?
#  @see Gaudi::Math::natural_number 
natural_number = cpp.Gaudi.Math.natural_number
## natural etry in histo-bin ? 
#  @see Gaudi::Math::natural_entry
natural_entry  = cpp.Gaudi.Math.natural_entry
# =============================================================================
cppyy.Gaudi = cpp.Gaudi
cppyy.LHcb  = cpp.LHCb
# =============================================================================
## @class ROOTCWD
#  context manager to preserve current directory (rather confusing stuff in ROOT)
#  @code
#  print ROOT.gROOT.CurrentDirectory() 
#  with ROOTCWD() :
#     print ROOT.gROOT.CurrentDirectory() 
#     rfile = ROOT.TFile( 'test.root' , 'recreate' )
#     print ROOT.gROOT.CurrentDirectory() 
#  print ROOT.gROOT.CurrentDirectory() 
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
#  @date 2015-07-30
class ROOTCWD(object) :
    """Context manager to preserve current directory
    (rather confusing stuff in ROOT) 
    >>> print ROOT.gROOT.CurrentDirectory() 
    >>> with ROOTCWD() :
    ...     print ROOT.gROOT.CurrentDirectory() 
    ...     rfile = ROOT.TFile( 'test.root' , 'recreate' )
    ...     print ROOT.gROOT.CurrentDirectory() 
    ... print ROOT.gROOT.CurrentDirectory() 
    """
    ## context manager ENTER 
    def __enter__ ( self ) :
        "Save current working directory"
        self._dir = ROOT.gROOT.CurrentDirectory() 
        
    ## context manager EXIT 
    def __exit__  ( self , *_ ) :
        "Make the previous directory current again"
        if self._dir :
            ## self._dir.cd()
            if   not hasattr ( self._dir , 'IsOpen' ) : self._dir.cd() 
            elif     self._dir.IsOpen()               : self._dir.cd()
            
## =============================================================================
#  global identifier for ROOT objects 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def rootID ( prefix = 'o_' ) :
    """
    Construct the unique ROOT-id 
    """
    _fun = lambda i : prefix + '%d'% i
    
    _root_ID = 1000
    ## 
    with ROOTCWD() : ## keep the current working directory:
        
        _id  = _fun ( _root_ID )
        grd  = ROOT.gROOT
        cwd  = grd.CurrentDirectory()
        while grd.FindObject    ( _id ) or \
              grd.FindObjectAny ( _id ) or \
              cwd.FindObject    ( _id ) or \
              cwd.FindObjectAny ( _id ) : 
                
            _root_ID += 10 
            _id       = _fun ( _root_ID ) 
            
    return _id                 ## RETURN
# =============================================================================
## global ROOT identified for function objects 
def funcID  () : return rootID  ( 'f_' )
## global ROOT identified for function objects 
def funID   () : return funcID  ( )
## global ROOT identified for function objects 
def fID     () : return funcID  ( )
## global ROOT identified for histogram objects 
def histoID () : return rootID  ( 'h_' )
## global ROOT identified for histogram objects 
def histID  () : return histoID ( )
## global ROOT identified for histogram objects 
def hID     () : return histoID ( )
## global ROOT identified for dataset objects 
def dsID    () : return rootID  ( 'ds_' )

# ==================================================================================
## get current directory in ROOT
#  @code
#  d = cwd()
#  print d
#  @endcode 
def cwd() :
    """ Get current directory in ROOT
    >>> d = cdw() 
    """
    return ROOT.gROOT.CurrentDirectory()

# =================================== ===============================================
## get current directory in ROOT
#  @code
#  print pwd() 
#  @endcode 
def pwd() :
    """ Get current directory in ROOT
    >>> print pwd() 
    """
    return ROOT.gROOT.CurrentDirectory().GetPath() 



# =============================================================================
## get the (gaussian) random number according to parameters
#
#  @code
#    >>> v = ...  ## the number with error
#
#    ## get 100 random numbers 
#    >>> for i in range ( 0, 100 ) : print v.gauss()
#    
#    ## get only non-negative numbers 
#    >>> for j in range ( 0, 100 ) : print v.gauss( lambda s : s > 0 )
#
#  @endcode
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-08-10
# 
def _gauss_ ( s , accept = lambda a : True , nmax = 1000 ) : 
    """ Get the gaussian random number
    >>> v = ...  ## the number with error
    ## get 100 random numbers 
    >>> for i in range ( 0, 100 ) : print v.gauss()    
    ## get only non-negative numbers 
    >>> for j in range ( 0, 100 ) : print v.gauss( lambda s : s > 0 )     
    """
    #
    if 0 >= s.cov2() or iszero ( s.cov2 () ) : return s.value() ## return
    #
    from scipy import random
    _gauss = random.normal
    #
    v = s.value ()
    e = s.error ()
    #
    for i in range( 0 , nmax ) : 
        r = v + e * _gauss ()
        if accept ( r ) : return r

    logger.warning("Can'n generate proper random number %s" % s )
    return v  
# =============================================================================
## generate poisson random number according to parameters 
#  @code
#    >>> v = ...  ## the number with error
#
#    ## get 100 random numbers 
#    >>> for i in range ( 0, 100 ) : print v.poisson ( fluctuate = True )
#    
#    ## get only odd numbers 
#    >>> for j in range ( 0, 100 ) : print v.poisson ( fluctuate = True , accept = lambda s : 1 ==s%2 )
#
#    ## do not fluctuate the mean of poisson:    
#    >>> for j in range ( 0, 100 ) : print v.poisson ( fluctuate = False  )
#
#  @endcode
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-08-10   
def _poisson_ ( s , fluctuate , accept = lambda s : True ) :
    """Generate poisson random number according to parameters 
    >>> v = ...  ## the number with error    
    ## get 100 random numbers 
    >>> for i in range ( 0, 100 ) : print v.poisson()    
    ## get only odd numbers 
    >>> for j in range ( 0, 100 ) : print v.poisson ( accept = lambda s : 1 ==s%2 )    
    ## do not fluctuate the mean of poisson:    
    >>> for j in range ( 0, 100 ) : print v.poisson ( fluctuate = False  )    
    """
    v = s.value() 
    if v < 0 and not fluctuate :
        raise TypeError, 'Negative mean without fluctuations (1)'
    if v < 0 and s.cov2() <= 0 :
        raise TypeError, 'Negative mean without fluctuations (2)'

    e = s.error() 
    if v < 0 and abs(v) > 3 * e  :
        logger.warning ( "Very inefficient mean fluctuations: %s" % s ) 

    mu = v
    if fluctuate :
        mu = s.gauss ()
        while mu < 0 :
            mu = s.gauss ()

    from scipy import random
    _poisson = random.poisson
    
    return _poisson ( mu ) 
    
VE.gauss   = _gauss_
VE.poisson = _poisson_ 
    


# =============================================================================
## FIX
#  @see https://sft.its.cern.ch/jira/browse/ROOT-6627'
_a  = VE( 1  , 1 )
_b  = VE( _a     )
if     isequal ( _a.error () , _b.error () ) : pass 
else :
    jira = 'https://sft.its.cern.ch/jira/browse/ROOT-6627'
    vers = ROOT.gROOT.GetVersion() 
    logger.warning ( 'The problem %s is not solved yet ( ROOT %s) ' %  ( jira , vers ) )
    logger.warning ( 'Temporarily disable cast of VE to float' )
    del VE.__float__

# =============================================================================
if '__main__' == __name__ :
    
        
    import Ostap.Line
    logger.info ( __file__  + '\n' + Ostap.Line.line  ) 
    logger.info ( 80*'*'   )
    logger.info ( __doc__  )
    logger.info ( 80*'*' )
    logger.info ( ' Author  : %s' %         __author__    ) 
    logger.info ( ' Version : %s' %         __version__   ) 
    logger.info ( ' Date    : %s' %         __date__      )
    logger.info ( ' Symbols : %s' %  list ( __all__     ) )
    logger.info ( 80*'*' ) 
    
# =============================================================================
# The END 
# =============================================================================
