#!/usr/bin/env python
# =============================================================================
# $Id: ExHlt.py,v 1.2 2004-11-25 15:21:02 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $
# =============================================================================
# @file 
#    The first attempt to write something 'Hlt'-related 
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2003-12-04
# =============================================================================
""" The first attempt to write something 'Hlt'-related  """

# import the Bender itself  
from   bendermodule  import *

# =============================================================================
# Specific physics analysis algorithm 
# =============================================================================

class ExHlt(Algo):
    """
    The first attempt to write something 'Hlt'-related 
    """
    def analyse ( self ) :
        # get kaons
        
        pions  = self.select( tag="pi+" , cuts = ( 'pi+' == ABSID ) & HASTRGTR )
        
        if pions.empty() : return SUCCESS
        
        mcpi    = self.mcselect( tag = 'mpi' , cuts = 'pi+' == MCABSID )
        mcK     = self.mcselect( tag = 'mK'  , cuts = 'K+'  == MCABSID )
        mcP     = self.mcselect( tag = 'mP'  , cuts = 'p+'  == MCABSID )
        mcE     = self.mcselect( tag = 'mE'  , cuts = 'e+'  == MCABSID )
        mcMu    = self.mcselect( tag = 'mM'  , cuts = 'mu+' == MCABSID )
        
        mc      = self.mctruth()
        truePi  = MCTRUTH( mc , mcpi )
        trueK   = MCTRUTH( mc , mcK  )
        trueP   = MCTRUTH( mc , mcP )
        trueE   = MCTRUTH( mc , mcE  )
        trueMu  = MCTRUTH( mc , mcMu )
        
        pi1  = self.select( tag    = "pimc1+" ,
                            source = pions    ,
                            cuts   = truePi   )
        pi2  = self.select( tag    = "pimc2+" ,
                            source = pions    ,
                            cuts   = trueK    )
        pi3  = self.select( tag    = "pimc3+" ,
                            source = pions    ,
                            cuts   = trueP    )
        pi4  = self.select( tag    = "pimc4+" ,
                            source = pions    ,
                            cuts   = trueE    )
        pi5  = self.select( tag    = "pimc5+" ,
                            source = pions    ,
                            cuts   = trueMu   )
        
        
        print " #pions = %3d [ MC: 'pi+'=%3d 'K+'=%3d 'p+'=%3d 'e+'=%3d 'mu+'=%3d ]" % ( pions.size () ,
                                                                                         pi1.size() ,
                                                                                         pi2.size() ,
                                                                                         pi3.size() ,
                                                                                         pi4.size() ,
                                                                                         pi5.size() )
        
        return SUCCESS 
    
    
# =============================================================================
# Generic job configuration 
# =============================================================================
def configure() :
    gaudi.config( files   =
                  [ '$BENDEREXAMPLEOPTS/BenderExample.opts' ,   # general options 
                    '$BENDEREXAMPLEOPTS/PoolCatalogs.opts'  ,   # pool catalogs
                    '$LOKIEXAMPLEOPTS/Bs_phiphi_DC04.opts'  ,   # input data 
                    '$TRGSYSROOT/options/L1.opts'           ,   # L1 procession 
                    '$TRGSYSROOT/options/Hlt.opts'          ,   # HLT configuration
                    '$TRGSYSROOT/options/TrgChecking.opts'  ] , # HLT configuration
                  options =
                  [ 'EcalPIDmu.OutputLevel     =   5  ' ,
                    'HcalPIDmu.OutputLevel     =   5  ' ,
                    'EcalPIDe.OutputLevel      =   5  ' ,
                    'HcalPIDe.OutputLevel      =   5  ' ,
                    'BremPIDe.OutputLevel      =   5  ' ,
                    'PrsPIDe.OutputLevel       =   5  ' ,
                    'NeutralPP2MC.OutputLevel  =   5  ' ,
                    'Hadrons.OutputLevel       =   5  ' ,
                    'EventSelector.PrintFreq   = 100  ' ] )
    
    #if not 'TrgTools'  in gaudi.DLLs : gaudi.DLLs += [ 'TrgTools'  ] 
    #if not 'TrgVelo'   in gaudi.DLLs : gaudi.DLLs += [ 'TrgVelo'   ] 
    #if not 'TrgVeloTT' in gaudi.DLLs : gaudi.DLLs += [ 'TrgVeloTT' ] 
    #if not 'STDAQ'     in gaudi.DLLs : gaudi.DLLs += [ 'STDAQ'     ] 
    if not 'TrgChecker'     in gaudi.DLLs : gaudi.DLLs += [ 'TrgChecker' ] 

    # specific job configuration 

    gaudi.addAlgorithm ( 'TrgTrack2MCParticle' )
    gaudi.addAlgorithm ( 'TrgLinks2TrgTables/Trg2MCTables' ) 
        
    # create analysis algorithm and add it to the list of
    ex = ExHlt('ExHlt')
    
    gaudi.addAlgorithm ( ex ) 
    
    ex        = gaudi.algorithm('ExHlt')
    ex.TRG2MC = [ 'Rec/Relations/TrgTracks2MCParticles' ]
    
    desktop = gaudi.tool('ExHlt.PhysDesktop')
    desktop.InputLocations  = [ "/Event/Phys/Trg"]
    
    return SUCCESS

# =============================================================================
# job execution 
# =============================================================================

if __name__ == '__main__' :
    import sys 
    # configure the job
    configure() 
    # run job 
    gaudi.run  ( 100  )
    # terminate the Application Manager 
    gaudi.exit ()
    
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.1  2004/11/25 14:55:05  ibelyaev
#  add Trg -> MC example
#
# =============================================================================
# The END 
# =============================================================================
