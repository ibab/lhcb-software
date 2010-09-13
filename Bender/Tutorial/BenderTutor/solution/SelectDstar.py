#!/usr/bin/env python
# =============================================================================
# $Id: SelectDstar.py,v 1.1 2010-09-13 13:24:05 ibelyaev Exp $ 
# =============================================================================
# $URL$ 
# =============================================================================
## @file solution/SelectDstar.py
#
#  Simple example that illustrates the selection of D*+ from micro DSTs
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
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-09-10
#
#  Last modification $Date: 2010-09-13 13:24:05 $
#                 by $Author: ibelyaev $
# =============================================================================
"""
Simple example that illustrates the selection of D*+ from micro DSTs

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
__date__    = " 2010-10-10 " 
__version__ = " Version $Revision: 1.1 $ "
# =============================================================================
## import everything from BENDER
from Bender.Main                   import *
# =============================================================================
## @class SelectDstar
#  Simple algoritm to select D*+ from stripped DSTs 
class SelectDstar(Algo):
    
    # =========================================================================
    ## the main method: analyse the event
    def analyse( self ) :
        """
        the main method: analyse the event
        """
        
        # select from input particles according to the decay pattern:
        dstars = self.select ( 'dstars' ,  
                               '[D*(2010)+ -> ( [D0]cc -> K- X+ ) pi+]CC' )

        dm   =  ( M - M1 ) 
        dmD0 =  ADMASS('D0') < 30 * MeV 

        pidK = MINTREE ( 'K+' == ABSID , PIDK - PIDpi )
        
        # loop over selected particles:
        for d in dstars : 

            print d.decay() , PT(d) , pidK( d )
            
            if PT   ( d ) < 2.0 * GeV : continue
            
            d0     = d.child(1)
            
            if VCHI2 ( d0.endVertex() ) >   9 : continue
            if abs ( LV01 ( d0 ) )      > 0.9 : continue
            
            deltaM = dm ( d ) 
            if dmD0 ( d0 ) and deltaM < 160 * MeV S:
                # fill the histogram
                self.plot ( deltaM / GeV  , 'delta-mass' , 0.135 , 0.160 )

            m1 = M1  ( d )  / GeV
            if abs ( deltaM - 145.5 * MeV ) < 2.5 * MeV and  1.800 < m1 < 1.900 :  
                self.plot ( M1 ( d ) / GeV  , ' D0 -mass ' , 1.800 , 1.900 )


            pv = self.bestVertex( d0 )
            if not pv : continue
            
            fun  = LTIME ( self.lifetimeFitter() , pv ) * c_light

            ctau = fun ( d0 )
            if 0.1 < ctau < 1.0 : 
                self.plot ( ctau  , 'c*tau for D^{0}' , 0.1 , 1 , 90 ) 
            
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
    


## The configuration of the job
def configure( inputdata , catalogs = [] ) :
    """
    The configuration of the job
    """    
    from Configurables import DaVinci , GaudiSequencer 
    DaVinci ( DataType        = '2010' ,
              Lumi            = False  )
    
    from Configurables import CondDB
    CondDB  ( IgnoreHeartBeat = True )
    
    setData ( inputdata , catalogs )
    
    ## get/create Application Manager
    gaudi = appMgr()
    
    # modify/update the configuration:
    
    ## create the algorithm
    alg = SelectDstar (
        'Dstar'                                        ,
        RootInTES      = '/Event/MicroDST/PromptCharm' ,
        InputLocations = [ 'SelDstarForPromptCharm'    ] 
        )

    ## use only this algorithm:
    gaudi.setAlgorithms ( [ alg ] ) 
    
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
        ## 'PromptCharm' micro-DSTs created using 'PromtCharm'-selection on Reco04-Stripping07 data:
        '/castor/cern.ch/grid' + lfn for lfn in [
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109439/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109442/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109444/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109446/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109449/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109452/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109454/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109456/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109459/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109461/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109463/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109465/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109468/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109471/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109474/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109476/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109479/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109481/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109484/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109486/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109488/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109490/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109493/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109495/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109498/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109500/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109503/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109505/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109508/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109512/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109513/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109516/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109520/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109522/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109526/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109529/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109532/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109536/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109539/PC.PromptCharm.mdst',
        '/lhcb/user/i/ibelyaev/2010_09/11109/11109542/PC.PromptCharm.mdst']
        ]
        
    configure( inputdata )
    
    ## event loop 
    run(10 )
    
# =============================================================================
# The END
# =============================================================================
