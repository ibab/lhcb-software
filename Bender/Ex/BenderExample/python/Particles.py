#!/usr/bin/env python2.4
# =============================================================================
# $Id: Particles.py,v 1.4 2007-03-22 18:53:00 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revison:$
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
from bendermodule import * 
# =============================================================================

# =============================================================================
## Simple class to deal with jets 
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-10-13
class Particles(Algo) :
    """ simple class to count particles """
    
    ## standard constructor
    def __init__ ( self , name = 'Particles' ) :
        """ standard constructor """ 
        return Algo.__init__ ( self , name )

    ## standard mehtod for analyses
    def analyse( self ) :
        """ standard mehtod for analyses """
        
        ## select all pions 
        pions = self.select( 'pions'  , 'pi+' == ABSID )
        ## select all kaons 
        kaons = self.select( 'kaons'  , 'K+'  == ABSID )
        ## select all muons
        muons = self.select( 'muons'  , 'mu+' == ABSID )
                
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
    """ Configure the job """
    
    ## read external configruation files
    gaudi.config ( files = [
        '$DAVINCIROOT/options/DaVinciCommon.opts'         ,
        '$COMMONPARTICLESROOT/options/StandardKaons.opts' ,
        '$COMMONPARTICLESROOT/options/StandardPions.opts' ,
        '$COMMONPARTICLESROOT/options/StandardMuons.opts'
        ] )
    
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
    
        ## get the input data
    import data_Bs2Jpsiphi_mm as input 
    
    ## get input data 
    evtSel = gaudi.evtSel()    
    evtSel.open ( input.PFNs ) 
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
# $Log: not supported by cvs2svn $
#
# =============================================================================
# The END 
# =============================================================================
