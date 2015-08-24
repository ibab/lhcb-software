#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file
#  Module with decoration of many ROOT objects for efficient use in python
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
#
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
"""Decoration of some ROOT objects for efficient use in python

Many native  root classes are equipped with new useful methods and operators,
in particular TH1(x) , TH2(x) , TAxis, TGraph(Errors), etc...

see also GaudiPython.HistoUtils 
"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-06-07"
# =============================================================================
__all__     = (
    #
    'cpp'             , ## global C++ namespace 
    'rootID'          , ## construct the (global) unique ROOT identifier
    'funcID'          , ## construct the (global) unique ROOT identifier
    'funID'           , ## construct the (global) unique ROOT identifier
    'hID'             , ## construct the (global) unique ROOT identifier
    'histoID'         , ## construct the (global) unique ROOT identifier
    'dsID'            , ## construct the (global) unique ROOT identifier
    #
    'VE'              , ## Gaudi::Math::ValueWithError
    'ValueWithError'  , ## Gaudi::Math::ValueWithError
    'histoGuess'      , ## guess the simple histo parameters
    'useLL'           , ## use LL for histogram fit?
    'allInts'         , ## natural histogram with natural entries?
    'SE'              , ## StatEntity
    'WSE'             , ## StatEntity with weights
    'iszero'          , ## is almost zero  ?
    'isequal'         , ## is almost equal ?
    #
    'binomEff'        , ## calculate binomial efficiency
    'binomEff2'       , ## calculate binomial efficiency
    'zechEff'         , ## calcualte binomial efficiency using Zech's          prescription
    'wilsonEff'       , ## calcualte binomial efficiency using Wilson's        prescription
    'agrestiCoullEff' , ## calcualte binomial efficiency using Agresti-Coull's prescription
    #
    'binomEff_h1'     , ## calculate binomial efficiency for 1D-histos
    'binomEff_h2'     , ## calculate binomial efficiency for 2D-ihstos
    'binomEff_h3'     , ## calculate binomial efficiency for 3D-ihstos
    #
    'makeGraph'       , ## make ROOT Graph from input data
    'hToGraph'        , ## convert historgam to graph
    'hToGraph2'       , ## convert historgam to graph
    'hToGraph3'       , ## convert historgam to graph
    'lw_graph'        , ## make Lafferty-Wyatt's graph from the histo 
    'h1_axis'         , ## book 1D-histogram from axis 
    'h2_axes'         , ## book 2D-histogram from axes
    'h3_axes'         , ## book 3D-histogram from axes
    'axis_bins'       , ## convert list of bin edges to axis
    've_adjust'       , ## adjust the efficiency to be in physical range
    #
    )
# =============================================================================
import ROOT,cppyy, math, sys  ## attention here!!
#
iLevel = int( ROOT.gErrorIgnoreLevel ) 
ROOT.gROOT.ProcessLine("gErrorIgnoreLevel = 2001; " ) 
    
cpp = cppyy.makeNamespace('')
# 
import LHCbMath.Types
Gaudi           = cpp.Gaudi
VE              = Gaudi.Math.ValueWithError
SE              = cpp.StatEntity 
WSE             = Gaudi.Math.WStatEntity 
ValueWithError  = Gaudi.Math.ValueWithError
#
binomEff        = Gaudi.Math.binomEff
binomEff2       = Gaudi.Math.binomEff2
zechEff         = Gaudi.Math.zechEff
wilsonEff       = Gaudi.Math.wilsonEff
agrestiCoullEff = Gaudi.Math.agrestiCoullEff
iszero          = cpp.LHCb.Math.Zero     ('double')()
isequal         = cpp.LHCb.Math.Equal_To ('double')()
#

# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger 
logger = getLogger( __name__ )
# =============================================================================
logger.info ( 'Zillions of decorations for ROOT   objects')
# =============================================================================
from Ostap.TFileDeco import ROOTCWD
## ============================================================================
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

try :
    natural_number = Gaudi.Math.natural_number
    logger.info     ( 'TMP: natural_number is loaded from C++, skip guard..' )
except AttributeError :
    logger.warning  ( 'TMP: Python version of natural_number is used, keep guard..' )
    def natural_number ( ve ) :
        return 0 <= v.value () and \
               0 <= v.cov2  () and \
               (  iszero ( v.cov2() ) or isequal (  v.value() , v.cov2() ) )
    
try :
    natural_entry = Gaudi.Math.natural_entry
    logger.info     ( 'TMP: natural_entry is loaded from C++, skip guard..' )
except AttributeError :
    logger.warning  ( 'TMP: Python version of natural_entry is used, keep guard..' )
    def natural_entry ( ve ) :
        return 0 <= v.value () and \
               0 <= v.cov2  () and \
               ( isequal (  v.value() , v.cov2() ) or
                 ( iszero ( v.value() ) and isequal ( 1.0 , v.cov2() ) ) )

# =============================================================================
## FIX
#  @see https://sft.its.cern.ch/jira/browse/ROOT-6627'
_a  = VE( 1 , 1 )
_b  = VE( _a    )
if     isequal ( _a.error () , _b.error () ) : pass 
else :
    jira = 'https://sft.its.cern.ch/jira/browse/ROOT-6627'
    vers = ROOT.gROOT.GetVersion() 
    logger.warning ( 'The problem %s is not solved yet ( ROOT %s) ' %  ( jira , vers ) )
    logger.warning ( 'Temporarily disable cast of VE to float' )
    del VE.__float__


# =============================================================================
## ensure that object/histogram is created in ROOT.gROOT
#  @attention clone is always goes to ROOT main memory!
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2015-08-14
def _h_new_init_ ( self , *args ) :
    """
    Modified TH* constructor:
    - ensure that created object/histogram goes to ROOT main memory
    """
    with ROOTCWD() :
        ROOT.gROOT.cd() 
        self._old_init_   ( *args      )
        self.SetDirectory ( ROOT.gROOT ) 

# =============================================================================
## a bit modified 'Clone' function for histograms
#  - it automaticlaly assign unique ID
#  - it ensures that cloned histogram is not going to die with
#    the accidentally opened file/directory
#  @attention clone is always goes to ROOT main memory!
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _new_clone_ ( self , hid = '' ) :
    """
    Modifiled Clone-function
    - it automatically assigns unique ID
    - it ensures that cloned histogram is not going to die with
      the accidentally opened file/directory
    """
    if not hid : hid = hID()
    #
    with ROOTCWD() :
        ROOT.gROOT.cd() 
        nh = self._old_clone_ ( hid )
        nh.SetDirectory ( ROOT.gROOT ) ## ATTENTION!  
    return nh


for h in ( ROOT.TH1F , ROOT.TH1D ,
           ROOT.TH2F , ROOT.TH2D ,
           ROOT.TH3F , ROOT.TH3D ) :
    
    if hasattr ( h , '_new_clone_' ) and hasattr ( h , '_old_clone_' ) : pass
    else : 
        h._old_clone_ = h.Clone
        h._new_clone_ = _new_clone_
        h.Clone       = _new_clone_
        h.clone       = _new_clone_

    if hasattr ( h , '_new_init_' ) and hasattr ( h , '_old_init_' ) : pass
    else : 
        h._old_init_  = h.__init__ 
        h._new_init_  = _h_new_init_
        h.__init__    = _h_new_init_


# ==================================================================================
## get current directory in ROOT
#  @code
#  d = cwd()
#  print d
#  @endcode 
def cwd() :
    """
    Get current directory in ROOT
    >>> d = cdw() 
    """
    return ROOT.gROOT.CurrentDirectory()

# =================================== ===============================================
## get current directory in ROOT
#  @code
#  print pwd() 
#  @endcode 
def pwd() :
    """
    Get current directory in ROOT
    >>> print pwd() 
    """
    return ROOT.gROOT.CurrentDirectory().GetPath() 

# =============================================================================
def _int ( ve , precision = 1.e-5 ) :
    #
    if isinstance  ( ve , ( int , long ) ) : return True
    #
    if isinstance  ( ve , float ) :
        if Gaudi.Math.isint ( ve ) or Gaudi.Math.islong ( ve ) : return True 
    # 
    if not hasattr ( ve , 'value' ) :
        return _int ( VE ( ve , abs ( ve ) ) , precision )  
    #
    diff = max ( 1 , abs ( ve.value() ) ) * precision 
    diff = min ( 0.1 , diff ) 
    # 
    if abs ( ve.value () - long ( ve.value () ) ) > diff : return False 
    if abs ( ve.value () -        ve.cov2  ()   ) > diff : return False
    #
    return True 

# =============================================================================
## check if  histogram is "natural" one, that represent event count
#  @see Gaudi::Math::round 
#  @see Gaudi::Math::islong
#  @see Gaudi::Math::isint
#  @author Vanya BELYAEV Ivan.Belayev@itep.ru
#  @date   2015-03-31
def _natural_ ( histo ) :
    """
    Check is the histogram is the ``natural'' one:
    - entries are non-negative integers
    - errors  are sqrt(entry)
    [ for null entries, the errors of  0 or 1 are both allowed] 
    
    >>> histo = ...
    >>> print 'natural? ', histo.natural()
    
    """
    _islong = cpp.Gaudi.Math.islong
    _round  = cpp.Gaudi.Math.round
    ## loop over all histogram bins 
    for i in histo :
        
        v  = histo [ i ]
        vv = v.value ()
        if not _islong ( vv )             : return False  ## integer
        vl = _round ( vv )
        if 0 > vl                         : return False  ## non-negative 
        #
        ve = v.cov2  ()
        if not _islong ( ve )             : return False  ## integer 
        ## 
        if isequal ( vv , ve )            : continue      ## OK
        if 0 == vl and isequal ( ve , 1 ) : continue      ## OK 
        #
        return False   

    return True

ROOT.TH1.natural = _natural_

# =============================================================================
## check if histogram has uniform binnings
def _uniform_bins_ ( histo ) :
    """
    Check if histogram has uniform binnings
    >>> histo = ...
    >>> uni   = histo.uniform_bins() 
    """
    axis = histo.GetXaxis()
    if 1 <= len ( axis.GetXbins () ) : return False
    axis = histo.GetYaxis()
    if 1 <= len ( axis.GetXbins () ) : return False
    axis = histo.GetZaxis()
    if 1 <= len ( axis.GetXbins () ) : return False
    #
    return True

ROOT.TH1.uniform_bins = _uniform_bins_
ROOT.TH1.uniform      = _uniform_bins_

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
    """
    Get the gaussian random number

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
    """
    Generate poisson random number according to parameters 
    
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
# Decorate histogram axis and iterators 
# =============================================================================

# =============================================================================
## iterator for histogram  axis 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _axis_iter_1_ ( a ) :
    """
    Iterator for axis

    >>> axis = ...
    >>> for i in axis : 
    """
    i = 1
    s = a.GetNbins()
    while i <= s :
        yield i
        i+=1        

ROOT.TAxis . __iter__ = _axis_iter_1_

# =============================================================================
## iterator for histogram  axis (reversed order) 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _axis_iter_reversed_ ( a ) :
    """
    Iterator for axis

    >>> axis = ...
    >>> for i in reverse(axis) : 
    """
    s = a.GetNbins()
    i = long( s )
    while 1 <= i :
        yield i
        i-=1


ROOT.TAxis . __reversed__ = _axis_iter_reversed_

ROOT.TAxis . __contains__ = lambda s , i : 1 <= i <= s.GetNbins()

# =============================================================================
## get item for the 1-D histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_get_item_ ( h1 , ibin ) :
    """
    ``Get-item'' for the 1D-histogram :
    
    >>> histo = ...
    >>> ve    = histo[ibin]
    
    """
    if not ibin in h1.GetXaxis()    : raise IndexErorr
    #
    val = h1.GetBinContent ( ibin ) 
    err = h1.GetBinError   ( ibin )
    #
    return VE ( val , err * err )

# ==========================================================================
## get item for the 2D histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_set_item_ ( h1 , ibin , v ) :
    """
    ``Set-item'' for the 1D-histogram :
    
    >>> histo[ibin] = value 
    
    """
    #
    vv = VE ( v ) 
    if   isinstance ( v , ( int , long ) ) :
        
        if   0  < v   : vv = VE ( v , v )  
        elif 0 == v   : vv = VE ( 0 , 1 ) 
        else          : vv = VE ( v , 0 ) 

    elif isinstance ( v , float ) :
        
        if _int ( v ) : return _h1_set_item_ ( h1 , ibin , long ( v ) )
        else          : vv = VE ( v , 0 ) 

    ## check the validity of the bin 
    if not ibin in h1 : raise IndexError 
    #
    h1.SetBinContent ( ibin , vv.value () )
    h1.SetBinError   ( ibin , vv.error () )
    
ROOT.TH1F. __setitem__ = _h1_set_item_
ROOT.TH1D. __setitem__ = _h1_set_item_

# ==========================================================================
## get item for the 2D histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h2_set_item_ ( h2 , ibin , v ) :
    """
    ``Set-item'' for the 2D-histogram :
    
    >>> histo[(ix,iy)] = value 
    
    """
    #
    vv = VE ( v ) 
    if   isinstance ( v , ( int , long ) ) :
        
        if   0  < v   : vv = VE ( v , v ) 
        elif 0 == v   : vv = VE ( 0 , 1 ) 
        else          : vv = VE ( v , 0 ) 
        
    elif isinstance ( v , float ) :
        
        if _int ( v ) : return _h2_set_item_ ( h2 , ibin , long ( v ) )
        else          : vv = VE ( v , 0 ) 

    ## check the validity of the bin 
    if not ibin in h2 : raise IndexError 
    #
    h2.SetBinContent ( ibin[0] , ibin[1] , vv.value () )
    h2.SetBinError   ( ibin[0] , ibin[1] , vv.error () )
    
ROOT.TH2F. __setitem__ = _h2_set_item_
ROOT.TH2D. __setitem__ = _h2_set_item_


# ==========================================================================
## get item for the 3D histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h3_set_item_ ( h3 , ibin , v ) :
    """
    ``Set-item'' for the 3D-histogram :
    
    >>> histo[(ix,iy,iz)] = value 
    
    """
    #
    vv = VE ( v ) 
    if   isinstance ( v , ( int , long ) ) :
        
        if   0  < v   : vv = VE ( v , v )
        elif 0 == v   : vv = VE ( 0 , 1 ) 
        else          : vv = VE ( v , 0 ) 
        
    elif isinstance ( v , float ) :
        
        if _int ( v ) : return _h3_set_item_ ( h3 , ibin , long ( v ) )
        else          : vv = VE ( v , 0 ) 

    ## check the validity of the bin 
    if not ibin in h3 : raise IndexError 
    #
    h3.SetBinContent ( ibin[0] , ibin[1] , ibin[2] , vv.value () )
    h3.SetBinError   ( ibin[0] , ibin[1] , ibin[2] , vv.error () )
    
ROOT.TH3F. __setitem__ = _h3_set_item_
ROOT.TH3D. __setitem__ = _h3_set_item_

# =============================================================================
## get item for the 2D histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h2_get_item_ ( h2 , ibin ) :
    """
    ``Get-item'' for the 2D-histogram :
    
    >>> histo = ...
    >>> ve    = histo[ ix,iy ]
    
    """
    #
    if not ibin[0] in h2.GetXaxis() : raise IndexError 
    if not ibin[1] in h2.GetYaxis() : raise IndexError 
    #
    val = h2.GetBinContent ( ibin[0] , ibin[1] )  
    err = h2.GetBinError   ( ibin[0] , ibin[1] ) 
    #
    return VE ( val , err * err ) 

# =============================================================================
## get item for the 3D histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h3_get_item_ ( h3 , ibin ) :
    """
    ``Get-item'' for the 3D-histogram :
    
    >>> histo = ...
    >>> ve    = histo[ ix,iy,iz ]
    
    """
    #
    if not ibin[0] in h3.GetXaxis() : raise IndexError 
    if not ibin[1] in h3.GetYaxis() : raise IndexError 
    if not ibin[2] in h3.GetZaxis() : raise IndexError 
    #
    val = h3.GetBinContent ( ibin[0] , ibin[1] , ibin[2] )   
    err = h3.GetBinError   ( ibin[0] , ibin[1] , ibin[2] ) 
    #
    return VE ( val , err * err ) 

# =============================================================================
## iterator for 1D-histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_iter_ ( h1 ) :
    """
    Iterator over 1D-histogram
    
    >>> for i in h1 : print i 
    """
    ax = h1.GetXaxis () 
    sx = ax.GetNbins ()
    for i in range ( 1 , sx + 1 ) : 
        yield i

# =============================================================================
## iterator for 1D-histogram in reverse order 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2015-07-06
def _h1_iter_reversed_ ( h1 ) :
    """
    Iterator over 1D-histogram
    
    >>> for i in reversed(h1) : print i 
    """
    ax = h1.GetXaxis () 
    sx = ax.GetNbins ()
    for i in range ( sx , 0 , -1 ) : 
        yield i

        
ROOT.TH1  . __iter__     = _h1_iter_ 
ROOT.TH1F . __iter__     = _h1_iter_ 
ROOT.TH1D . __iter__     = _h1_iter_ 
ROOT.TH1F . __reversed__ = _h1_iter_reversed_ 
ROOT.TH1D . __reversed__ = _h1_iter_reversed_ 

# =============================================================================
## iterator for 2D-histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h2_iter_ ( h2 ) :
    """
    Iterator over 2D-histogram
    
    >>> for i in h2 : print i 
    """
    #
    ax = h2.GetXaxis()
    ay = h2.GetYaxis()
    #
    sx = ax.GetNbins()
    sy = ay.GetNbins()
    #
    for ix in range ( 1 , sx + 1 ) : 
        for iy in range ( 1 , sy + 1 ) : 
            yield ix , iy

# =============================================================================
## iterator for 2D-histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h2_iter_reversed_ ( h2 ) :
    """
    Iterator over 2D-histogram
    
    >>> for i in reversed(h2) : print i 
    """
    #
    ax = h2.GetXaxis()
    ay = h2.GetYaxis()
    #
    sx = ax.GetNbins()
    sy = ay.GetNbins()
    #
    for iy in range ( sy , 0 , -1 ) : 
        for ix in range ( sx , 0 , -1  ) : 
            yield ix , iy


ROOT.TH2  . __iter__     = _h2_iter_ 
ROOT.TH2F . __iter__     = _h2_iter_ 
ROOT.TH2D . __iter__     = _h2_iter_ 
ROOT.TH2F . __reversed__ = _h2_iter_reversed_ 
ROOT.TH2D . __reversed__ = _h2_iter_reversed_ 

# =============================================================================
## iterator for 3D-histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h3_iter_ ( h3 ) :
    """
    Iterator over 3D-histogram
    
    >>> for i in h3 : print i 
    """
    #
    ax = h3.GetXaxis()
    ay = h3.GetYaxis()
    az = h3.GetZaxis()
    #
    sx = ax.GetNbins()
    sy = ay.GetNbins()    
    sz = az.GetNbins()
    #
    for ix in range ( 1 , sx + 1 ) : 
        for iy in range ( 1 , sy + 1 ) : 
            for iz in range ( 1 , sz + 1 ) : 
                yield  ix , iy , iz

# =============================================================================
## iterator for 3D-histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h3_iter_reversed_ ( h3 ) :
    """
    Iterator over 3D-histogram
    
    >>> for i in reversed(h3) : print i 
    """
    #
    ax = h3.GetXaxis()
    ay = h3.GetYaxis()
    az = h3.GetZaxis()
    #
    sx = ax.GetNbins()
    sy = ay.GetNbins()    
    sz = az.GetNbins()
    #
    for iz in range ( sz , 0 , -1 ) : 
        for iy in range ( sy , 0 , -1  ) : 
            for ix in range ( sx , 0 , -1  ) : 
                yield  ix , iy , iz
                
                    
ROOT.TH3  . __iter__     = _h3_iter_ 
ROOT.TH3F . __iter__     = _h3_iter_ 
ROOT.TH3D . __iter__     = _h3_iter_ 
ROOT.TH3F . __reversed__ = _h3_iter_reversed_ 
ROOT.TH3D . __reversed__ = _h3_iter_reversed_ 

# =============================================================================
# interpolate 
# =============================================================================
## linear interpolation 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def interpolate_1D ( x       ,
                     x0 , v0 ,
                     x1 , v1 ) :
    """
    Linear interpolation 
    """
    if hasattr ( x  , 'value' ) : x  = x  . value()
    if hasattr ( x0 , 'value' ) : x0 = x0 . value()
    if hasattr ( x1 , 'value' ) : x1 = x1 . value()

    c1 = ( x - x0 ) / ( x1 - x0 )
    c0 = ( x - x1 ) / ( x0 - x1 )

    return c0 * v0 + c1 * v1

# ========================================================================
## bilinear interpolation 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def interpolate_2D ( x   , y   ,
                     x0  , x1  , 
                     y0  , y1  ,
                     v00 , v01 ,
                     v10 , v11 ) : 
    
    """
    bi-linear interpolation 
    """
    #
    if hasattr ( x  , 'value' ) : x  = x  . value()
    if hasattr ( y  , 'value' ) : y  = y  . value()
    #
    if hasattr ( x0 , 'value' ) : x0 = x0 . value()
    if hasattr ( x1 , 'value' ) : x1 = x1 . value()
    if hasattr ( y0 , 'value' ) : y0 = y0 . value()
    if hasattr ( y1 , 'value' ) : y1 = y1 . value()
    
    c00 =  ( x - x1 ) * ( y - y1 ) / ( x0 - x1 ) / ( y0 - y1 )
    c01 =  ( x - x1 ) * ( y - y0 ) / ( x0 - x1 ) / ( y1 - y0 )
    c10 =  ( x - x0 ) * ( y - y1 ) / ( x1 - x0 ) / ( y0 - y1 )
    c11 =  ( x - x0 ) * ( y - y0 ) / ( x1 - x0 ) / ( y1 - y0 )
    
    return c00 * v00 + c01 * v01 + c10 * v10 + c11 * v11 



## parabolic interpolation
_PARAB = cpp.Gaudi.Math.ParabolaErr

# =============================================================================
## histogram as function 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_call_ ( h1 , x , func = lambda s : s , interpolate = True ) :
    """
    Histogram as function:
    
    >>> histo = ....
    >>> ve    = histo ( x )                       ## default interpolation
    >>> be    = histo ( x , interpolate = False ) ## no interpolation 
    >>> be    = histo ( x , interpolate = True  ) ## default interpolation  
    >>> be    = histo ( x , interpolate = 2     ) ## parabolic interpolation  
    """
    #
    if hasattr ( x , 'value' )  : x = x.value() ## return _h1_call_ ( h1 ,  x.value() )
    #
    ax = h1.GetXaxis (   )
    ix = ax.FindBin  ( x )
    mx = ax.GetXmax  (   )
    mn = ax.GetXmin  (   )
    nb = ax.GetNbins (   )

    if  0      == ix and isequal ( x , mn ) : ix += 1  ## fresh trick
    if  nb + 1 == ix and isequal ( x , mx ) : ix -= 1  ## fresh trick 
    elif not 1 <= ix <= nb               : return VE(0,0)
    #
    val = h1.GetBinContent ( ix ) 
    err = h1.GetBinError   ( ix )
    #
    value = VE ( val , err**2 )
    #
    if not interpolate : return func ( value )   ## RETURN 
    #
    ## try parabolic interpolation 
    if 2 == interpolate and 3 <= nb :

        i,j,k             = ix-1 ,   ix , ix+1
        if i < 1  : i,j,k =    1 ,    2 ,    3
        if k > nb : i,j,k = nb-2 , nb-1 ,   nb

        xi = ax.GetBinCenter(i)
        if x < xi : return func ( value ) ## treat the first bin  (correct?)
        
        xk = ax.GetBinCenter(k)
        if x > xk : return func ( value ) ## treat the last bin   (correct?)
        
        xj = ax.GetBinCenter(j)     ## middle bin
        
        yi = VE ( h1.GetBinContent ( i ) , h1.GetBinError ( i ) ** 2 )
        yj = VE ( h1.GetBinContent ( j ) , h1.GetBinError ( j ) ** 2 )
        yk = VE ( h1.GetBinContent ( k ) , h1.GetBinError ( k ) ** 2 )
        #
        ## create interpolation object:
        #
        parab = _PARAB ( xi , yi , xj , yj , xk , yk )
        #
        return func ( parab ( x ) ) 
    # 
    ## make linear interpolation
    # 
    bc   = ax.GetBinCenter( ix )
    ibin = ix - 1 if x < bc else ix + 1
    #
    if 1<= ibin <= nb : 
        #
        bx =      ax.GetBinCenter  ( ibin )
        bv = VE ( h1.GetBinContent ( ibin ) , h1.GetBinError ( ibin )**2 )
        #
        value = interpolate_1D (
            x  ,
            bc , value ,
            bx , bv    )
        #
    return func ( value )  ## RETURN 
        
ROOT.TH1F  . __getitem__  = _h1_get_item_
ROOT.TH1D  . __getitem__  = _h1_get_item_
ROOT.TH2F  . __getitem__  = _h2_get_item_
ROOT.TH2D  . __getitem__  = _h2_get_item_
ROOT.TH3F  . __getitem__  = _h3_get_item_
ROOT.TH3D  . __getitem__  = _h3_get_item_

ROOT.TH1F  . __call__     = _h1_call_
ROOT.TH1D  . __call__     = _h1_call_

ROOT.TH1   . __len__      = lambda s : s.size() 
ROOT.TH1   .   size       = lambda s : s.GetNbinsX() * s.GetNbinsY() * s.GetNbinsZ() 
ROOT.TH1   . __contains__ = lambda s , i : 1 <= i <= s.size() 

ROOT.TH2   . __len__      = lambda s : s.size() 
ROOT.TH2   .   size       = lambda s : s.GetNbinsX() * s.GetNbinsY() * s.GetNbinsZ()

ROOT.TH3   . __len__      = lambda s : s.size() 
ROOT.TH3   .   size       = lambda s : s.GetNbinsX() * s.GetNbinsY() * s.GetNbinsZ()

ROOT.TH1D  . nbins        = lambda s : s.GetNbinsX() 
ROOT.TH1F  . nbins        = lambda s : s.GetNbinsX() 
ROOT.TH1D  .  bins        = lambda s : s.GetNbinsX() 
ROOT.TH1F  .  bins        = lambda s : s.GetNbinsX() 

ROOT.TH2D  . nbinsx       = lambda s : s.GetNbinsX()
ROOT.TH2D  . nbinsy       = lambda s : s.GetNbinsY()
ROOT.TH2F  . nbinsx       = lambda s : s.GetNbinsX()
ROOT.TH2F  . nbinsy       = lambda s : s.GetNbinsY()
ROOT.TH2D  .  binsx       = lambda s : s.GetNbinsX()
ROOT.TH2D  .  binsy       = lambda s : s.GetNbinsY()
ROOT.TH2F  .  binsx       = lambda s : s.GetNbinsX()
ROOT.TH2F  .  binsy       = lambda s : s.GetNbinsY()


# =============================================================================
## check bin in 2D-histo 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h2_contains_ ( s , ibin ) :
    """
    Check if the bin contains in 3D-histogram:

    >>> (3,5) in h2
    
    """    
    return ibin[0] in s.GetXaxis() and ibin[1] in s.GetYaxis()


ROOT.TH2   . __contains__ = _h2_contains_
ROOT.TH2F  . __contains__ = _h2_contains_
ROOT.TH2D  . __contains__ = _h2_contains_

# ============================================================================
## check bin in 3D-histo 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h3_contains_ ( s , ibin ) :
    """
    Check if the bin contains in 3D-histogram:

    >>> (3,5,10) in h3
    
    """
    return ibin[0] in s.GetXaxis() and \
           ibin[1] in s.GetYaxis() and \
           ibin[2] in s.GetZaxis()


ROOT.TH3   . __contains__ = _h3_contains_
ROOT.TH3F  . __contains__ = _h3_contains_
ROOT.TH3D  . __contains__ = _h3_contains_




# =============================================================================
## number of "empty" bins
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-05-20
def _num_empty_ ( h ) :
    """
    Check number of empty bins :

    >>> h = ...
    >>> e = h.numEmpty()
    """
    ne  = 0 
    for i in h.iteritems() : 
        v = i[-1]
        if 0 == v.value() and 0 == v.cov2 () : ne +=1
    return ne 

ROOT.TH1 . numEmpty = _num_empty_

# =============================================================================
## find bin in 1D-histogram
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_find_ ( h1 , x ) :
    """
    Find the bin in 1D-histogram

    >>> ibin = h1.findBin ( x ) 
    """
    if hasattr ( x , 'value' ) : x = x.value()
    #
    ax = h1.GetXaxis()
    #
    return ax.FindBin ( x )
# =============================================================================
## find bin in 2D-histogram
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h2_find_ ( h2 , x , y ) :
    """
    Find the bin in 3D-histogram

    >>> ibin = h2.findBin ( x , y ) 
    """
    if hasattr ( x , 'value' ) : x = x.value()
    if hasattr ( y , 'value' ) : y = y.value()
    #
    ax = h2.GetXaxis()
    ay = h2.GetYaxis()
    #
    return  ( ax.FindBin ( x ) ,
              ay.FindBin ( y ) )
# =============================================================================
## find bin in 3D-histogram
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h3_find_ ( h3 , x , y , z ) :
    """
    Find the bin in 3D-histogram

    >>> ibin = h3.findBin ( x , y , z ) 
    """
    if hasattr ( x , 'value' ) : x = x.value()
    if hasattr ( y , 'value' ) : y = y.value()
    if hasattr ( z , 'value' ) : z = z.value()
    #
    ax = h3.GetXaxis()
    ay = h3.GetYaxis()
    az = h3.GetZaxis()
    #
    return  ( ax.FindBin ( x ) ,
              ay.FindBin ( y ) ,
              az.FindBin ( z ) )


ROOT.TH1F . findBin  = _h1_find_
ROOT.TH1D . findBin  = _h1_find_
ROOT.TH2F . findBin  = _h2_find_
ROOT.TH2D . findBin  = _h2_find_
ROOT.TH3F . findBin  = _h3_find_
ROOT.TH3D . findBin  = _h3_find_


# =============================================================================
## get mean for 2D-histogram 
#  @code 
#  >>> histo = ...
#  >>> x,y   = histo.mean ()
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-03-20
def _h2_mean_ ( h2 ) :
    """
    Get the mean

    >>> histo = ...
    >>> x,y   = histo.mean () 
    """
    x  = h2.GetMean      (1)
    ex = h2.GetMeanError (1)
    y  = h2.GetMean      (2)
    ey = h2.GetMeanError (2)
    #
    return VE ( x , ex * ex ), VE ( y , ey * ey ) 

ROOT.TH2F . mean = _h2_mean_
ROOT.TH2D . mean = _h2_mean_

# ============================================================================
## find the first X-value for the certain Y-value 
def _h1_find_X ( self             ,
                 v                ,
                 forward   = True ) : 
    
    ##
    if hasattr ( v , 'value' ) : v = v.value()
    ##
    mn , mx  = self.xminmax()
    ##
    if v < mn.value() :
        return hist.xmin() if forward else hist.xmax ()  
    if v > mx.value() :
        return hist.xmax() if forward else hist.xmin () 
    
    nb  = len ( hist ) 
    
    ax  = hist.GetXaxis()
    
    for i in hist :
        
        j  = i+1
        if not j in hist : continue
        
        ib = i if forward else nb + 1 - i 
        jb = j if forward else nb + 1 - j 
        
        vi = hist [ ib ].value () 
        vj = hist [ jb ].value ()
        
        if  vi <= v <= vj or vj <= v <= vi :
            
            xi = ax.GetBinCenter ( ib )
            xj = ax.GetBinCenter ( jb )
            
            if   vi == v             : return xi
            elif vj == v             : return xj 
            
            dv = vi - vj
            dx = xi - xj

            if   vi == vj or 0 == dv : return 0.5 * ( xi + xj ) 
            
            return (v*dx+vi*xj-vj*xi)/dv


# =============================================================================
## histogram as function 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h2_call_ ( h2 , x , y , func = lambda s : s , interpolate = True ) :
    """
    Histogram as function:
    
    >>> histo = ....
    >>> ve    = histo ( x , y ) 
    """    
    #
    if hasattr ( x , 'value' )  : return _h2_call_ ( h2 ,  x.value() , y          )
    if hasattr ( y , 'value' )  : return _h2_call_ ( h2 ,  x         , y.value () )
    #
    ax = h2.GetXaxis (   )
    ix = ax.FindBin  ( x )
    if not  1 <= ix <= ax.GetNbins() : return VE ( 0, 0 )
    #
    ay = h2.GetYaxis (   )
    iy = ay.FindBin  ( y )
    if not  1 <= iy <= ay.GetNbins() : return VE ( 0, 0 )
    ##
    val = h2.GetBinContent ( ix , iy ) 
    err = h2.GetBinError   ( ix , iy )
    #
    value = VE ( val , err*err )
    ##
    if not interpolate : return func ( value )
    ##
    bcx = ax.GetBinCenter ( ix )
    ibx = ix - 1 if x < bcx else ix + 1
    #
    bcy = ay.GetBinCenter ( iy )
    iby = iy - 1 if y < bcy else iy + 1
    #
    if ibx in ax and iby in ay :  ## regular interpolation
        
        bx = ax.GetBinCenter  ( ibx )
        by = ay.GetBinCenter  ( iby )

        value = interpolate_2D (
            x   , y  ,
            bcx , bx ,
            bcy , by ,
            ##
            value    , 
            VE ( h2.GetBinContent (  ix , iby ) , h2.GetBinError (  ix , iby )**2 ) ,
            VE ( h2.GetBinContent ( ibx ,  iy ) , h2.GetBinError ( ibx ,  iy )**2 ) ,
            VE ( h2.GetBinContent ( ibx , iby ) , h2.GetBinError ( ibx , iby )**2 ) )
        
    elif ibx in ax : ## interpolation in X-direction 

        bx = ax.GetBinCenter  ( ibx )
        
        value = interpolate_1D (
            x   ,
            bcx , value , 
            bx  ,
            VE ( h2.GetBinContent ( ibx ,  iy ) , h2.GetBinError ( ibx ,  iy )**2 )
            )

    elif iby in ay : ## interpolation in Y-direction 

        by = ay.GetBinCenter  ( iby )
        
        value = interpolate_1D (
            y   ,
            bcy , value , 
            by  ,
            VE ( h2.GetBinContent ( ix , iby ) , h2.GetBinError ( ix , iby )**2 )
            )

    
    return func ( value )


ROOT.TH2   . __call__     = _h2_call_
ROOT.TH2F  . __getitem__  = _h2_get_item_
ROOT.TH2D  . __getitem__  = _h2_get_item_

# =============================================================================
## histogram as function 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h3_call_ ( h3 , x , y , z , func = lambda s : s , interpolate = True ) :
    """
    Histogram as function:
    
    >>> histo = ....
    >>> ve    = histo ( x , y , z ) 
    """    
    #
    if hasattr ( x , 'value' )  : return _h3_call_ ( h3 ,  x.value() , y          , z          )
    if hasattr ( y , 'value' )  : return _h3_call_ ( h3 ,  x         , y.value () , z          )
    if hasattr ( z , 'value' )  : return _h3_call_ ( h3 ,  x         , y          , z.value () )
    #
    ax  = h3.GetXaxis (   )
    ix  = ax.FindBin  ( x )
    nbx = ax.GetNbins (   )  
    if not  1 <= ix <= nbx : return VE ( 0, 0 )
    #
    ay  = h3.GetYaxis (   )
    iy  = ay.FindBin  ( y )
    nby = ay.GetNbins (   )  
    if not  1 <= iy <= nby : return VE ( 0, 0 )
    ##
    az  = h3.GetZaxis (   )
    iz  = az.FindBin  ( z )
    nbz = az.GetNbins (   )  
    if not  1 <= iz <= nbz : return VE ( 0, 0 )
    ##
    if not interpolate :
        #
        val = h3.GetBinContent ( ix , iy , iz ) 
        err = h3.GetBinError   ( ix , iy , iz )
        #
        return func ( VE ( val , err * err ) ) 
    # 
    ## make trilinear interpolation:
    #  http://en.wikipedia.org/wiki/Trilinear_interpolation
    #
    xc = ax.GetBinCenter ( ix )
    yc = ay.GetBinCenter ( iy )
    zc = az.GetBinCenter ( iz )
    #
    if   1   == ix and x < xc : pass
    elif nbx == ix and x > xc : pass
    #
    if   1   == iy and y < yc : pass
    elif nby == iy and y > yc : pass
    #
    if   1   == iz and z < zc : pass
    elif nbz == iz and z > zc : pass
    #
    if x <= xc : ix0,ix1 = max ( 1 , ix - 1 ) ,             ix
    else       : ix0,ix1 =           ix       , min ( nbx , ix + 1 )
    #
    if y <= yc : iy0,iy1 = max ( 1 , iy - 1 ) ,             iy
    else       : iy0,iy1 =           iy       , min ( nby , iy + 1 )
    #
    if z <= zc : iz0,iz1 = max ( 1 , iz - 1 ) ,             iz
    else       : iz0,iz1 =           iz       , min ( nbz , iz + 1 )
    #
    x0,x1 = ax.GetBinCenter ( ix0 ) , ax.GetBinCenter ( ix1 )
    y0,y1 = ay.GetBinCenter ( iy0 ) , ay.GetBinCenter ( iy1 )
    z0,z1 = az.GetBinCenter ( iz0 ) , az.GetBinCenter ( iz1 )
    #
    if ix0 == ix1 : xd =   0 
    else          : xd = ( x - x0 ) / ( x1 - x0 )
    if iy0 == iy1 : yd =   0 
    else          : yd = ( y - y0 ) / ( y1 - y0 )
    if iz0 == iz1 : zd =   0 
    else          : zd = ( z - z0 ) / ( z1 - z0 )
    #
    v000 = VE ( h3.GetBinContent ( ix0 , iy0 , iz0 ) , h3.GetBinError ( ix0 , iy0 , iz0 ) ** 2 )
    v100 = VE ( h3.GetBinContent ( ix1 , iy0 , iz0 ) , h3.GetBinError ( ix1 , iy0 , iz0 ) ** 2 )
    v010 = VE ( h3.GetBinContent ( ix0 , iy1 , iz0 ) , h3.GetBinError ( ix0 , iy1 , iz0 ) ** 2 )
    v110 = VE ( h3.GetBinContent ( ix1 , iy1 , iz0 ) , h3.GetBinError ( ix1 , iy1 , iz0 ) ** 2 )
    v001 = VE ( h3.GetBinContent ( ix0 , iy0 , iz1 ) , h3.GetBinError ( ix0 , iy0 , iz1 ) ** 2 )
    v101 = VE ( h3.GetBinContent ( ix1 , iy0 , iz1 ) , h3.GetBinError ( ix1 , iy0 , iz1 ) ** 2 )
    v011 = VE ( h3.GetBinContent ( ix0 , iy1 , iz1 ) , h3.GetBinError ( ix0 , iy1 , iz1 ) ** 2 )
    v111 = VE ( h3.GetBinContent ( ix1 , iy1 , iz1 ) , h3.GetBinError ( ix1 , iy1 , iz1 ) ** 2 )
    # 
    c00 = v000 * ( 1 - xd ) + v100 * xd
    c10 = v010 * ( 1 - xd ) + v110 * xd
    c01 = v001 * ( 1 - xd ) + v101 * xd
    c11 = v011 * ( 1 - xd ) + v111 * xd
    #
    c0  = c00  * ( 1 - yd ) + c10  * yd
    c1  = c01  * ( 1 - yd ) + c11  * yd
    #
    return c0  * ( 1 - zd ) + c1   * zd 

ROOT.TH3   . __call__     = _h3_call_
ROOT.TH3F  . __getitem__  = _h3_get_item_
ROOT.TH3D  . __getitem__  = _h3_get_item_

# =============================================================================
# iterate over items
# =============================================================================
## iterate over entries in 1D-histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_iteritems_ ( h1 , low = 1 , high = sys.maxsize ) :
    """
    Iterate over histogram items:
    
    >>> h1 = ...
    >>> for i,x,y in h1.iteritems()  : ...
    
    """
    ax = h1.GetXaxis()
    sx = ax.GetNbins()
    if low  < 1   : low = 1 
    
    for ix in range ( max (      1 , low  )  ,
                      min ( sx + 1 , high ) ) : 
        
        x   =       ax.GetBinCenter ( ix )
        xe  = 0.5 * ax.GetBinWidth  ( ix )
        
        y   =       h1.GetBinContent ( ix )
        ye  =       h1.GetBinError   ( ix )
        
        yield ix, VE(x,xe*xe) , VE ( y,ye*ye)
        

ROOT.TH1F  . iteritems     = _h1_iteritems_
ROOT.TH1D  . iteritems     = _h1_iteritems_


# =============================================================================
## return information about the bin center and width
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-07-27
def _h1_bin_ ( h1 , ibin ) :
    """
    Get the information about the bin center and width 

    >>> h1 = ... ## the histo
    >>> print h1.bin(1)
    
    """
    if not ibin in h1.GetXaxis() : raise IndexError

    ax  = h1.GetXaxis()
    x   =       ax.GetBinCenter ( ibin )
    xe  = 0.5 * ax.GetBinWidth  ( ibin )

    return VE(x,xe*xe)

# =============================================================================
## return information about the bin center and width
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-07-27
def _h2_bin_ ( h2 , *ibin ) :
    """
    Get the information about the bin center and width 

    >>> h2 = ... ## the histo
    >>> print h2.bin(1,2)
    
    """
    if 2 != len ( ibin ) : raise IndexError 
    if not ibin in h2    : raise IndexError
    
    ax  = h2.GetXaxis()
    x   =       ax.GetBinCenter ( ibin[0] )
    xe  = 0.5 * ax.GetBinWidth  ( ibin[0] )

    ay  = h2.GetYaxis()
    y   =       ay.GetBinCenter ( ibin[1] )
    ye  = 0.5 * ay.GetBinWidth  ( ibin[1] )

    return VE(x,xe*xe),VE(y,ye*ye)


# =============================================================================
## return information about the bin center and width
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-07-27
def _h3_bin_ ( h3 , *ibin ) :
    """
    Get the information about the bin center and width 

    >>> h3 = ... ## the histo
    >>> print h3.bin(1,2,15)
    
    """
    if 3 != len ( ibin ) : raise IndexError 
    if not ibin in h3    : raise IndexError
    
    ax  = h3.GetXaxis()
    x   =       ax.GetBinCenter ( ibin[0] )
    xe  = 0.5 * ax.GetBinWidth  ( ibin[0] )

    ay  = h3.GetYaxis()
    y   =       ay.GetBinCenter ( ibin[1] )
    ye  = 0.5 * ay.GetBinWidth  ( ibin[1] )

    az  = h3.GetZaxis()
    z   =       az.GetBinCenter ( ibin[2] )
    ze  = 0.5 * az.GetBinWidth  ( ibin[2] )

    return VE(x,xe*xe),VE(y,ye*ye),VE(z,ze*ze) 

ROOT.TH1F.bin = _h1_bin_
ROOT.TH1D.bin = _h1_bin_
ROOT.TH2F.bin = _h2_bin_
ROOT.TH2D.bin = _h2_bin_
ROOT.TH3F.bin = _h3_bin_
ROOT.TH3D.bin = _h3_bin_

# =============================================================================
## iterate over entries in 2D-histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h2_iteritems_ ( h2 ) :
    """
    Iterate over histogram items:
    
    >>> h2 = ...
    >>> for ix,iy,x,y,z in h2.iteritems() : ...
    
    """
    ax = h2.GetXaxis()
    sx = ax.GetNbins()

    ay = h2.GetYaxis()
    sy = ay.GetNbins()
    
    for ix in range( 1 , sx + 1 ) :  
        x   =       ax.GetBinCenter ( ix )
        xe  = 0.5 * ax.GetBinWidth  ( ix )
        #
        for iy in range ( 1 , sy + 1 ) : 
            #
            y   =       ay.GetBinCenter  ( iy      )
            ye  = 0.5 * ay.GetBinWidth   ( iy      )
            #
            z   =       h2.GetBinContent ( ix , iy )
            ze  =       h2.GetBinError   ( ix , iy )
            #
            yield ix, iy, VE(x,xe*xe) , VE ( y,ye*ye) , VE( z,ze*ze) 
            #
            
        
ROOT.TH2F  . iteritems     = _h2_iteritems_
ROOT.TH2D  . iteritems     = _h2_iteritems_

# =============================================================================
## iterate over entries in 3D-histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h3_iteritems_ ( h3 ) :
    """
    Iterate over histogram items:
    
    >>> h3 = ...
    >>> for ix,iy,iz,x,y,z,V in h3 : ...
    
    """
    ax = h3.GetXaxis ()
    sx = ax.GetNbins ()

    ay = h3.GetYaxis ()
    sy = ay.GetNbins ()

    az = h3.GetZaxis ()
    sz = az.GetNbins ()
    
    for ix in range( 1 , sx + 1 ) :
        #
        x   =       ax.GetBinCenter ( ix )
        xe  = 0.5 * ax.GetBinWidth  ( ix )
        #
        for iy in range ( 1 , sy + 1 ) : 
            #
            y   =       ay.GetBinCenter  ( iy      )
            ye  = 0.5 * ay.GetBinWidth   ( iy      )
            #
            for iz in range ( 1 , sz + 1 ) :
                #
                z   =       az.GetBinCenter  ( iz      )
                ze  = 0.5 * az.GetBinWidth   ( iz      )
                #
                
                v   =       h3.GetBinContent ( ix , iy , iz )
                ve  =       h3.GetBinError   ( ix , iy , iz )
                #
                yield ix, iy, iz , VE(x,xe*xe) , VE ( y,ye*ye) , VE( z,ze*ze) , VE( v, ve*ve) 
                #

            
ROOT.TH3F  . iteritems     = _h3_iteritems_
ROOT.TH3D  . iteritems     = _h3_iteritems_


# =============================================================================
## iterate over items in TAxis
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _a_iteritems_ ( axis ) :
    """
    Iterate over items in axis 
    
    >>> axis = ...
    >>> for ibin,low,center,high in axis.iteritems() : 
    
    """
    for i in axis :

        l = axis.GetBinLowEdge ( i )
        c = axis.GetBinCenter  ( i )
        u = axis.GetBinUpEdge  ( i )

        yield i,l,c,u

ROOT.TAxis. iteritems     = _a_iteritems_


# =============================================================================
## get bin parameters : low- and up-edges 
def _a_get_item_ ( axis , i ) :
    """
    Get bin parameter: low- and up-edges

    >>> axis = ...
    >>> low,high = axis[1]
    
    """
    if not i in axis : raise IndexError
    
    l = axis.GetBinLowEdge ( i )
    u = axis.GetBinUpEdge  ( i )
    
    return l,u

ROOT.TAxis.__getitem__  = _a_get_item_

# =============================================================================
# some minor decoration for 2D-histos 
# =============================================================================

# =============================================================================
## transpose 2D-ihstogram 
def _h2_transpose_ ( h2 ) :
    """
    Transpose 2D-histogram

    >>> h2 = ...
    >>> ht = h2.T()
    
    """
    if hasattr ( h2 , 'GetSumw2' ) and not h2.GetSumw2() : h2.Sumw2()
    #
    xa = h2.GetXaxis()
    ya = h2.GetYaxis()
    #
    hn = h2_axes ( ya , xa )  
    ##
    for i in h2 :
        hn[ (i[1],i[0]) ] = h2[ i ] 

    return hn


ROOT.TH2F.T         = _h2_transpose_
ROOT.TH2D.T         = _h2_transpose_
ROOT.TH2F.Transpose = _h2_transpose_
ROOT.TH2D.Transpone = _h2_transpose_
ROOT.TH2F.transpose = _h2_transpose_
ROOT.TH2D.transpone = _h2_transpose_

# =============================================================================
## Draw 2D-histogram as 'colz'
def _h2_colz_ ( h2 , opts = '' ) :
    """
    Draw 2D-histogram as 'colz'
    
    >>> h2.colz()
    
    """
    return h2.Draw ( 'colz ' + opts )


ROOT.TH2F . colz = _h2_colz_
ROOT.TH2D . colz = _h2_colz_
ROOT.TH2F . Colz = _h2_colz_
ROOT.TH2D . Colz = _h2_colz_

## add it also for TF2 
ROOT.TF2  . colz = _h2_colz_

# =============================================================================
## Draw 2D-histogram as 'text'
def _h2_text_ ( h2 , opts = '' , fmt = '' ) :
    """
    Draw 2D-histogram as 'text'
    
    >>> h2.text( fmt = '5.3f')
    
    """
    if fmt : ROOT.gStyle.SetPaintTextFormat ( fmt )        
    return h2.Draw ( 'text ' + opts )
    
ROOT.TH2F . text = _h2_text_
ROOT.TH2D . text = _h2_text_

# =============================================================================
## Draw 2D-histogram as 'text'
def _h2_texte_ ( h2 , opts = '' , fmt = '' ) :
    """
    Draw 2D-histogram as 'texte'
    
    >>> h2.texte( fmt = '5.2f')
    
    """
    if fmt : ROOT.gStyle.SetPaintTextFormat ( fmt )        
    return h2.Draw ( 'texte ' + opts )
    
ROOT.TH2F . texte = _h2_texte_
ROOT.TH2D . texte = _h2_texte_

# =============================================================================
## get the guess for three major parameters of the histogram:
#    - number of signal events
#    - background level under the signal (per bin)
#    - background slope
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def histoGuess ( histo , mass , sigma ) :
    """
    Get the guess for three major parameters of the histogram:
    - number of signal events
    - background level under the signal (per bin)
    - background slope
    - (minimal and maximal slopes to ensure 'positive' background)

    >>> histo = ...
    >>> signal, background, slope, slope_min, slope_max = histoGuess ( histo , mass , sigma )
    
    """
    tot0  = 0
    bin0  = 0
    tot3r = 0
    bin3r = 0
    tot3l = 0
    bin3l = 0
    tot4  = 0
    bin4  = 0

    axis  = histo.GetXaxis()   
    for ibin in axis:

        xbin = axis.GetBinCenter   ( ibin )
        dx   = ( xbin - mass ) / sigma
        val  = histo.GetBinContent ( ibin )
        
        if   abs ( dx ) < 2 :  ## +-2sigma
            tot0  += val 
            bin0  += 1
        elif   2 < dx  <  4 :  ## 'near' right sideband: 2 4 sigma 
            tot3r += val 
            bin3r += 1
        elif  -4 < dx  < -2 :  ## 'near' left sideband: -4 -2 sigma 
            tot3l += val 
            bin3l += 1
        else :
            tot4  += val 
            bin4  += 1

    bin3 = bin3r + bin3l + bin4
    tot3 = tot3r + tot3l + tot4 

    p00 = 0
    p03 = 0
    p04 = 0
    
    if bin3          : p03 = float(tot3)/bin3 
    if bin0 and bin3 : p00 = max ( float(tot0)-bin0*p03 , 0 ) 

    if bin3r and bin3l and tot3r and tot3l : 
        p04 = ( tot3r - tot3l ) / ( tot3r + tot3l ) / 3 / sigma 
    
    _xmin = axis.GetXmin() - 0.5* axis.GetBinWidth ( 1               )
    _xmax = axis.GetXmax() + 0.5* axis.GetBinWidth ( axis.GetNbins() )
    
    s1   = -1.0 / ( _xmin - mass )
    s2   = -1.0 / ( _xmax - mass )
    
    smin = min ( s1 , s2 )
    smax = max ( s1 , s2 )
    
    # if   p04 < smin : p04 = smin
    # elif p04 > smax : p04 = smax 
    
    return p00, p03, p04 , smin , smax  


ROOT.TH1F . histoGuess = histoGuess
ROOT.TH1D . histoGuess = histoGuess

# =============================================================================
## use likelihood in histogram fit ? 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def useLL ( histo         ,
            minc  = 10    ,
            diff  = 1.e-5 ) :
    """
    Use Likelihood in histogram fit?
    """    
    minv = 1.e+9    
    for ibin in histo : 

        v = histo [ ibin ]
        
        if 0 > v.value()         : return False
        if not _int ( v , diff ) : return False 
        
        minv = min ( minv , v.value() )
        
    return  minv < abs ( minc ) 


ROOT.TH1.useLL = useLL

# =============================================================================
## Natural histogram with all integer entries ? (generic version)
#  @code
#  h = ...
#  if h.natural() : print 'OK!'
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def allInts ( histo         ,
              diff  = 1.e-4 ) :
    """
    Natural histogram with all integer entries ?
    >>> h = ...
    >>> if h.natural() : print 'OK!'
    """
    
    for ibin in histo : 
        v = histo [ ibin ]
        if not natural_entry ( v ) : return False   ## RETURN 
        
    return True

ROOT.TH1.allInts = allInts
ROOT.TH1.natural = allInts

# =============================================================================
## Natural histogram with all integer entries ? (a bit faster vection for TH1)
#  @code
#  h1 = ...
#  if h1.natural() : print 'OK!'
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_allInts ( h1            ,
                  diff  = 1.e-4 ) :
    """
    Natural histogram with all integer entries ?
    >>> h1 = ...
    >>> if h1.natural() : print 'OK!'
    """
    
    for i,v in histo.iteritems() :  
        if not natural_entry ( v ) : return False   ## RETURN 
        
    return True

for t in ( ROOT.TH1F , ROOT.TH1D ) : 
    t.allInts = _h1_allInts
    t.natural = _h1_allInts


# =============================================================================
## calculate the efficiency histogram using the binomial errors
#  @code 
#  >>> accepted   = ...
#  >>> total      = ...
#  >>> efficiency = acepted // total
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def binomEff_h1 ( h1 , h2 , func = binomEff ) :
    """
    Calculate the efficiency histogram using the binomial errors
    
    >>> accepted   = ...
    >>> total      = ...
    >>> efficiency = accepted // total
    
    """
    #
    if isinstance ( h1 , ROOT.TProfile ) :
        hh = h1.asH1()
        return binomEff_h1 ( hh , h2 , func )
                    
    if                                 not h1.GetSumw2() : h1.Sumw2()
    if hasattr ( h2 , 'GetSumw2' ) and not h2.GetSumw2() : h2.Sumw2()
    #
    result = h1.Clone( hID () )
    if not result.GetSumw2() : result.Sumw2()
    #
    for i1,x1,y1 in h1.iteritems() :
        #
        assert ( _int ( y1 ) )
        #
        y2 = h2 ( x1.value() ) 
        assert ( _int ( y2 ) )
        #
        l1 = long ( y1.value () )
        l2 = long ( y2.value () )
        #
        assert ( l1 <= l2 )
        #
        v = VE ( func ( l1 , l2 ) ) 
        #
        if not v.isfinite() : continue 
        #
        result.SetBinContent ( i1 , v.value () ) 
        result.SetBinError   ( i1 , v.error () )
        
    return result 


ROOT.TH1F.  binomEff    = binomEff_h1 
ROOT.TH1D.  binomEff    = binomEff_h1 

# =============================================================================
## calculate the efficiency histogram using the binomial errors 
#  @code 
#  >>> accepted   = ...
#  >>> total      = ...
#  >>> efficiency = acepted // total
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def binomEff_h2 ( h1 , h2 , func = binomEff ) :
    """
    Calculate the efficiency histogram using the binomial errors

    >>> accepted   = ...
    >>> total      = ...
    >>> efficiency = accepted // total
    
    """
    #
    if                                 not h1.GetSumw2() : h1.Sumw2()
    if hasattr ( h2 , 'GetSumw2' ) and not h2.GetSumw2() : h2.Sumw2()
    #
    result = h1.Clone( hID () )
    if not result.GetSumw2() : result.Sumw2()
    #
    for ix1,iy1,x1,y1,z1 in h1.iteritems() :
        #
        assert ( _int ( z1 ) )
        #
        z2 = h2 ( x1.value() , y1.value() ) 
        assert ( _int ( z2 ) )
        #
        l1 = long ( z1.value () )
        l2 = long ( z2.value ()  )
        #
        assert ( l1 <= l2 )
        #
        v = VE ( func ( l1 , l2 ) ) 
        #
        if not v.isfinite() : continue 
        #
        result.SetBinContent ( ix1 , iy1 , v.value () ) 
        result.SetBinError   ( ix1 , iy1 , v.error () ) 
        
    return result 

ROOT.TH2F.  binomEff    = binomEff_h2 
ROOT.TH2D.  binomEff    = binomEff_h2 

# =============================================================================
## calculate the efficiency histogram using the binomial errors 
#  @code 
#  >>> accepted   = ...
#  >>> total      = ...
#  >>> efficiency = acepted // total
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def binomEff_h3 ( h1 , h2 , func = binomEff ) :
    """
    Calculate the efficiency histogram using the binomial errors

    >>> accepted   = ...
    >>> total      = ...
    >>> efficiency = accepted // total
    
    """
    #
    if                                 not h1.GetSumw2() : h1.Sumw2()
    if hasattr ( h2 , 'GetSumw2' ) and not h2.GetSumw2() : h2.Sumw2()
    #
    result = h1.Clone( hID () )
    if not result.GetSumw2() : result.Sumw2()
    #
    for ix1,iy1,iz1,x1,y1,z1,v1 in h1.iteritems() :
        #
        assert ( _int ( v1 ) )
        #
        v2 = h2 ( x1.value() , y1.value() , z1.value() ) 
        assert ( _int ( v2 ) )
        #
        l1 = long ( v1.value () )
        l2 = long ( v2.value ()  )
        #
        assert ( l1 <= l2 )
        #
        v = VE ( func ( l1 , l2 ) ) 
        #
        if not v.isfinite() : continue 
        #
        result.SetBinContent ( ix1 , iy1 , iz1 , v.value () ) 
        result.SetBinError   ( ix1 , iy1 , iz1 , v.error () ) 
        
    return result 

ROOT.TH3F .  binomEff     = binomEff_h3 
ROOT.TH3D .  binomEff     = binomEff_h3 

ROOT.TH1F . __floordiv__  = binomEff_h1 
ROOT.TH1D . __floordiv__  = binomEff_h1 
ROOT.TH2F . __floordiv__  = binomEff_h2
ROOT.TH2D . __floordiv__  = binomEff_h2
ROOT.TH3F . __floordiv__  = binomEff_h3
ROOT.TH3D . __floordiv__  = binomEff_h3


# =============================================================================
## calculate the efficiency histogram using the binomial errors
#  @code 
#  >>> accepted   = ...
#  >>> total      = ...
#  >>> efficiency = acepted // total
#  @endcode 
#  @see Gaudi::Math::zechEff
#  @param h1 histogram of "accepted" sample
#  @param h2 histogram of "total"    sample 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def zechEff_h1 ( h1 , h2 ) :
    """
    Calculate the efficiency histogram using the binomial errors
    
    >>> accepted  = ... ##  histogram for accepted sample 
    >>> total     = ... ##  histogram for total    sample 
    >>> efficiency = accepted % total
    
    """
    func = zechEff 
    #
    if isinstance ( h1 , ROOT.TProfile ) :
        hh = h1.asH1()
        return zechEff_h1 ( hh , h2 )
    
    if                                 not h1.GetSumw2() : h1.Sumw2()
    if hasattr ( h2 , 'GetSumw2' ) and not h2.GetSumw2() : h2.Sumw2()
    #
    result = h1.Clone( hID () )
    if not result.GetSumw2() : result.Sumw2()
    #
    for i1,x1,y1 in h1.iteritems() :
        #
        y2 = h2 ( x1.value() ) 
        #
        v  = VE ( func ( y1 , y2 ) )
        #
        if v.cov2() < 0 :
            ## if uncertainty is unphysical, make simple divizion
            v = y1/y2 
            #
        if not v.isfinite() : continue 
        #
        result.SetBinContent ( i1 , v.value () ) 
        result.SetBinError   ( i1 , v.error () )
        
    return result 

ROOT.TH1F.  zechEff    = zechEff_h1 
ROOT.TH1D.  zechEff    = zechEff_h1 

# =============================================================================
## calculate the efficiency histogram using the binomial errors
#  @code 
#  >>> accepted   = ...
#  >>> total      = ...
#  >>> efficiency = acepted // total
#  @endcode 
#  @see Gaudi::Math::zechEff
#  @param h1 histogram of "accepted" sample
#  @param h2 histogram of "total"    sample 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def zechEff_h2 ( h1 , h2 ) :
    """
    Calculate the efficiency histogram using the binomial errors
    
    >>> accepted  = ... ##  histogram for accepted sample 
    >>> total     = ... ##  histogram for total    sample 
    >>> efficiency = accepted % total
    
    """
    func = zechEff 
    #
    if                                 not h1.GetSumw2() : h1.Sumw2()
    if hasattr ( h2 , 'GetSumw2' ) and not h2.GetSumw2() : h2.Sumw2()
    #
    result = h1.Clone( hID () )
    if not result.GetSumw2() : result.Sumw2()
    #
    for ix1,iy1,x1,y1,z1 in h1.iteritems() :
        #
        z2 = h2 ( x1.value() , y1.value() ) 
        #
        v  = VE ( func ( z1 , z2 ) )
        #
        if v.cov2() < 0 :
            ## if uncertainty is unphysical, make simple divizion
            v = z1/z2 
            #
        if not v.isfinite() : continue 
        #
        result.SetBinContent ( ix1 , iy1 , v.value () ) 
        result.SetBinError   ( ix1 , iy1 , v.error () ) 
        
    return result 

ROOT.TH2F.  zechEff    = zechEff_h2
ROOT.TH2D.  zechEff    = zechEff_h2 

# =============================================================================
## calculate the efficiency histogram using the binomial errors
#  @code 
#  >>> accepted   = ...
#  >>> total      = ...
#  >>> efficiency = acepted // total
#  @endcode 
#  @see Gaudi::Math::zechEff
#  @param h1 histogram of "accepted" sample
#  @param h2 histogram of "total"    sample 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def zechEff_h3 ( h1 , h2 ) :
    """
    Calculate the efficiency histogram using the binomial errors
    
    >>> accepted  = ... ##  histogram for accepted sample 
    >>> total     = ... ##  histogram for total    sample 
    >>> efficiency = accepted % total
    
    """
    func = zechEff 
    #
    if                                 not h1.GetSumw2() : h1.Sumw2()
    if hasattr ( h2 , 'GetSumw2' ) and not h2.GetSumw2() : h2.Sumw2()
    #
    result = h1.Clone( hID () )
    if not result.GetSumw2() : result.Sumw2()
    #
    for ix1,iy1,iz1,x1,y1,z1,v1 in h1.iteritems() :
        #
        v2 = h2 ( x1.value() , y1.value() , z1.value() ) 
        #
        v  = VE ( func ( v1 , v2 ) )
        #
        if v.cov2() < 0 :
            ## if uncertainty is unphysical, make simple divizion
            v = v1/v2 
            #
        if not v.isfinite() : continue 
        #
        result.SetBinContent ( ix1 , iy1 , iz1 , v.value () ) 
        result.SetBinError   ( ix1 , iy1 , iz1 , v.error () ) 

        
    return result 

ROOT.TH3F .  zechEff = zechEff_h3
ROOT.TH3D .  zechEff = zechEff_h3

ROOT.TH1F . __mod__  = zechEff_h1 
ROOT.TH1D . __mod__  = zechEff_h1 
ROOT.TH2F . __mod__  = zechEff_h2
ROOT.TH2D . __mod__  = zechEff_h2
ROOT.TH3F . __mod__  = zechEff_h3
ROOT.TH3D . __mod__  = zechEff_h3


# =============================================================================
## consider object as function:
def objectAsFunction ( obj ) :

    if   isinstance ( obj , ( int , long , float ) ) :
        
        val  = float  ( obj ) 
        func = lambda x,*y : VE ( val , 0 )
        return func                                     ## RETURN
    
    elif isinstance ( obj  ,    VE ) :
        
        val  =     VE ( obj )
        func = lambda x,*y : val
        return func                                      ## RETURN
    
    elif isinstance ( obj ,   ROOT.TF1 ) :
        
        f1   = obj
        func = lambda x,*y     : VE ( f1 ( float ( x ) , 0 ) )
        return func                                      ## RETURN 

    elif isinstance ( obj ,   ROOT.TF2 ) :
        
        f2   = obj
        func = lambda x,y,*z   : VE ( f2 ( float ( x ) , float ( y ) , 0 ) )
        return func                                      ## RETURN 

    elif isinstance ( obj ,   ROOT.TF3 ) :
        
        f3   = obj
        func = lambda x,y,z,*t : VE ( f3 ( float ( x ) , float ( y ) , float ( z ) , 0 ) )
        return func                                      ## RETURN 

    ## the original stuff 
    return obj

# =============================================================================
## operation with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_oper_ ( h1 , h2 , oper ) :
    """
    Operation with the histogram

    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 (oper) h2 
    """
    if isinstance ( h1 , ROOT.TProfile ) :
        hh = h1.asH1()
        return _h1_oper_ ( hh , h2 , oper ) 
    #
    if                                 not h1.GetSumw2() : h1.Sumw2()
    if hasattr ( h2 , 'GetSumw2' ) and not h2.GetSumw2() : h2.Sumw2()
    #
    result = h1.Clone( hID() )
    if not result.GetSumw2() : result.Sumw2()

    ## 
    f2 = objectAsFunction ( h2 )
    
    ##
    for i1,x1,y1 in h1.iteritems() :
        #
        result.SetBinContent ( i1 , 0 ) 
        result.SetBinError   ( i1 , 0 )
        #
        y2 = f2 ( x1.value() ) 
        #
        v  = VE ( oper ( y1 , y2 ) ) 
        #
        if not v.isfinite() : continue 
        #
        result.SetBinContent ( i1 , v.value () ) 
        result.SetBinError   ( i1 , v.error () )
        
    return result

# =============================================================================
## operation with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_ioper_ ( h1 , h2 , oper ) :
    """
    Operation with the histogram 
    """
    
    if                                 not h1.GetSumw2() : h1.Sumw2()
    if hasattr ( h2 , 'GetSumw2' ) and not h2.GetSumw2() : h2.Sumw2()
    #
    f2 = objectAsFunction ( h2 ) 
    ##
    for i1,x1,y1 in h1.iteritems() :
        #
        y2 = f2 ( x1.value() ) 
        #
        v  = VE ( oper ( y1 , y2 ) ) 
        #
        if not v.isfinite() : continue 
        #
        h1.SetBinContent ( i1 , v.value () ) 
        h1.SetBinError   ( i1 , v.error () )
        
    return h1 

# =============================================================================
##  Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_div_ ( h1 , h2 ) :
    """
    Divide the histograms 
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 / h2  
    """
    #
    return _h1_oper_ ( h1 , h2 , lambda x,y : x/y ) 
# =============================================================================
##  Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_mul_ ( h1 , h2 ) :
    """
    Multiply the histograms 
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 * h2  
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : x*y ) 
# =============================================================================
##  Addition with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_add_ ( h1 , h2 ) :
    """
    Add the histograms 
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 + h2  
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : x+y ) 
# =============================================================================
##  Subtraction of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_sub_ ( h1 , h2 ) :
    """
    Subtract the histogram 
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 - h2  
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : x-y ) 
# =============================================================================
##  Fraction of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_frac_ ( h1 , h2 ) :
    """
    ``Fraction'' the histogram h1/(h1+h2)
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1.frac  ( h2 ) 
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : x.frac(y) ) 
# =============================================================================
##  ``Asymmetry'' of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_asym_ ( h1 , h2 ) :
    """
    ``Asymmetry'' the histogram (h1-h2)/(h1+h2)
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1.asym ( h2 ) 
    
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : x.asym(y) ) 
# =============================================================================
## ``Difference'' of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_diff_ ( h1 , h2 ) :
    """
    ``Difference'' the histogram 2*(h1-h2)/(h1+h2)
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1.diff ( h2 ) 
    
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : 2*x.asym(y) ) 
# =============================================================================
##  ``Chi2-tension'' of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_chi2_ ( h1 , h2 ) :
    """
    ``Chi2-tension'' the histogram
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1.chi2  ( h2 ) 
    
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : VE ( x.chi2 ( y ) , 0 ) ) 
# =============================================================================
##  ``Average'' of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_mean_ ( h1 , h2 ) :
    """
    ``Mean'' the histograms
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1.average  ( h2 ) 
    
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : x.mean ( y ) ) 

# =============================================================================
## 'pow' the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_pow_ ( h1 , val ) :
    """
    ``pow'' the histogram 
    """
    if isinstance ( h1 , ROOT.TProfile ) :
        hh = h1.asH1()
        return _h1_pow_ ( hh , val )
    #
    if not h1.GetSumw2() : h1.Sumw2()
    #
    result = h1.Clone( hID() )
    if not result.GetSumw2() : result.Sumw2()
    #
    for i1,x1,y1 in h1.iteritems() :
        #
        result.SetBinContent ( i1 , 0 ) 
        result.SetBinError   ( i1 , 0 )
        #
        v = VE ( pow ( y1 , val ) ) 
        #
        if not v.isfinite() : continue 
        #
        result.SetBinContent ( i1 , v.value () ) 
        result.SetBinError   ( i1 , v.error () )
        
    return result 


# =============================================================================
## 'abs' the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-04-29
def _h1_abs_ ( h1 ) :
    """
    ``abs'' the histogram

    >>> h      = ...
    >>> result = abs ( h )
    """
    if not h1.GetSumw2() : h1.Sumw2()
    #
    result = h1.Clone( hID() )
    if not result.GetSumw2() : result.Sumw2()
    #
    for i1,x1,y1 in h1.iteritems() :
        #
        result.SetBinContent ( i1 , 0 ) 
        result.SetBinError   ( i1 , 0 )
        #
        v = abs ( y1 )  
        #
        result.SetBinContent ( i1 , v.value () ) 
        result.SetBinError   ( i1 , v.error () )
        
    return result 

# =============================================================================
## Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_idiv_ ( h1 , h2 ) :
    """
    Divide the histograms 
    
    >>> h1  = ...
    >>> h2  = ...
    >>> h1 /=  h2 
    
    """
    return _h1_ioper_ ( h1 , h2 , lambda x,y : x/y ) 

# =============================================================================
## Multiplication with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_imul_ ( h1 , h2 ) :
    """
    Multiply the histograms 
    
    >>> h1  = ...
    >>> h2  = ...
    >>> h1 *=  h2 
    
    """
    return _h1_ioper_ ( h1 , h2 , lambda x,y : x*y ) 

# =============================================================================
## Addition with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_iadd_ ( h1 , h2 ) :
    """
    Add the histograms
    
    >>> h1  = ...
    >>> h2  = ...
    >>> h1 +=  h2 
    
    """
    return _h1_ioper_ ( h1 , h2 , lambda x,y : x+y ) 
# =============================================================================
##  Subtraction of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_isub_ ( h1 , h2 ) :
    """
    Subtract the histogram
    
    >>> h1  = ...
    >>> h2  = ...
    >>> h1 -=  h2 
    
    """
    return _h1_ioper_ ( h1 , h2 , lambda x,y : x-y ) 

# =============================================================================
## Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_rdiv_ ( h1 , h2 ) :
    """
    Divide the histograms

    >>> h1     = ...
    >>> obj    = ...
    >>> result = obj / h1 
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : y/x ) 
# =============================================================================
## Multiplication with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_rmul_ ( h1 , h2 ) :
    """
    Multiply the histograms 

    >>> h1     = ...
    >>> obj    = ...
    >>> result = obj * h1 
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : y*x ) 

# =============================================================================
## Addition with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_radd_ ( h1 , h2 ) :
    """
    Add the histograms
    
    >>> h1     = ...
    >>> obj    = ...
    >>> result = obj + h1 
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : y+x )

# =============================================================================
## Subtraction of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_rsub_ ( h1 , h2 ) :
    """
    Subtract the histogram
            
    >>> h1     = ...
    >>> obj    = ...
    >>> result = obj - h1 
    """
    return _h1_oper_ ( h1 , h2 , lambda x,y : y-x ) 

# =============================================================================
## Feed the histogram from other object, e.g. function
#  @code
#  h1 = ....         ## create the histogram 
#  math.sin  >> h1   ## feed it using function  
#  @endcode
#  The action is equivalent to
#  @code
#  h1.Reset()        ## reset it 
#  h1 += math.sin    ## update it using function  
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_rrshift_ ( h1 , obj ) :
    """
    Feed the histogram from external source, e.g. function
    
    >>> h1 = ...        ## the histogram
    >>> math.sin >> h1  ## feed it from the function
    
    """
    h1.Reset() 
    h1 += obj
    
    return h1

for t in ( ROOT.TH1F , ROOT.TH1D ) : 
    
    t . _oper_      = _h1_oper_
    t . _ioper_     = _h1_ioper_
    t . __div__     = _h1_div_
    t . __mul__     = _h1_mul_
    t . __add__     = _h1_add_
    t . __sub__     = _h1_sub_
    t . __pow__     = _h1_pow_
    
    t . __idiv__    = _h1_idiv_
    t . __imul__    = _h1_imul_
    t . __iadd__    = _h1_iadd_
    t . __isub__    = _h1_isub_
    
    t . __rdiv__    = _h1_rdiv_
    t . __rmul__    = _h1_rmul_
    t . __radd__    = _h1_radd_
    t . __rsub__    = _h1_rsub_

    t . __rrshift__ = _h1_rrshift_
    
    t . __abs__     = _h1_abs_
    t .  frac       = _h1_frac_
    t .  asym       = _h1_asym_
    t .  diff       = _h1_diff_
    t .  chi2       = _h1_chi2_
    t .  average    = _h1_mean_

# =============================================================================
## find the first X-value for the given Y-value 
def _h1_xfind_ ( self           ,
                 v              ,
                 forward = True ) :
    """
    
    Find the first X-value for the given Y-value

    >>> h1 = ...
    >>> x = h1.find_X ( 1000 )

    >>> h1 = ...
    >>> x = h1.find_X ( 1000 , False )
    
    """
    #
    if hasattr (  v , 'value' ) : v = v.value()
    #
    mn = self [ self.GetMinimumBin() ]
    mx = self [ self.GetMaximumBin() ]

    #
    if v < mn.value() :
        return self.xmin() if forward else self.xmax ()
    #
    if v > mx.value() :
        return self.xmax() if forward else self.xmin () 
    
    nb = len ( self )
    ax = self.GetXaxis () 
    
    for i in self :
        
        j  = i+1
        if not j in self : continue
        
        ib = i if forward else nb + 1 - i 
        jb = j if forward else nb + 1 - j 
        
        vi = self [ ib ].value () 
        vj = self [ jb ].value ()
        
        if  vi <= v <= vj or vj <= v <= vi :
            
            xi = ax.GetBinCenter ( ib )
            xj = ax.GetBinCenter ( jb )
            
            if   vi == v             : return xi   ## RETURN 
            elif vj == v             : return xj   ## RETURN 
            
            dv = vi - vj
            dx = xi - xj
            
            if   vi == vj or 0 == dv : return 0.5 * ( xi + xj )  ## RETURN
            
            return (v*dx+vi*xj-vj*xi)/dv                         ## RETURN
        

ROOT.TH1F . find_X = _h1_xfind_
ROOT.TH1D . find_X = _h1_xfind_


# =======================================================================
## get "n-sigma" interval assuming the presence of peak
def _h1_CL_interval_ ( self       ,
                       nsigma = 1 ) : 
    """
    Get ``n-sigma'' interval for the distribution.

    >>> h = ...
    >>> xlow,xhigh = h.cl_interval( 1 ) 
    """
    mv   = self.maxv()
    import math 
    
    try :
        #
        f    = math.exp    ( -0.5 * nsigma * nsigma )
        x1   = self.find_X ( mv * f , True  )
        x2   = self.find_X ( mv * f , False )
        #
    except :
        #
        x1,x2 = self.xminmax () 
        
    return x1,x2 
    
ROOT.TH1F . cl_interval = _h1_CL_interval_
ROOT.TH1D . cl_interval = _h1_CL_interval_
            
# =============================================================================
## get the minumum value for the histogram 
def _h_minv_ ( self ) :
    """
    Get the minimum value for the histogram

    >>> h  = ...
    >>> mv = h.minv ()
    """
    mv = VE ( 1.e+100 , -1 ) 
    for ibin in self :
        v = self[ibin]
        if v.value() <= mv.value() or mv.error() < 0 : mv = v
    return mv 

# =============================================================================
## get the maximum value for the histogram 
def _h_maxv_ ( self ) :
    """
    Get the minimum value for the histogram

    >>> h  = ...
    >>> mv = h.maxv ()
    """
    mv = VE ( -1.e+100 , -1 ) 
    for ibin in self :
        v = self[ibin]
        if v.value() >= mv.value() or mv.error() < 0 : mv = v
    return mv 
        
# =============================================================================
## get the minmaximum values for the histogram 
def _h_minmax_ ( self ) :
    """
    Get the minmax pair for the histogram
    
    >>> h     = ...
    >>> mn,mx = h.minmax ()
    """
    return self.minv() , self.maxv() 

ROOT.TH1 . minv    = _h_minv_
ROOT.TH1 . maxv    = _h_maxv_
ROOT.TH1 . minmax  = _h_minmax_

# ============================================================================
## get x-range for the function:
def _f_xminmax_ ( self ) :
    """
    Get x-range for the function

    >>> f = ...
    >>> xmn,xmx = f.xminmax()
    """
    return self.GetXmin(),self.GetXmax()

# ============================================================================
## get minmax-range for the function:
def _f_minmax_ ( self ) :
    """
    Get minmax-range for the function

    >>> f = ...
    >>> mn,mx = f.minmax()
    """
    return self.GetMinimum(),self.GetMaximum()


ROOT.TF1 .  minv    =  lambda s : s.GetMinimum()
ROOT.TF1 .  maxv    =  lambda s : s.GetMaximum()
ROOT.TF1 .  minmax  = _f_minmax_
ROOT.TF1 . xminmax  = _f_xminmax_

import LHCbMath.deriv 
_tf1 = ROOT.TF1
if not hasattr ( _tf1 , 'mean'     ) : _tf1.mean     = LHCbMath.deriv.mean 
if not hasattr ( _tf1 , 'variance' ) : _tf1.variance = LHCbMath.deriv.variance 
if not hasattr ( _tf1 , 'rms'      ) : _tf1.rms      = LHCbMath.deriv.rms  
if not hasattr ( _tf1 , 'mediane'  ) : _tf1.mediane  = LHCbMath.deriv.mediane
if not hasattr ( _tf1 , 'mode'     ) : _tf1.mode     = LHCbMath.deriv.mode 
if not hasattr ( _tf1 , 'moment'   ) : _tf1.moment   = LHCbMath.deriv.moment
if not hasattr ( _tf1 , 'quantile' ) : _tf1.quantile = LHCbMath.deriv.quantile            

# ============================================================================
## get the minimum value for X-axis 
def _ax_min_ ( self ) :
    """
    Get the minimum value for X-axis

    >>> xmin = h.xmin()
    """
    ax = self.GetXaxis () 
    return ax.GetXmin()
# ============================================================================
## get the minimum value for y-axis 
def _ay_min_ ( self ) :
    """
    Get the minimum value for Y-axis

    >>> ymin = h.ymin()
    """
    ay = self.GetYaxis () 
    return ay.GetXmin()
# ============================================================================
## get the minimum value for z-axis 
def _az_min_ ( self ) :
    """
    Get the minimum value for Z-axis

    >>> zmin = h.zmin()
    """
    az = self.GetZaxis () 
    return az.GetXmin()

# ============================================================================
## get the maximum value for X-axis 
def _ax_max_ ( self ) :
    """
    Get the maximum value for X-axis

    >>> xmax = h.xmax()
    """
    ax = self.GetXaxis () 
    return ax.GetXmax()
# ============================================================================
## get the maximum value for y-axis 
def _ay_max_ ( self ) :
    """
    Get the maximum value for Y-axis

    >>> ymax = h.ymax()
    """
    ay = self.GetYaxis () 
    return ay.GetXmax()
# ============================================================================
## get the maximum value for z-axis 
def _az_max_ ( self ) :
    """
    Get the maximum value for Z-axis

    >>> zmax = h.zmax()
    """
    az = self.GetZaxis () 
    return az.GetXmax()

ROOT.TH1D. xmin    = _ax_min_
ROOT.TH1D. xmax    = _ax_max_
ROOT.TH1D. ymin    = _h_minv_
ROOT.TH1D. ymax    = _h_maxv_

ROOT.TH1F. xmin    = _ax_min_
ROOT.TH1F. xmax    = _ax_max_
ROOT.TH1F. ymin    = _h_minv_
ROOT.TH1F. ymax    = _h_maxv_

ROOT.TH2 . xmin    = _ax_min_
ROOT.TH2 . xmax    = _ax_max_
ROOT.TH2 . ymin    = _ay_min_
ROOT.TH2 . ymax    = _ay_max_
ROOT.TH2 . zmin    = _h_minv_
ROOT.TH2 . zmax    = _h_maxv_

ROOT.TH3 . xmin    = _ax_min_
ROOT.TH3 . xmax    = _ax_max_
ROOT.TH3 . ymin    = _ay_min_
ROOT.TH3 . ymax    = _ay_max_
ROOT.TH3 . zmin    = _az_min_
ROOT.TH3 . zmax    = _az_max_

ROOT.TH1D. xminmax = lambda s : ( s.xmin() , s.xmax() )
ROOT.TH1D. yminmax = lambda s : ( s.ymin() , s.ymax() )
ROOT.TH1F. xminmax = lambda s : ( s.xmin() , s.xmax() )
ROOT.TH1F. yminmax = lambda s : ( s.ymin() , s.ymax() )

ROOT.TH2 . xminmax = lambda s : ( s.xmin() , s.xmax() )
ROOT.TH2 . yminmax = lambda s : ( s.ymin() , s.ymax() )
ROOT.TH2 . zminmax = lambda s : ( s.zmin() , s.zmax() )

ROOT.TH3 . xminmax = lambda s : ( s.xmin() , s.xmax() )
ROOT.TH3 . yminmax = lambda s : ( s.ymin() , s.ymax() )
ROOT.TH3 . zminmax = lambda s : ( s.zmin() , s.zmax() )


# =============================================================================
## get the random pair from 2D-histogram
#  @code 
#  >>> h2  = ...
#  >>> x,y = h2.random() 
#  @endcode
#  @see ROOT::TH2::GetRandom2
#  @see ROOT::TH2
def _h2_random_ ( h2 ) :
    """
    get the random value from 2D-histogram
    
    >>> h2  = ...
    >>> x,y = h2.random() 
    """
    #
    _x = ROOT.Double(0.0)
    _y = ROOT.Double(1.0)
    h2.GetRandom2( _x , _y )
    #
    return float(_x) , float(_y)

# =============================================================================
## get the random triplet from 3D-histogram
#  @code 
#  >>> h3    = ...
#  >>> x,y,z = h3.random() 
#  @endcode
#  @see ROOT::TH3::GetRandom3
#  @see ROOT::TH3
def _h3_random_ ( h3 ) :
    """
    get the random value from 2D-histogram
    
    >>> h3    = ...
    >>> x,y,z = h3.random() 
    """
    #
    _x = ROOT.Double(0.0)
    _y = ROOT.Double(1.0)
    _z = ROOT.Double(2.0)
    h3.GetRandom3( _x , _y ,_z)
    #
    return float(_x) , float(_y) , float(_z) 

ROOT.TH2F.random = _h2_random_
ROOT.TH2D.random = _h2_random_

ROOT.TH3F.random = _h3_random_
ROOT.TH3D.random = _h3_random_

ROOT.TH1F.random = lambda s : s.GetRandom() 
ROOT.TH1D.random = lambda s : s.GetRandom() 

# =============================================================================
## convert TProfile to TH1D  (needed for the proper math) 
#  @see ROOT::TProfile
#  @see ROOT::TH1D 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-08-28
def _prof_asH1_ ( p ) :
    """ 
    Convert TProfile to TH1D (needed for the proper math)

    >>> profile = ...
    >>> h1      = profile.asH1()
    """
    h1 = h1_axis ( p.GetXaxis() , double = True )
    ## copy the content bin-by-bin
    for i in h1 : h1[i] = p[i]
    ##
    return h1 

ROOT.TProfile . asH1     = _prof_asH1_ 
ROOT.TProfile . toH1     = _prof_asH1_
ROOT.TProfile . asHisto  = _prof_asH1_

ROOT.TProfile . __idiv__ = NotImplemented 
ROOT.TProfile . __imul__ = NotImplemented 
ROOT.TProfile . __iadd__ = NotImplemented 
ROOT.TProfile . __isub__ = NotImplemented
ROOT.TProfile . __abs__  = NotImplemented 

# =============================================================================
## operation with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h2_oper_ ( h1 , h2 , oper ) :
    """
    Operation with the histogram
        
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 (oper) h2
    
    """
    if                                 not h1.GetSumw2() : h1.Sumw2()
    if hasattr ( h2 , 'GetSumw2' ) and not h2.GetSumw2() : h2.Sumw2()
    #
    result = h1.Clone( hID() )
    if not result.GetSumw2() : result.Sumw2()
    #
    f2 = objectAsFunction ( h2 )
    # 
    for ix1,iy1,x1,y1,z1 in h1.iteritems() :
        #
        result.SetBinContent ( ix1 , iy1 , 0 ) 
        result.SetBinError   ( ix1 , iy1 , 0 )
        #
        z2 = f2 ( x1.value() , y1.value() ) 
        #
        v  = VE ( oper ( z1 , z2 ) ) 
        #
        if not v.isfinite() : continue 
        #
        result.SetBinContent ( ix1 , iy1 , v.value () ) 
        result.SetBinError   ( ix1 , iy1 , v.error () )
        
    return result

# =============================================================================
## operation with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-06-03
def _h2_ioper_ ( h1 , h2 , oper ) :
    """
    Operation with the histogram 
    """
    if                                 not h1.GetSumw2() : h1.Sumw2()
    if hasattr ( h2 , 'GetSumw2' ) and not h2.GetSumw2() : h2.Sumw2()
    #
    f2 = objectAsFunction ( h2 )
    # 
    for ix1,iy1,x1,y1,z1 in h1.iteritems() :
        #
        h1.SetBinContent ( ix1 , iy1 , 0 ) 
        h1.SetBinError   ( ix1 , iy1 , 0 )
        #
        z2 = f2 ( x1.value() , y1.value() ) 
        #
        v  = VE ( oper ( z1 , z2 ) ) 
        #
        if not v.isfinite() : continue 
        #
        h1.SetBinContent ( ix1 , iy1 , v.value () ) 
        h1.SetBinError   ( ix1 , iy1 , v.error () )

    return h1

# =============================================================================
## Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h2_div_ ( h1 , h2 ) :
    """
    Divide the histograms
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 / h2
    
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : x/y ) 
# =============================================================================
## Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h2_mul_ ( h1 , h2 ) :
    """
    Multiply the histograms 
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 * h2 
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : x*y ) 
# =============================================================================
## Addition with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h2_add_ ( h1 , h2 ) :
    """
    Add the histograms 
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 + h2 
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : x+y ) 
# =============================================================================
## Subtraction of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h2_sub_ ( h1 , h2 ) :
    """
    Subtract the histogram
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 - h2 
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : x-y ) 

# =============================================================================
## Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h2_rdiv_ ( h1 , h2 ) :
    """
    Divide the histograms
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 / h2
    
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : y/x ) 
# =============================================================================
## Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h2_rmul_ ( h1 , h2 ) :
    """
    Multiply the histograms 
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 * h2 
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : y*x ) 
# =============================================================================
## Addition with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h2_radd_ ( h1 , h2 ) :
    """
    Add the histograms 
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 + h2 
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : y+x ) 
# =============================================================================
## Subtraction of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h2_rsub_ ( h1 , h2 ) :
    """
    Subtract the histogram
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 - h2 
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : y-x ) 

# =============================================================================
##  ``Fraction'' of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h2_frac_ ( h1 , h2 ) :
    """
    ``Fraction'' the histogram h1/(h1+h2)
    
    >>> h1     = ...
    >>> h2     = ...
    >>> frac   = h1.frac ( h2 )

    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : x.frac(y) ) 
# =============================================================================
## ``Asymmetry'' of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h2_asym_ ( h1 , h2 ) :
    """
    ``Asymmetry'' the histogram (h1-h2)/(h1+h2)
    
    >>> h1     = ...
    >>> h2     = ...
    >>> asym   = h1.asym ( h2 )
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : x.asym(y) ) 
# =============================================================================
## ``Difference'' of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-06-03
def _h2_diff_ ( h1 , h2 ) :
    """
    ``Difference'' the histogram 2*(h1-h2)/(h1+h2)
    
    >>> h1     = ...
    >>> h2     = ...
    >>> diff   = h1.diff ( h2 )
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : 2*x.asym(y) ) 
# =============================================================================
##  ``Chi2-tension'' the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h2_chi2_ ( h1 , h2 ) :
    """
    ``Chi2-tension'' for the histograms
    
    >>> h1     = ...
    >>> h2     = ...
    >>> chi2   = h1.chi2 ( h2 ) 
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : VE ( x.chi2 ( y ) , 0 ) ) 
# =============================================================================
##  ``Average'' the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h2_mean_ ( h1 , h2 ) :
    """
    ``Average'' for the histograms
    
    >>> h1     = ...
    >>> h2     = ...
    >>> mean   = h1.average ( h2 ) 
    """
    return _h2_oper_ ( h1 , h2 , lambda x,y : x.mean ( y ) )  

# =============================================================================
## 'pow' the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h2_pow_ ( h1 , val ) :
    """
    ``pow'' the histogram
    
    >>> h2     = ...
    >>> result = h2 ** 2 

    """
    if not h1.GetSumw2() : h1.Sumw2()
    #
    result = h1.Clone( hID() )
    if not result.GetSumw2() : result.Sumw2()
    #
    for ix1,iy1,x1,y1,z1 in h1.iteritems() :
        #
        result.SetBinContent ( ix1 , iy1 , 0 ) 
        result.SetBinError   ( ix1 , iy1 , 0 )
        #
        v = VE ( pow ( z1 , val ) ) 
        #
        if not v.isfinite() : continue 
        #
        result.SetBinContent ( ix1 , iy1 , v.value () ) 
        result.SetBinError   ( ix1 , iy1 , v.error () )
        
    return result 

# =============================================================================
## 'abs' th histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h2_abs_ ( h1 ) :
    """
    ``abs'' the histogram

    >>> h2     = ...
    >>> result = abs ( h2 ) 
    """
    if not h1.GetSumw2() : h1.Sumw2()
    #
    result = h1.Clone( hID() )
    if not result.GetSumw2() : result.Sumw2()
    #
    for ix1,iy1,x1,y1,z1 in h1.iteritems() :
        #
        result.SetBinContent ( ix1 , iy1 , 0 ) 
        result.SetBinError   ( ix1 , iy1 , 0 )
        #
        v = abs  ( z1 ) 
        #
        result.SetBinContent ( ix1 , iy1 , v.value () ) 
        result.SetBinError   ( ix1 , iy1 , v.error () )
        
    return result 

# =============================================================================
## Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-06-03
def _h2_idiv_ ( h1 , h2 ) :
    """
    Divide the histograms 
    
    >>> h1  = ...
    >>> h2  = ...
    >>> h1 /=  h2 
    
    """
    return _h2_ioper_ ( h1 , h2 , lambda x,y : x/y ) 
# =============================================================================
## Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-06-03
def _h2_imul_ ( h1 , h2 ) :
    """
    Multiply the histograms 
    
    >>> h1  = ...
    >>> h2  = ...
    >>> h1 *=  h2 
    
    """
    return _h2_ioper_ ( h1 , h2 , lambda x,y : x*y ) 
# =============================================================================
## Addition with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-06-03
def _h2_iadd_ ( h1 , h2 ) :
    """
    Add the histograms
    
    >>> h1  = ...
    >>> h2  = ...
    >>> h1 +=  h2 
    
    """
    return _h2_ioper_ ( h1 , h2 , lambda x,y : x+y ) 
# =============================================================================
## Subtraction of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-06-03
def _h2_isub_ ( h1 , h2 ) :
    """
    Subtract the histogram
    
    >>> h1  = ...
    >>> h2  = ...
    >>> h1 -=  h2 
    
    """
    return _h2_ioper_ ( h1 , h2 , lambda x,y : x-y ) 
# =============================================================================

def _h2_box_   ( self , opts = '' ) : return self.Draw ( opts + ' box'   )
def _h2_lego_  ( self , opts = '' ) : return self.Draw ( opts + ' lego'  )
def _h2_surf_  ( self , opts = '' ) : return self.Draw ( opts + ' surf'  )
def _h2_surf2_ ( self , opts = '' ) : return self.Draw ( opts + ' surf2' )

for t in ( ROOT.TH2F , ROOT.TH2D ) : 
    
    t . _oper_   = _h2_oper_
    t . __div__  = _h2_div_
    t . __mul__  = _h2_mul_
    t . __add__  = _h2_add_
    t . __sub__  = _h2_sub_
    t . __pow__  = _h2_pow_
    t . __abs__  = _h2_abs_

    t . __rdiv__ = _h2_rdiv_
    t . __rmul__ = _h2_rmul_
    t . __radd__ = _h2_radd_
    t . __rsub__ = _h2_rsub_

    t . __idiv__ = _h2_idiv_
    t . __imul__ = _h2_imul_
    t . __iadd__ = _h2_iadd_
    t . __isub__ = _h2_isub_
    
    t .  frac    = _h2_frac_
    t .  asym    = _h2_asym_
    t .  diff    = _h2_diff_
    t .  chi2    = _h2_chi2_
    t .  average = _h2_mean_

    t .  box     = _h2_box_
    t .  lego    = _h2_lego_
    t .  surf    = _h2_surf_
    t .  surf2   = _h2_surf2_


# =============================================================================
## operation with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h3_oper_ ( h1 , h2 , oper ) :
    """
    Operation with the 3D-histogram 
    
    >>> h1 = ...
    >>> h2 = ...
    >>> h3 = h1 (oper) h2
    
    """
    if                                 not h1.GetSumw2() : h1.Sumw2()
    if hasattr ( h2 , 'GetSumw2' ) and not h2.GetSumw2() : h2.Sumw2()
    #
    result = h1.Clone( hID() )
    if not result.GetSumw2() : result.Sumw2()
    #
    f2 = objectAsFunction ( h2 ) 
    # 
    for ix1,iy1,iz1,x1,y1,z1,v1 in h1.iteritems() :
        #
        result.SetBinContent ( ix1 , iy1 , iz1 , 0 ) 
        result.SetBinError   ( ix1 , iy1 , iz1 , 0 )
        #
        v2 = f2 ( x1.value() , y1.value() , z1.value() ) 
        #
        v  = VE ( oper ( v1 , v2 ) )
        #
        if not v.isfinite() : continue 
        #
        result.SetBinContent ( ix1 , iy1 , iz1 , v.value () ) 
        result.SetBinError   ( ix1 , iy1 , iz1 , v.error () )

    return result

# =============================================================================
## operation with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-06-03
def _h3_ioper_ ( h1 , h2 , oper ) :
    """
    Operation with the 3D-histogram 
    """
    if                                 not h1.GetSumw2() : h1.Sumw2()
    if hasattr ( h2 , 'GetSumw2' ) and not h2.GetSumw2() : h2.Sumw2()
    #
    f2 = objectAsFunction ( h2 ) 
    # 
    for ix1,iy1,iz1,x1,y1,z1,v1 in h1.iteritems() :
        #
        h1.SetBinContent ( ix1 , iy1 , iz1 , 0 ) 
        h1.SetBinError   ( ix1 , iy1 , iz1 , 0 )
        #
        v2 = f2 ( x1.value() , y1.value() , z1.value() ) 
        #
        v  = VE ( oper ( v1 , v2 ) ) 
        #
        if not v.isfinite() : continue 
        #
        h1.SetBinContent ( ix1 , iy1 , iz1 , v.value () ) 
        h1.SetBinError   ( ix1 , iy1 , iz1 , v.error () )

    return h1

# =============================================================================
##  Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h3_div_ ( h1 , h2 ) :
    """
    Divide the histograms 
    
    >>> h1 = ...
    >>> h2 = ...
    >>> h3 = h1 / h2 
    """
    return _h3_oper_ ( h1 , h2 , lambda x,y : x/y ) 
# =============================================================================
##  Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h3_mul_ ( h1 , h2 ) :
    """
    Multiply the histograms
    
    >>> h1 = ...
    >>> h2 = ...
    >>> h3 = h1 * h2 
    """
    return _h3_oper_ ( h1 , h2 , lambda x,y : x*y ) 
# =============================================================================
##  Addition with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h3_add_ ( h1 , h2 ) :
    """
    Add the histograms 
    
    >>> h1 = ...
    >>> h2 = ...
    >>> h3 = h1 + h2 
    """
    return _h3_oper_ ( h1 , h2 , lambda x,y : x+y ) 
# =============================================================================
##  Subtraction of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h3_sub_ ( h1 , h2 ) :
    """
    Subtract the histogram
    
    >>> h1 = ...
    >>> h2 = ...
    >>> h3 = h1 - h2 
    """
    return _h3_oper_ ( h1 , h2 , lambda x,y : x-y ) 
# =============================================================================
##  ``Fraction'' of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h3_frac_ ( h1 , h2 ) :
    """
    ``Fraction'' the histogram h1/(h1+h2)
    
    >>> h1 = ...
    >>> h2 = ...
    >>> h3 = h1.frac ( h2 )
    
    """
    return _h3_oper_ ( h1 , h2 , lambda x,y : x.frac(y) ) 
# =============================================================================
##  ``Asymmetry'' of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h3_asym_ ( h1 , h2 ) :
    """
    ``Asymmetry'' the histogram (h1-h2)/(h1+h2)
    
    >>> h1 = ...
    >>> h2 = ...
    >>> h3 = h1.asym ( h2 )
    
    """
    return _h3_oper_ ( h1 , h2 , lambda x,y : x.asym(y) ) 
# =============================================================================
##  ``Chi2-tension'' the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h3_chi2_ ( h1 , h2 ) :
    """
    ``Chi2-tension'' for the histograms
    
    >>> h1 = ...
    >>> h2 = ...
    >>> h3 = h1.chi2 ( h2 )
    
    """
    return _h3_oper_ ( h1 , h2 , lambda x,y : VE ( x.chi2 ( y ) , 0 ) ) 
# =============================================================================
##  ``Average'' the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h3_mean_ ( h1 , h2 ) :
    """
    ``Average'' for the histograms

    >>> h1 = ...
    >>> h2 = ...
    >>> h3 = h1.average ( h2 ) 
    """
    return _h3_oper_ ( h1 , h2 , lambda x,y : x.mean ( y ) )  

# =============================================================================
## 'pow' the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h3_pow_ ( h1 , val ) :
    """
    ``pow'' the histogram 
    
    >>> h1      = ...
    >>> result  = h1 ** 3 
    """
    if not h1.GetSumw2() : h1.Sumw2()
    #
    result = h1.Clone( hID() )
    if not result.GetSumw2() : result.Sumw2()
    #
    for ix1,iy1,iz1,x1,y1,z1,v1 in h1.iteritems() :
        #
        result.SetBinContent ( ix1 , iy1 , iz1 , 0 ) 
        result.SetBinError   ( ix1 , iy1 , iz1 , 0 )
        #
        v = VE ( pow ( v1 , val ) ) 
        #
        if not v.isfinite() : continue 
        #
        result.SetBinContent ( ix1 , iy1 , iz1 , v.value () ) 
        result.SetBinError   ( ix1 , iy1 , iz1 , v.error () )
        
    return result 


# =============================================================================
## Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h3_rdiv_ ( h1 , h2 ) :
    """
    Divide the histograms
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 / h2
    
    """
    return _h3_oper_ ( h1 , h2 , lambda x,y : y/x ) 
# =============================================================================
## Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h3_rmul_ ( h1 , h2 ) :
    """
    Multiply the histograms 
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 * h2 
    """
    return _h3_oper_ ( h1 , h2 , lambda x,y : y*x ) 
# =============================================================================
## Addition with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h3_radd_ ( h1 , h2 ) :
    """
    Add the histograms 
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 + h2 
    """
    return _h3_oper_ ( h1 , h2 , lambda x,y : y+x ) 
# =============================================================================
## Subtraction of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h3_rsub_ ( h1 , h2 ) :
    """
    Subtract the histogram
    
    >>> h1     = ...
    >>> h2     = ...
    >>> result = h1 - h2 
    """
    return _h3_oper_ ( h1 , h2 , lambda x,y : y-x ) 



# =============================================================================
## 'abs' the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h3_abs_ ( h1 ) :
    """
    ``abs'' the histogram

    >>> h3     = ...
    >>> result = abs ( h3 ) 
    """
    if not h1.GetSumw2() : h1.Sumw2()
    # 
    result = h1.Clone( hID() )
    if not result.GetSumw2() : result.Sumw2()
    #
    for ix1,iy1,iz1,x1,y1,z1,v1 in h1.iteritems() :
        #
        result.SetBinContent ( ix1 , iy1 , iz1 , 0 ) 
        result.SetBinError   ( ix1 , iy1 , iz1 , 0 )
        #
        v = abs  ( v1 ) 
        #
        result.SetBinContent ( ix1 , iy1 , iz1 , v.value () ) 
        result.SetBinError   ( ix1 , iy1 , iz1 , v.error () )
        
    return result 

# =============================================================================
## Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-06-03
def _h3_idiv_ ( h1 , h2 ) :
    """
    Divide the histograms 
    
    >>> h1  = ...
    >>> h2  = ...
    >>> h1 /=  h2 
    
    """
    return _h3_ioper_ ( h1 , h2 , lambda x,y : x/y ) 
# =============================================================================
## Division with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-06-03
def _h3_imul_ ( h1 , h2 ) :
    """
    Multiply the histograms 
    
    >>> h1  = ...
    >>> h2  = ...
    >>> h1 *=  h2 
    
    """
    return _h3_ioper_ ( h1 , h2 , lambda x,y : x*y ) 
# =============================================================================
## Addition with the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-06-03
def _h3_iadd_ ( h1 , h2 ) :
    """
    Add the histograms
    
    >>> h1  = ...
    >>> h2  = ...
    >>> h1 +=  h2 
    
    """
    return _h3_ioper_ ( h1 , h2 , lambda x,y : x+y ) 
# =============================================================================
## Subtraction of the histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-06-03
def _h3_isub_ ( h1 , h2 ) :
    """
    Subtract the histogram
    
    >>> h1  = ...
    >>> h2  = ...
    >>> h1 -=  h2 
    
    """
    return _h3_ioper_ ( h1 , h2 , lambda x,y : x-y ) 
# =============================================================================


ROOT.TH3._oper_    = _h3_oper_
    
ROOT.TH3.__div__   = _h3_div_
ROOT.TH3.__mul__   = _h3_mul_
ROOT.TH3.__add__   = _h3_add_
ROOT.TH3.__sub__   = _h3_sub_

ROOT.TH3.__rdiv__  = _h3_rdiv_
ROOT.TH3.__rmul__  = _h3_rmul_
ROOT.TH3.__radd__  = _h3_radd_
ROOT.TH3.__rsub__  = _h3_rsub_

ROOT.TH3.__idiv__  = _h3_idiv_
ROOT.TH3.__imul__  = _h3_imul_
ROOT.TH3.__iadd__  = _h3_iadd_
ROOT.TH3.__isub__  = _h3_isub_

ROOT.TH3.__abs__   = _h3_abs_
ROOT.TH3.__pow__   = _h3_pow_

ROOT.TH3.  frac    = _h3_frac_
ROOT.TH3.  asym    = _h3_asym_
ROOT.TH3.  chi2    = _h3_chi2_
ROOT.TH3.  average = _h3_mean_

# =============================================================================
## get the runnig sum over the histogram
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_sumv_ ( h , increasing = True ) :
    """
    Create the ``runnig sum'' over the histogram 

    >>> h   = ...
    >>> h1 = h.sumv()
    
    """
    result = h.Clone ( hID() )
    result.Reset() 
    if not result.GetSumw2() : result.Sumw2()
    
    if increasing :
        
        _s = VE ( 0 , 0 ) 
        for ibin in h :
            _s            += h [ibin]
            result [ibin]  = VE( _s ) 
    else :
        
        for ibin in h :
            _s  = VE(0,0) 
            for jbin in h :
                if jbin < ibin : continue
                _s += h [jbin]
                
            result [ibin]  = VE( _s ) 
        
    return result 

# ==============================================================================
for t in  (ROOT.TH1F , ROOT.TH1D ) : 
    t . sumv   = _h1_sumv_ 
        

# =============================================================================
## Calculate the "cut-efficiency from the histogram
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_effic_ ( h , increasing = True ) :
    """
    Calculate the cut efficiency for the histogram

    >>> h  = ...
    >>> he = h.effic ( 14.2 )
    
    """
    
    result = h.Clone ( hID() )
    result.Reset() 
    if not result.GetSumw2() : result.Sumw2()

    for ibin in h :

        s1 = VE(0,0)
        s2 = VE(0,0)
        
        for jbin in h :
            
            if jbin < ibin : s1 += h [ jbin ]
            else           : s2 += h [ jbin ]

        result [ibin] = s1.frac( s2 ) if increasing else s2.frac( s1 ) 

    return result 


# =============================================================================
## Calculate the "cut-efficiency from the histogram
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_effic2_ ( h , value , increasing = True ) :
    """
    Calculate the cut efficiency for the histogram

    >>> h  = ...
    >>> he = h.efficiency ( 14.2 )
    
    """

    s1 = VE(0,0)
    s2 = VE(0,0)

    for i,x,y in h.iteritems () :

        if x.value() < value : s1 += y 
        else                 : s2 += y 
        
    return s1.frac( s2 ) if increasing else s2.frac ( s1 ) 

ROOT.TH1F.effic      = _h1_effic_ 
ROOT.TH1D.effic      = _h1_effic_ 
ROOT.TH1F.efficiency = _h1_effic2_ 
ROOT.TH1D.efficiency = _h1_effic2_ 

# ================================================================================
_sqrt_2_ = math.sqrt( 2.0 ) 
## helper function : convolution of gaussian with the single pulse 
def _cnv_ ( x , x0 , dx , sigma ) :
    """
    Simple wrapper over error-function:
    convolution of gaussian with the single pulse 
    """
    _erf_ = ROOT.Math.erfc
    
    s = abs ( sigma )
    if hasattr ( s , 'value' ) : s = s.value()
    
    h = ( x - ( x0 + 0.5 * dx ) ) / _sqrt_2_ / s 
    l = ( x - ( x0 - 0.5 * dx ) ) / _sqrt_2_ / s 
    
    return 0.5 * ( _erf_ ( h ) - _erf_ ( l ) ) / dx 

# =============================================================================
## "Smear" : make a convolution of the histogram with gaussian function
#  @param sigma     the gaussian resolutuon
#  @param addsigmas the parameter to treat the bounary conditions 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-09-20
def _smear_ ( h1 , sigma , addsigmas = 5 ) :
    """
    Smear the histogram through the convolution with the gaussian function:

    >>> histo   = ...
    >>> smeared = h.smear ( sigma = 0.01 )
    """
    #
    ## clone the input histogram
    #
    h2  = h1.Clone ( hID () ) ; h2.Reset() ;
    if not h2.GetSumw2() : h2.Sumw2()

    first_bin = None
    last_bin  = None

    ## collect all bins
    bins = []

    for ibin in h1.iteritems() : bins.append ( ibin[1:] )

    ## add few artificially replicated bins
    fb    = bins [  0 ]
    lb    = bins [ -1 ]
    xmin  = fb[1].value() - fb[1].error()
    xmax  = lb[1].value() + lb[1].error()    
    x_min = xmin - abs ( addsigmas * sigma ) 
    x_max = xmax + abs ( addsigmas * sigma ) 

    ## add few fictive bins 
    while  xmin > x_min :
        
        bin0 = bins[0]
        
        xc   = bin0[0]
        val  = bin0[1]
        
        bin  = (xc-2*xc.error(),val)
        bins.insert (  0, bin )
        fb    = bins [  0 ]
        
        xmin  = bin[0].value()-bin[0].error()

    ## add few fictive bins 
    while  xmax < x_max :
        
        bin0 = bins[-1]
        
        xc   = bin0[0]
        val  = bin0[1]
        
        bin  = (xc+2*xc.error(),val)
        bins.append ( bin )
        fb    = bins [  0 ]
        
        xmax  = bin[0].value()+bin[0].error()
    

    for ibin1 in bins :
        
        x1c  =     ibin1 [0].value () 
        x1w  = 2 * ibin1 [0].error () 
        val1 =     ibin1 [1]
        
        if 0 == val1.value() and 0 == val1.cov2() : continue
        
        for ibin2 in h2.iteritems() :

            i2      =     ibin2 [0]
            x2c     =     ibin2 [1].value () 
            x2w     = 2 * ibin2 [1].error () 

            val2    = VE ( val1 )
            val2   *= x2w  
            val2   *= _cnv_ ( x2c , x1c , x1w , sigma ) 
            
            h2[i2] += val2

    return h2


ROOT.TH1F. smear = _smear_
ROOT.TH1D. smear = _smear_

# =============================================================================
## make transformation of histogram content 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-10-23  
def _h1_transform_ ( h1 , func ) :
    """
    
    Make the transformation of the histogram content 
    
    >>> func = lambda x,y: y   ## identical transformation/copy
    >>> h1 = ...
    >>> h2 = h1.fransform ( func ) 
    
    """
    #
    if not h1.GetSumw2() : h1.Sumw2()
    h2 = h1.Clone( hID() )
    if not h2.GetSumw2() : h2.Sumw2()
    #
    for i,x,y in h1.iteritems() :
        
        h2 [ i ] = func ( x, y ) 
        
    return h2 

ROOT.TH1F. transform = _h1_transform_ 
ROOT.TH1D. transform = _h1_transform_ 

import LHCbMath.math_ve as math_ve
for h in ( ROOT.TH1F , ROOT.TH1D ) : 
    
    h. __exp__    = lambda s : _h1_transform_ ( s , lambda x,y : math_ve.exp    ( y ) ) 
    h. __expm1__  = lambda s : _h1_transform_ ( s , lambda x,y : math_ve.expm1  ( y ) ) 
    h. __log__    = lambda s : _h1_transform_ ( s , lambda x,y : math_ve.log    ( y ) )
    h. __log10__  = lambda s : _h1_transform_ ( s , lambda x,y : math_ve.log10  ( y ) )
    h. __log1p__  = lambda s : _h1_transform_ ( s , lambda x,y : math_ve.log1p  ( y ) )
    h. __sqrt__   = lambda s : _h1_transform_ ( s , lambda x,y : math_ve.sqrt   ( y ) )
    h. __cbrt__   = lambda s : _h1_transform_ ( s , lambda x,y : math_ve.cbrt   ( y ) )
    h. __sin__    = lambda s : _h1_transform_ ( s , lambda x,y : math_ve.sin    ( y ) )
    h. __cos__    = lambda s : _h1_transform_ ( s , lambda x,y : math_ve.cos    ( y ) )
    h. __tan__    = lambda s : _h1_transform_ ( s , lambda x,y : math_ve.tan    ( y ) )
    h. __sinh__   = lambda s : _h1_transform_ ( s , lambda x,y : math_ve.sinh   ( y ) )
    h. __cosh__   = lambda s : _h1_transform_ ( s , lambda x,y : math_ve.cosh   ( y ) )
    h. __tanh__   = lambda s : _h1_transform_ ( s , lambda x,y : math_ve.tanh   ( y ) )
    h. __asin__   = lambda s : _h1_transform_ ( s , lambda x,y : math_ve.asin   ( y ) )
    h. __acos__   = lambda s : _h1_transform_ ( s , lambda x,y : math_ve.acos   ( y ) )
    h. __atan__   = lambda s : _h1_transform_ ( s , lambda x,y : math_ve.atan   ( y ) )
    h. __asinh__  = lambda s : _h1_transform_ ( s , lambda x,y : math_ve.asinh  ( y ) )
    h. __acosh__  = lambda s : _h1_transform_ ( s , lambda x,y : math_ve.acosh  ( y ) )
    h. __atanh__  = lambda s : _h1_transform_ ( s , lambda x,y : math_ve.atanh  ( y ) )
    h. __erf__    = lambda s : _h1_transform_ ( s , lambda x,y : math_ve.erf    ( y ) )
    h. __erfc__   = lambda s : _h1_transform_ ( s , lambda x,y : math_ve.erfc   ( y ) )
    h. __gamma__  = lambda s : _h1_transform_ ( s , lambda x,y : math_ve.gamma  ( y ) )
    h. __tgamma__ = lambda s : _h1_transform_ ( s , lambda x,y : math_ve.tgamma ( y ) )
    h. __lgamma__ = lambda s : _h1_transform_ ( s , lambda x,y : math_ve.lgamma ( y ) )
    
# =============================================================================
## make transformation of histogram content 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-10-23  
def _h2_transform_ ( h2 , func ) :
    """
    
    Make the transformation of the 2D-histogram content 
    
    >>> func = lambda x,y,z: z   ## identical transformation/copy
    >>> h2 = ...
    >>> h3 = h2.fransform ( func ) 
    
    """
    #
    if not h2.GetSumw2() : h2.Sumw2()
    h3 = h2.Clone( hID() )
    if not h3.GetSumw2() : h3.Sumw2()
    #
    for ix,iy,x,y,z in h2.iteritems() :
        
        h3 [ ix , iy ] = func ( x, y , z ) 
        
    return h3 

ROOT.TH2F. transform = _h2_transform_ 
ROOT.TH2D. transform = _h2_transform_ 

import LHCbMath.math_ve as math_ve
for h in ( ROOT.TH2F , ROOT.TH2D ) : 
    
    h. __exp__    = lambda s : _h2_transform_ ( s , lambda x,y,z : math_ve.exp    ( z ) ) 
    h. __expm1__  = lambda s : _h2_transform_ ( s , lambda x,y,z : math_ve.expm1  ( z ) ) 
    h. __log__    = lambda s : _h2_transform_ ( s , lambda x,y,z : math_ve.log    ( z ) )
    h. __log10__  = lambda s : _h2_transform_ ( s , lambda x,y,z : math_ve.log10  ( z ) )
    h. __logp1__  = lambda s : _h2_transform_ ( s , lambda x,y,z : math_ve.logp1  ( z ) )
    h. __sqrt__   = lambda s : _h2_transform_ ( s , lambda x,y,z : math_ve.sqrt   ( z ) )
    h. __cbrt__   = lambda s : _h2_transform_ ( s , lambda x,y,z : math_ve.cbrt   ( z ) )
    h. __sin__    = lambda s : _h2_transform_ ( s , lambda x,y,z : math_ve.sin    ( z ) )
    h. __cos__    = lambda s : _h2_transform_ ( s , lambda x,y,z : math_ve.cos    ( z ) )
    h. __tan__    = lambda s : _h2_transform_ ( s , lambda x,y,z : math_ve.tan    ( z ) )
    h. __sinh__   = lambda s : _h2_transform_ ( s , lambda x,y,z : math_ve.sinh   ( z ) )
    h. __cosh__   = lambda s : _h2_transform_ ( s , lambda x,y,z : math_ve.cosh   ( z ) )
    h. __tanh__   = lambda s : _h2_transform_ ( s , lambda x,y,z : math_ve.tanh   ( z ) )
    h. __asin__   = lambda s : _h2_transform_ ( s , lambda x,y,z : math_ve.asin   ( z ) )
    h. __acos__   = lambda s : _h2_transform_ ( s , lambda x,y,z : math_ve.acos   ( z ) )
    h. __atan__   = lambda s : _h2_transform_ ( s , lambda x,y,z : math_ve.atan   ( z ) )
    h. __asinh__  = lambda s : _h2_transform_ ( s , lambda x,y,z : math_ve.asinh  ( z ) )
    h. __acosh__  = lambda s : _h2_transform_ ( s , lambda x,y,z : math_ve.acosh  ( z ) )
    h. __atanh__  = lambda s : _h2_transform_ ( s , lambda x,y,z : math_ve.atanh  ( z ) )
    h. __erf__    = lambda s : _h2_transform_ ( s , lambda x,y,z : math_ve.erf    ( z ) )
    h. __erfc__   = lambda s : _h2_transform_ ( s , lambda x,y,z : math_ve.erfc   ( z ) )
    h. __gamma__  = lambda s : _h2_transform_ ( s , lambda x,y,z : math_ve.gamma  ( z ) )
    h. __tgamma__ = lambda s : _h2_transform_ ( s , lambda x,y,z : math_ve.tgamma ( z ) )
    h. __lgamma__ = lambda s : _h2_transform_ ( s , lambda x,y,z : math_ve.lgamma ( z ) )
    
# =============================================================================
# Few "specific" transformations
# =============================================================================

# =============================================================================
## transform the histogram into "precision" histogram 
def _h1_precision_ ( self ) :
    """
    Make precsion histogram, each bin constains ``precision''

    >>> h =
    >>> p = h.precision()
    
    """
    return _h1_transform_ ( self ,  lambda x,y  : y.precision() )
# =============================================================================
## transform the histogram into "precision" histogram 
def _h2_precision_ ( self ) :
    """
    Make precsion histogram, each bin constains ``precision''

    >>> h =
    >>> p = h.precision()
    
    """
    return _h2_transform_ ( self ,  lambda x,y,z: z.precision() )
# =============================================================================
## transform the histogram into "B/S" histogram 
def _h1_b2s_       ( self ) :
    """
    Make B/S histogram, each bin constains ``B/S''

    >>> h    =
    >>> btos = h.b2s()
    
    """
    return _h1_transform_ ( self ,  lambda x,y  : y.b2s () )
# =============================================================================
## transform the histogram into "B/S" histogram 
def _h2_b2s_       ( self ) :
    """
    Make B/S histogram, each bin constains ``B/S''

    >>> h    =
    >>> btos = h.b2s()
    
    """
    return _h2_transform_ ( self ,  lambda x,y,z: z.b2s () )

ROOT.TH1F. precision = _h1_precision_
ROOT.TH1D. precision = _h1_precision_
ROOT.TH2F. precision = _h2_precision_
ROOT.TH2D. precision = _h2_precision_
ROOT.TH1F. b2s       = _h1_b2s_
ROOT.TH1D. b2s       = _h1_b2s_
ROOT.TH2F. b2s       = _h2_b2s_
ROOT.TH2D. b2s       = _h2_b2s_

# =============================================================================
## rescale the histogram for effective uniform bins
#  new_content = old_content * factor / bin_width
#  @code
#  >>> h1 = ...
#  >>> h2 = h1.rescale_bins ( h1 , 1 )
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-03-19   
def _h1_rescale_ ( h1 , factor = 1 ) :
    """
    Rescale the histogram for effective uniform bins : 
    
    new_bin_content = old_bin_content * factor / bin_width

    >>> h1 = ...
    >>> h2 = h1.rescale_bins ( h1 , 1 )
    
    """
    return _h1_transform_ ( h1  ,  lambda x , y : ( 0.5 * factor / x.error() ) * y )

ROOT.TH1F. rescale_bins = _h1_rescale_ 
ROOT.TH1D. rescale_bins = _h1_rescale_ 


# =============================================================================
## rescale the histogram for effective uniform bins
#  new_content = old_content * factor / bin_area
#  @code
#  >>> h2 = ...
#  >>> h3 = h2.rescale_bins ( h2 , 1 )
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-03-19   
def _h2_rescale_ ( h2 , factor = 1 ) :
    """
    Rescale the histogram for effective uniform bins : 
    
    new_bin_content = old_bin_content * factor / bin_area

    >>> h1 = ...
    >>> h2 = h1.rescale_bins ( h1 , 1 )
    
    """
    return _h2_transform_ ( h2  ,  lambda x , y , z : ( 0.25 * factor / x.error() / y.error() ) * z )

ROOT.TH2F. rescale_bins = _h2_rescale_ 
ROOT.TH2D. rescale_bins = _h2_rescale_ 

# =============================================================================
## sample the histogram using gaussian hypothesis
#
#  @code
#
#   >>> h = ... ##  the histogram
#
#   >>> s1 = h.sample()  ## the sampled hist
#
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  
def _sample_ ( histo , accept = lambda s : True , nmax = 1000 ) :
    """
    Sample the histogram using gaussian hypothesis
 
    >>> h = ... ##  the histogram
 
    >>> s1 = h.sample()  ## the sampled hist

    """
    #
    result = histo.Clone ( hID () )     
    if not result.GetSumw2() : result.Sumw2()
    
    for bin in histo :

        ## getbin content
        v1 = histo[bin]
        
        ## sample it! 
        v2 = VE( v1.gauss ( accept = accept , nmax = nmax ) )
        
        v2.setCov2 ( v1.cov2() )
        
        result [bin] = v2
        
    return result

ROOT.TH1 ._sample_ = _sample_
ROOT.TH1 .sample   = _sample_

# =============================================================================
## Get the Figure-of-Merit (FoM) for the pure signal distribution,
#  e.g. from sPlot)
#  the FoM is defined from the relative precision of the signal yield
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-10-23  
def _fom_2_ ( h1 , increase = True ) :
    """
    Get figure-of-merit (FOM) distribution for signal

    >>> h1 = ...  ## signal distribution
    >>> f1 = h1.FoM2 () 
    """
    #
    h = h1.sumv( increase )
    #
    return _h1_transform_ ( h , func = lambda x,y : y.precision() ) 

# =============================================================================
## Calculate S/sqrt(S+a*B)
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-10-23   
def _sb_ ( s , b  , a = 1 ) :
    """
    Calculate S/sqrt(S+a*B) 
    """
    #
    v     = ( s + a * b ).value()
    if 0 >= v : return VE ( 0 , 0 )
    #
    F     = s.value() / pow ( v , 0.5 )
    #
    # (dF/dS)**2 
    dFdS2 = ( 0.5 * s  + a * b ).value() 
    dFdS2 = dFdS2**2  / v**3
    #
    # (dR/dB)**2 
    dFdB2 = ( -0.5 * a * s  ).value() 
    dFdB2 = dFdB2**2  / v**3
    #
    return VE ( F , dFdS2 * s.cov2() + dFdB2 * b.cov2() ) 

# =============================================================================
## Get the figure-of-merit (FoM) for the signal and background distributions 
#  the FoM is defined as S/sqrt(S+alpha*B) 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-10-23  
def _fom_1_ ( s , b , alpha = 1 , increase = True ) :
    """
    Get figure-of-merit FoM = S/sqrt(S+a*B)
    
    >>> s = ... ## signal distributions
    >>> b = ... ## background distributions
    >>> fom = s.FoM1( b , alpha = 1.0 )  
    
    """
    #
    if not s.GetSumw2() : s.Sumw2()
    h  = s.Clone( hID() )
    if not h.GetSumw2() : h.Sumw2()
    #
    hs = s.sumv ( increase )
    hb = b.sumv ( increase )
    #
    from math import sqrt, pow 
    #
    for i,x,y in hs.iteritems() :
        
        ## the signal 
        si = y
        
        ## the background 
        bi = hb ( x ) 
        
        h [i] = _sb_ ( si , bi , alpha ) 
        
    return h 

ROOT.TH1D . fom_1 = _fom_1_ 
ROOT.TH1D . fom_2 = _fom_2_ 
ROOT.TH1F . fom_1 = _fom_1_ 
ROOT.TH1F . fom_2 = _fom_2_

ROOT.TH1D . FoM_1 = _fom_1_ 
ROOT.TH1D . FoM_2 = _fom_2_ 
ROOT.TH1F . FoM_1 = _fom_1_ 
ROOT.TH1F . FoM_2 = _fom_2_ 

# =============================================================================
# rebin the histograms
# =============================================================================
## get the overlap for 1D-bins 
def _bin_overlap_1D_ ( x1 , x2 ) :
    """
    """
    #
    x1v = x1.value()
    x1e = x1.error()
    #
    xmin_1 = x1v - x1e 
    xmax_1 = x1v + x1e
    if xmin_1 >= xmax_1  : return 0         ## RETURN
    #
    x2v = x2.value()
    x2e = x2.error()
    #
    xmin_2 = x2v - x2e 
    xmax_2 = x2v + x2e
    if xmin_2 >= xmax_2  : return 0         ## RETURN
    #
    xmin = max ( xmin_1 , xmin_2 )
    xmax = min ( xmax_1 , xmax_2 )
    #
    if xmin >= xmax      : return 0         ## RETURN 
    #
    return ( xmax - xmin ) / ( 2.0 * x1e ) 
# =============================================================================
## get the overlap for 2D-bins 
def _bin_overlap_2D_ ( x1 , y1 , x2 , y2 ) :
    """
    """
    #
    x1v = x1.value()
    x1e = x1.error()
    #
    xmin_1 = x1v - x1e
    xmax_1 = x1v + x1e
    if xmin_1 >= xmax_1  : return 0         ## RETURN
    #
    x2v = x2.value()
    x2e = x2.error()    
    #
    xmin_2 = x2v - x2e 
    xmax_2 = x2v + x2e
    if xmin_2 >= xmax_2  : return 0         ## RETURN
    #
    y1v = y1.value()
    y1e = y1.error()    
    #
    ymin_1 = y1v - y1e 
    ymax_1 = y1v + y1e
    if ymin_1 >= ymax_1  : return 0         ## RETURN
    #
    y2v = y2.value()
    y2e = y2.error()        
    #
    ymin_2 = y2v - y2e 
    ymax_2 = y2v + y2e
    if ymin_2 >= ymax_2  : return 0         ## RETURN
    #
    xmin = max ( xmin_1 , xmin_2 )
    xmax = min ( xmax_1 , xmax_2 )
    if xmin >= xmax      : return 0         ## RETURN 
    #
    ymin = max ( ymin_1 , ymin_2 )
    ymax = min ( ymax_1 , ymax_2 )
    if ymin >= ymax      : return 0         ## RETURN 
    #
    #
    return ( xmax - xmin ) * ( ymax - ymin ) / ( 4.0 * x1e * y1e )

# ==============================================================================
## rebin 1D-histogram with NUMBERS 
def _rebin_nums_1D_ ( h1 , template ) :
    """
    Rebin 1D-histogram assuming it is a histogram with *NUMBERS*

    >>> horig    = ...  ## the original histogram 
    >>> template = ...  ## the template with binnings

    >>> h = horig.rebinNumbers ( template ) 
    """
    ##
    # clone it!
    h2 = template.Clone( hID() )
    if not h2.GetSumw2() : h2.Sumw2()
    #
    ## reset the histogram 
    for i2 in h2 : h2[i2] = VE(0,0)
    #
    for i2 in h2.iteritems() :

        xb  = i2[1]
        xbv = xb.value ()
        xbe = xb.error ()
        
        bl = h1.findBin ( xbv - xbe ) - 1
        bh = h1.findBin ( xbv + xbe ) + 1
        
        for i1 in h1.iteritems( bl , bh + 1 ) :
            
            o = _bin_overlap_1D_ ( i1[1] , i2[1] )
            
            h2 [ i2[0] ] +=  o * i1[2] 
            
    return h2 
# =============================================================================
## rebin 1D-histogram as FUNCTION 
def _rebin_func_1D_ ( h1 , template ) :
    """
    Rebin 1D-histogram assuming it is a FUNCTION

    >>> horig    = ...  ## the original histogram 
    >>> template = ...  ## the template with binnings

    >>> h = horig.rebinFunction ( template ) 
    """
    # clone it!
    h2 = template.Clone( hID() )
    if not h2.GetSumw2() : h2.Sumw2()
    ## reset the histogram 
    for i2 in h2 : h2[i2] = VE(0,0)
    #
    for i2 in h2.iteritems() :
        
        xb  = i2[1]
        xbv = xb.value ()
        xbe = xb.error ()
        
        bl = h1.findBin ( xbv - xbe ) - 1  
        bh = h1.findBin ( xbv + xbe ) + 1
        
        for i1 in h1.iteritems( bl , bh + 1 ) :

            o = _bin_overlap_1D_ ( i2[1] , i1[1] ) ## NOTE THE ORDER!!! 
            
            h2 [ i2[0] ] +=  o * i1[2]
            
    return h2 

# ==============================================================================
## rebin 2D-histogram with NUMBERS 
def _rebin_nums_2D_ ( h1 , template ) :
    """
    Rebin 2D-histogram assuming it is a histogram with *NUMBERS*

    >>> horig    = ...  ## the original histogram 
    >>> template = ...  ## the template with binnings

    >>> h = horig.rebinNumbers ( template ) 
    """
    #
    # clone it!
    h2 = template.Clone( hID() )
    if not h2.GetSumw2() : h2.Sumw2()
    ## reset the histogram 
    for i2 in h2 : h2[i2] = VE(0,0)
    #
    for i2 in h2.iteritems() :

        for i1 in h1.iteritems() :
            
            o = _bin_overlap_2D_ ( i1[2] , i1[3] , i2[2] , i2[3] )
            
            h2 [ i2[0] , i2[1] ] +=  o * i1[4] 
            
    return h2 
# =============================================================================
## rebin 2D-histogram as FUNCTION 
def _rebin_func_2D_ ( h1 , template ) :
    """
    Rebin 2D-histogram assuming it is a FUNCTION

    >>> horig    = ...  ## the original histogram 
    >>> template = ...  ## the template with binnings

    >>> h = horig.rebinFunction ( template ) 
    """
    # clone it!
    h2 = template.Clone( hID() )
    if not h2.GetSumw2() : h2.Sumw2()
    ## reset the histogram 
    for i2 in h2 : h2[i2] = VE(0,0)
    #
    for i2 in h2.iteritems() :
        
        for i1 in h1.iteritems() :
            
            o = _bin_overlap_1D_ ( i2[2] , i2[3] , i1[2] , i2[3] ) ## NOTE THE ORDER!!! 
            
            h2 [ i2[0] , i2[1] ] +=  o * i1[4]
            
    return h2 

for t in ( ROOT.TH1F , ROOT.TH1D ) :
    t.rebinNumbers  = _rebin_nums_1D_
    t.rebinFunction = _rebin_func_1D_
    
for t in ( ROOT.TH2F , ROOT.TH2D ) :
    t.rebinNumbers  = _rebin_nums_2D_
    t.rebinFunction = _rebin_func_2D_

# =============================================================================
## Create NULL-line for the histogram and (optionally) draw it
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-11-01
def _h1_null_ ( h1 , draw = False , style = 1 ) :
    """
    Create NULL-line for the histogram and (optionally) draw it
    
    """
    axis = h1.GetXaxis()
    line = ROOT.TLine ( axis.GetXmin() , 0 ,
                        axis.GetXmax() , 0 )
    
    line.SetLineStyle ( style )
    
    if draw :
        line.Draw()

    return line 

ROOT.TH1D.null = _h1_null_
ROOT.TH1F.null = _h1_null_

# =============================================================================
## get edges from the axis:
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _edges_ ( axis ) :
    """
    Get list of edges from the TAxis

    >>> axis
    >>> edges = axis.edges() 
    """
    #
    bins  = [ axis.GetBinLowEdge ( i ) for i in axis ]
    bins += [ axis.GetXmax() ]
    #
    return tuple( bins )

# =============================================================================
ROOT.TAxis.edges = _edges_

# =============================================================================
## make axis from bin-edges 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def axis_bins ( bins         ) :
    """
    Make axis according to the binning 
    
    >>> bins = [ ... ] 
    >>> axis = axis_bins ( bins )  
    
    """
    #
    bins = set  ( bins )
    bins = [ i for i in bins ]
    bins.sort()
    #
    if 2 > len ( bins ) :
        raise AtributeError("axis_bins: insufficient length of bins: %s" % bins )
    #
    from array import array 
    return ROOT.TAxis ( len ( bins ) - 1 , array ( 'd' , bins ) )
    #

# =============================================================================
## prepare "slice" for the axis
#  @code 
#    >>> axis  = ...
#    >>> naxis = axis[2:10] ## keep only bins from 2nd (inclusive) till 10 (exclusive)
#  @endcode     
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-05-20
def _axis_getslice_ ( self , i , j ) :
    """
    Make a ``slice'' for the axis:

    >>> axis  = ...
    >>> naxis = axis[2:10] ## keep only bins from 2nd (inclusive) till 10 (exclusive)
    
    """
    nb = self.GetNbins()
    
    while i < 1 : i += nb
    while j < 1 : j += nb
    
    i = min ( nb , i )
    j = min ( nb , j )
    
    if i >= j : raise IndexError 
    
    edges = self.edges()
    
    return axis_bins ( edges [i-1:j] ) 


ROOT.TAxis. __getslice__ = _axis_getslice_

# =============================================================================
## get "slice" for 1D histogram
#  @code     
#    >>> h1 = ...
#    >>> nh = h1[2:10] ## keep only bins from 2nd (inclusive) till 10 (exclusive)
#  @endcode     
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-05-20 
def _h1_getslice_ ( h1 , i , j ) :
    """
    Get the ``slice'' for 1D-histogram:
    
    >>> h1 = ...
    >>> nh = h1[2:10] ## keep only bins from 2nd (inclusive) till 10 (exclusive)
    
    """
    axis = h1  .GetXaxis()
    nb   = axis.GetNbins()
    
    while i < 0 : i += nb
    while j < 0 : j += nb
    
    i = max ( 1 , min ( nb + 1 , i ) ) 
    j = max ( 1 , min ( nb + 1 , j ) ) 

    if i >= j :
        raise IndexError 
    
    edges = axis.edges ()
    edges = edges [i-1:j]
    
    typ = h1.__class__
    from array import array
    result = typ ( hID  ()       ,
                   h1.GetTitle() ,
                   len ( edges ) - 1 , array ( 'd' , edges ) )
    
    result.Sumw2()
    result += h1
    
    return result 

ROOT.TH1F  . __getslice__  =   _h1_getslice_ 
ROOT.TH1D  . __getslice__  =   _h1_getslice_ 

# =============================================================================
## make 1D-histogram from axis
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def h1_axis ( axis           ,
              title  = '1D'  , 
              name   = None  ,
              double = False ) :
    """
    Make 1D-histogram with binning defined by already created axes
    
    >>> axis = ...
    >>> h1 = h1_axes ( axis , title = 'MyHisto' ) 
    
    """
    #
    if not name : name = hID()
    #
    if not issubclass ( type ( axis ) , ROOT.TAxis ) : axis = axis_bins   ( axis )
    #
    bins  = axis.edges()
    #
    typ = ROOT.TH1D if double else ROOT.TH1F
    from array import array 
    h1  = typ ( name  ,
                title ,
                len ( bins ) - 1 , array ( 'd' , bins ) )
    ##
    h1.Sumw2()
    return h1 

# =============================================================================
## make 2D-histogram from axes
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def h2_axes ( x_axis            ,
              y_axis            ,
              title  = '2D'     , 
              name   = None     ,
              double = False    ) :
    """
    Make 2D-histogram with binning deifned by already created axes
    
    >>> x_axis = ...
    >>> y_axis = ...
    >>> h2 = h2_axes ( x_axis , y_axis , title = 'MyHisto' ) 
    
    """
    #
    if not name : name = hID() 
    #
    if not issubclass ( type ( x_axis ) , ROOT.TAxis ) : x_axis = axis_bins   ( x_axis )
    if not issubclass ( type ( y_axis ) , ROOT.TAxis ) : y_axis = axis_bins   ( y_axis )
    #
    # 
    x_bins  = x_axis.edges()
    y_bins  = y_axis.edges()
    #
    typ = ROOT.TH2D if double else ROOT.TH2F
    from array import array 
    h2  =  typ ( name  ,
                 title ,
                 len ( x_bins ) - 1 , array ( 'd' , x_bins ) ,
                 len ( y_bins ) - 1 , array ( 'd' , y_bins ) )
    ##
    h2.Sumw2()
    return h2

# =============================================================================
## make 3D-histogram from axes
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-07-18
def h3_axes ( x_axis            ,
              y_axis            ,
              z_axis            ,
              title  = '3D'     , 
              name   = None     ,
              double = False    ) :
    """
    Make 3D-histogram with binning deifned by already created axes
    
    >>> x_axis = ...
    >>> y_axis = ...
    >>> z_axis = ...
    >>> h3 = h3_axes ( x_axis , y_axis , z_axis , title = 'MyHisto' ) 
    
    """
    #
    if not name : name = hID() 
    #
    if not issubclass ( type ( x_axis ) , ROOT.TAxis ) : x_axis = axis_bins   ( x_axis )
    if not issubclass ( type ( y_axis ) , ROOT.TAxis ) : y_axis = axis_bins   ( y_axis )
    if not issubclass ( type ( z_axis ) , ROOT.TAxis ) : z_axis = axis_bins   ( z_axis )
    #
    # 
    x_bins  = x_axis.edges()
    y_bins  = y_axis.edges()
    z_bins  = z_axis.edges()
    #
    typ = ROOT.TH3D if double else ROOT.TH3F
    from array import array 
    return typ ( name  ,
                 title ,
                 len ( x_bins ) - 1 , array ( 'd' , x_bins ) ,
                 len ( y_bins ) - 1 , array ( 'd' , y_bins ) , 
                 len ( z_bins ) - 1 , array ( 'd' , z_bins ) ) 

# =======================================================================
## calculate the ``difference'' between two histograms 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h_diff_ ( h1 , h2 , func = lambda s1,s2 : (s1/s2).value() ) :
    """
    Estimate the ``difference'' between two histograms

    """
    
    se = SE()
    
    for bin in h1 :
        
        v1 = h1 [ bin ] 
        v2 = h2 [ bin ] 
        
        se += func ( v1 , v2 )
        
    return se 


ROOT.TH1F.histoDiff = _h_diff_
ROOT.TH1D.histoDiff = _h_diff_
ROOT.TH2F.histoDiff = _h_diff_
ROOT.TH2D.histoDiff = _h_diff_
ROOT.TH3F.histoDiff = _h_diff_
ROOT.TH3D.histoDiff = _h_diff_


# =============================================================================
## perform some accumulation for the histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_accumulate_ ( h                         ,
                     func = lambda s,v : s + v ,
                     cut  = lambda s   : True  , 
                     init = VE ()              ) :
    """
    Accumulate the function value over the histogram

    >>> h =...
    >>> sum = h.accumulate() 
    """
    result = init
    for i in h.iteritems() :
        if cut ( i ) : result = func ( result , i[2] )
    return result 

# =============================================================================
## perform some accumulation for the histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h2_accumulate_ ( h                         ,
                      func = lambda s,v : s + v ,
                      cut  = lambda s   : True  , 
                      init = VE ()              ) :
    """
    Accumulate the function value over the histogram

    >>> h =...
    >>> sum = h.accumulate() 
    """
    result = init
    for i in h.iteritems() :
        if cut ( i ) : result = func ( result , i[4] )
    return result 

# =============================================================================
## get the sum of entries 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_sum_ ( h    ,
              low  ,
              high ) :
    """
    Get the histogram integral  over the specified range:
    
    >>> h = ....
    >>> h.sum ( 1 , 20 )
    
    """
    return _h1_accumulate_ ( h , cut = lambda s : low<=s[1].value()<=high ) 

# =============================================================================
## simple scaling
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h_scale_ ( histo , val = 1.0 ) :
    """
    Scale the histogram to certain integral

    >>> h = ...
    >>> h.scale ( 15 )
    
    """
    factor  = 0.0
    val     = float(val)
    if 0   != val :

        total = VE()
        for ibin in histo :
            total += histo[ ibin ]
        total = total.value()
        
        if 0 != total :
            factor = val/total

    if not histo.GetSumw2() : histo.Sumw2()

    for ibin in histo :

        value          = histo [ ibin ]
        value         *= factor
        histo[ ibin ]  = value 

    return histo

# =============================================================================
## simple shift of the histogram
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_shift_ ( h , bias ) :
    """
    Simple shift of the histogram :
    
    >>> h = ... # the histogram
    >>> h2 = h.shift ( -5 * MeV )
    
    """
    #
    if not h     .GetSumw2()  : h    .Sumw2()
    result = h.Clone( hID() ) ;
    result.Reset() ;
    if not result.GetSumw2() : result.Sumw2()
    #
    for i,x,y in result.iteritems() :
        
        y         += bias
        result[i]  = h ( y )
        
    return result      

# =============================================================================
## simple shift of the histogram
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_irshift_ ( h , ibias ) :
    """
    Simple shift of the histogram :
    
    >>> h   = ... # the histogram
    >>> h >>= 5   # shift for 5 bins left 
    
    """
    #
    ##
    if not isinstance ( ibias , ( int , long ) ) : return NotImplemented 
    if not h     .GetSumw2()  : h    .Sumw2()
    ##
    if   0 == ibias : return h ## RETURN 
    elif ibias < 0  :
        h <<= abs(ibias)
        return h               ## RETURN
    ##
    for i in reversed ( h ) : 
        j  = i - ibias
        if j in h :  h [ i ] = h[ j ]
        else      :  h [ i ] = VE() 
        
    return h     

# =============================================================================
## simple shift of the histogram
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_ilshift_ ( h , ibias ) :
    """
    Simple shift of the histogram :
    
    >>> h   = ...  # the histogram
    >>> h <<= 5    # shift for 5 bins left 
    
    """
    #
    ##
    if not isinstance ( ibias , ( int , long ) ) : return NotImplemented 
    if not h     .GetSumw2()  : h    .Sumw2()
    ##
    if   0 == ibias : return h  ## RETURN 
    elif ibias < 0  :
        h >>= abs(ibias) 
        return h                ## RETURN
    ##
    for i in h :
        j = i + ibias
        if j in h :  h [ i ] = h[ j ]
        else      :  h [ i ] = VE() 
        
    return h     

# =============================================================================
## simple shift of the histogram
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_lshift_ ( h , ibias ) :
    """
    Simple shift of the histogram :
    
    >>> h = ...      # the histogram
    >>> h2 = h << 5  # shift for 5 bins left 
    
    """
    #
    ##
    if not isinstance ( ibias , ( int , long ) ) : return NotImplemented 
    ## 
    if not h     .GetSumw2()  : h    .Sumw2()
    result = h.Clone( hID() ) ;
    result.Reset() ;
    if not result.GetSumw2()  : result.Sumw2()
    #
    result <<= ibias
    return result 


# =============================================================================
## simple shift of the histogram
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_rshift_ ( h , ibias ) :
    """
    Simple shift of the histogram :
    
    >>> h = ...      # the histogram
    >>> h2 = h >> 5  # shift for 5 bins right 
    
    """
    #
    if isinstance ( ibias , ROOT.TH1 ) : return _h1_rrshift_ ( ibias , h ) 
    ##
    if not isinstance ( ibias , ( int , long ) ) : return NotImplemented 
    ##
    return _h1_lshift_ ( h1 , -1 * ibias )


ROOT.TH1F .   shift     = _h1_shift_
ROOT.TH1D .   shift     = _h1_shift_
ROOT.TH1D . __rshift__  = _h1_rshift_
ROOT.TH1F . __rshift__  = _h1_rshift_
ROOT.TH1D . __lshift__  = _h1_lshift_
ROOT.TH1F . __lshift__  = _h1_lshift_
ROOT.TH1D . __ilshift__ = _h1_ilshift_
ROOT.TH1F . __ilshift__ = _h1_ilshift_
ROOT.TH1D . __irshift__ = _h1_irshift_
ROOT.TH1F . __irshift__ = _h1_irshift_


# =============================================================================    
for t in ( ROOT.TH1F , ROOT.TH1D ) :    
    t . accumulate = _h1_accumulate_ 
    t . sum        = _h1_sum_ 
    t . integral   = _h1_sum_ 

for t in ( ROOT.TH2F , ROOT.TH2D ) :    
    t . accumulate = _h2_accumulate_ 
    t . sum        = _h2_accumulate_
    t . integral   = _h2_accumulate_ 

## generic
ROOT.TH1 . scale      = _h_scale_

    
HStats   = cpp.Gaudi.Utils.HStats

# =============================================================================
## calculate bin-by-bin momenta 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_moment_ ( h1 , order ) :
    """
    Get ``bin-by-bin''-moment around the specified value
    
    >>> histo = ...
    >>> mom   = histo.moment ( 4 , 0 ) 
    """
    #
    m = HStats.moment    ( h1 , order )
    e = HStats.momentErr ( h1 , order )
    #
    return VE ( m , e*e )  if 0<= e  else VE ( m , -e*e )

_h1_moment_ .__doc__ += '\n' + HStats.moment    .__doc__
_h1_moment_ .__doc__ += '\n' + HStats.momentErr .__doc__

# =============================================================================
## calculate bin-by-bin central momenta 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_central_moment_ ( h1 , order ) :
    """
    Get ``bin-by-bin'' central moment
    
    >>> histo = ...
    >>> cmom  = histo.centralMoment ( 4 ) 
    """
    #
    m = HStats.centralMoment    ( h1 , order )
    e = HStats.centralMomentErr ( h1 , order )
    #
    return VE ( m , e*e )  if 0<= e  else VE ( m , -e*e ) 

_h1_central_moment_ .__doc__ += '\n' + HStats.centralMoment    .__doc__
_h1_central_moment_ .__doc__ += '\n' + HStats.centralMomentErr .__doc__

# =============================================================================
## get skewness
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_skewness_ ( h1 ) :
    """
    Get the skewness

    >>> histo = ...
    >>> skew  = histo.skewness () 
    """
    m = HStats.skewness    ( h1 )
    e = HStats.skewnessErr ( h1 )
    #
    return VE ( m , e*e )  if 0<= e  else VE ( m , -e*e ) 

_h1_skewness_ .__doc__ += '\n' + HStats.skewness    .__doc__
_h1_skewness_ .__doc__ += '\n' + HStats.skewnessErr .__doc__

# =============================================================================
## get kurtosis
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_kurtosis_ ( h1 ) :
    """
    Get the kurtosis

    >>> histo = ...
    >>> k     = histo.kurtosis () 
    """
    m = HStats.kurtosis    ( h1 )
    e = HStats.kurtosisErr ( h1 )
    #
    return VE ( m , e*e )  if 0<= e  else VE ( m , -e*e ) 

_h1_kurtosis_ .__doc__ += '\n' + HStats.kurtosis    .__doc__
_h1_kurtosis_ .__doc__ += '\n' + HStats.kurtosisErr .__doc__

# =============================================================================
## get mean
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_mean_ ( h1 ) :
    """
    Get the mean

    >>> histo = ...
    >>> k     = histo.mean () 
    """
    m = HStats.mean    ( h1 )
    e = HStats.meanErr ( h1 )
    #
    return VE ( m , e*e )  if 0<= e  else VE ( m , -e*e ) 

_h1_mean_ .__doc__ += '\n' + HStats.mean    .__doc__
_h1_mean_ .__doc__ += '\n' + HStats.meanErr .__doc__

# =============================================================================
## get RMS
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def _h1_rms_ ( h1 ) :
    """
    Get the rms
    
    >>> histo = ...
    >>> s     = histo.rms () 
    """
    m = HStats.rms    ( h1 )
    e = HStats.rmsErr ( h1 )
    #
    return VE ( m , e*e )  if 0<= e  else VE ( m , -e*e ) 

_h1_rms_ .__doc__ += '\n' + HStats.rms    .__doc__
_h1_rms_ .__doc__ += '\n' + HStats.rmsErr .__doc__

for h in ( ROOT.TH1F , ROOT.TH1D ) :
    
    h.mean           = _h1_mean_
    h.rms            = _h1_rms_
    h.skewness       = _h1_skewness_
    h.kurtosis       = _h1_kurtosis_
    h.moment         = _h1_moment_
    h.centralMoment  = _h1_central_moment_
    #
    h.nEff           = h.GetEffectiveEntries 


# =============================================================================
## get some statistic information on the histogram content
#  @code 
#  >>> histo = ... 
#  >>> stat  = histo.stat()
#  >>> print stat
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-03-26
def _h_stat_ ( h ) :
    """
    Get some statistic infomration on the histogram content

    >>> histo = ... 
    >>> stat  = histo.stat()
    >>> print stat 
    """
    cnt = SE() 
    for i in h : cnt += h[i].value()  
    return cnt

ROOT.TH1.stat = _h_stat_

# =============================================================================
## get some (weighted) statistic information on the histogram content
#  @code 
#  >>> histo = ... 
#  >>> wstat = histo.wstat()
#  >>> print wstat
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-03-26
def _h_wstat_ ( h ) :
    """
    Get some weighted statistic infomration on the histogram content

    >>> histo = ... 
    >>> wstat  = histo.wstat()
    >>> print wstat 
    """
    cnt = WSE() 
    for i in h :
        v = h[i]
        cnt.add ( v.value() , 1.0/v.cov2() ) 
    return cnt

ROOT.TH1.wstat = _h_wstat_

# =============================================================================
## adjust the "efficiency"
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
def ve_adjust ( ve , mn = 0 , mx = 1.0 ) :
    """
    Adjust the efficiency
    """
    if ve.value() < mn : ve.setValue ( mn )
    if ve.value() > mx : ve.setValue ( mx )
    #
    return ve

# =============================================================================
## draw the line for the histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-01-21 
def _level_ ( self , level = 0 , linestyle = 2 ) :
    """
    Draw NULL-line for the histogram

    >>> h.level ( 5 )
    
    """
    mn,mx = self.xminmax() 
    line = ROOT.TLine ( mn , level , mx , level )
    line.SetLineStyle ( linestyle )
    self._line_ = line
    self._line_.Draw() 
    return self._line_
# =============================================================================
## draw null-level for histogram  
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-01-21 
def _null_ ( self , linestyle = 2 ) :
    """
    Draw NULL-line for the histogram
    
    >>> h.null() 
    """
    return _level_ ( self , 0 , linestyle ) 

ROOT.TH1D. level = _level_
ROOT.TH1F. level = _level_
ROOT.TH1D. null  = _null_
ROOT.TH1F. null  = _null_


# =============================================================================
## make the solution for equation   h(x)=v
#
#  @code 
#    >>> histo = ...
#    >>> value = ...
#    >>> solutions = histo.solve ( values )
#  @endcode
#  @author Vanya BELYAVE Ivan.Belyaev@itep.ru
#  @date 2013-05-13
def _solve_ ( h1 , value ) :
    """
    Make a solution for equation h(x)=v
    
    >>> histo = ...
    >>> value = ...
    >>> solutions = histo.solve ( values )
    
    """
    #
    if hasattr ( value , 'value' ) : value = value.value()
    #
    solutions = []
    _size = len ( h1 ) 
    for i in h1.iteritems() :

        ibin = i[0]
        x    = i[1]
        y    = i[2]

        yi   = y.value()
        if value == yi : 
            solutions.append  ( x.value () )
            continue

        di   = value - yi 
        xi   = x.value()

        j = ibin + 1
        if not j <= _size : continue

        yj  = h1.GetBinContent ( j )
        xj  = h1.GetBinCenter  ( j )
        
        dj   = value - yj 

        if 0 <= di * dj : continue  ## the same sign, skip 
        
        dd   = yi - yj
        
        if 0 == dd    : continue
        
        xs   = ( xi * dj  - xj * di ) / dd 

        solutions.append ( xs )


    return tuple  ( solutions ) 

# =============================================================================
## propose edge for "equal" bins 
#
#  @code
#
#    >>> histo = ....
#    >>> edges = histo.equal_edges ( 10 )
#
#  @endcode
#  @author Vanya BELYAVE Ivan.Belyaev@itep.ru
#  @date 2013-05-13
def _equal_edges_ ( h1 , num  , wmax = -1 ) :
    """
    Propose the edged for ``equal-bins''

    >>> histo = ....
    >>> edges = histo.equal_edges ( 10 )
    
    """
    if not isinstance ( num , ( int , long ) ) :
        return TypeError, "'num' is not integer!"
    elif 1 >  num :
        return TypeError, "'num' is invalid!"
    elif 1 == num :
        _bins = [ h1.xmin() , h1.xmax() ] ## trivial binnig scheme
    else :
        
        ## integrate it! 
        _eff = h1.effic()
        
        _bins = [ h1.xmin()  ]
        d     = 1.0 / num
        
        from   Ostap.PyRoUts         import _solve_

        for i in range ( 1 , num ) :
            vi  = float ( i ) / num
            eqs = _solve_ ( _eff , vi )
            if not eqs : continue
            _bins.append ( eqs[0] )
            
        _bins.append ( h1.xmax() )

    #
    ## split if needed
    #
    if 0 < wmax :
        _lst  = []
        _lst.append ( _bins[0] )
        for i in range ( 1 , len( _bins ) ) :
            b    = _bins[i]
            last = _lst[-1] 
            dist = b - last 
            if dist > wmax :
                n   = 1 + int ( dist // wmax ) 
                dlt = dist / float( n )
                for j in range(1,n) :
                    _lst.append ( last + j * dlt )
            _lst.append ( b )
            
        _bins = _lst 

    return tuple ( _bins ) 
    
ROOT.TH1F . solve       = _solve_                              
ROOT.TH1D . solve       = _solve_                              
ROOT.TH1F . equal_edges = _equal_edges_                              
ROOT.TH1D . equal_edges = _equal_edges_                         

# =============================================================================
# slices
# =============================================================================
## define 2D slice for 2D-histogram
def _h2_get_slice_ ( h2 , axis , ibins ) :
    """
    Get 1D-slice for 2D-histogram

    >>> histo2 = ...
    >>> h1 = histo2.sliceX ( 1        ) ## along 1st X-bin
    >>> h2 = histo2.sliceY ( [2,4,19] ) ## along 2nd,4th and 19th Y-bins
    
    """
    #
    if   1 == axis :
        if isinstance ( ibins , ( int , long ) ) :
            if not ibins in h2.GetXaxis() :
                raise TypeError, 'Illegal bin  index %s' % ibins
            ibins = ibins,
        h_slice = h1_axis ( h2.GetYaxis () ,
                            title  = h2.GetTitle() + ":X-slice %s" % ibins , 
                            double = isinstance ( h2 , ROOT.TH2D )         )              
    elif 2 == axis : 
        if isinstance ( ibins , ( int , long ) ) :
            if not ibins in h2.GetYaxis() :
                raise TypeError, 'Illegal bin  index %s' % ibins 
            ibins = ibins,
        h_slice = h1_axis ( h2.GetXaxis () ,
                            title  = h2.GetTitle() + ":Y-slice %s" % ibins , 
                            double = isinstance ( h2 , ROOT.TH2D )         )  
            
    else :
        raise TypeError, 'Illegal axis index %s' % axis 

    ## fill the slice histogram
    for i in h2 :
        
        ix = i[0]
        iy = i[1]
        
        if   1 == axis and ix in ibins : h_slice [ iy ] += h2 [ i ]
        elif 2 == axis and iy in ibins : h_slice [ ix ] += h2 [ i ]
        
    return h_slice 

ROOT.TH2F . slice  = _h2_get_slice_ 
ROOT.TH2F . sliceX = lambda s , ibin : _h2_get_slice_ ( s , 1 , ibin ) 
ROOT.TH2F . sliceY = lambda s , ibin : _h2_get_slice_ ( s , 2 , ibin ) 

ROOT.TH2D . slice  = _h2_get_slice_
ROOT.TH2D . sliceX = lambda s , ibin : _h2_get_slice_ ( s , 1 , ibin ) 
ROOT.TH2D . sliceY = lambda s , ibin : _h2_get_slice_ ( s , 2 , ibin ) 


# =============================================================================
## define 2D slice for 3D-histogram
def _h3_get_slice_ ( h3 , axis , ibins ) :
    """
    Get 2D-slice for 3D-histogram
    
    >>> histo3 = ...
    >>> h1 = histo3.sliceX ( 1          ) ## along 1st X-bin
    >>> h2 = histo3.sliceY ( [2,4,19]   ) ## along 2nd,4th and 19th Y-bins
    >>> h3 = histo3.sliceZ ( range(3,8) ) ## along 3-8th Z-bins

    """
    #
    if   1 == axis :
        if isinstance ( ibins , (int,long) ) : 
            if not ibins in h3.GetXaxis() :
                raise TypeError, 'Illegal bin  index %s' % ibins
            ibins = ibins ,
        h_slice = h2_axes ( h3.GetYaxis () ,
                            h3.GetZaxis () ,
                            title  = h3.GetTitle()+ ":X-slice %s" % ibins , 
                            double = isinstance ( h3 , ROOT.TH3D )        )
        
    elif 2 == axis : 
        if isinstance ( ibins , (int,long) ) : 
            if not ibins in h3.GetYaxis() :
                raise TypeError, 'Illegal bin  index %s' % ibins
            ibins = ibins ,
        h_slice = h2_axes ( h3.GetXaxis () ,
                            h3.GetZaxis () ,
                            title  = h3.GetTitle()+ ":Y-slice %s" % ibnis , 
                            double = isinstance ( h3 , ROOT.TH2D )        )             
    elif 3 == axis : 
        if isinstance ( ibins , (int,long) ) : 
            if not ibins in h3.GetZaxis() :
                raise TypeError, 'Illegal bin  index %s' % ibins 
            ibins = ibins ,
        h_slice = h2_axes ( h3.GetXaxis () , 
                            h3.GetYaxis () ,
                            title  = h3.GetTitle()+ ":Z-slice %s" % ibins , 
                            double = isinstance ( h3 , ROOT.TH2D ) )     
    else :
        raise TypeError, 'Illegal axis index %s' % axis 

    ## fill the slice histogram
    for i in h3 :
        
        ix = i[0]
        iy = i[1]
        iz = i[2]
        
        if   1 == axis and ix in ibins : h_slice [ iy , iz ] += h3 [ i ]
        elif 2 == axis and iy in ibins : h_slice [ ix , iz ] += h3 [ i ]
        elif 3 == axis and iz in ibins : h_slice [ ix , iy ] += h3 [ i ] 

    return h_slice 

ROOT.TH3F . slice  = _h3_get_slice_
ROOT.TH3F . sliceX = lambda s,ibin : _h3_get_slice_ ( s , 1 , ibin ) 
ROOT.TH3F . sliceY = lambda s,ibin : _h3_get_slice_ ( s , 2 , ibin ) 
ROOT.TH3F . sliceZ = lambda s,ibin : _h3_get_slice_ ( s , 3 , ibin ) 

ROOT.TH3D . slice  = _h3_get_slice_ 
ROOT.TH3D . sliceX = lambda s,ibin : _h3_get_slice_ ( s , 1 , ibin ) 
ROOT.TH3D . sliceY = lambda s,ibin : _h3_get_slice_ ( s , 2 , ibin ) 
ROOT.TH3D . sliceZ = lambda s,ibin : _h3_get_slice_ ( s , 3 , ibin ) 


# ============================================================================
## make projections
#  see ROOT::TH2::ProjectionX
#  see ROOT::TH2::ProjectionY
def _h2_proj_ ( h2 , axis , *args ) :
    """
    Make histogram projection along the axis

    >>> histo2 = ...
    >>> hx = histo2.projX ()
    >>> hy = histo2.projY ()
    
    """
    if   1 == axis : return h2.ProjectionX ( hID () , *args ) 
    elif 2 == axis : return h2.ProjectionY ( hID () , *args )

    raise TypeError, 'Illegal axis index %s' % axis 

_h2_proj_ . __doc__ += '\n'
_h2_proj_ . __doc__ += ROOT.TH2.ProjectionX.__doc__
_h2_proj_ . __doc__ += ROOT.TH2.ProjectionY.__doc__


ROOT.TH2F . proj   = _h2_proj_
ROOT.TH2F . projX  = lambda s , *args : _h2_proj_ ( s , 1 , *args )
ROOT.TH2F . projY  = lambda s , *args : _h2_proj_ ( s , 2 , *args )

ROOT.TH2D . proj   = _h2_proj_
ROOT.TH2D . projX  = lambda s , *args : _h2_proj_ ( s , 1 , *args )
ROOT.TH2D . projY  = lambda s , *args : _h2_proj_ ( s , 2 , *args )


ROOT. TH3 . projX  = lambda s : s.Project ( 'x'  )
ROOT. TH3 . projY  = lambda s : s.Project ( 'y'  )
ROOT. TH3 . projZ  = lambda s : s.Project ( 'z'  )
ROOT. TH3 . projXY = lambda s : s.Project ( 'xy' )
ROOT. TH3 . projYX = lambda s : s.Project ( 'yx' )
ROOT. TH3 . projXZ = lambda s : s.Project ( 'xz' )
ROOT. TH3 . projZX = lambda s : s.Project ( 'zx' )
ROOT. TH3 . projYZ = lambda s : s.Project ( 'yz' )
ROOT. TH3 . projZY = lambda s : s.Project ( 'zy' )


# =============================================================================
# Other decorations 
# =============================================================================
import Ostap.TreeDeco
import Ostap.MinuitDeco
import Ostap.RooFitDeco
import Ostap.HTextDeco
import Ostap.HParamDeco
import Ostap.HCmpDeco
import Ostap.TFileDeco
import Ostap.MiscDeco
import Ostap.Models 


from Ostap.GraphDeco import makeGraph, hToGraph, hToGraph2, hToGraph3, lw_graph  

## restore the warnings level 
ROOT.gROOT.ProcessLine("gErrorIgnoreLevel = %d; " % iLevel ) 

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
