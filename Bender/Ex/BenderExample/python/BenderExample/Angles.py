#!/usr/bin/env python
# =============================================================================
# $Id$
# =============================================================================
## @file BenderExample/Angles.py
#  The simple Bender-based example to illustrate the calculation of angles
#    for P -> VV decays 
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
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2010-05-31
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""
The simple Bender-based example to illustrate the calculation of angles
for P -> VV decays 
  
This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

                    $Revision$
  Last modification $Date$
                 by $Author$
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__date__    = " 2010-05-31 " 
__version__ = " version $Revision$ "
# =============================================================================
## import everything from bender 
from   Bender.Awesome import *
from   LoKiCore.math  import * 
from   ROOT           import Double
from   PartProp.Nodes import * 
# =============================================================================

IP2VVPartAngleCalculator   = cpp.IP2VVPartAngleCalculator
IP2VVMCPartAngleCalculator = cpp.IP2VVMCPartAngleCalculator

# =============================================================================
## Helper function to extract three angles
#
#   @code
#
#    >>> calculator = ...  # get the angle calculator tool 
#    >>> particle   = ...  # get the particle
#    >>> costheta_l, costheta_k, phi, code = calculator.angles ( particle )
#    >>> if code.isFailure() : print 'ERROR!'
#
#   @endcode 
#    
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-05-31
def _p2vv_angles_ ( self , particle ) :
    """
    Helper function to extract three angles

    >>> calculator = ...  # get the angle calculator tool 
    >>> particle   = ...  # get the particle
    >>> costheta_l, costheta_k, phi, code = calculator.angles ( particle )
    >>> if code.isFailure() : print 'ERROR!'
    
    """
    from ROOT import Double
    from math import cos
    
    _theta_l = Double ( -1.e+10 )
    _theta_k = Double ( -1.e+10 )
    _phi     = Double ( -1.e+10 )

    sc = self.calculateAngles ( particle ,
                                _theta_l ,
                                _theta_k ,
                                _phi     )
    return ( cos( _theta_l ) , cos ( _theta_k ) , _phi, sc )

_p2vv_angles_ . __doc__ += '\n' + IP2VVPartAngleCalculator   . calculateAngles . __doc__ 
_p2vv_angles_ . __doc__ += '\n' + IP2VVMCPartAngleCalculator . calculateAngles . __doc__ 

IP2VVPartAngleCalculator.angles   = _p2vv_angles_
IP2VVMCPartAngleCalculator.angles = _p2vv_angles_

# =============================================================================
## Helper function to extract three angles
#
#   @code
#
#    >>> calculator = ...  # get the angle calculator tool 
#    >>> particle   = ...  # get the particle
#    >>> costheta_tr, phi, psi, code = calculator.tr_angles ( particle )
#    >>> if code.isFailure() : print 'ERROR!'
#
#   @endcode 
#    
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-05-31
def _p2vv_tr_angles_ ( self , particle ) :
    """
    Helper function to extract three angles

    >>> calculator = ...  # get the angle calculator tool 
    >>> particle   = ...  # get the particle
    >>> costheta_tr, phi, psi, code = calculator.angles ( particle )
    >>> if code.isFailure() : print 'ERROR!'
    
    """
    from ROOT import Double
    from math import cos,sin
    
    _theta_tr = Double ( -1.e+10 )
    _phi      = Double ( -1.e+10 )
    _psi      = Double ( -1.e+10 )
    
    sc = self.calculateTransversityAngles ( particle  ,
                                            _theta_tr ,
                                            _phi      ,
                                            _psi      )
    return ( cos( _theta_tr ) , _phi, cos(_psi) , sc )

_p2vv_tr_angles_ . __doc__ += '\n' + IP2VVPartAngleCalculator   . calculateTransversityAngles . __doc__ 
_p2vv_tr_angles_ . __doc__ += '\n' + IP2VVMCPartAngleCalculator . calculateTransversityAngles . __doc__ 

IP2VVPartAngleCalculator   .tr_angles = _p2vv_tr_angles_
IP2VVMCPartAngleCalculator .tr_angles = _p2vv_tr_angles_

# =============================================================================
## @class Angles
#  Simple class to check various ``angle calculators''
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-05-31
class Angles (AlgoMC) :
    """
    Simple class to check various ``angle calculators''
    """
    
    ## initialize the algorithm (locate tom's and Greig's tools) 
    def initialize ( self ) :
        """
        initialize the algorithm (locate tom's and Greig's tools)
        """
        
        sc = AlgoMC.initialize ( self )
        if sc.isFailure() : return sc
        
        self.tool_rc = self.tool ( cpp.IP2VVPartAngleCalculator   , 'Bs2JpsiPhiAngleCalculator'   )
        self.tool_mc = self.tool ( cpp.IP2VVMCPartAngleCalculator , 'MCBs2JpsiPhiAngleCalculator' ) 

        
        ## how to calculate  cos(theta_l) ? 
        self._cos_L_rc  = [
            ## use decay descritors  
            COSPOL ( 'Xb ->  ( J/psi(1S) -> ^mu+  mu- ) X ' ,
                     'Xb -> ^( J/psi(1S) ->  mu+  mu- ) X ' ) ,
            COSPOL ( 'Xb ->  ( J/psi(1S) -> ^mu+  mu- ) X ' ,
                     'Xb ->  ( J/psi(1S) ->  mu+ ^mu- ) X ' , False ) ,
            ## use nodes 
            COSPOL ( EllPlus      , Meson & Charm )       ,
            COSPOL ( EllPlus      , Pid ( 'J/psi(1S)' ) ) , 
            COSPOL ( Pid('mu+')   , Pid ( 'J/psi(1S)' ) ) ,
            COSPOL ( Pid('mu+')   , Pid ( 'mu-' ) , False ) ,
            COSPOL ( EllPlus      , EllMinus      , False ) , 
            ## use cuts
            COSPOL ( 'mu+'       == ID , 'J/psi(1S)' == ID )         , 
            COSPOL ( 'mu+'       == ID , 'mu-'       == ID , False ) 
            ]
        
        ## how to calculate  cos(theta_K) ? 
        self._cos_K_rc  = [
            ## use decay descritors  
            COSPOL ( 'Xb ->  ( phi(1020) -> ^K+  K- ) X ' ,
                     'Xb -> ^( phi(1020) ->  K+  K- ) X ' ) ,
            COSPOL ( 'Xb ->  ( phi(1020) -> ^K+  K- ) X ' ,
                     'Xb ->  ( phi(1020) ->  K+ ^K- ) X ' , False ) ,
            ## use nodes 
            COSPOL ( Hadron & Positive & Strange ,
                     Hadron & Neutral  & Strange & ~Beauty )   ,
            COSPOL ( Hadron & Positive & Strange , Pid ( 'phi(1020)' ) ) , 
            COSPOL ( Pid('K+')   , Pid ( 'phi(1020)' ) ) ,
            COSPOL ( Pid('K+')   , Pid ( 'K-' )  , False ) ,
            COSPOL ( Hadron & Positive & Strange ,
                     Hadron & Negative & Strange , False ) , 
            ## use cuts
            COSPOL ( 'K+'       == ID , 'phi(1020)' == ID )         ,
            COSPOL ( 'K+'       == ID , 'K-'        == ID , False ) 
            ]
        
        ## how to calculate  sin(chi) ? 
        self._sin_chi_rc  = [
            ## use decay descritors  
            SINCHI ( 'Xb ->  ( J/psi(1S) -> ^mu+   mu- ) phi(1020)'  ,
                     'Xb ->  ( J/psi(1S) ->  mu+  ^mu- ) phi(1020) ' ,
                     'Xb ->  J/psi(1S) ( phi(1020) -> ^K+  K- ) '    ,
                     'Xb ->  J/psi(1S) ( phi(1020) ->  K+ ^K- ) '    ) ,
            ## use nodes
            SINCHI ( EllPlus           ,
                     EllMinus          ,
                     Hadron & Positive ,
                     Hadron & Negative ) ,
            ## use cuts
            SINCHI ( 'mu+'       == ID ,
                     'mu-'       == ID ,
                     'K+'        == ID ,
                     'K-'        == ID ) 
            ]
        
        ## how to calculate  cos(chi) ? 
        self._cos_chi_rc  = [
            ## use decay descritors  
            COSCHI ( 'Xb ->  ( J/psi(1S) -> ^mu+   mu- ) phi(1020)'  ,
                     'Xb ->  ( J/psi(1S) ->  mu+  ^mu- ) phi(1020) ' ,
                     'Xb ->  J/psi(1S) ( phi(1020) -> ^K+  K- ) '    ,
                     'Xb ->  J/psi(1S) ( phi(1020) ->  K+ ^K- ) '    ) ,
            ## use nodes
            COSCHI ( EllPlus           ,
                     EllMinus          ,
                     Hadron & Positive ,
                     Hadron & Negative )  ,
            ## use cuts
            COSCHI ( 'mu+'       == ID ,
                     'mu-'       == ID ,
                     'K+'        == ID ,
                     'K-'        == ID ) 
            ]
        
        ## how to calculate  angle chi itself?
        self._ang_chi_rc  = [
            ## use decay descritors  
            ANGLECHI ( 'Xb ->  ( J/psi(1S) -> ^mu+   mu- ) phi(1020)'  ,
                       'Xb ->  ( J/psi(1S) ->  mu+  ^mu- ) phi(1020) ' ,
                       'Xb ->  J/psi(1S) ( phi(1020) -> ^K+  K- ) '    ,
                       'Xb ->  J/psi(1S) ( phi(1020) ->  K+ ^K- ) '    ) ,
            ## use nodes
            ANGLECHI ( EllPlus           ,
                       EllMinus          ,
                       Hadron & Positive ,
                       Hadron & Negative ) ,
            ## use cuts
            ANGLECHI ( 'mu+'       == ID ,
                       'mu-'       == ID ,
                       'K+'        == ID ,
                       'K-'        == ID ) 
            ]

            
        ## how to calculate  cos(theta_l) ? 
        self._cos_L_mc  = [
            ## use decay descritors  
            MCCOSPOL ( 'Xb =>  ( J/psi(1S) => ^mu+  mu- ) X ' ,
                       'Xb => ^( J/psi(1S) =>  mu+  mu- ) X ' ) ,
            MCCOSPOL ( 'Xb =>  ( J/psi(1S) => ^mu+  mu- ) X ' ,
                       'Xb =>  ( J/psi(1S) =>  mu+ ^mu- ) X ' , False ) ,
            ## use nodes 
            MCCOSPOL ( EllPlus      , Meson & Charm )       ,
            MCCOSPOL ( EllPlus      , Pid ( 'J/psi(1S)' ) ) , 
            MCCOSPOL ( Pid ('mu+')  , Pid ( 'J/psi(1S)' ) ) ,
            MCCOSPOL ( Pid ('mu+')  , Pid ( 'mu-' ) , False ) ,
            MCCOSPOL ( EllPlus      , EllMinus      , False ) ,
            ## use cuts
            MCCOSPOL ( 'mu+'       == MCID , 'J/psi(1S)' == MCID )         ,
            MCCOSPOL ( 'mu+'       == MCID , 'mu-'       == MCID , False ) 
            ]
        
        ## how to calculate  cos(theta_K) ? 
        self._cos_K_mc  = [
            ## use decay descritors  
            MCCOSPOL ( 'Xb =>  ( phi(1020) => ^K+  K- ) X ' ,
                       'Xb => ^( phi(1020) =>  K+  K- ) X ' ) ,
            MCCOSPOL ( 'Xb =>  ( phi(1020) => ^K+  K- ) X ' ,
                       'Xb =>  ( phi(1020) =>  K+ ^K- ) X ' , False ) ,
            ## use nodes 
            MCCOSPOL ( Hadron & Positive & Strange           ,
                       Hadron & Neutral  & Strange & ~Beauty )   ,
            MCCOSPOL ( Hadron & Positive & Strange , Pid ( 'phi(1020)' ) ) , 
            MCCOSPOL ( Pid('K+')   , Pid ( 'phi(1020)' ) ) ,
            MCCOSPOL ( Pid('K+')   , Pid ( 'K-' )  , False ) ,
            MCCOSPOL ( Hadron & Positive & Strange ,
                       Hadron & Negative & Strange , False ) , 
            ## use cuts
            MCCOSPOL ( 'K+'        == MCID , 'phi(1020)' == MCID )         ,
            MCCOSPOL ( 'K+'        == MCID , 'K-'        == MCID , False ) 
            ]
        
        ## how to calculate  sin(chi) ? 
        self._sin_chi_mc  = [
            ## use decay descritors  
            MCSINCHI ( 'Xb =>  ( J/psi(1S) => ^mu+   mu- ) phi(1020)'  ,
                       'Xb =>  ( J/psi(1S) =>  mu+  ^mu- ) phi(1020) ' ,
                       'Xb =>  J/psi(1S) ( phi(1020) => ^K+  K- ) '    ,
                       'Xb =>  J/psi(1S) ( phi(1020) =>  K+ ^K- ) '    ) ,
            ## use nodes
            MCSINCHI ( EllPlus           ,
                       EllMinus          ,
                       Hadron & Positive ,
                       Hadron & Negative ) ,
            ## use cuts
            MCSINCHI ( 'mu+'       == MCID ,
                       'mu-'       == MCID ,
                       'K+'        == MCID ,
                       'K-'        == MCID ) 
            ]
        
        ## how to calculate  cos(chi) ? 
        self._cos_chi_mc  = [
            ## use decay descritors  
            MCCOSCHI ( 'Xb =>  ( J/psi(1S) => ^mu+   mu- ) phi(1020)'  ,
                       'Xb =>  ( J/psi(1S) =>  mu+  ^mu- ) phi(1020) ' ,
                       'Xb =>  J/psi(1S) ( phi(1020) => ^K+  K- ) '    ,
                       'Xb =>  J/psi(1S) ( phi(1020) =>  K+ ^K- ) '    ) ,
            ## use nodes
            MCCOSCHI ( EllPlus           ,
                       EllMinus          ,
                       Hadron & Positive ,
                       Hadron & Negative )  ,
            ## use cuts
            MCCOSCHI ( 'mu+'       == MCID ,
                       'mu-'       == MCID ,
                       'K+'        == MCID ,
                       'K-'        == MCID ) 
            ]
        
        ## how to calculate  angle chi itself?
        self._ang_chi_mc  = [
            ## use decay descritors  
            MCANGLECHI ( 'Xb =>  ( J/psi(1S) => ^mu+   mu- ) phi(1020)'  ,
                         'Xb =>  ( J/psi(1S) =>  mu+  ^mu- ) phi(1020) ' ,
                         'Xb =>  J/psi(1S) ( phi(1020) => ^K+  K- ) '    ,
                         'Xb =>  J/psi(1S) ( phi(1020) =>  K+ ^K- ) '    ) ,
            ## use nodes
            MCANGLECHI ( EllPlus           ,
                         EllMinus          ,
                         Hadron & Positive ,
                         Hadron & Negative ) ,
            ## use cuts
            MCANGLECHI ( 'mu+'       == MCID ,
                         'mu-'       == MCID ,
                         'K+'        == MCID ,
                         'K-'        == MCID ) 
            ]
            

        return SUCCESS 
                                   
    ## The standard method for analysis
    def analyse ( self ) :
        """
        The standard method for analysis
        
        """
        
        fn  = LoKi.Kinematics.cosThetaTr
        fn2 = LoKi.Kinematics.cosPhiTr
        fn3 = LoKi.Kinematics.sinPhiTr
        fn4 = LoKi.Kinematics.anglePhiTr
        
        ## get recontructed Bs mesons
        bs    = self.select ( 'Bs' , 'B_s0' == ABSID )
        if 1 !=   bs.size () : return SUCCESS                  ## RETURN

        ## find true MC-decays
        mcbs = self.mcselect ( 'mcBs' , ' [ B_s0 => (  J/psi(1S) =>  mu+  mu- ) (  phi(1020) =>  K+  K- ) ]CC' )
        if 1 != mcbs.size () : return SUCCESS                  ## RETURN
        
        mc = mcbs[0]
        rc =   bs[0]
        
        print 'P2VV tool MC  : ', self.tool_mc.angles ( mc )  
        print 'P2VV tool RC  : ', self.tool_rc.angles ( rc )  
                
        print 'cos_theta_L_RC: ' , [ f ( rc ) for f in self._cos_L_rc   ]
        print 'cos_theta_L_MC: ' , [ f ( mc ) for f in self._cos_L_mc   ]

        print 'cos_theta_K_RC: ' , [ f ( rc ) for f in self._cos_K_rc   ]
        print 'cos_theta_K_MC: ' , [ f ( mc ) for f in self._cos_K_mc   ]

        print 'sin_chi_RC    : ' , [ f ( rc ) for f in self._sin_chi_rc ]
        print 'sin_chi_MC    : ' , [ f ( mc ) for f in self._sin_chi_mc ]

        
        print 'cos_chi_RC    : ' , [ f ( rc ) for f in self._cos_chi_rc ]
        print 'cos_chi_MC    : ' , [ f ( mc ) for f in self._cos_chi_mc ]

        print 'ang_chi_RC    : ' , [ f ( rc ) for f in self._ang_chi_rc ]
        print 'ang_chi_MC    : ' , [ f ( mc ) for f in self._ang_chi_mc ]

        print 'P2VV tool MC-tr  : ', self.tool_mc.tr_angles ( mc )  
        print 'P2VV tool RC-tr  : ', self.tool_rc.tr_angles ( rc )  

        mcCosThetaTr = MCCOSTHETATR (
            'Xb => ( J/psi(1S) => ^mu+   mu- ) phi(1020)'  ,
            'Xb => ( J/psi(1S) =>  mu+  ^mu- ) phi(1020) ' ,
            'Xb => J/psi(1S) ( phi(1020) => ^K+  K- ) '    ,
            'Xb => J/psi(1S) ( phi(1020) =>  K+ ^K- ) '
            )
        mcAnglePhiTr = MCANGLEPHITR (
            'Xb => ( J/psi(1S) => ^mu+   mu- ) phi(1020)'  ,
            'Xb => ( J/psi(1S) =>  mu+  ^mu- ) phi(1020) ' ,
            'Xb => J/psi(1S) ( phi(1020) => ^K+  K- ) '    ,
            'Xb => J/psi(1S) ( phi(1020) =>  K+ ^K- ) '
            )
        
        cosThetaTr = COSTHETATR (
            'Xb -> ( J/psi(1S) -> ^mu+   mu- ) phi(1020)'  ,
            'Xb -> ( J/psi(1S) ->  mu+  ^mu- ) phi(1020) ' ,
            'Xb -> J/psi(1S) ( phi(1020) -> ^K+  K- ) '    ,
            'Xb -> J/psi(1S) ( phi(1020) ->  K+ ^K- ) '
            )
        anglePhiTr = ANGLEPHITR (
            'Xb -> ( J/psi(1S) -> ^mu+   mu- ) phi(1020)'  ,
            'Xb -> ( J/psi(1S) ->  mu+  ^mu- ) phi(1020) ' ,
            'Xb -> J/psi(1S) ( phi(1020) -> ^K+  K- ) '    ,
            'Xb -> J/psi(1S) ( phi(1020) ->  K+ ^K- ) '
            )
        
        print 'transversity ' , \
              (   cosThetaTr ( rc ) ,   anglePhiTr ( rc ) ) , \
              ( mcCosThetaTr ( mc ) , mcAnglePhiTr ( mc ) ) 
        

        
        return SUCCESS


# =============================================================================
## configure the job
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-05-31
def configure ( datafiles , catalogs = [] , castor = False ) :
    """
    Configure the job
    """
    ##
    ## Static Configuration 
    ##
    
    from Configurables import DaVinci
    daVinci = DaVinci (
        DataType      = '2010'  ,
        Simulation    = True    ,
        Persistency   = 'ROOT'  ,
        HistogramFile = 'Angles_Histos.root' ,
        TupleFile     = 'Angles_Tuples.root' )

    #
    ## define input data
    #
    setData  ( datafiles , catalogs , castor )
    
    from StandardParticles import StdTightKaons, StdTightMuons
    
    StdTightKaons = StdTightKaons.outputLocation()
    StdTightMuons = StdTightMuons.outputLocation()
    
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
        ## MC-relations
        PP2MCs = [ 'Relations/Rec/ProtoP/Charged' ] ,
        ## input particles :
        Inputs = [ StdTightKaons , StdTightMuons  ]
        )
    
    angles = Angles (
        'Angles'              ,
        ## print histos 
        HistoPrint = True     ,
        ## N-tuple LUN 
        NTupleLUN  = "PsiPhi" ,
        ## MC-relations
        PP2MCs = [ 'Relations/Rec/ProtoP/Charged' ] ,
        ## input particles :
        Inputs = [ 'Phys/PsiPhi/Particles' ]
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
    print '*'*120  
    
    ## configure the job:
    inputdata = [
        '/lhcb/MC/MC10/ALLSTREAMS.DST/00008919/0000/00008919_00000%03d_1.allstreams.dst' % i for i in range ( 1 , 90 ) 
        ]

    configure( inputdata , castor = True )  
    
    ## run the job
    run(501)

    
# =============================================================================
# The END 
# =============================================================================
