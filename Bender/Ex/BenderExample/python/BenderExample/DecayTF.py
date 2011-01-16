#!/usr/bin/env python
# =============================================================================
# $Id$
# =============================================================================
# $URL$ 
# =============================================================================
## @file BenderExample/DecayTF.py
#  The simple Bender-based example to illustrate the usage of decay
#  tree fitter 
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
#  @date 2010-05-31
#
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""
The simple Bender-based example to illustrate the usage of decay tree fitter 
  
This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the campain of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

Last modification $Date$
               by $Author$
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__date__    = " 2010-06-08 " 
__version__ = " Version $Revision$ "
# =============================================================================
## import everything from bender 
from   Bender.Awesome import *
from   LoKiCore.math  import * 
from   ROOT           import Double
from   PartProp.Nodes import * 
# =============================================================================
import LHCbMath.Types

# =============================================================================
## @class DecayTF
#  Simple class to test "decay-tree-fitter"
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-05-31
class DecayTF(AlgoMC) :
    """
    Simple class to check various ``decay-tree-fitter''
    """
    ## initialize the algorithm 
    def initialize ( self ) :
        """
        Initialize the algorithm 
        """

        ## initialize the base 
        sc = AlgoMC.initialize ( self ) ## initialize the base
        if sc.isFailure() : return sc

        self._dtf = self.decayTreeFitter('LoKi::DecayTreeFit')

        print self._dtf.name()
        
        return SUCCESS
    
    
    ## The standard method for analysis
    def analyse ( self ) :
        """
        The standard method for analysis
        
        """
        
        ## get recontructed Bs mesons
        bs    = self.select ( 'Bs' , 'B_s0' == ABSID )
        if 1 !=   bs.size () : return SUCCESS                  ## RETURN

        ## find true MC-decays
        mcbs = self.mcselect ( 'mcBs' , ' [ B_s0 => (  J/psi(1S) =>  mu+  mu- ) (  phi(1020) =>  K+  K- ) ]CC' )
        if 1 != mcbs.size () : return SUCCESS                  ## RETURN

        ## unfitted bs
        print 'Unfitted Bs mass ' , bs[0].momentum().M() 

        Bs = bs[0]
        if not Bs : return SUCCESS 

        # =====================================================================
        ## A. use DecayTreeFitter.Fitter directly 
        
        DTF    = cpp.DecayTreeFitter.Fitter
        
        fitter = DTF ( Bs )
        fitter.fit()
        if DTF.Success != fitter.status() :
            self.Warning ('Error from DecayTreeFitter.Fitter.fit'  )
        else :
            print 'Fitter : fit chi2 ', fitter.chiSquare()
            param = fitter.fitParams()
            if param :
                print 'Fitter:  Fitted Bs: ' , \
                      param.momentum().m  () , \
                      param.momentum().p  () , \
                      param.momentum().e  () , \
                      param.momentum().pt () 

        # =====================================================================
        ## B. Use IDecayTreeFitter tool 

        fitter = self._dtf 
        sc = fitter.fit ( Bs )
        if sc.isFailure() :
            self.Warning ('Error from IDecayTreeFit', sc )
        else :
            print 'Tool   : fit chi2 ', fitter.chi2()
            param = fitter.fitted()
            if not not param :
                print 'Tool  :  Fitted Bs: ' , \
                      param.momentum().m  () , \
                      param.momentum().p  () , \
                      param.momentum().e  () , \
                      param.momentum().pt () 

        # =====================================================================
        ## C. Use IDecayTreeFitter tool & mass-constarints 

        fitter.addConstraint ( LoKi.Particles.pidFromName( "J/psi(1S)" ) ) 
        sc = fitter.fit ( Bs )
        if sc.isFailure() :
            self.Warning ('Error from IDecayTreeFit', sc )
        else :
            print 'Tool+MC: fit chi2 ', fitter.chi2()
            param = fitter.fitted()
            if not not param :
                print 'Tool+MC:  Fitted Bs: ' ,  \
                      param.momentum().m  () , \
                      param.momentum().p  () , \
                      param.momentum().e  () , \
                      param.momentum().pt () 
                      
                
        # =====================================================================
        ## D1. Use LoKi-functors without PV-constraints
        
        fun1 = DTF_FUN ( M , False  ) 
        print ' LoKi, without PV     '  , fun1  ( Bs )

        ## D2. Use LoKi-functors without PV-constraints
        
        fun2 = DTF_FUN ( M , True   ) 
        print ' LoKi, with    PV      ' , fun2 ( Bs )

        ## D3. Use LoKi-functors without PV-constraint and with J/psi mass 
        
        fun3 = DTF_FUN ( M , False ,  'J/psi(1S)') 
        print ' LoKi, without PV + MC ' , fun3 ( Bs )

        ## D4. Use LoKi-functors without PV-constraints and withJ/psi mass 
        
        fun4 = DTF_FUN ( M , True  , 'J/psi(1S)' ) 
        print ' LoKi, with    PV + MC ' , fun4 ( Bs )
        
        return SUCCESS


# =============================================================================
## configure the job
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-05-31
def configure ( datafiles , catalogs = [] ) :
    """
    Configure the job
    """
    ##
    ## Static Configuration 
    ##
    
    from Configurables import DaVinci
    daVinci = DaVinci (
        DataType   = 'MC09'      ,
        Simulation = True
        )
    
    from Configurables import HistogramPersistencySvc
    HistogramPersistencySvc ( OutputFile = 'Angles_Histos.root' ) 
    
    from Configurables import NTupleSvc
    NTupleSvc ( Output = [ "PsiPhi DATAFILE='Bs2PsiPhi_Tuples.root' TYPE='ROOT' OPT='NEW'"] )
    
    ## define input data 
    setData  ( datafiles , catalogs )
    
    ##
    ## Dynamic Configuration: Jump into the wonderful world of GaudiPython 
    ##
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr()
    
    ## create local algorithm:
    from BenderExample.Bs2PsiPhi import Bs2PsiPhi
    alg = Bs2PsiPhi(
        'PsiPhi'              , 
        ## print histos 
        HistoPrint = True     ,
        ## N-tuple LUN 
        NTupleLUN  = "PsiPhi" ,
        ## MC-relations
        PP2MCs = [ 'Relations/Rec/ProtoP/Charged' ] ,
        ## input particles :
        InputLocations = [ 'StdTightKaons' , 'StdTightMuons' ]
        )

    angles = DecayTF (
        'DecayTF'             ,
        ## print histos 
        HistoPrint = True     ,
        ## N-tuple LUN 
        NTupleLUN  = "PsiPhi" ,
        ## MC-relations
        PP2MCs = [ 'Relations/Rec/ProtoP/Charged' ] ,
        ## input particles :
        InputLocations = [ 'PsiPhi' ]
        )
    
    gaudi.setAlgorithms( [ alg , angles ] )

    return SUCCESS 

# =============================================================================
## job steering 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-05-31
if __name__ == '__main__' :
    
    ## make printout of the own documentations 
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print ' dir(%s) : %s ' % ( __name__    , dir() )
    print '*'*120  
    
    ## configure the job:
    inputdata = [ 
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005135/0000/00005135_00000514_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005135/0000/00005135_00000515_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005135/0000/00005135_00000516_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005135/0000/00005135_00000517_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005135/0000/00005135_00000518_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005135/0000/00005135_00000519_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005135/0000/00005135_00000520_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005135/0000/00005135_00000521_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005135/0000/00005135_00000522_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005135/0000/00005135_00000523_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'"
        ]

    configure( inputdata ) 
    ## run the job
    run(51)

    
# =============================================================================
# The END 
# =============================================================================
