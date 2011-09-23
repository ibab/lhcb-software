#!/usr/bin/env python
# ============================================================================
# $Id:$
# ============================================================================
# @file AnalysisPython/uStat.py
#
# Helper module to get ``U-statistics'' useful for ``Goodnes-Of-Fit'' tests
#
# This is a simple translation of
#   the original C++ lines written by Greig Cowan into python
#
# Usage is fairly trivial:
#
#  @code
# 
#   >>> pdf  = ...               ## pdf
#   >>> data = ...               ## dataset
#   >>> pdf.fitTo( data , ... )  ## fit it!
#
#   >>> import AnalysisPython.uStat as uStat
#
#   >>> r,histo = uStat.uPlot ( pdf , data ) 
#   >>> print r                  ## print fit results
#   >>> histo.Draw()             ## plot the results  
#
#  @endcode
#
# @author Vanya Belyaev Ivan.Belyaev@cern.ch
# @date 2011-09-21
#
#                   $Revision$
# Last modification $Date$
#                by $Author$
#
# ============================================================================
"""

Helper module to get ``U-statistics'' useful for ``Goodness-Of-Fit'' tests

This is a simple translation of
  the original C++ lines written by Greig Cowan into Python

Usage is fairly trivial:

   >>> pdf  = ...               ## pdf
   >>> data = ...               ## dataset
   >>> pdf.fitTo( data , ... )  ## fit it!

   >>> import AnalysisPython.uStat as uStat

   >>> r,histo = uStat.uPlot ( pdf , data ) 
   >>> print r                  ## print fit results
   >>> histo.Draw()             ## plot the results  
   
"""
# ============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@cern.ch"
__date__    = "2010-09-21"
__version__ = "$Revision$"
# ============================================================================
__all__     = (
    "uPlot" ,  ## make  plot of U-statistics 
    "uDist" ,  ## calculate  U-statistics 
    "uCalc" ,  ## calclulate the distance between two data points 
    )
# ============================================================================

import ROOT
import AnalysisPython.PyRoUts 
from math import sqrt, exp, pi 


# =============================================================================
## calculate the distance between two data points 
#  @author Vanya Belyaev Ivan.Belyaev@cern.ch
#  @date 2011-09-21
def uDist ( x , y ) :
    """
    Calculate the distance between two data points 
    """
    
    ix = x.createIterator()
    iy = y.createIterator()
    
    dist = 0.0
    
    xv = ix.Next()
    yv = iy.Next()
    while xv and yv :

        if not hasattr ( xv  , 'getVal' ) : break 
        if not hasattr ( yv  , 'getVal' ) : break
        
        d     = xv.getVal()-yv.getVal()
        dist += d*d
        xv = ix.Next()
        yv = iy.Next()

    del ix
    del iy
    
    return sqrt( dist )


# =============================================================================
##  calculate U-statistics
#   @param pdf    (input) PDF
#   @param args   (input) arguments/variables
#   @param data   (input) dataset 
#   @param histo  (input) the histogram to be filled 
#   @param silent (input) keep the silence 
#   @author Vanya Belyaev Ivan.Belyaev@cern.ch
#   @date 2011-09-21
def uCalc ( pdf            ,
            args           , 
            data           ,
            histo          ,
            silent = False )  :
    """
    Calculate U-statistics 
    """
    numEntries = data.numEntries ()
    dim        = args.getSize    ()
    data_clone = data.Clone      ()
    
    i_print    = 100
    if   numEntries >    1000 : i_print =   500
    elif numEntries >   10000 : i_print =  1000
    elif numEntries >  100000 : i_print =  5000
    elif numEntries > 1000000 : i_print = 10000
    
    if silent : i_print = numEntries + 1

    for i in xrange ( 0 , numEntries ) :

        if 0 == i % i_print :
            print ' uStat: process %10d/%-10d ' % (  i , numEntries )
            
        event_x = data_clone.get ( i ) 
        event_i = event_x.selectCommon ( args )
        
        ## fill args and evaluate PDF 
        for a in args : a.setVal ( event_i.getRealValue ( a.GetName () ) )        
        pdfValue = pdf.getVal ( args )
        
        small_v = 1.e+100 
        small_j = 0
        for j in xrange ( 0 , numEntries ) :
            
            if j == i  : continue

            event_y = data.get( j )
            event_j = event_y.selectCommon ( args ) 

            dist = uDist ( event_i , event_j )
            
            if 0 == j or dist < small_v :
                small_v = dist
                small_j = j 
                
        value = 0 
        if   1 == dim :
            value  = small_v
            value *= numEntries * pdfValue 
        elif 2 == dim :
            value  = small_v**2
            value *= numEntries * pdfValue
            value *= pi 
        else :
            print ' Not-implemented (yet)', dim
            continue 

        value = exp ( -1 * value )

        histo.Fill ( value ) 

        
    del data_clone 
    return histo

# =============================================================================
##  make the plot of U-statistics
#
#   @code
#
#    >>> pdf  = ...               ## pdf
#    >>> data = ...               ## dataset
#    >>> pdf.fitTo( data , ... )  ## fit it!
#    >>> vars = ...               ## get variables
#    
#    >>> import AnalysisPython.uStat as uStat
#    
#    >>> r,histo = uStat.uPlot ( pdf , data ) 
#    >>> print r                  ## print fit results
#    >>> histo.Draw()             ## plot the results
#
#   @endcode
#
#   @param pdf    (input) PDF
#   @param args   (input) arguments/variables 
#   @param data   (input) dataset 
#   @param bins   (input) bumbef of bins in histogram 
#   @param silent (input) keep the silence 
def uPlot ( pdf            ,
            data           ,
            bins   = 20    ,
            args   = None ,
            silent = False ) :
    """
    Make the plot of U-statistics 
    
    >>> pdf  = ...               ## pdf
    >>> data = ...               ## dataset
    >>> pdf.fitTo( data , ... )  ## fit it!
    
    >>> import AnalysisPython.uStat as uStat
    
    >>> r,histo = uStat.uPlot ( pdf , data ) 
    >>> print r                  ## print fit results
    >>> histo.Draw()             ## plot the results  

    """
    hID = AnalysisPython.PyRoUts.hID
    
    histo = ROOT.TH1F ( hID () ,'U-statistics', bins , 0 , 1 )
    histo.Sumw2      (   )
    histo.SetMinimum ( 0 )

    if not args : args = pdf.getObservables ( data )
    
    uCalc ( pdf       ,
            args      ,
            data      ,
            histo     ,
            silent    )    
    
    return  histo.Fit ( 'pol0' , 'SLQ' ), histo

# ===========================================================================

if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print ' Symbols : ' , __all__    
    print 80*'*'


# ===========================================================================
# The END 
# ===========================================================================
