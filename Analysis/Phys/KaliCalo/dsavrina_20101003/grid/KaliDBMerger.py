#!/usr/bin/env  python
# =============================================================================
## @file grig/KaliDBMerger.py
#  Helper merger script for to merge Kali-databases
#  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
#  @date 2010-03-25
# =============================================================================
"""
Helper merger script for to merge Kali-databases
"""
#  Helper merger script for to merge Kali-databases
# =============================================================================
__author__  = " Vanya BELYAEV  Ivan.Belyaev@itep.ru "
__date__    = " 2010-03-25 "
__version__ = " CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ "
# =============================================================================
__all__     = (
    'mergefiles',
    )
# =============================================================================


## the only one interesting function
def mergefiles ( filelist , outputfile ) :
    """
    The only one interesting function: merge Kali-databases
    """
    
    try :
        
        import KaliCalo.Kali as Kali
        histos = Kali.HistoMap()
        histos.read ( filelist   )
        histos.save ( outputfile ) 
        
    except :
        return -1 
    
    return 0 


# =============================================================================
if '__main__' == __main__ :
    
    ## make printout of the own documentations 
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print ' all(%s) : '    %   __all__
    
    print '*'*120  



# =============================================================================
# The END 
# =============================================================================


