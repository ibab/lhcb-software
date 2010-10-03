#!/usr/bin/env python
# =============================================================================
## @file KaliCalo/Det.py
#
#  Helper script to extract list of cellIDs
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2010-03-18
# =============================================================================
"""

Helper script to extract list of Ecal cell IDs

"""
# =============================================================================
__author__   = " Vanya BELYAEV Ivan.Belyaev@itep.ru   "
__date__     = " 2010-03-18 "
__version__  = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $ "
# =============================================================================
from   GaudiPython.Bindings import gbl as cpp
import CaloUtils.CellID
LHCb          = cpp.LHCb
DeCalorimeter = cpp.DeCalorimeter

# =============================================================================
## get DeCalorimeter
def getCalo ( calo = '/dd/Structure/LHCb/DownstreamRegion/Ecal' ,
              DataType = '2010' ,  **args ) :
    """
    Get DeCalorimeter by name

    >>> name = ...              ## the name of DeCalorimeter 
    
    >>> calo = getCalo ( name ) ## get the calorimeter
    
    """

    from GaudiPython.Bindings import AppMgr
    from GaudiPython.Bindings import _gaudi
    if not _gaudi :
        from DaVinci.Configuration import DaVinci
        dv = DaVinci ( DataType = DataType , **args )
        g = AppMgr()
        g.initialize() 
    
    gaudi   = AppMgr()
    detSvc  = gaudi.detSvc()
    return detSvc[ calo ] 

    
# =============================================================================
if '__main__' == __name__ :
    
    ## make printout of the own documentations 
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120  


    ecal = getCalo()

    cp = ecal.cellParams()

    cells = []

    for p in cp :
        cell = p.cellID()
        if ecal.valid ( cell ) and not cell.isPin() :
            cells.append ( cell )

    cells.sort()
    
    import KaliCalo.ZipShelve as ZipShelve
    dbase = ZipShelve.open( 'ecal_db.gz' )
    dbase ['AllEcalCells'] = cells
    dbase.close()

    
    
    
# =============================================================================
# The END 
# =============================================================================


