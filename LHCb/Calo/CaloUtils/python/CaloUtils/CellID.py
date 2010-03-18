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
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__date__    = " 2009-09-28 "  
__version__ = " CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $ "
# =============================================================================
__all__     = () 
# =============================================================================

from GaudiPython.Bindings import gbl as cpp

LHCb                = cpp.LHCb
std                 = cpp.std 
CaloCellCode        = cpp.CaloCellCode

# =============================================================================
## Get the cellID from the string
def _fromString_ ( self , input ) :
    """
    Get the cellID form string

    >>> input =
    >>> cell  = 
    >>> print cell.fromString ( input )
    
    Uses: 
    
    """
    return cpp.CaloCellCode.Cell2String.cellFromString (  self , input ) 

_fromString_ .__doc__ += '\n' + cpp.CaloCellCode.Cell2String.cellFromString . __doc__ 

LHCb.CaloCellID. __fromString__ = _fromString_
LHCb.CaloCellID.   fromString   = _fromString_
LHCb.CaloCellID. __str__        = LHCb.CaloCellID. toString 
LHCb.CaloCellID. __repr__       = LHCb.CaloCellID. toString 


_SET1 = std.set    ('LHCb::CaloCellID'      )
_VCT1 = std.vector ('LHCb::CaloCellID'      )
_SET2 = std.set    ('const LHCb::CaloDigit*')
_VCT2 = std.vector ('const LHCb::CaloDigit*')


def _set_at_ ( self , index ) :
    """
    Get the item form set through index 
    """
    return LHCb.CaloHelpers._get_at_ ( self , index )

_set_at_ . __doc__ += '\n'  +  LHCb.CaloHelpers._get_at_ . __doc__

    
## Iterator through the set 
def _set_iter_ ( self ) :
    """
    Iterator through the set 
    """
    _size  = self.size()
    _index = 0  
    while _index < _size :
        yield self.at( _index ) 
        _index += 1 

## insert in set 
def _set_insert_ ( self , obj ) :
    """
    insert in set 
    """
    return LHCb.CaloHelpers._insert_( self , obj )

for s in ( _SET1 , _SET2 ) : 
    s .__at__    = _set_at_
    s .  at      = _set_at_
    s . __iter__ = _set_iter_
    s . __len__  = s . size
    s . insert   = _set_insert_


## Set-printout 
def _set_str_ ( self ) :
    """
    Set-printout 
    """
    _res  = '('
    for i in range( self.size() ) :
        if 0 != i : _res += ' , '
        _res += str ( self.at ( i ) ) 
    _res += ')'
    return _res


_SET1. __str__      = _set_str_
_SET1. __repr__     = _set_str_
_SET1. __toString__ = _set_str_
_SET1.   toString   = _set_str_


## Convert set to vector 
def _toVector_ ( self )  :
    """
    Convert set to vector 
    """        
    return LHCb.CaloHelpers.toVector( self )

## Convert vector to set 
def _toSet_ ( self )  :
    """
    Convert vector to set 
    """        
    return LHCb.CaloHelpers.toSet( self )


_SET1._toVector_ = _toVector_
_SET2. toVector  = _toVector_

_VCT1._toSet_ = _toSet_
_VCT2. toSet  = _toSet_


for t in ( _SET1 , _SET2 , _VCT1 , _VCT2 ) :
    
    t. __fromString__ = _fromString_
    t.   fromString   = _fromString_


LHCb.CaloCellID.Set    = _SET1
LHCb.CaloCellID.Vector = _VCT1

LHCb.CaloDigit.Set     = _SET2
LHCb.CaloDigit.Vector  = _VCT2


# =============================================================================
## iterator over CaloVector 
def __calovector_iter__ ( self ) :
    """
    Iterator over Calo Vector
    
    >>> calovector = ...                ## get the vector
    
    >>> for i in calovector : print i   ## iterate over its content
    
    """
    _size = self.size()
    _indx = 0 
    while _indx < _size :
        yield self.at (  _indx )
        _indx += 1

## ============================================================================
## decorate calo-vector 
for t in ( 'CellParam'        ,
           'LHCb::CaloDigit'  ,
           'LHCb::CaloDigit*' ,
           'LHCb::CaloAdc'    ) : 
    vct =  cpp.CaloVector ( t , LHCb.CaloCellID )
    vct.__iter__ = __calovector_iter__
    

# =============================================================================
if '__main__' == __name__ :

    ## make printout of the own documentations 
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120  


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

    s = _SET1()
    
    s.insert (  LHCb.CaloCellID('Prs',0,10,10) )
    s.insert (  LHCb.CaloCellID('Prs',0,11,10) )
    s.insert (  LHCb.CaloCellID('Prs',0,10,12) )
    s.insert (  LHCb.CaloCellID('Prs',0,10,10) )

    print s
    
    
# =============================================================================
# The END 
# =============================================================================
