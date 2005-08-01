#!/usr/bin/env python
# =============================================================================
# $Id: VeloClusterMC.py,v 1.2 2005-08-01 09:50:19 ibelyaev Exp $
# =============================================================================
# CVS version $Revision: 1.2 $
# =============================================================================
# CVS tag     $Name: not supported by cvs2svn $
# =============================================================================
# @file 
# Simple algorithm to manipulate VelColuster -> MC truth relations 
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2003-12-04
# =============================================================================
"""
Simple algorithm to manipulate VelColuster -> MC truth relations.
The algorithm is 'identical' to the C++ algorihtm LoKi_VeloClusterMC
from Ex/LoKiExample package, see
$LOKIEXAMPLEROOT/src/LoKi_VeloClusterMC.cpp and
$LOKIEXAMPLEROOT/options/DV_LoKi_VeloClusterMC.opts 
"""
# =============================================================================
__author__ = 'Vanya BELYAEV belyaev@lapp.in2p3.fr'
# =============================================================================

# import the Bender itself  
from   bendermodule  import *
# explicite import, in future will be importen implicitely 
import benderrels
import math

# =============================================================================
# Specific physics analysis algorithm 
# =============================================================================
class VeloClusterMC(Algo):
    """
    Simple algorithm to manipulate VeloCLuster -> MC relations.
    The algorithm is 'identical' to the C++ algorihtm LoKi_VeloClusterMC
    from Ex/LoKiExample package, see
    $LOKIEXAMPLEROOT/src/LoKi_VeloClusterMC.cpp and
    $LOKIEXAMPLEROOT/options/DV_LoKi_VeloClusterMC.opts 
    """
    def __init__ ( self , name = 'VeloMC' ) :
        Algo.__init__( self , name )
        
    def analyse ( self ) :
        
        # get all VeloClusters
        clusters = self.get('Raw/Velo/Clusters')

        self.plot( value = math.tanh( clusters.size() / 1000 ) ,
                   title = " tanh( #clusters / 1000 ) "        ,
                   high  = 1.02 , bins = 51                    )
        
        # relation table for VeloCluster -> MCParticle
        table1= self.get('Rec/Relations/VeloClusters2MCParticles')
        
        # relation table for VeloCluster -> MCVeloHit
        table2 = self.get('Rec/Relations/VeloClusters2MCHits')
        
        tup = self.nTuple( title = 'VeloCluster -> MC ' )

        index = 0 
        for cluster in clusters :
            index += 1 
            
            # do here something with cluster 
            tup.column ( name = 'NC'      , value = clusters.size()   )
            tup.column ( name = 'IC'      , value = index             )
            tup.column ( name = 'ClSize'  , value = cluster.size()    )
            tup.column ( name = 'ClSens'  , value = cluster.sensor()  )
            
            # get all associated MC particles 
            mcps = table1.relations( cluster )
            
            # loop over all asscociatred MC particles and get the sum of all weights 
            w1   = 0. 
            for entry in mcps :
                mcp  = entry.to_()     # MC particle 
                w1  += entry.weight_() # "weight" associated with the link 
                
            tup.column ( name = 'nMCP'  , value = len( mcps ) )
            tup.column ( name = 'nMCPw' , value = w1          )

            self.plot (  value = len( mcps )     ,
                         title = " # of MCPs "   ,
                         high  = 10 , bins = 20  ) 
            
            if not mcps.empty() :
                minval1 = mcps[0].weight_()
                maxval1 = mcps[len(mcps)-1].weight_()
            else :
                minval1 = -1000000.0
                maxval1 = -1000000.0
        
            tup.column ( name = 'mcpMinw' , value = minval1  )
            tup.column ( name = 'mcpMaxw' , value = maxval1  )


            # get all associated MC hits 
            mchits = table2.relations( cluster )
            
            # loop over all associated MC hits and get the sum of all weights
            w2 = 0.
            for entry in mchits :
                mchit  = entry.to_()     # MC hit
                w2    += entry.weight_() # "weight" associated with the link

            tup.column ( name = 'nMCH'  , value = len( mchits)  )
            tup.column ( name = 'nMCHw' , value = w2            )
            
            self.plot (  value = len( mchits )   ,
                         title = " # of MCHs "   ,
                         high  = 10 , bins = 20  ) 
            
            if not mchits.empty() :
                minval2 = mchits[0].weight_()
                maxval2 = mchits[len(mcps)-1].weight_()
            else :
                minval2 = -1000000.0
                maxval2 = -1000000.0

            tup.column ( name = 'mchMinw' , value = minval2  )
            tup.column ( name = 'mchMaxw' , value = maxval2  )

            tup.write()
            
        return SUCCESS 
        
# =============================================================================
# Generic job configuration 
# =============================================================================
def configure() :
    
    gaudi.config( files   =
                  [ '$DAVINCIROOT/options/DaVinciCommon.opts'   ,   # common options 
                    '$DAVINCIROOT/options/DaVinciReco.opts'     ,   # general 'Reco' options
                    '$VELOASSOCIATORSROOT/options/VeloAssociators.opts' ,
                    '$DAVINCIROOT/options/DaVinciTestData.opts' ] )
     
    # add extra libraries 
    if not 'VeloDAQ'         in gaudi.DLLs   : gaudi.DLLs   += ['VeloDAQ']
    if not 'VeloAssociators' in gaudi.DLLs   : gaudi.DLLs   += ['VeloAssociators']
    
    # create my algorithm
    alg = VeloClusterMC('VeloMC')
    
    # specific job configuration
    gaudi.setAlgorithms( [ 'DecodeVeloRawBuffer'       ,
                           'VeloCluster2MCParticleAlg' ,
                           'VeloMC'                    ] ) 
    
    # output histogram file 
    hsvc = gaudi.service( 'HistogramPersistencySvc' )
    hsvc.HistogramPersistency = "HBOOK" 
    hsvc.OutputFile = 'MCVeloHist.hbook'
    
    # add the printout of the histograms
    hsvc = gaudi.service( 'HbookHistSvc' )
    hsvc.PrintHistos = True
    
    # configure the N-Tuples:
    ntsvc = gaudi.nTupleSvc()
    ntsvc.defineOutput( { 'VELO' : 'MCVelo.hbook' } , 'HBOOK' )

    alg = gaudi.algorithm('VeloMC')
    alg.NTupleLUN = "VELO" ;

    evtSel = gaudi.evtSel()
    evtSel.PrintFreq = 10
    
    return SUCCESS

# =============================================================================
# job execution 
# =============================================================================

if __name__ == '__main__' :
    import sys 
    # configure the job
    configure() 
    # run job 
    gaudi.run  ( 100 )
    
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
