#!/usr/bin/env python
# =============================================================================
# $Id: SelectDplus.py,v 1.1 2010-09-13 13:24:04 ibelyaev Exp $ 
# =============================================================================
## @file solution/SelectDplus.py
#
#  Simple example that illustrates the selection of D+/D_s+ from stripped DSTs
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
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-09-10
#
#  Last modification $Date: 2010-09-13 13:24:04 $
#                 by $Author: ibelyaev $
# =============================================================================
"""
Simple example that illustrates the selection of D+/D_s+ from stripped DSTs

This file is a part of BENDER project:
    ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
    ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
    ``No Vanya's lines are allowed in LHCb/Gaudi software.''

Last modification $Date: 2010-09-13 13:24:04 $
               by $Author: ibelyaev $
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__date__    = " 2010-10-10 " 
__version__ = " Version $Revision: 1.1 $ "
# =============================================================================
## import everything from BENDER
from Bender.Main import *
# =============================================================================
## @class SelectDplus
#  Simple algoritm to select D+/D_s+ from stripped DSTs 
class SelectDplus(Algo):
    
    # =========================================================================
    ## the main method: analyse the event
    def analyse( self ) :
        """
        the main method: analyse the event
        """
        
        # select from input particles according to the decay pattern:
        d_1  = self.select ( 'd1'  , "[ D+   --> K- pi+ pi+]CC " )        
        ds_1 = self.select ( 'ds1' , "[ D_s+ --> K- K+  pi+]CC " )
        
        # retrieve N-tuple (book-on-demand)
        tup = self.nTuple('My N-tuple')
        for d in d_1  :
            print 'D1 : ' , d.decay() , M(d)
            self.plot ( M(d) / GeV , 'D^{+} #rightarrow K^{-} #pi^{+} #pi^{+}'            , 1.8 , 1.9 , 100 )
            
            pv = self.bestVertex( d )
            if not pv : continue
            
            fun  = LTIME ( self.lifetimeFitter() , pv ) * c_light

            ctau = fun ( d )
            if 0.1 < ctau < 1.0 : 
                self.plot ( ctau  , 'c*tau for D^{+}' , 0.1 , 1 , 90 ) 

            # add the columns into n-tuple 
            tup.column ( 'm'    , M  ( d ) )
            tup.column ( 'pt'   , PT ( d ) )
            tup.column ( 'ctau' , ctau )
            # commit the row 
            tup.write()
            
        for d in ds_1 :
            print 'Ds1: ' , d.decay() , M(d)
            self.plot ( M(d) / GeV , 'D^{+}/D_{s}^{+} #rightarrow K^{-} K^{+} #pi^{+} 2 ' , 1.8 , 2.050 , 250 ) 
            
        return SUCCESS

    ## finalize & print histos 
    def finalize ( self ) :
        """
        Finalize & print histos         
        """
        #
        self.dumpHistos ( 50 , 30 , True )
        #
        return Algo.finalize ( self )
    

# =============================================================================
## The configuration of the job
def configure( inputdata , catalogs = [] ) :
    """
    The configuration of the job
    """
    
    ## Define event-level filters:
    from PhysConf.Filters import LoKi_Filters    
    fltrs = LoKi_Filters (
        STRIP_Code = """
        HLT_PASS_RE ( 'Stripping.*ForPromptCharm.*Decision'  ) 
        """ ,
        VOID_Code  = """
        ( 0.5 < CONTAINS ('/Event/Charm/Phys/DForPromptCharm/Particles'  ) )
        |
        ( 0.5 < CONTAINS ('/Event/Charm/Phys/DsForPromptCharm/Particles' ) ) 
        """
        )
    filters = fltrs.filters('Filters')
    filters.reverse() 
    
    from Configurables import DaVinci , GaudiSequencer 
    DaVinci ( DataType        = '2011'   ,
              EventPreFilters = filters  ,
              Lumi            = False    ,
              HistogramFile   = 'Dplus_Histos.root' ,
              TupleFile       = 'Dplus_Tuples.root' )
    
    from Configurables import CondDB
    CondDB  ( IgnoreHeartBeat = True )
    
    setData ( inputdata , catalogs )
    
    ## get/create Application Manager
    gaudi = appMgr ()
    
    # modify/update the configuration:
    
    ## create the algorithm
    alg = SelectDplus (
        'Dplus'                               , 
        ## Reco10-Stripping13 :
        RootInTES      = '/Event/Charm'    ,
        # 
        Inputs = [ 'Phys/DForPromptCharm/Particles'       ,
                   'Phys/DsForPromptCharm/Particles'      ]
        )
    
    ## add algorithm into DaVinci sequence
    dvUser = gaudi.algorithm ( 'GaudiSequencer/DaVinciUserSequence' , True )
    dvUser.Members += [ alg.name() ]
    
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
        ## Charm microDST, Stripping 13 
        "/castor/cern.ch/grid/lhcb/LHCb/Collision11/CHARM.MDST/00010646/0000/00010646_000000%02d_1.charm.mdst" % i for i in range(1,20) 
        ]
    
    configure ( inputdata )
    
    ## event loop 
    run ( 1000 )
    
    
# =============================================================================
# The END
# =============================================================================
