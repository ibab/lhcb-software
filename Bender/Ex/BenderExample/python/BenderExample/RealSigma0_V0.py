#!/usr/bin/env python
# =============================================================================
## @file BenderExample/RealSigma_V0.py
#
#  The script to analyse the Sigma0
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
#  @date   2010-02-10
# ===========================================================================================
"""

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
# ===========================================================================================
__author__   = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__     = "2010-02-10"
__version__  = "CVS Tag $Name: not supported by cvs2svn $, verison $Release:$"
# ===========================================================================================
import ROOT                           ## needed to produce/visualize the histograms
import LHCbMath.Types                 ## easy access to various geometry routines 
from   Bender.Main         import *   ## import all bender goodies 
from   Gaudi.Configuration import *   ## needed for job configuration

from   GaudiKernel.SystemOfUnits     import GeV, MeV, mm, cm 
from   GaudiKernel.PhysicalConstants import c_light
from   GaudiPython.GaudiAlgs         import Vector

Photons = LoKi.Photons

# =============================================================================
## @class Sigma0
#  Simple algorithm to study Sigma0 -> Lambda0 + gamma 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2010-02-11
class Sigma0(Algo) :
    """
    Simple algorithm to study Sigma0 -> Lambda0 + gamma 
    """    
    ## the standard initialization
    def initialize ( self ) :
        """
        Initialization
        """
        sc = Algo.initialize ( self )
        if sc.isFailure() : return sc
        
        return SUCCESS
    
    ## the only one esential method: 
    def analyse  (self ) :
        """
        The only one essential method
        """
        
        ## get lambdas from TES:
        
        lams = self.select ( 'lam' , 'Lambda0' == ABSID )        
        
        if lams.empty()   :
            return self.Warning ( 'No Lambdas are found' , SUCCESS )
        
        primaries = self.vselect ( 'pv' , ISPRIMARY  )
        if primaries.empty() :
            return self.Warning ( 'No primary vertices are found in algo' , SUCCESS )
        
        gammas = self.select ( "gamma_" , ( 'gamma' == ID ) & ( PT > 200 * MeV ) ) 
        if gammas.empty() :
            return self.Warning ( 'No Gammas (1) are found'  , SUCCESS )
        
        ## remove gammas from pi0s
        output = std.vector('const LHCb::Particle*')()
        Photons.pi0Veto (
            gammas    ,
            20 * MeV  ,
            -1        , 
            output    ) 

        cnt  = self.counter(' rejected')
        cnt += ( float(len(output))/ len(gammas) )
        
        gammas = self.select ( "gamma" , output , 'gamma' == ID ) 
        if gammas.empty() :
            return self.Warning ( 'No Gammas (2) are found'  , SUCCESS )
        
        mips   = MIPCHI2 ( primaries , self.geo() )
        
        odin = self.get('DAQ/ODIN')
        runNum = odin.runNumber   ()
        evtNum = odin.eventNumber () 
        evt1   = int ( evtNum/2**32  ) 
        evt2   = int ( evtNum - 2**32*evt1 ) 
        
        tup  = self.nTuple ( 'Sigma' )        
        
        sigmas = self.loop ( "lam gamma" , "Sigma0" ) 
        for sigma in sigmas :
            
            m = sigma.mass(1,2) / GeV
            if m > 1.3 : continue 
            self.plot ( m , 'Sigma0 - mass 1', 1.100 , 1.300 , 100 ) 


        pions  = self.select ( 'pi' , 'pi+' == ABSID )

        sigmas = self.loop ( "lam pi" , "Sigma0" ) 
        for sigma in sigmas :
            
            m = sigma.mass(1,2) / GeV
            if m > 1.5 : continue

            vchi2 = VCHI2 ( sigma )
            if not 0 <= vchi2 < 49 : continue
            
            lam     = sigma(1) 
            pi1     = sigma(2)

            pr  = lam.child(1)
            pi2 = lam.child(2)

            
            if pi1.particleID() == pi2.particleID() :
                m1 = pi1.momentum()
                m2 = pi2.momentum()
                if     abs ( m1.Px() - m2.Px() ) < 10 * MeV \
                   and abs ( m1.Py() - m2.Py() ) < 10 * MeV \
                   and abs ( m1.Pz() - m2.Pz() ) < 10 * MeV : continue

            
            if 0 < pi1.charge() * pr.charge() :  
                self.plot ( m , 'Sigma* mass 1', 1.200 , 1.800 , 300 ) 
            else  :
                self.plot ( m , 'Sigma* mass 2', 1.200 , 1.800 , 300 ) 

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

    ##
    ## 1. Static configuration using "Configurables"
    ##
    
    from Configurables           import DaVinci    
    davinci = DaVinci (
        DataType      = '2009' ,
        Simulation    = False  ,
        PrintFreq     = 1000   ,
        HistogramFile = 'RealSigma0_V0_Histos.root' 
        )
    
    from GaudiConf.Configuration import NTupleSvc   
    NTupleSvc (
        Output = [ "SIGMA0 DATAFILE='RealSigma0_V0.root' TYPE='ROOT' OPT='NEW'" ]
        )
    
    ## define the input data:
    setData ( datafiles , catalogs )
    
    ##
    ## 2. Jump into the wonderful world of the actual Gaudi components!
    ## 
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr() 
    
    ## create local algorithm:
    alg = Sigma0 (
        'Sigma0'             ,   ## Algorithm name
        NTupleLUN = 'SIGMA0' ,   ## Logical unit for output file with N-tuples
        ##
        ## RecoStripping-04 conventions! 
        InputLocations = [ '/Event/V0/Phys/StrippingLambda0' ,  ## input particles 
                           'StdLooseAllPhotons' ,
                           'StdLoosePions'      ] 
        )
    
    gaudi.setAlgorithms ( [ alg ] ) 
    ## gaudi.addAlgorithm ( alg  ) 
    
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
        [ '/castor/cern.ch/grid' + '/lhcb/data/2010/V0.DST/00006614/0000/00006614_00000%03d_1.v0.dst' % n for n in range ( 2 , 196 ) ] 
        ) 
    
    run ( 5000 )
    

# =============================================================================
# The END 
# =============================================================================
