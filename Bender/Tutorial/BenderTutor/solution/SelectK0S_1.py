#!/usr/bin/env python
# =============================================================================
# $Id: SelectK0S_1.py,v 1.2 2010-09-13 13:24:05 ibelyaev Exp $ 
# =============================================================================
# $URL$ 
# =============================================================================
## @file solution/SelectK0S_1.py
#
#  Simple example that illustrates the selection of K0S from stripped DSTs
#  The example reuses the same algorithm, but apply EventPreFilters
#   
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  ``C++ ToolKit for Smart and Friendly Physics Analysis''
#
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-10-12
#
#  Last modification $Date: 2010-09-13 13:24:05 $
#                 by $Author: ibelyaev $
# =============================================================================
"""
Simple example that illustrates the selection of K0S from stripped DSTs
The example reuses the same algorithm, but apply EventPreFilters

This file is a part of BENDER project:
    ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
    ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the campain of Dr.O.Callot et al.: 
    ``No Vanya's lines are allowed in LHCb/Gaudi software.''

Last modification $Date: 2010-09-13 13:24:05 $
               by $Author: ibelyaev $
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__date__    = " 2006-10-12 " 
__version__ = " Version $Revision: 1.2 $ "
# =============================================================================
## import everything from BENDER
from Bender.Main import *
# =============================================================================


from SelectK0S import SelectKs

## The configuration of the job
def configure( inputdata , catalogs = [] ) :
    """
    The configuration of the job
    """
    
    ## define event-levele filters:
    from PhysConf.Filters import LoKi_Filters
    fltrs = LoKi_Filters (
        HLT_Code   = " HLT_PASS_RE ('Hlt1MBMicro.*Decision') | HLT_PASS_RE ('Hlt1.*Hadron.*Decision') " ,
        STRIP_Code = " HLT_PASS('StrippingK0SLineDecision') " ,
        VOID_Code  = " EXISTS ('/Event/V0') " 
        )
    
    from Configurables import DaVinci , GaudiSequencer 
    DaVinci ( DataType        = '2010' ,
              EventPreFilters = fltrs.filters ('Filters') ,
              Lumi            = False  )
    
    from Configurables import CondDB
    CondDB  ( IgnoreHeartBeat = True )
    
    setData ( inputdata , catalogs )
    
    ## get/create Application Manager
    gaudi = appMgr()
    
    # modify/update the configuration:
    
    ## create the algorithm
    alg = SelectKs (
        'SelectKs'                         ,
        RootInTES      = '/Event/V0'       ,
        InputLocations = [ 'StrippingK0S'  ] 
        )

    ## add algorithm into main DaVinci sequence
    dvMain = gaudi.algorithm('GaudiSequencer/DaVinciMainSequence' , True )
    dvMain.Members += [ alg.name() ]
    
    return SUCCESS 
# =============================================================================


# =============================================================================
## Job steering 
if __name__ == '__main__' :
    
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120
    
    ## job configuration
    inputdata = [
        ## dbpath: /LHCb/Collision10/Beam3500GeV-VeloClosed-MagDown/Reco05-Striping09-Merged/90000000/V0.DST
        '/castor/cern.ch/grid' + '/lhcb/data/2010/V0.DST/00007551/0000/00007551_00000%03d_1.v0.dst' % i for i in range ( 1 , 691 )
        ]
        
    configure( inputdata )
    
    ## event loop 
    run(500)
    
# =============================================================================
# The END
# =============================================================================
