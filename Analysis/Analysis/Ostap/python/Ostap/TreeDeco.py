#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file
#  Module with decoration of Tree/Chain objects for efficient use in python
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
#
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
"""Decoration of Tree/Chain objects for efficient use in python"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-06-07"
__all__     = () 
# =============================================================================
import ROOT, cppyy              ## attention here!!
cpp = cppyy.makeNamespace('')
VE  = cpp.Gaudi.Math.ValueWithError 
# 
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger 
if '__main__' ==  __name__ : logger = getLogger( 'Ostap.TreeDeco' )
else                       : logger = getLogger( __name__ )
# =============================================================================
logger.debug ( 'Some useful decorations for Tree/Chain objects')
# =============================================================================
_large = 2**63
# =============================================================================
## Iterator over ``good events'' in TTree/TChain:
#  @code 
#    >>> tree = ... # get the tree
#    >>> for i in tree.withCuts ( 'pt>5' ) : print i.y
#  @endcode
#  @attention: TTree::GetEntry is already invoked for accepted events,
#              no need in second call
#  @see Analysis::PyIterator
#  @see Analysis::Formula
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-05-06
def _iter_cuts_ ( self , cuts , first = 0 , last = _large , progress = False ) :
    """
    Iterator over ``good events'' in TTree/TChain:
    
    >>> tree = ... # get the tree
    >>> for i in tree.withCuts ( 'pt>5' ) : print i.y
    
    Attention: TTree::GetEntry is already invoked for accepted events,
               no need in second call 
    """
    #
    last = min ( last , len ( self )  )
    if last < first : return 
        
    _pit = cpp.Analysis.PyIterator ( self , cuts , first , last )
    if  not _pit.ok() :
        raise TypeError ( "Invalid Formula: %s" % cuts )
    #
    step = 2000.0 / ( last - first ) 
    from Ostap.progress_bar import ProgressBar 
    with ProgressBar ( min_value = first        ,
                       max_value = last  - 1    ,
                       silent    = not progress ) as bar :
        _t = _pit.tree()
        while _t :
            
            current = _pit.current() - 1 
            s       = long ( step * ( current - first ) )
            if 0 == s % 10 or \
               0 == current % 50000 or \
               0 == ( current - first ) % 10000 : 
                bar.update_amount( current )
                
            yield _t
            _t   = _pit.next()

            #
        del _pit

ROOT.TTree .withCuts  = _iter_cuts_ 
ROOT.TChain.withCuts  = _iter_cuts_ 

ROOT.TTree. __len__   = lambda s : s.GetEntries()


# =============================================================================
## help project method for ROOT-trees and chains 
#
#  @code 
#    >>> h1   = ROOT.TH1D(... )
#    >>> tree.Project ( h1.GetName() , 'm', 'chi2<10' ) ## standart ROOT 
#    
#    >>> h1   = ROOT.TH1D(... )
#    >>> tree.project ( h1.GetName() , 'm', 'chi2<10' ) ## ditto 
#    
#    >>> h1   = ROOT.TH1D(... )
#    >>> tree.project ( h1           , 'm', 'chi2<10' ) ## use histo
#  @endcode
#
#  @see TTree::Project
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-07-06
def _tt_project_ ( tree , histo , what , *args ) :
    """
    Helper project method

    >>> tree = ...
    
    >>> h1   = ROOT.TH1D(... )
    >>> tree.Project ( h1.GetName() , 'm', 'chi2<10' ) ## standart ROOT 
    
    >>> h1   = ROOT.TH1D(... )
    >>> tree.project ( h1.GetName() , 'm', 'chi2<10' ) ## ditto 
    
    >>> h1   = ROOT.TH1D(... )
    >>> tree.project ( h1           , 'm', 'chi2<10' ) ## use histo 
    
    """
    #
    hname = histo 
    if hasattr (  histo , 'GetName' ) : hname = histo.GetName()
    #
    if args and isinstance ( args[0] , ROOT.TCut ) :
        _args     = list  ( args     )
        _args [0] = str   ( _args[0] )  
        args      = tuple ( _args    )
    #
    from Ostap.TFileDeco import ROOTCWD
    with ROOTCWD() :
        ROOT.gROOT.cd () 
        ## make projection 
        result = tree.Project ( hname , what , *args )
        if   isinstance ( histo , ROOT.TH1 ) : return result, histo
        elif isinstance ( histo , str      ) :
            h = ROOT.gROOT.FindObject ( hname )
            if h : return result, h
            
    return result, hname

ROOT.TTree .project = _tt_project_
ROOT.TChain.project = _tt_project_

# =============================================================================
## get the statistic for certain expression in Tree/Dataset
#  @code
#  tree  = ... 
#  stat1 = tree.statVar( 'S_sw/effic' )
#  stat2 = tree.statVar( 'S_sw/effic' ,'pt>1000')
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-09-15
def _stat_var_ ( tree , expression , *cuts ) :
    """
    Get a statistic for the  expression in Tree/Dataset
    
    >>> tree  = ... 
    >>> stat1 = tree.statVar ( 'S_sw/effic' )
    >>> stat2 = tree.statVar ( 'S_sw/effic' ,'pt>1000')
    
    """
    return cpp.Analysis.StatVar.statVar ( tree , expression , *cuts )

ROOT.TTree     . statVar = _stat_var_
ROOT.TChain    . statVar = _stat_var_

# =============================================================================
## @var _h_one_
#  special helper histogram for summation
_h_one_ = ROOT.TH1D( "_h_one_", '' , 3 , -1 , 2 ) ; _h_one_.Sumw2()
# =============================================================================
## make a sum over expression in Tree/Dataset
#
#  @code
#
#  >>> dataset = ...
#  ## get corrected number of events 
#  >>> n_corr  = dataset.sumVar ( "S_sw/effic" )
#
#  @endcode
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-09-15
def _sum_var_old_ ( tree , expression ) :
    """
    Make a sum over expression in Tree/Dataset
    
    >>> dataset = ...
    ## get corrected number of signale events  
    >>> n_corr  = dataset.sumVar ( 'S_sw/effic' )
    
    """
    _h_one_.Reset() 
    tree.project ( _h_one_ , '1' , expression )
    return _h_one_.accumulate()

    
ROOT.TTree      . sumVar_ = _sum_var_old_
ROOT.TChain     . sumVar_ = _sum_var_old_

# =============================================================================
## make a sum over expression in Tree/Dataset
#
#  @code
#
#  >>> dataset = ...
#
#  ## get corrected number of events 
#  >>> n_corr     = dataset.sumVar ( "S_sw/effic" )
#
#  ## get corrected number of events 
#  >>> n_corr_pt  = dataset.sumVar ( "S_sw/effic" , 'pt>1')
#
#  @endcode
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-09-15
def _sum_var_ ( tree , expression , *cuts ) :
    """
    Make a sum over expression in Tree/Dataset
    
    >>> dataset = ...
    ## get corrected number of signal events  
    >>> n_corr     = dataset.sumVar ( 'S_sw/effic' )
    
    ## get corrected number of signal events  
    >>> n_corr_pt  = dataset.sumVar ( 'S_sw/effic' , 'pt>1')
    
    """
    w = tree.statVar ( expression , *cuts )
    return VE ( w.sum() , w.sum2() )

ROOT.TTree      . sumVar = _sum_var_
ROOT.TChain     . sumVar = _sum_var_

# =============================================================================
## get the leaves for the given tree/chain
#  @see TTree
#  @code
#
#  >>> tree = ...
#  >>> lst = tree.leaves()
#  >>> for l in lst : print l
#
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-02-04
def _rt_leaves_ ( t ) :
    """
    Get the list of leaves names
    
    
    """
    _lst =  t.GetListOfLeaves()
    if not _lst : return tuple() 
    _lst = [ l.GetName() for l in _lst ] 
    _lst.sort()
    return tuple( _lst ) 

ROOT.TTree.leaves   = _rt_leaves_

# =============================================================================
## get the branches for the given tree/chain
#  @see TTree
#  @code
#
#  >>> tree = ...
#  >>> lst = tree.branches()
#  >>> for b in lst : print b
#
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-02-04
def _rt_branches_ ( t ) :
    """
    Get the list of branch names
    
    >>> tree = ...
    >>> lst = tree.branches()
    >>> for b in lst : print b
    
    """
    _lst =  t.GetListOfBranches()
    if not _lst : return tuple() 
    _lst = [ l.GetName() for l in _lst ] 
    _lst.sort()
    return tuple( _lst ) 

ROOT.TTree.branches = _rt_branches_


# =============================================================================
## simplified printout for TTree/TChain
#  @see TTree
#  @code
#
#  >>> tree = ...
#  >>> print tree
#
#  @endcode 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-02-04
def _rt_print_ ( t ) :
    """
    Simplified print out for tree/chain

    >>> tree = ...
    >>> print tree
    """
    ##
    res = "Name: %s Enries/#%d" %  ( t.GetName() , t.GetEntries() ) 
    if hasattr ( t , 'GetNtrees' ) : res += " Chain/#%d " %       t.GetNtrees()
    ##
    _b          = t.branches ()
    res        +=        "\nBranches: %s" % list( _b )
    _l          = t.leaves   ()
    if _l != _b : res += "\nLeaves: %s"   % list( _l )
    return res

ROOT.TTree.__repr__ = _rt_print_
ROOT.TTree.__str__  = _rt_print_

# =============================================================================
## get lst of files used for the given chain
#  @code
#
#  >>> chain = ... ## get the files 
#  >>> files = chain.files() 
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2014-02-04
def _rc_files_ ( chain ) :
    """
    Get the list of files used for the chain
    
    >>> chain = ... ## get the files 
    >>> files = chain.files()
    """
    lst = chain.GetListOfFiles()
    return [ i.GetTitle() for i in lst ]


ROOT.TChain. files = _rc_files_


# =============================================================================
## get "slice" from TTree in a form of numpy.array
#  @code
#  tree = ...
#  varr = tree.slice('Pt','eta>3')
#  print varr 
#  @endcode 
#  @see numpy.array 
#  @author Albert BURSCHE
#  @date 2015-07-08
def _rt_slice_ ( tree , varname , cut = '' ) :
    """
    Get ``slice'' from TTree in a form of numpy.array
    ##
    >>> tree = ...
    >>> varr = tree.slice('Pt','eta>3')
    >>> print varr 
    """
    #
    ## decode the name (if needed)
    if isinstance ( varname , str ) :
        varname = varname.strip()
        varname = varname.replace ( ':' , ',' )
        varname = varname.replace ( ';' , ',' )
        varname = varname.replace ( ' ' , ',' )
        varname = varname.split   (       ',' )
        if 1 == len ( varname ) : varname = varname[0].strip()
        else :
            for i in range( 0 , len(varname) ) : 
                varname[i] = varname[i].strip()  
    #
    if       isinstance ( varname ,  ( list , tuple ) ) :
        ## forward to appropriate method 
        return tree.slices ( varname , cut )
    elif not isinstance ( varname , str ) :
        raise AttibuteError ( 'Invalid type %s' % varname )
    
    ##
    p1 = varname.find( '[')
    if 0 < p1 :
        p2 = varname.find( ']' , p1 + 1 )
        if p1 < p2 :
            raise AttributeError("TTree:slice: can't slice array-like variable '%s'" % varname )
            
    ge   = long( tree.GetEstimate() ) 
    tree.SetEstimate ( max ( len ( tree ) , ge ) )
    ##
    n    = tree.Draw ( varname , cut , "goff" )
    ##
    import numpy
    sl =   numpy.array ( numpy.frombuffer ( tree.GetV1() , count = n ) , copy = True )
    ##
    tree.SetEstimate ( ge ) 
    return sl 


# =============================================================================
## get "slices" from TTree in a form of numpy.array
#  @code
#  tree = ...
#  varrs1 = tree.slices ( ['Pt','eta'] , 'eta>3' )
#  print varrs1 
#  varrs2 = tree.slices (  'Pt , eta'  , 'eta>3' )
#  print varrs2
#  varrs3 = tree.slices (  'Pt : eta'  , 'eta>3' )
#  print varrs3
#  @endcode 
#  @see numpy.array 
#  @author Albert BURSCHE
#  @date 2015-07-08  
def _rt_slices_ ( tree , varnames , cut = '' ) :
    """
    Get ``slices// from TTree in a form of numpy.array
    
    >>> tree = ...
    
    >>> varrs1 = tree.slices( ['Pt' , 'eta'] ,'eta>3')
    >>> print varrs1
    
    >>> varrs2 = tree.slices( 'Pt,eta'  ,'eta>3')
    >>> print varrs2
    
    >>> varrs3 = tree.slices( 'Pt : eta' ,'eta>3')
    >>> print varrs3
    """
    #
    varname = varnames 
    ## decode the name (if needed)
    for sep in ( ',' , ':' , ';' ) :
        if isinstance ( varname , str ) :
            varname = varname.strip() 
            varname = varname.split( sep )
            if 1 == len ( varname ) : varname = varname[0].strip()
            else :
                for i in range( 0 , len(varname) ) : 
                    varname[i] = varname[i].strip()  
    #
    if       isinstance ( varname , str ) :
        ## forward to appropriate method 
        return tree.slice ( varname , cut )
    elif not isinstance ( varname ,  ( list , tuple ) ) :
        raise AttibuteError ( 'Invalid type %s' % varname )
    ##
    import numpy
    a = numpy.array ( [tree.slice(name, cut) for name in varname ] )
    a.sort()
    return a


ROOT.TTree .slice  = _rt_slice_
ROOT.TTree .slices = _rt_slices_

def _not_implemented_ ( self , method , *args , **kwargs ) :
    raise NotImplementedError('%s: the method "%s" is not implemented' % ( self.__class__ , method ) ) 

ROOT.TChain.slice  = lambda s,*x : _not_implemented_( s , 'slice'  , *x ) 
ROOT.TChain.slices = lambda s,*x : _not_implemented_( s , 'slices' , *x ) 


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
