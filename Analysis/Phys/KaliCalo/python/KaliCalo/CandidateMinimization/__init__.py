#!/usr/bin/env python
# =============================================================================
# @file __init__.py
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @reshuffled by Albert Puig (Albert.Puig@cern.ch)
# @date 2010-11-09
# =============================================================================
"""
"""

__author__  = " Vanya BELYAEV Ivan.Belyaev@itep.ru "
__date__    = "$Date$"
__version__ = "$Revision$"

__all__ = ['Histos', 'HistoMap']

from KaliCalo.Kali.HistoBase import HistosBase as _HistosBase
from KaliCalo.Kali.HistoBase import HistoMapBase as _HistoMapBase

class Histos( _HistosBase ):
    def __init__ ( self, cellID , background ):
        histoList = [ 'All' , 'PrsLT10' , 'All_BG' , 'PrsLT10_BG' , 'All_Subtracted' , 'PrsLT10_Subtracted' ]
        _HistosBase.__init__ ( self , cellID , histoList , 2 )
        
class HistoMap( _HistoMapBase ):
    def __init__ ( self, background = True , *args ):
        _HistoMapBase.__init__ ( self , Histos , background , *args )

# =============================================================================
## get the histograms fomr data base (databases)
def  getHistosFromDB ( dbases ) :
    """
    Get the histograms from data base (databases)
    """
    if issubclass ( type ( dbases ) , HistoMap ) :
        return dbases
    histos = HistoMap()
    histos.updateFromDB ( dbases )
    return histos

# EOF
