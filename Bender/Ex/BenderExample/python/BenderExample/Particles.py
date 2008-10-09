#!/usr/bin/env python
# =============================================================================
## The simple Bender-based example: count the particles
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
#  @date 2006-10-12
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
"""
The simple Bender-based example: count the particles
"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $   "
# =============================================================================

# =============================================================================
## import everything form bender 
from Bender.Main import * 
# =============================================================================
## Simple class to count particles 
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-10-13
class Particles(Algo) :
    """
    Simple class to count particles
    """    
    ## standard constructor
    def __init__ ( self , name = 'Particles' ) :
        """
        Standard constructor
        """ 
        return Algo.__init__ ( self , name )

    ## standard method for analysis
    def analyse( self ) :
        """
        Standard method for analysis
        """
        
        ## select all pions 
        pions = self.select ( 'pions'  , 'pi+' == ABSID )
        ## select all kaons 
        kaons = self.select ( 'kaons'  , 'K+'  == ABSID )
        ## select all muons
        muons = self.select ( 'muons'  , 'mu+' == ABSID )
                
        msg = ' Particles pi/K/mu : %d/%d/%d' % ( pions.size() ,
                                                  kaons.size() ,
                                                  muons.size() ) 
        self.Print( msg )
        
        ## count particles
        nPions  = self.counter('#pions')
        nKaons  = self.counter('#kaons')
        nMuons  = self.counter('#muons')
        
        nPions += pions.size()
        nKaons += kaons.size()
        nMuons += muons.size()
        
        self.setFilterPassed( True ) 
        return SUCCESS                              # RETURN 
    
    
# =============================================================================
## configure the job
def configure ( **args ) :
    """
    Configure the job
    """
    
    ## read external configruation files
    importOptions('$DAVINCIROOT/options/DaVinciCommon.opts')
    importOptions('$COMMONPARTICLESROOT/options/StandardKaons.opts')
    importOptions('$COMMONPARTICLESROOT/options/StandardPions.opts')
    importOptions('$COMMONPARTICLESROOT/options/StandardMuons.opts')

    ## get (create if needed) the actual application manager
    gaudi = appMgr()
    
    ## create local algorithm:
    alg = Particles()
    
    ##gaudi.addAlgorithm ( alg ) 
    gaudi.setAlgorithms( [alg] ) 
    
    ## configure the desktop
    desktop = gaudi.tool ( 'Particles.PhysDesktop' )
    desktop.InputLocations = [
        '/Event/Phys/StdTightPions' ,
        '/Event/Phys/StdTightKaons' ,
        '/Event/Phys/StdTightMuons' ]
    
    ## get input data
    import LoKiExample.Bs2Jpsiphi_mm_data as input 
    
    evtSel = gaudi.evtSel()    
    evtSel.open ( input.Files ) 
    evtSel.PrintFreq = 1
    
    return SUCCESS                                # RETURN 

# =============================================================================
## job steering 
if __name__ == '__main__' :

    ## print own documentation
    print __doc__
    
    ## configure the job:
    configure()
    
    ## run the job
    run(50)
    
# =============================================================================
# The END 
# =============================================================================
