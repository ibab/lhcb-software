#!/usr/bin/env python
# ======================================================================
## @file KaliCalo/Cells.py
#  Utilities for cells 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2010-04-29
# ======================================================================
"""
Utilities for cells 
"""
# ======================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@itep.ru "
__date__    = " 2010-04-29 "
__version__ = " CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.2 $ "
__all__     = (
    'CellID'     , 
    'GlobalRow'  ,
    'GlobalCol'  ,
    #
    'EcalZone'   ,
    'InnerZone'  ,
    'MiddleZone' ,
    'OuterZone'  ,
    'Zones'
    )
# ======================================================================
import ROOT
from GaudiPython.Bindings import gbl as cpp
LHCb       = cpp.LHCb
## decorate CaloCellID class 
import CaloUtils.CellID

GlobalRow = 31
GlobalCol = 31 

CellID = LHCb.CaloCellID 

EcalZone   = CellID ( 'Ecal' , 'PinArea', GlobalRow , GlobalCol ) ## the whole Ecal 
## inner  zone only 
InnerZone  = CellID ( 'Ecal' , 'Inner'  , GlobalRow , GlobalCol ) ## inner  zone only 
## middle zone only 
MiddleZone = CellID ( 'Ecal' , 'Middle' , GlobalRow , GlobalCol ) ## middle zone only 
## Outer  zone only 
OuterZone  = CellID ( 'Ecal' , 'Outer'  , GlobalRow , GlobalCol ) ## Outer  zone only 

Zones      = (
    OuterZone  ,
    MiddleZone , 
    InnerZone  ,
    EcalZone   
    )


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
