#!/usr/bin/env python
# =============================================================================
# $Id: CaloClusterMCTruth.py,v 1.1 2005-02-05 13:47:40 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.1 $
# =============================================================================
# @file
# Simple script to test the CaloCLuster -> MCParticle relations 
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2003-12-04 
# =============================================================================
"""
Simple module to test CaloCluster -> MCParticle relations
"""
# =============================================================================
__author__ = 'Vanya BELYAEV belyaev@lapp.in2pr.fr'
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
class CaloClusterMCTruth(Algo):
    """
    Simple algorithm to test CaloCLuster -> MCParticle relations.
    The Algorithm performs an extensibe debug printout of the relation tables
    for some events and fills few reference histograms
    """
        
    def analyse ( self ) :

        nev = self.get('/Event/Header').evtNum()
        
        clusters = self.get('Rec/Calo/EcalClusters')
        self.plot( value = clusters.size()       ,
                   title = '# of Ecal clusters'  ,
                   low   = 0 , high = 100        )
        
        table = self.get("Rec/Relations/EcalClusters2MCParticles")

        # z-coordinate of MCParticle production vertex [cm]
        vz = MCVXFUN( MCVZ ) / cm

        makePrintOut = nev < 5 or 0 == nev % 50
        
        for cluster in clusters :

            #get all relations from the given Cluster 
            range = table.relations( cluster )
            
            self.plot( value = range.size()            ,
                       title = 'Multiplicity of links' ,
                       low   = 0 , high = 50 )
            
            if makePrintOut :
                print ( 'start loop over MC relations (#=%3s) ' + \
                        'for cluster with energy %7.3f [GeV]  ' ) \
                        % ( range.size() , cluster.e() / GeV )
                
            if range.empty() : continue

            # plot minimal and maximal weights 
            self.plot ( value = math.tanh( range.front().weight_() /    GeV ) ,
                        title = 'tanh ( Minimal weight /    GeV ) '           ,
                        low   = 0 , high = 1.02  , bins = 51                 )
            self.plot ( value = math.tanh( range.back().weight_()  / ( 10 * GeV ) ) ,
                        title = 'tanh ( Maximal weight / 10 GeV ) '           ,
                        low   = 0 , high = 1.02  , bins = 51                 )
            
            # loop over all MC relations 
            for entry in range :
                 
                mcp = entry.to_()             # MC particle 
                ene = entry.weight_() / GeV   # energy contribution to CaloCluster
                
                # plot all weight 
                self.plot ( value = math.tanh( ene / 5 )          ,
                            title = 'tanh ( weight / 5 GeV ) '    ,
                            low   = 0 , high = 1.02 , bins = 51   )

                if not makePrintOut : continue
                
                name = nameFromPID( mcp.particleID() )
                print ( " name=%10s ; vz=%7.3f [cm]; " + \
                        " contribution = %7.3f [GeV] " ) \
                        % ( name , vz( mcp )  , ene )
                
                
        return SUCCESS 

# =============================================================================
# job configuration 
# =============================================================================
def configure () :    
    # Generic job configuration     
    gaudi.config( files   =
                  [ '$BENDEREXAMPLEOPTS/BenderExample.opts'     ,   # general options 
                    '$DAVINCIROOT/options/DaVinciTestData.opts' ] ) # input data 

    # specific job configuration 
    # create analysis algorithm and add it to the list of
    alg       = CaloClusterMCTruth('CC2MCEx')
    gaudi.setAlgorithms( [ alg ] )
    
    # output histogram file 
    hsvc = gaudi.histoSvc()
    hsvc.setOutput( 'CaloClusterMCTruth.hbook' , 'HBOOK' )
    
    # add the printout of the histograms
    hsvc = gaudi.service( 'HbookHistSvc' )
    hsvc.PrintHistos = True

    # decrease the printout drequence 
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

    # execute 
    gaudi.run( 1000 )
    
    # terminate 
    gaudi.exit()

# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
