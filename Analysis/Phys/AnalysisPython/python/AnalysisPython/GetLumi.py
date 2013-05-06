#!/usr/bin/env python
# =============================================================================
# $Id$
# =============================================================================
## @file
#  Helper function to extract luminosity 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-10-16
#
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
"""
Helper function to extract luminosity 

Get lumi :

>>> l1 = getLumi ( 'myfile.root' )
>>> l2 = getLumi ( tree  )
>>> l3 = getLumi ( chain )
>>> l4 = getLumi ( file  )
>>> l5 = getLumi ( [ any sequence of above ]  )

"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2012-10-16"
# =============================================================================
__all__     = (
    #
    'getLumi'  ,  ## get the lumi
    #
    )
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger 
logger = getLogger( __name__ )
# =============================================================================
## get luminotsity from Lumi tuple
#
#  @param data  (INPUT) tree, chain, file, filename or sequence
#  @return the luminosity
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-09-20
def getLumi ( data , *args ) :
    """
    Get lumi :
    
    >>> l1 = getLumi ( 'myfile.root' )
    >>> l2 = getLumi ( tree  )
    >>> l3 = getLumi ( chain )
    >>> l4 = getLumi ( file  )
    >>> l5 = getLumi ( [ any sequence of above ]  )

    """
    tree_name = 'GetIntegratedLuminosity/LumiTuple'
    #
    import ROOT 
    from AnalysisPython.PyRoUts import VE, hID  
    #
    if args :
        data = [ data ]
        for a in args : data.append ( a )
        return getLumi ( data ) 
        
    if isinstance ( data , str ) :
        ## expand the actual file name
        import os 
        data = os.path.expandvars ( data )
        data = os.path.expanduser ( data )
        data = os.path.expandvars ( data )
        data = os.path.expandvars ( data )
        
        try :    
            tree = ROOT.TChain (  tree_name ) 
            tree.Add ( data )   
            lumi = getLumi ( tree )
            return lumi
        except :
            logger.error('Unable to get lumi(1) for %s' % data )
            return VE()
        
        #
    if isinstance ( data , ROOT.TFile ) :
        try :
            tree = data.Get( tree_name ) 
            return getLumi ( tree ) 
        except:
            logger.error('Unable to get lumi(2) for %s' % data.GetName() )
            return VE()
        
    if isinstance ( data , ROOT.TTree ) :
        try:
            h = ROOT.TH1F( hID() , '' , 10, -1, 4 ) ; h.Sumw2()
            l1 = VE()
            #
            data.Project ( h.GetName() , " 1 " ,  '1.0*IntegratedLuminosity' ) 
            l1 = h.accumulate()
            #
            data.Project ( h.GetName() , " 1 " ,
                           '1.0*IntegratedLuminosity+0.0*IntegratedLuminosityErr' ) 
            l1 = h.accumulate()
            # 
            data.Project ( h.GetName() , " 1 " ,
                           '1.0*IntegratedLuminosity+1.0*IntegratedLuminosityErr' ) 
            l2 = h.accumulate()
            # 
            data.Project ( h.GetName() , " 1 " ,
                           '1.0*IntegratedLuminosity-1.0*IntegratedLuminosityErr' ) 
            l3 = h.accumulate()
            # 
            #
            l1.setError ( 0.5 * abs( l2.value () - l3.value () ) )
            #
            # 
            return l1
        except :
            logger.error('Unable to get lumi(3) for %s' % data.GetName() )
            return VE()

    l = VE() 
    for i in data :
        k = getLumi ( i ) 
        l = VE ( l.value() + k.value() , ( l.error() + k.error () ) ** 2 ) 

    return l 


# =============================================================================
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print ' Symbols : ' , __all__    
    print 80*'*'
    
# =============================================================================
# The END 
# =============================================================================
