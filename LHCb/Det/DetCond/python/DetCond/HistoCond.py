#!/usr/bin/env python
# =============================================================================
## @file DetCond/HistoCond.py
#  Decorate the Condition object with functions to extract 1d and 2d-histograms
#  @authorVanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-12-01
# =============================================================================
"""
Decorate the Condition object with functions to extract 1d and 2d-histograms, and
define functions to convert 1d and 2d histograms to Condition parameter strings.  
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl and Dmitry Golubkov"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, verison $Revision: 1.1 $"
# =============================================================================
# export nothing 
__all__ = ()                                                  ## export nothing 
# =============================================================================

## import HistoStrings.Histos
from   GaudiPython.Bindings import gbl as cpp

_C  = cpp.Condition
# =============================================================================
## function to convert 1D histogram to an xml param string
def histo1DAsParam ( h1d, name = 'name', comment = 'comment' ) :
    """
    Convert 1D histogram to an xml param string
    """
    param_head = '\n    <param name = "%s" type = "Histo1D" comment="%s">\n'
    param_tail = '\n    </param>\n'

    return param_head % (name, comment) + h1d.toString() + param_tail
# =============================================================================
## function to convert 2D histogram to an xml param string
def histo2DAsParam ( h2d, name = 'name', comment = 'comment' ) :
    """
    Convert 2D histogram to an xml param string
    """
    param_head = '\n    <param name = "%s" type = "Histo2D" comment="%s">\n'
    param_tail = '\n    </param>\n'

    return param_head % (name, comment) + h2d.toString() + param_tail
# =============================================================================
## decorate the condition object:
if not hasattr ( _C , 'paramAsH1' ) :

    _F1 = cpp.DetDesc.Params.paramAsHisto1D

    # =========================================================================
    ## Extract the 1D-histogram form condition objects
    #  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
    #  @date   2009-12-01 
    def _paramAsH1_ ( self , name ) :
        """
        Extract the 1D-histiogram from condition objects

        >>> cond = ... ## get the condition object
        >>> name = ... ## the name

        >>> h1 = cond.paramAsH1 ( name )
        >>> h1 = cond.paramAsHisto1D ( name )  # same as Condition.paramAsH1
        """
        return _F1 ( self , name )

    _paramAsH1_ . __doc__ += '\n' + _F1 . __doc__ 
    _C .paramAsH1      = _paramAsH1_
    _C .paramAsHisto1D = _paramAsH1_

# =============================================================================
## decorate the condition object:
if not hasattr ( _C , 'paramAsH2' ) :
    
    _F2 = cpp.DetDesc.Params.paramAsHisto2D   
    
    # =========================================================================
    ## Extract the 2D-histogram form condition objects
    #  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
    #  @date   2009-12-01
    def _paramAsH2_ ( self , name ) :
        """
        Extract the 2D-histiogram from condition objects

        >>> cond = ... ## get the condition object
        >>> name = ... ## the name

        >>> h2 = cond.paramAsH2 ( name )
        >>> h2 = cond.paramAsHisto2D ( name )  # same as Condition.paramAsH2()
        """
        return _F2 ( self , name )

    _paramAsH2_ . __doc__ += '\n' + _F2 . __doc__ 
    _C .paramAsH2      = _paramAsH2_
    _C .paramAsHisto2D = _paramAsH2_

if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__

# =============================================================================
# The END 
# =============================================================================
