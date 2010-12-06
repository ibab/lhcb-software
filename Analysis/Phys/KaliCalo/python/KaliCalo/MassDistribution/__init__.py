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

__all__ = ['Histos', 'HistoMap', 'getHistosFromDB']

from KaliCalo.Kali.HistoBase import HistosBase as _HistosBase
from KaliCalo.Kali.HistoBase import Histos as _Histos

class Histos( _HistosBase):
    def __init__ ( cellID , background = False ):
        histoList = [ '_LL' , '_LG' , '_GG' ]
        if background:
          histoList.extend( [ '_LL_BG' , '_LG_BG' , '_GG_BG' ] )
        _HistosBase.__init__ ( self , cellID , histoList , 2 )
        
class HistoMap( _HistoMapBase ):
    def __init__ ( background = False , *args ):
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
