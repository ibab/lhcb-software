#!/usr/bin/env python
# ==========================================================================================
## @file BenderExample/RealLam0.py
#
#  A little bit modified version of original script by Alexander KOZLINSKY  and
#  Thomas BAUER to look for Lambda0 -> p pi- peak on real data
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>"Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis"</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  "C++ ToolKit for Smart and Friendly Physics Analysis"
#
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#  "No Vanya's lines are allowed in LHCb/Gaudi software."
#
#  @author Alexander KOZLINSKY  akozlins@gmail.com
#  @author Thomas    BAUER       thomas@nikhef.nl
#  @author Vanya     BELYAEV    vanya@nikhef.nl
# ===========================================================================================
"""

A little bit modified version of original script by Alexander KOZLINSKY and
Thomas BAUER to look for Lambda0 -> p pi- peak on real data

"""
# ===========================================================================================
__author__   = "Alexander KOZLINSKY, Thomas BAUER & Vanya BELYAEV "
__version__  = "CVS Tag $Name: not supported by cvs2svn $, verison $Release:$"
# ===========================================================================================

import ROOT                            ## needed to produce/visualize the histograms
from   Gaudi.Configuration    import * ## needed for configuration of the job
from   Bender.Main            import * 
from   BenderExample.RealLam0 import * 
                
# =============================================================================
## configure the job 
def configure ( datafiles , catalogs = [] ) :
    """
    Job configuration 
    """
    from Configurables           import DaVinci       ## needed for job configuration
    from Configurables           import EventSelector ## needed for job configuration 
    from GaudiConf.Configuration import FileCatalog   ## needed for job configuration 
    from GaudiConf.Configuration import NTupleSvc     ## needed for job configuration 

    davinci = DaVinci (
        DataType      = '2009' ,
        Simulation    = True   ,
        PrintFreq     = 1000   ,
        EvtMax        = -1     , 
        HistogramFile = 'RealLam0_MC_Histos.root' 
        )
    
    if datafiles : 
        EventSelector (
            Input     = datafiles
            )

    NTupleSvc (
        Output = [ "LAM0 DATAFILE='RealLambda_MC.root' TYPE='ROOT' OPT='NEW'" ]
        )

    
    gaudi = appMgr()

    from BenderExample.RealLam0 import Lam0
    
    alg = Lam0(
        'Lam0'             ,   ## Algorithm name
        NTupleLUN = 'LAM0' ,   ## Logical unit for output file with N-tuples 
        InputLocations = [ 'StdNoPIDsPions'        ,
                           'StdNoPIDsDownPions'    ,
                           'StdNoPIDsProtons'      ,
                           'StdNoPIDsDownProtons'  ] ## input particles 
        )
    
    gaudi.setAlgorithms ( [ alg ] ) 
    
    return SUCCESS 


# =============================================================================
# The actual job steering
if '__main__' == __name__ :
    
    from Configurables  import EventSelector
    
    EventSelector( PrintFreq = 1000 )
    
    configure ( [] )

    gaudi = appMgr()
    
    evtsel = gaudi.evtSel()
    
    mcfiles = [
        '/castor/cern.ch/grid/lhcb/MC/2009/XDST/00005780/0000/00005780_00000001_1.xdst' ,
        '/castor/cern.ch/grid/lhcb/MC/2009/XDST/00005780/0000/00005780_00000002_1.xdst' ,
        '/castor/cern.ch/grid/lhcb/MC/2009/XDST/00005780/0000/00005780_00000003_1.xdst' ,
        '/castor/cern.ch/grid/lhcb/MC/2009/XDST/00005780/0000/00005780_00000004_1.xdst'
        ]
    
    files = mcfiles 
    evtsel.open( files ) 
    
    run ( -1 )
    
    myalg = gaudi.algorithm ( 'Lam0' )
    
    import GaudiPython.HistoUtils
    histos = myalg.Histos()
    for (k,h) in histos.items() :
        if hasattr ( h , 'dump' ) :
            print h.dump ( 50 , 25 , False )
        
# =============================================================================
# The END 
# =============================================================================
