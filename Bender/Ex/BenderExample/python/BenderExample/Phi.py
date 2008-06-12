#!/usr/bin/env python
# =============================================================================
## The simple Bender-based example: plot dikaon mass peak
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>"Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis"</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  "C++ ToolKit for Smart and Friendly Physics Analysis"
#
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#  "No Vanya's lines are allowed in LHCb/Gaudi software."
#
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-10-12
# =============================================================================
"""
The simple Bender-based example plot dikaon mass peak
"""
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu"
# =============================================================================
## import everything form bender 
from Bender.Main import * 

# =============================================================================
## Simple class to plot dikaon mass peak
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-10-13
class Phi(Algo) :
    """
    Simple class to plot dikaon mass peak
    """    
    ## standard constructor
    def __init__ ( self , name = 'Phi' ) :
        """
        Standard constructor
        """ 
        return Algo.__init__ ( self , name )

    ## standard mehtod for analysis
    def analyse( self ) :
        """
        The standard method for analysis
        """
        ## select all kaons 
        kaons = self.select( 'kaons'  , 'K+'  == ABSID )
        
        self.select( "K+" , kaons , 0 < Q )
        self.select( "K-" , kaons , 0 > Q )
        
        dikaon = self.loop( "K+ K-" , "phi(1020)" )
        for phi in dikaon :
            m12 = phi.mass(1,2) / 1000 
            if 0 > m12 or 1.1 < m12  : continue
            chi2 = VCHI2( phi )
            if 0 > chi2              : continue 
            self.plot( M(phi)/1000 , "K+K- mass " , 1. , 1.1 ) 
            if 0 > chi2 or 49 < chi2 : continue
            self.plot( M(phi)/1000 , "K+K- mass chi2<49  " , 1. , 1.1 ) 
            
        self.setFilterPassed( True )

        return SUCCESS

# =============================================================================
## configure the job
def configure () :
    """
    Configure the job
    """

    importOptions ('$DAVINCIROOT/options/DaVinciCommon.opts')
    importOptions ('$COMMONPARTICLESROOT/options/StandardKaons.opts') 
        
    ## get the input data
    import data_Bs2Jpsiphi_mm as input 

    ## get the actual application manager (create if needed)
    gaudi = appMgr() 
    
    ## create local algorithm:
    alg = Phi()

    ## print histos 
    alg.HistoPrint = True

    ## if runs locally at CERN lxplus 
    gaudi.setAlgorithms( [alg] ) ## gaudi.addAlgorithm ( alg ) 
    
    ## configure the desktop
    desktop = gaudi.tool ( 'Phi.PhysDesktop' )
    desktop.InputLocations = [ '/Event/Phys/StdTightKaons' ]    
    
    ## get input data 
    evtSel = gaudi.evtSel()    
    evtSel.open ( input.FILEs ) 
    evtSel.PrintFreq = 100
    
    return SUCCESS 
    
# =============================================================================
## job steering 
if __name__ == '__main__' :
    
    ## make printout of the own documentations 
    print __doc__
    
    ## configure the job:
    configure()
    
    ## run the job
    run(500)

    
# =============================================================================
# The END 
# =============================================================================
