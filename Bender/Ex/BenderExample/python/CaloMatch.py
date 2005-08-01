#!/usr/bin/env python
# =============================================================================
# $Id: CaloMatch.py,v 1.2 2005-08-01 09:50:19 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.2 $
# =============================================================================
# @file
# Simple script to test the CaloCLuster -> TrStoredTrack matching
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2003-12-04 
# =============================================================================

# import the Bender itself  
from   bendermodule import *

# later it will beincludes into bendermodule
import benderrels
import math

# =============================================================================
# Specific physics analysis algorithm 
# =============================================================================

# create my own algorithm  
class CaloMatch(Algo):
    """
    Simple algorithm to test relations
    """
    def __init__ ( self , name = 'CaloMatch' ) : Algo.__init__ ( self , name )
        
    def analyse ( self ) :

        nev = self.get('/Event/Header').evtNum()
        
        clusters = self.get('Rec/Calo/EcalClusters')
        self.plot( value = clusters.size()       ,
                   title = '# of Ecal clusters'  ,
                   low   = 0 , high = 100        )
        
        table = self.get('Rec/Calo/PhotonMatch')

        for cluster in clusters :

            #get all relations from the given Cluster 
            range = table.relations( cluster )
            
            self.plot( value = range.size()            ,
                       title = 'Multiplicity of links' ,
                       low   = 0 , high = 50 )
            
            # loop over all CaloCluster -> TrStoredTrack relations 
            for entry in range :
                 
                track = entry.to_()        # TrStoredTrack
                chi2  = entry.weight_()    # chi2 of "Photon Matching"
                
                # plot all weight 
                self.plot ( value = math.tanh( chi2 / 50 )      ,
                            title = 'tanh ( chi2 / 50 ) '       ,
                            low   = 0 , high = 1.02 , bins = 51  )
                
                
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
    alg       = CaloMatch()
    gaudi.setAlgorithms( [ alg ] )
    
    # output histogram file 
    hsvc = gaudi.histoSvc()
    hsvc.setOutput( 'CaloMatch.hbook' , 'HBOOK' )
    
    # add the printout of the histograms
    hsvc = gaudi.service( 'HbookHistSvc' )
    hsvc.PrintHistos = True

    # decrease the printout drequence 
    evtSel = gaudi.evtSel()
    evtSel.PrintFreq = 50
    
    # switch off native DaVinci histograms
    dv = gaudi.algorithm('DaVinci')
    dv.HistoProduce = False
    
    return SUCCESS 

# =============================================================================
# job execution 
# =============================================================================
if __name__ == '__main__' :
    import sys 
    # configure the job 
    configure()

    # execute 
    gaudi.run( 100)
    
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
