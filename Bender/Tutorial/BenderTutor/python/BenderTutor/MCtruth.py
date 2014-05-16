#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
## @file MCtruth.py 
#
#  Very simple manipulations with MC-truth events 
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
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2006-10-12
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""

Very simple manipulations with MC-truth events 

This file is a part of BENDER project:
   ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
   ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
    ``No Vanya's lines are allowed in LHCb/Gaudi software.''

                  $Revision$
Last modification $Date$
               by $Author$
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@itep.ru "
__date__    = " 2006-10-12 " 
__version__ = " Version $Revision$ "
# =============================================================================
import ROOT 
from   GaudiKernel.SystemOfUnits import GeV 
## import everything from bender 
from   Bender.MainMC import * 
# =============================================================================
## @class MCtruth 
#  Make combinatorics and composed particle creation in Bender 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
class MCtruth(AlgoMC):                        ## <--- Note the base class here
    """
    Make combinatorics and composed particle creation in Bender 
    """
    ## the main 'analysis' method 
    def analyse( self ) :   ## IMPORTANT! 
        """
        The main 'analysis' method
        """

        ## get "all" decays 
        mybs = self.mcselect ( 'mybs' , '[ B_s0 ==> J/psi(1S) K+ K- pi+ pi-]CC' )

        if mybs.empty() :
            self.Warning('No true Bs-decays are found!')
            allb = self.mcselect( 'allmcb' , BEAUTY )
            print allb
            return SUCCESS

        
        ## 1) get decays to K*K* 
        mybs1 = self.mcselect ( 'mybs1' , '[B_s0       => J/psi(1S) K*(892)0 K*(892)~0 ]CC' )  

        ## 2) get decays to K* K pi
        mybs2 = self.mcselect ( 'mybs2' , '[ [B_s0]cc  => J/psi(1S) K*(892)0 K- pi+    ]CC' )     

        ## 3) get decays to J/psi phi rho 
        mybs3 = self.mcselect ( 'mybs3' , '[B_s0  =>  J/psi(1S) ( phi(1020) => K+ K- ) rho(770)0 ]CC' )
        
        ## 4) get decays to J/psi phi pi pi 
        mybs4 = self.mcselect ( 'mybs4' , '[B_s0  =>  J/psi(1S) ( phi(1020) => K+ K- ) pi+ pi-   ]CC' )

        ## 5) get decays to J/psi K K pi pi 
        mybs5 = self.mcselect ( 'mybs5' , '[B_s0  =>  J/psi(1S) K+ K- pi+ pi-   ]CC' )

        ## 6)  get decays to psi2S phi  
        mybs6 = self.mcselect ( 'mybs6' , '[B_s0  => ( psi(2S) ==> J/psi(1S) pi+ pi- ) ( phi(1020) =>  K+ K- ) ]CC' )

        ## 7) get decays to psi2S K K  
        mybs7 = self.mcselect ( 'mybs7' , '[B_s0  => ( psi(2S) ==> J/psi(1S) pi+ pi- ) K+ K- ]CC' )

        ## 8) get decays to X(3872) phi  
        mybs8 = self.mcselect ( 'mybs8' , '[B_s0  => ( X_1(3872) ==> J/psi(1S) pi+ pi- ) ( phi(1020) => K+ K- ) ]CC' )

        ## 9) get decays to X(3872) K K
        mybs9 = self.mcselect ( 'mybs9' , '[B_s0  => ( X_1(3872) ==> J/psi(1S) pi+ pi- ) K+ K- ]CC' )


        tup1 = self.nTuple ( 'MyTuple' )
        
        tup1.column_int ( 'nBs'  , len ( mybs  ) ) 
        tup1.column_int ( 'nBs1' , len ( mybs1 ) ) 
        tup1.column_int ( 'nBs2' , len ( mybs2 ) ) 
        tup1.column_int ( 'nBs3' , len ( mybs3 ) ) 
        tup1.column_int ( 'nBs4' , len ( mybs4 ) ) 
        tup1.column_int ( 'nBs5' , len ( mybs5 ) ) 
        tup1.column_int ( 'nBs6' , len ( mybs6 ) ) 
        tup1.column_int ( 'nBs7' , len ( mybs7 ) ) 
        tup1.column_int ( 'nBs8' , len ( mybs8 ) ) 
        tup1.column_int ( 'nBs9' , len ( mybs9 ) )
        
        tup1.write()
        
        if len(mybs) != len(mybs1)+len(mybs2)+len(mybs3)+len(mybs4)+len(mybs5)+len(mybs6)+len(mybs7)+len(mybs8)+len(mybs9) :
            self.Warning('Something wrong happens with decay descriptors!')
            print len(mybs), len(mybs1), len(mybs2), len(mybs3) , len(mybs4), len(mybs5) , len(mybs6) , len(mybs7), len(mybs8),len(mybs9) 
            print mybs


        tup = self.nTuple ( 'MyTuple2' )
        psi_selector = LoKi.MCChild.Selector( 'J/psi(1S)' == MCID )
        km_selector  = LoKi.MCChild.Selector( 'K-'        == MCID )
        kp_selector  = LoKi.MCChild.Selector( 'K+'        == MCID )
        
        for b in mybs :

            psi = b.child ( psi_selector )
            if not psi : continue

            km  = b.child ( km_selector )
            if not km : continue

            kp  = b.child ( kp_selector )
            if not kp : continue

            tup.column ( 'psi' , psi.momentum() / GeV )
            tup.column ( 'kp'  , kp .momentum() / GeV )
            tup.column ( 'km'  , km .momentum() / GeV )

            tup.write ()

        ## 
        return SUCCESS      ## IMPORTANT!!! 
# =============================================================================

# =============================================================================
## The configuration of the job
def configure ( inputdata        ,    ## the list of input files  
                catalogs = []    ,    ## xml-catalogs (filled by GRID)
                castor   = False ,    ## use the direct access to castor/EOS ? 
                params   = {}    ) :
    
    ## import DaVinci    
    from Configurables import DaVinci
    ## delegate the actual configuration to DaVinci
    dv = DaVinci ( Simulation      = True                      , ## IMPORTANT 
                   DDDBtag         = 'dddb-20130929-1'         , ## IMPORTANT 
                   CondDBtag       = 'sim-20130522-1-vc-mu100' , ## IMPORTANT 
                   DataType        = '2012'         ,
                   InputType       = 'DST'          ,
                   TupleFile       = 'MCtruth.root' )
    
    ## add the name of Bender algorithm into User sequence sequence 
    alg_name = 'MCtruth'
    dv.UserAlgorithms += [ alg_name ]
    
    ## define the input data
    setData  ( inputdata , catalogs , castor )
    
    ## get/create application manager
    gaudi = appMgr() 
    
    ## (1) create the algorithm with given name 
    alg   = MCtruth( alg_name  , PPMCs =  [] )
    
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
    ## BKQuery('/MC/2012/Beam4000GeV-2012-MagUp-Nu2.5-Pythia8/Sim08d/Digi13/Trig0x409f0045/Reco14a/Stripping20NoPrescalingFlagged/13246002/ALLSTREAMS.DST')
    inputdata = [
        '/lhcb/MC/2012/ALLSTREAMS.DST/00033494/0000/00033494_00000013_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00033494/0000/00033494_00000040_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00033494/0000/00033494_00000022_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00033494/0000/00033494_00000004_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/000`33494/0000/00033494_00000023_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00033494/0000/00033494_00000058_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00033494/0000/00033494_00000016_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00033494/0000/00033494_00000026_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00033494/0000/00033494_00000051_1.allstreams.dst',
        '/lhcb/MC/2012/ALLSTREAMS.DST/00033494/0000/00033494_00000018_1.allstreams.dst',
        ]
    
    configure( inputdata , castor = True )
    
    ## event loop 
    run(1000 )
    
# =============================================================================
# The END
# =============================================================================


