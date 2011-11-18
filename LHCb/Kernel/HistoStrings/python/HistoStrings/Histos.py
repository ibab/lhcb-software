#!/usr/bin/env python
# =============================================================================
## @file HistoStrings/Histos.py
#  Helper module for  histogram <--> string conversion
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-21
# =============================================================================
"""

Helper module for  histogram <--> string conversion

>>> import HistoStrings.Histos


"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $"
## nothing is exported 
__all__     = ()  # nothing is exported 
# =============================================================================

import ROOT 
from   GaudiPython.Bindings import gbl as cpp


_HS = cpp.Gaudi.Utils.Histos.HistoStrings

# =============================================================================
## helper fucntion for converison of histogram to string
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-21
def __to_string__ ( self , *args ) :
    """
    Helper function for converison of ROOT(or AIDA) histogram to string :

    >>> histo

    >>> print histo.toString()        ## print as string
    >>> print histo.toString ( True ) ## print as XML string

    """
    return _HS.toString ( self , *args )

# =============================================================================
## helper fucntion for converison of string to the histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-21
def __from_string__ ( self , input ) :
    """
    Helper function for converison of string to ROOT(or AIDA) histogram 

    >>> mystr = ...

    >>> code = histo.fromString( mystr )      

    """
    return _HS.fromString ( self , input )


# =============================================================================
## helper fucntion for converison of histogram to XML-string
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-21
def __to_xml__ ( self ) :
    """
    Helper function for converison of ROOT(or AIDA) histogram to XML string :

    >>> histo

    >>> print histo.toXml ()        ## print as XML-string

    """
    return _HS.toXml ( self  )

# =============================================================================
## helper fucntion for converison of XML-string to the histogram 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-21
def __from_xml__ ( self , input ) :
    """
    Helper function for converison of XML-string to ROOT(or AIDA) histogram 

    >>> mystr = ...

    >>> code = histo.fromXml ( mystr )      

    """
    return _HS.fromXml  ( self , input )


__to_string__   . __doc__  += '\n' + _HS.toString   .__doc__ 
__to_xml__      . __doc__  += '\n' + _HS.toXml      .__doc__ 
__from_string__ . __doc__  += '\n' + _HS.fromString .__doc__ 
__from_xml__    . __doc__  += '\n' + _HS.fromXml    .__doc__ 



for t in ( ROOT.TH1D             ,
           ROOT.TH2D             ,
           ROOT.TH3D             , 
           ROOT.TH1F             ,
           ROOT.TH2F             ,
           ROOT.TH3F             , 
           ROOT.TProfile         ,
           ROOT.TProfile2D       , 
           cpp.AIDA.IHistogram1D ,
           cpp.AIDA.IHistogram2D ,
           cpp.AIDA.IHistogram3D ,
           cpp.AIDA.IProfile1D   ,
           cpp.AIDA.IProfile2D   ) :
    
    if not hasattr ( t , '__toString__'   ) :
        setattr ( t , '__toString__'   , __to_string__   )
        
    if not hasattr ( t ,   'toString'     ) :
        setattr ( t ,   'toString'     , __to_string__   )
        
    if not hasattr ( t , '__toXml__'      ) :
        setattr ( t , '__toXml__'      , __to_xml__      )
        
    if not hasattr ( t ,   'toXml'        ) :
        setattr ( t ,   'toXml'        , __to_xml__      )
        
    if not hasattr ( t , '__fromString__' ) :
        setattr ( t , '__fromString__' , __from_string__ )
        
    if not hasattr ( t ,   'fromString'   ) :
        setattr ( t ,   'fromString'   , __from_string__ )
        
    if not hasattr ( t , '__fromXml__' ) :
        setattr ( t , '__fromXml__'    , __from_xml__    )
        
    if not hasattr ( t ,   'fromXml'   ) :
        setattr ( t ,   'fromXml'      , __from_xml__    ) 

# =============================================================================
if '__main__' == __name__ :
    print __doc__
    print __author__
    print __version__
    
# =============================================================================
    

# =============================================================================
# The END 
# =============================================================================

