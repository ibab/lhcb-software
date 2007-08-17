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
""" The simple Bender-based example: count the particles """
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu"
# =============================================================================

# =============================================================================
## import everything form bender 
from Bender.Main import * 
# =============================================================================

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
    
    ## get the input data
    import data_Bs2Jpsiphi_mm as input 

    ## read external configruation files
    gaudi.config (
        files = [
        '$DAVINCIROOT/options/DaVinciCommon.opts'         ,
        '$COMMONPARTICLESROOT/options/StandardKaons.opts' ,
        '$COMMONPARTICLESROOT/options/StandardPions.opts' ,
        '$COMMONPARTICLESROOT/options/StandardMuons.opts' ] ,
        options = [
        "PoolDbCacheSvc.Catalog = %s "%input.catalog_CERN ]
        )
    
    ## StagerSvc at CERN
    if 'CERN' == os.environ.get('CMTSITE',None) and \
           os.environ.has_key('GAUDISITESVCROOT') :
        stager = gaudi.service('StagerSvc')
        stager.BlockSize    =  20
        stager.InitialStage =   5 
        if not 'GaudiSiteSvc' in gaudi.DLLs   : gaudi.DLLs   += [ 'GaudiSiteSvc']
        if not 'StagerSvc'    in gaudi.ExtSvc : gaudi.ExtSvc += [ 'StagerSvc'   ]    
        
    ## create local algorithm:
    alg = Particles()

    gaudi.addAlgorithm ( alg ) 

    ## configure the desktop
    desktop = gaudi.tool ( 'Particles.PhysDesktop' )
    desktop.InputLocations = [
        '/Event/Phys/StdLooseKaons' ,
        '/Event/Phys/StdLooseMuons' ,
        '/Event/Phys/StdLoosePions' ]
    
    
    ## get input data 
    evtSel = gaudi.evtSel()    
    evtSel.open ( input.FILEs ) 
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
    gaudi.run(50)
    
# =============================================================================
# The END 
# =============================================================================
