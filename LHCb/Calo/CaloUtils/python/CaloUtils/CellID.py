#!/usr/bin/env python
# =============================================================================
## @file CaloUtils/CellID.py
#  Helper module to perform some decorations for CaloCellID objects
#  @see LHCb::CaloCellID
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date   2009-09-28
# =============================================================================
"""
Helper module to performs some decorations for LHCb::CaloCellID object

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__     = () 
# =============================================================================

from GaudiPython.Bindings import gbl as cpp

LHCb         = cpp.LHCb
CaloCellCode = cpp.CaloCellCode

# =============================================================================
## Get the cellID from the string
def _fromString_ ( self , input ) :
    """
    Get the cellID form string

    >>> input =
    >>> cell  = 
    >>> print cell.fronString ( input )
    
    Uses: 
    
    """
    return cpp.CaloCellCode.Cell2String.cellFromString (  self , input ) 

_fromString_ .__doc__ += '\n' + cpp.CaloCellCode.Cell2String.cellFromString . __doc__ 

LHCb.CaloCellID. __fromString__ = _fromString_
LHCb.CaloCellID.   fromString   = _fromString_
LHCb.CaloCellID. __str__        = LHCb.CaloCellID. toString 
LHCb.CaloCellID. __repr__       = LHCb.CaloCellID. toString 


if '__main__' == __name__ :
    print __doc__ , __author__ , __version__

    cell1 = LHCb.CaloCellID(24312)
    
    print cell1

    cell2 = LHCb.CaloCellID()

    print 'FROM STRING: ', cell2.fromString ( str(cell1) )        , cell2
    
    arg = str(cell1)
    
    print 'FROM TUPLE:  ' , LHCb.CaloCellID(*eval(arg)) 
    print 'FROM ALL:    ' , LHCb.CaloCellID(cell1.all()) 
    print 'FROM INDEX:  ' , LHCb.CaloCellID(cell1.index()) 
    
    print '0H :  ' , LHCb.CaloCellID('Hcal','Outer'   ,10,10)
    print '1H :  ' , LHCb.CaloCellID('Hcal','Inner'   ,10,10)
    print '3H :  ' , LHCb.CaloCellID('Hcal','PinArea' ,10,10)
    
    print '0H :  ' , LHCb.CaloCellID('Hcal',0,10,10)
    print '1H :  ' , LHCb.CaloCellID('Hcal',1,10,10)
    print '3H :  ' , LHCb.CaloCellID('Hcal',3,10,10)
    

    print '0E :  ' , LHCb.CaloCellID('Ecal','Outer'   ,10,10)
    print '1E :  ' , LHCb.CaloCellID('Ecal','Middle'  ,10,10)
    print '2E :  ' , LHCb.CaloCellID('Ecal','Inner'   ,10,10)
    print '3E :  ' , LHCb.CaloCellID('Ecal','PinArea' ,10,10)
    
    print '0E :  ' , LHCb.CaloCellID('Ecal',0,10,10)
    print '1E :  ' , LHCb.CaloCellID('Ecal',1,10,10)
    print '2E :  ' , LHCb.CaloCellID('Ecal',2,10,10)
    print '3E :  ' , LHCb.CaloCellID('Ecal',3,10,10)

    print '0S :  ' , LHCb.CaloCellID('Spd','Outer'   ,10,10)
    print '1S :  ' , LHCb.CaloCellID('Spd','Middle'  ,10,10)
    print '2S :  ' , LHCb.CaloCellID('Spd','Inner'   ,10,10)
    
    print '0S :  ' , LHCb.CaloCellID('Spd',0,10,10)
    print '1S :  ' , LHCb.CaloCellID('Spd',1,10,10)
    print '2S :  ' , LHCb.CaloCellID('Spd',2,10,10)

    print '0P :  ' , LHCb.CaloCellID('Prs','Outer'   ,10,10)
    print '1P :  ' , LHCb.CaloCellID('Prs','Middle'  ,10,10)
    print '2P :  ' , LHCb.CaloCellID('Prs','Inner'   ,10,10)
    
    print '0P :  ' , LHCb.CaloCellID('Prs',0,10,10)
    print '1P :  ' , LHCb.CaloCellID('Prs',1,10,10)
    print '2P :  ' , LHCb.CaloCellID('Prs',2,10,10)

    
# =============================================================================
# The END 
# =============================================================================
