#!/usr/bin/env python
# =============================================================================
# $Id: RealDstar_CHARM.py,v 1.3 2010-08-28 13:43:12 ibelyaev Exp $
# =============================================================================
## @file BenderExample/RealDstar_V0.py
#
#  The script to analyse the D*+ -> ( D0 -> K- pi+ ) pi+
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
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2010-06-18
#
#                    $Revision:$ 
#  Last modification $Date: 2010-08-28 13:43:12 $
#                 by $Author: ibelyaev $
# =============================================================================
"""

The script to analyse the D*+ -> ( D0 -> K- pi+ ) pi+

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the campain of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

                  $Revision:$ 
Last modification $Date: 2010-08-28 13:43:12 $
               by $Author: ibelyaev $
"""
# =============================================================================
__author__   = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__date__     = " 2010-06-18 "
__version__  = " Version $Revision: 1.3 $ "
# =============================================================================
import ROOT                           ## needed to produce/visualize the histograms
import LHCbMath.Types                 ## easy access to various geometry routines 
from   Bender.Main         import *   ## import all bender goodies 
from   Gaudi.Configuration import *   ## needed for job configuration

from   GaudiKernel.SystemOfUnits     import GeV, MeV, mm, cm 
from   GaudiKernel.PhysicalConstants import c_light
from   GaudiPython.GaudiAlgs         import Vector

from LoKiHlt.algorithms import ALG_PASSED, ALG_EXECUTED

# =============================================================================
## @class Dstar
#  The algorithm to analyse the D*+ -> ( D0 -> K- pi+ ) pi+
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2010-02-11
class Dstar(Algo) :
    """
    The algorithm to analyse the D*+ -> ( D0 -> K- pi+ ) pi+
    """
    
    ## the only one esential method: 
    def analyse  (self ) :
        """
        The only one essential method
        """

        ## get D*+ 
        dstars = self.select ( 'dstar' , '[ D*(2010)+ -> ( D0 -> K- pi+ ) pi+]CC' )        
        ## dstars = self.select ( 'dstar' , PALL )        
        if dstars.empty()   :
            return self.Warning ( 'No D*+ decays are found' , SUCCESS )

        dm0 = ( M - M1  ) / GeV 
        dm1 = DTF_FUN ( dm0 , False )
        
        for d in dstars :
            print d.decay()

            _v0 = dm0 ( d ) 
            self.plot ( _v0 , "delta m0  " ,  0.135 , 0.160 ) 
            _v1 = dm1 ( d ) 
            self.plot ( _v1 , "delta m1  " ,  0.135 , 0.160 )            
            self.plot ( _v0 - _v1 , "delta delta" , -0.0025 , 0.0025 , 200 )


            
        if len ( dstars ) :
            self.setFilterPassed ( True  )
        else :
            self.setFilterPassed ( False )
       
        
        return SUCCESS 
    
    
    ## finalize & print histos 
    def finalize ( self ) :
        """
        Finalize & print histos         
        """
        histos = self.Histos()
        for key in histos :
            h = histos[key]
            if hasattr ( h , 'dump' ) : print h.dump(50,30,True)
        return Algo.finalize ( self )


_local_algs_ = [] 
# =============================================================================
## configure the job 
def configure ( datafiles , catalogs = [] ) :
    """
    Job configuration 
    """
    
    from PhysConf.Filters import LoKi_Filters
    fltrs = LoKi_Filters (
        HLT_Code   = " HLT_PASS_RE ('Hlt1MBMicro.*Decision') | HLT_PASS_RE ('Hlt1.*Hadron.*Decision') " ,
        STRIP_Code = " HLT_PASS('StrippingDstarForPromptCharmDecision') " ,
        VOID_Code  = " EXISTS ('/Event/Strip') " 
        )
    
    ##
    ## 1. Static configuration using "Configurables"
    ##
    
    from Configurables           import DaVinci    
    davinci = DaVinci (
        DataType      = '2010' ,
        PrintFreq     = 1000   ,
        HistogramFile = 'RealDstar_CHARM_Histos.root' ,
        ##
        DDDBtag         = "head-20101026" ,
        CondDBtag       = "head-20101112" ,
        ##
        EventPreFilters = fltrs.filters('Filters') ,
        #
        Lumi = True 
        )
    
    ## TEMPORARY to be removed!
    from Configurables import CondDB
    CondDB( IgnoreHeartBeat = True ) 
    
    from Configurables import NTupleSvc
    ntSvc = NTupleSvc() 
    ntSvc.Output += [ "DSTAR DATAFILE='RealDstar_CHARM.root' TYPE='ROOT' OPT='NEW'" ]

    
    ## define the input data:
    setData ( datafiles , catalogs )
    
    ##
    ## 2. Jump into the wonderful world of the actual Gaudi components!
    ## 
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr() 
    
    SELECTION = 'DstarForPromptCharm'
    
    ## create local algorithm:
    alg = Dstar (
        #
        'Dstar'             ,   ## Algorithm name
        #
        NTupleLUN = 'DSTAR' ,   ## Logical unit for output file with N-tuples
        #
        ## RecoStripping-09 conventions! 
        RootInTES        =  '/Event/Charm/' , 
        InputLocations   = [ SELECTION ] ## input particles 
        )
    
    ## finally inform Application Manager about our algorithm
    userSeq = gaudi.algorithm ('GaudiSequencer/DaVinciUserSequence',True)    
    userSeq.Members += [ alg.name() ]
    
    return SUCCESS 


# =============================================================================
# The actual job steering
if '__main__' == __name__ :
    
    ## make printout of the own documentation
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120  
    
    ## stripping-10 CHARM-DST
    files = [
        '/castor/cern.ch/grid/lhcb/data/2010/CHARM.DST/00007954/0000/00007954_00000%03d_1.charm.dst' % i for i in range(1,700) 
        ]
    
    configure ( files )
    
    ## run the job
    run (1000)
    

# =============================================================================
# The END 
# =============================================================================
