#!/usr/bin/env python
# =============================================================================
## @file BenderExample/RealDstar_V0.py
#
#  The script to analyse the D*+ -> ( D0 -> K- pi+ ) pi+
#     from the V0-stripping RecoStripping04 
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
# =============================================================================
"""

The script to analyse the D*+ -> ( D0 -> K- pi+ ) pi+
    from the V0-stripping RecoStripping04 

The script to analyse the Sigma0 from the V0-stripping

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the campain of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
# =============================================================================
__author__   = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__     = "2010-06-18"
__version__  = "CVS Tag $Name: not supported by cvs2svn $, verison $Release:$"
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

        if not hasattr ( self , '_algs' )  :
            self._algs = ALG_EXECUTED ('MySeq') & ALG_PASSED ('MySeq' )
            
        if not self._algs() :
            self.setFilterPassed ( False ) 
            return SUCCESS
        
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
        ## ODIN_Code  = " in_range ( 1000 , ODIN_RUN  " ,
        HLT_Code   = " HLT_PASS_RE ('Hlt1MBMicro.*Decision') | HLT_PASS_RE ('Hlt1.*Hadron.*Decision') " ,
        STRIP_Code = " HLT_PASS('StrippingDstarPromptWithD02HHNoPtDecision') " ,
        VOID_Code  = " EXISTS ('/Event/Strip') " 
        )
    
    ##
    ## 1. Static configuration using "Configurables"
    ##
    
    from Configurables           import DaVinci    
    davinci = DaVinci (
        DataType      = '2010' ,
        PrintFreq     = 1000   ,
        HistogramFile = 'RealDstar_V0_Histos.root'
        )
    
    from GaudiConf.Configuration import NTupleSvc   
    NTupleSvc (
        Output = [ "DSTAR DATAFILE='RealDstar_V0.root' TYPE='ROOT' OPT='NEW'" ]
        )
    
    mysequence = fltrs.sequence ( 'MySeq' )
    ## mysequence.Members += [ 'Dstar' ]
    
    ## define the input data:
    setData ( datafiles , catalogs )
    
    ##
    ## 2. Jump into the wonderful world of the actual Gaudi components!
    ## 
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr() 
    
    ## create local algorithm:
    alg = Dstar (
        'Dstar'             ,   ## Algorithm name
        NTupleLUN = 'DSTAR' ,   ## Logical unit for output file with N-tuples
        ##
        ## PreloadData = False , 
        PreloadData = True , 
        ## RecoStripping-04 conventions! 
        InputLocations = [
        '/Event/Charm/Phys/SelectionDstar2D0PiWithD02PiPiKPiKK' ## input particles 
        ]
        )
    
    gaudi.setAlgorithms ( [ 'GaudiSequencer/MySeq'] ) 

    gaudi.addAlgorithm ( alg ) 
    
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
    
    configure (
        [ '/castor/cern.ch/grid' + '/lhcb/data/2010/CHARM.DST/00006579/0000/00006579_00000%03d_1.charm.dst' % n for n in range ( 1 , 118 ) ] 
        ) 
    
    run ( 10000 )
    
##  |    Counter                                      |     #     |    sum     | mean/eff^* | rms/err^*  |     min     |     max     |
##  | "# /Event/Charm/Phys/SelectionDstar2D0PiWithD02P|       540 |       1028 |     1.9037 |     1.4549 |      1.0000 |      12.000 |
##  | "# input particles"                             |     10000 |       1028 |    0.10280 |    0.54720 |      0.0000 |      12.000 |
##  | "No P->PV table at /Event/Charm/Phys/SelectionDs|      9073 |       9073 |     1.0000 |     0.0000 |      1.0000 |      1.0000 |
##  | "Non-existing location /Event/Charm/Phys/Selecti|      9460 |       9460 |     1.0000 |     0.0000 |      1.0000 |      1.0000 |
##  | "Recalculating P->PV table"                     |      9188 |       9188 |     1.0000 |     0.0000 |      1.0000 |      1.0000 |
## TimingAuditor.T...   INFO ------------------------------------------------------------------------------------------------
## TimingAuditor.T...   INFO EVENT LOOP                    |     3.094 |     5.623 |    1.798    1409.5 |   10000 |    56.226 |
## TimingAuditor.T...   INFO  MySeq                        |     0.791 |     1.773 |    0.291    1068.2 |   10000 |    17.729 |
## TimingAuditor.T...   INFO   MySeq_HLT                   |     0.415 |     0.429 |    0.254    1029.4 |   10000 |     4.289 |
## TimingAuditor.T...   INFO   MySeq_STRIP                 |     0.368 |     1.339 |    0.831      38.7 |    9998 |    13.388 |
## TimingAuditor.T...   INFO   MySeq_VOID                  |     0.015 |     0.009 |    0.004       0.2 |     538 |     0.005 |
## TimingAuditor.T...   INFO  Dstar                        |     0.953 |     1.485 |    0.245     703.2 |   10000 |    14.847 |
## TimingAuditor.T...   INFO * createODIN                  |     0.916 |     1.699 |    0.182     938.4 |   10000 |    16.988 |
## TimingAuditor.T...   INFO *   HltDecReportsDecoder      |     0.352 |     0.364 |    0.212     932.2 |   10000 |     3.635 |
## TimingAuditor.T...   INFO *  UnpackRecVertex            |     0.141 |     0.195 |    0.096      69.9 |   10000 |     1.949 |
## TimingAuditor.T...   INFO *  UnpackCharged              |     4.765 |    12.302 |    0.237     610.3 |     205 |     2.522 |
## TimingAuditor.T...   INFO *  UnpackTrack                |    14.593 |    29.506 |    0.341     697.6 |     205 |     6.049 |
## TimingAuditor.T...   INFO ------------------------------------------------------------------------------------------------
    
## TimingAuditor.T...   INFO EVENT LOOP                    |     2.857 |     6.720 |    1.065   10528.6 |   10000 |    67.203 |
## TimingAuditor.T...   INFO  MySeq                        |     0.791 |     2.594 |    0.327    9502.0 |   10000 |    25.937 |
## TimingAuditor.T...   INFO   MySeq_HLT                   |     0.419 |     1.259 |    0.249    9333.6 |   10000 |    12.590 |
## TimingAuditor.T...   INFO   MySeq_STRIP                 |     0.366 |     1.330 |    0.845     168.4 |    9998 |    13.296 |
## TimingAuditor.T...   INFO   MySeq_VOID                  |     0.007 |     0.009 |    0.005       0.2 |     538 |     0.005 |
## TimingAuditor.T...   INFO  Dstar                        |     1.060 |     2.179 |    0.247     928.2 |   10000 |    21.794 |
## TimingAuditor.T...   INFO * createODIN                  |     0.890 |     1.814 |    0.187     711.7 |   10000 |    18.144 |
## TimingAuditor.T...   INFO *   HltDecReportsDecoder      |     0.346 |     1.174 |    0.206    9054.3 |   10000 |    11.739 |
## TimingAuditor.T...   INFO *  UnpackRecVertex            |     0.143 |     0.249 |    0.096     239.9 |   10000 |     2.491 |
## TimingAuditor.T...   INFO *  UnpackCharged              |     4.487 |    14.773 |    0.289     735.0 |     205 |     3.028 |
## TimingAuditor.T...   INFO *  UnpackTrack                |    14.222 |    33.248 |    0.344     913.5 |     205 |     6.816 |

# =============================================================================
# The END 
# =============================================================================
