#!/usr/bin/env python
# =============================================================================
# $Id$ 
# =============================================================================
## @file BenderExample/RadLen.py
#
#  The simple Bender-based example for radiation lenth studies 
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
#  @date 2006-10-12
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""
The simple Bender-based example for radiation length studies

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

Last modification $Date$
               by $Author$
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__date__    = " 2006-10-12 "
__version__ = " $Revision$ "
# =============================================================================
## import everything from bender 
from   Bender.All                  import *
from   GaudiKernel.SystemOfUnits   import GeV, meter 
from   LoKiCore.functions          import * 
import LoKiPhys.Phys
import LoKiMC.MC
# =============================================================================
## Simple class for radiation length studies 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2006-10-13
class RadLen(AlgoMC) :
    """
    Simple class for radiation length studies
    """
    
    ## standard algorithm initialization
    def initialize ( self ) :
        """
        Standard initialization
        """
        
        ## initialize the base class
        sc = AlgoMC.initialize ( self )
        if sc.isFailure() : return sc
        
        ## get transport service:
        iTrSvc = self.svc( cpp.ITransportSvc , 'TransportSvc' )
        if not iTrSvc :
            raise RuntimeError , 'Unable to locate Transport Service '
        self.iTrSvc = iTrSvc
        return SUCCESS 
            
            
    ## standard mehtod for analyses
    def analyse( self ) :
        """
        Standard method for analyses
        """

        # get stable, basic MC-particles 
        parts = ( 'pi+' == MCABSID ) | ( 'mu+' == MCABSID ) | ( 'e+'  == MCABSID ) | ( 'K+'  == MCABSID ) | ( 'p+'   == MCABSID )
        # "right" PIDs, pz>0, origin point < 300 mm, and at least one meter  flying distance 
        mcgood = self.mcselect ( 'mcgood' ,
                                  parts & ( 0 < MCPZ ) & ( MCVXFUN ( MCVZ ) < 200  ) & ( MCCTAU > meter ) )
        
        if mcgood.empty() :
            return self.Warning('No Good MC-particles are found' ,SUCCESS )

        mcCut = MCTRUTH ( self.mcTruth() , mcgood ) 
        
        # get reconstructed pions
        rcgood = self.select ( 'rcgood' , ( 'pi+' == ABSID ) & ( 0 < PZ ) )

        if rcgood.empty() :
            return self.Warning('No Good RC-particles are found' ,SUCCESS )

        rcCut = RCTRUTH ( self.mcTruth() , rcgood ) 

        nt1 = self.nTuple ( 'MCTuple')
        
        # loop over mc-particles
        for mc in mcgood :

            pid = mc.particleID().pid()
            mov = mc.originVertex()
            if not mov : continue
            mom = mc.momentum ()
            
            ov  = mov.position() 

            nt1 . column ( 'pid'   , pid  )  ## PID 
            nt1 . column ( 'ov'    , ov   )  ## origin point 
            nt1 . column ( 'p'     , mom  )  ## 4-momentum

            nt1 . column ( 'phi'   , MCPHI   ( mc ) )
            nt1 . column ( 'theta' , MCTHETA ( mc ) )
            nt1 . column ( 'eta'   , MCETA   ( mc ) )
            nt1 . column ( 'p'     , MCP     ( mc ) )
            nt1 . column ( 'pt'    , MCPT    ( mc ) )

            rcFlag = True if rcCut ( mc ) else False
            
            nt1 . column ( 'rc'    , rcFlag )  ## properly recontructed?

            mat = -100.01
            if rcFlag : 
                mat = self.radLen ( ov , mom.Vect() , 1.5 * meter )

            nt1 . column ( 'mat' , mat )  ## x / X0 
            
            nt1 . write () 
            
        nt2 = self.nTuple ( 'RCTuple')        
        # loop over RC-particles
        for rc in rcgood :

            pid = rc.particleID().pid()
            
            mom  = rc.momentum ()
            
            ov   = rc.referencePoint()
            
            if not -100 < ov.Z() < 400 : continue
            

            nt2 . column ( 'pid'   , pid  )  ## PID 
            nt2 . column ( 'ov'    , ov   )  ## reference point 
            nt2 . column ( 'p'     , mom  )  ## 4-momentum

            nt2 . column ( 'phi'   , PHI   ( rc ) )
            nt2 . column ( 'eta'   , ETA   ( rc ) )
            nt2 . column ( 'p'     , P     ( rc ) )
            nt2 . column ( 'pt'    , PT    ( rc ) )

            mcFlag = True if mcCut ( rc ) else False
            
            nt2 . column ( 'mc'    , mcFlag )  ## properly recontructed?

            mat = self.radLen ( ov , mom.Vect() , 1.5 * meter )

            nt2 . column ( 'mat' , mat )  ## x / X0 
            
            nt2 . write () 
        
        return SUCCESS 
        
    ## evaluate the radiation length using Transport Service 
    def radLen ( self , point1 , vct , z ) :
        """
        Evaluate the radiation length using Transport Service
        """
        
        deltaZ  = z - point1.Z()
        newVct  = Gaudi.XYZVector ( vct )
        newVct *=  ( deltaZ  / newVct.Z() ) 
        point2  = Gaudi.XYZPoint ( point1 )
        point2 += newVct 

        return self.iTrSvc.distanceInRadUnits ( point1 , point2 )
    
# =============================================================================
## configure the job
def configure ( datafiles , catalogs = [] , castor = False ) :
    """
    Configure the job
    """
    
    ##
    ## Static configuration using "Configurables"
    ##
    
    from Configurables import DaVinci
    daVinci = DaVinci (
        DataType      = '2010' ,
        Simulation    = True   ,
        HistogramFile = 'RadLen_Histos.root' , 
        TupleFile     = 'RadLen_Tuples.root' 
        ) 
    
    from StandardParticles import StdNoPIDsPions
    InputParticles = [ StdNoPIDsPions.outputLocation() ]
    
    ## define/set the input data 
    setData ( datafiles , catalogs , castor )
    
    ##
    ## jump into the world of the actual Gaudi components!
    ## 
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr()
    
    ## create local algorithm:
    alg = RadLen(
        'RadLen'                   ,
        HistoPrint     = True      ,
        PP2MCs         = [ 'Relations/Rec/ProtoP/Charged'] , 
        Inputs         = InputParticles 
        )
    
    gaudi.setAlgorithms ( [alg] ) 
    
    return SUCCESS 

# =============================================================================
## job steering 
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
        '/lhcb/MC/MC10/ALLSTREAMS.DST/00008919/0000/00008919_00000%03d_1.allstreams.dst' % i for i in range ( 11 , 90 ) 
        ]
    
    configure( inputdata , castor = True )  

    ## run the job
    run(500)

# =============================================================================
# The END 
# =============================================================================
