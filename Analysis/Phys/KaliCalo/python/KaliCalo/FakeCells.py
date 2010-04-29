#!/usr/bin/env python
# ======================================================================
## @file KaliCalo/Kali.py
#  'fake-cells' for grouping
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2010-04-28
# ======================================================================
"""
``Fake-cells'' for grouping
"""
# ======================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@itep.ru "
__date__    = " 2010-04-28 "
__version__ = " CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.4 $ "
# ======================================================================
from KaliCalo.Cells import CellID, GlobalRow, GlobalCol

# ======================================================================
## the simplest 'cell-func'
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2010-04-28
class SameCell(object) :
    """
    The simplest 'cell-func'
    """
    def __call__ ( self , cell ) : return CellID ( cell ) 


# =============================================================================
## "Ring"-zones 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2010-04-28
class Rings(object) :
    """
    'Ring' zones 
    """
    def __call__ ( self , cell ) :
        i  = cell.row()
        j  = cell.col()
        i -= 31.5 
        j -= 31.5
        
        row  = int ( math.sqrt ( i*i + j*j ) ) + 1 
        k1   =  0 if  ( 0 > i ) else 1
        k2   =  0 if  ( 0 > j ) else 2
        
        col  =  1 + k1 + k2
        
        if row == GlobalRow and col == GlobalCol : 
            col += 4 
        
        return CellID ( cell.calo() ,
                        cell.area() ,
                        row , col   ) 
    
# =============================================================================
## "Rectangular rings"
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2010-04-28
class RectRings( object ) :
    """
    'Rectangular ring' zones 
    """
    def __call__ ( self , cell ) :

        k1 = 0
        k2 = 0
        k3 = 0
        i  = cell.row()
        j  = cell.col()

        
        if 32 <= i :
            i -= 32
            k1 =  1
        else : i = 32 - i
        
        if 32 <= j :
            j -= 32
            k2 =  2 
        else : j = 32 - i 
            
        newrow = max ( i , j )
        
        if i < j and newrow > 16 : k3 = 4

        row = 1 + newrow
        col = 1 + k1 + k2 + k3
        
        if row == GlobalRow and col == GlobalCol : 
            col += 8
                
        return CellID ( cell.calo() ,
                        cell.area() ,
                        row ,
                        col ) 
    

# =============================================================================
## ``Un-group'' the cells
# @param calib the map of claibration coefficientf for ``grouped'' cells
# @param fakeCell the grouping function
# @param ecalCells the fulllist of Eca cells
# @return the calibration map for ``un-grouped'' cells
def unGroup ( calib , fakeCell , ecalCells ) :
    """
    ``Un-group'' the cells
    """
    result = {}
    for cell in ecalCells :

        ## get the group (``fake cell'') 
        fCell = fakeCell ( cell  )
        
        if calib.has_key ( fCell ) :
            result[ cell ] = calib[ fCell ]

    return result 
    
# =============================================================================
if '__main__' == __name__ :
    
    ## make printout of the own documentations 
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print ' dir(%s) : %s ' % ( __name__    , dir() )
    print '*'*120  
          
# =============================================================================
# The END 
# =============================================================================
