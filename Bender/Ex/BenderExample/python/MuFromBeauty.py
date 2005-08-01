#!/usr/bin/env python
# =============================================================================
# $Id: MuFromBeauty.py,v 1.2 2005-08-01 09:50:19 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.2 $
# =============================================================================
# @file
# Simple script to select muons from beauty 
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2003-12-04 
# =============================================================================

# import the Bender itself  
from   bendermodule import *

# =============================================================================
# Specific physics analysis algorithm 
# =============================================================================

# create my own algorithm  
class MuFromB(Algo):
    """
    Simple algorithm to select muons from beauty
    """
    def __init__  ( self , name = 'MuFromB' ) :
        self.nmu = 0 # number of direct muons 
        self.nev = 0 # number of events 
        Algo.__init__( self , name )
        
    def finalize ( self ) :
        line = 'Number of direct muons is %s from %s events ' % ( self.nmu , self.nev )
        self.Print( message = line , code = SUCCESS , level = MSG.ALWAYS ) 
        return Algo.finalize( self ) ;
    
    def analyse ( self ) :

        # all MC muons 
        mu1 = self.mcselect( tag = "mu1" ,
                             cuts = 'mu+' == MCABSID )
        
        # all MC muons from beauty decay trees
        bs  = self.mcselect( tag  = 'beauty' , cuts = BEAUTY ) 
        mu2 = self.mcselect( tag  = "mu2" ,
                             cuts = ( 'mu+' == MCABSID ) & ( FROMMCTREE( bs ) ) ) 

        # all direct MC muons from beauty decays 
        mu3 = self.mcselect( tag  = "mu3" ,
                             cuts = ( 'mu+' == MCABSID ) & ( MCMOTHCUT( BEAUTY , False ) ) )
        
        # all cascade MC muons from beauty decays 
        mu4 = self.mcselect( tag    = "mu4" ,
                             source = mu2 , 
                             cuts   = ( 'mu+' == MCABSID ) & ( ~MCMOTHCUT( BEAUTY , False ) ) )
        
        print 'MC Muons all:%d fromB:%d direct:%d cascade:%d ' % ( mu1.size() ,
                                                                   mu2.size() ,
                                                                   mu3.size() ,
                                                                   mu4.size() )
        
        self.nmu += mu3.size()
        self.nev += 1
        
        decay = DecayChain() 
        for mu in mu3 :
            # get mother particle
            mother = mu.mother()
            print decay.printOut( mother , MCALL , ISMCPART( mu ) )
            
        return SUCCESS 

# =============================================================================
# job configuration 
# =============================================================================
def configure () :    
    # Generic job configuration     
    gaudi.config( files   =
                  [ '$DAVINCIROOT/options/DaVinciCommon.opts'   ,   # common options 
                    '$DAVINCIROOT/options/DaVinciReco.opts'     ,   # general 'Reco' options 
                    '$DAVINCIROOT/options/DaVinciTestData.opts' ] ) 
    
    # specific job configuration 
    # create analysis algorithm and add it to the list of
    alg       = MuFromB()
    gaudi.setAlgorithms( [ alg ] )

    return SUCCESS 

# =============================================================================
# job execution 
# =============================================================================
if __name__ == '__main__' :
    import sys 
    # configure the job 
    configure()
    # execute 
    gaudi.run( 100  )

# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
