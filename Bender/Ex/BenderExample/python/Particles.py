#!/usr/bin/env python2.4
# =============================================================================
# $Id: Particles.py,v 1.1 2006-11-09 14:16:00 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revison:$
# =============================================================================
""" The simple Bender-based example: count the particles """
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu"
# =============================================================================
## @file
#  The simple Bender-based example: count the particles
#  @date 2006-10-12
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================

## import everything form bender 
from bendermodule import * 

## @class Particles
#  simple class to cound different particles species
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-10-13
class Particles(Algo) :
    """ simple class to cound different particles species """
    
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
        return SUCCESS
    
    
## configure the job
def configure ( **args ) :
    """ Configure the job """
    
    ## read external configruation files
    gaudi.config ( files = [
        '$DAVINCIROOT/options/DaVinciCommon.opts',
        '$COMMONPARTICLESROOT/options/StandardKaons.opts',
        '$COMMONPARTICLESROOT/options/StandardPions.opts',
        '$COMMONPARTICLESROOT/options/StandardMuons.opts',
        '$DAVINCIROOT/options/DaVinciTestData.opts'
        ] )
    
    ## create local algorithm:
    alg = Particles()

    gaudi.addAlgorithm ( alg ) 
    ## add to main sequence in Davinci
    #davinci = gaudi.algorithm('DaVinciMainSeq')
    #davinci.Members += ['Particles']

    ## configure the desktop
    desktop = gaudi.tool ( 'Particles.PhysDesktop' )
    desktop.InputLocations = [
        '/Event/Phys/StdLooseKaons' ,
        '/Event/Phys/StdLooseMuons' ,
        '/Event/Phys/StdLoosePions' ]
    
    desktop.PropertiesPrint = True

    dos = gaudi.service('DataOnDemandSvc')
    print dos.Algorithms
    
    return SUCCESS 
    
## report about what is imported
if __name__ == '__main__' :

    ## configure the job:
    configure()

    ## run the job
    gaudi.run(50)
    

# =============================================================================
# $Log: not supported by cvs2svn $ 
# =============================================================================
# The END 
# =============================================================================
