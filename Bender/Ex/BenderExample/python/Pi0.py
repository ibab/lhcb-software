#!/usr/bin/env python
# =============================================================================
# $Id: Pi0.py,v 1.2 2005-02-08 20:34:24 ibelyaev Exp $
# =============================================================================
# CVS tag     $Name: not supported by cvs2svn $; version $Revision: 1.2 $
# =============================================================================
# @file 
# "Demo" algorithm for pi0 -> gamma gamma reconstruction "
# =============================================================================
# @author Vanya BELYAEV belyaev@lapp.in2p3.fr
# @date   2005-02-05
# =============================================================================
"""
Simple 'demo' module to fill a proper NTuple of 'good' pi0->gamma gamma

The algorithm is 'identical' to C++/LoKi algorithm LoKi_Pi0 from
Ex/LoKiExample package, see 
$LOKIEXAMPLEROOT/src/LoKi_Pi0.cpp and
$LOKIEXAMPLEOPTS/DV_LoKi_Pi0.opts
"""
# =============================================================================
__author__ = 'Vanya BELYAEV belyaev@lapp.in2p3.fr'
# =============================================================================

# import the Bender itself  
from   bendermodule  import *
import benderrels

def goodMCgamma( gamma ) :
    """
    definitiontion of 'Good MC photon'
    """
    if not gamma                   : return False 
    if MCPT( gamma ) < 100 * MeV   : return False
    z  = 12 * meter
    ev = gamma.endVertices()
    if not ev.empty() : z = ev[0].position().z()
    if z < 10 * meter              : return False
    return True

def goodMCpi0  ( pi0 ) :
    """
    Definition of 'Good MC pi0'
    """
    if not pi0                     : return None 
    ev = pi0.endVertices()
    if 1 != ev.size()              : return None
    ev = ev[0]
    if ev.position().z() > 30 * cm : return None
    gs = ev.products()
    if 2 != gs.size()              : return None
    gamma1 = gs[0]
    if not goodMCgamma ( gamma1 )  : return None
    gamma2 = gs[1] 
    if not goodMCgamma ( gamma2 )  : return None
    return (pi0,gamma1,gamma2)

def clusterFromGamma ( gamma ) :
    """
    Trivial method to extract a Ecal Cluster from photon,
    it extratst THE FIRST hypo from  Protoparticle and
    THE FIRST cluster from hypo
    """
    if not gamma                    : return None
    pp = ppFromParticle( gamma )
    if not pp                       : return None
    calo = pp.calo()
    if calo.empty()                 : return None
    hypo = calo[0]
    if not hypo                     : return None
    clusters = hypo.clusters()
    if clusters.empty()             : return None
    return clusters[0]

def goodPair ( g1 , g2 , triplet , mc ) :
    """
    get the ordered pair of MC photons which matches given pair 
    """
    mcpi0 = triplet[0]
    mcg1  = triplet[1]
    mcg2  = triplet[2]
    #
    if not mc.match( reco = g1 , mc = mcpi0 ) : return None
    if not mc.match( reco = g2 , mc = mcpi0 ) : return None
    #
    if mc.match( reco = g1  , mc = mcg1 ) and \
       mc.match( reco = g2  , mc = mcg2 )     : return (mcg1,mcg2)
    if mc.match( reco = g2  , mc = mcg1 ) and \
       mc.match( reco = g1  , mc = mcg2 )     : return (mcg2,mcg1)
    #
    return None

def energyFromMC ( cluster , mc , table ) :
    """
    Trivial function whcih return an energy deposition from
    Monte Carlo particle to Calo Cluster
    """
    if not cluster : return None
    if not mc      : return None
    range = table.relations( cluster )
    fun   = ISMCCO( mc ) 
    for entry in range :
        if fun( entry.to_() ) : return entry.weight_()
    #
    return None
    
# =============================================================================
# Specific physics analysis algorithm 
# =============================================================================
class Pi0(Algo):
    """
    My own analysis algorithm for analysis pi0-> gamma gamma
    
    The algorithm is 'identical' to C++/LoKi algorithm LoKi_Pi0 from
    Ex/LoKiExample package, see 
    $LOKIEXAMPLEROOT/src/LoKi_Pi0.cpp and
    $LOKIEXAMPLEOPTS/DV_LoKi_Pi0.opts
    
    """
    def __init__ ( self , name = 'Pi0' ) :
        Algo.__init__ ( self , name )
        
    def analyse ( self ) :

        # select true MC pi0 which originates near the primary vertex 
        mc     = self.mctruth()
        mcpi0  = mc.find( decay = ' pi0 -> gamma gamma ' )
        mccut  = ( MCVXFUN( MCVZ ) < (  50 *  cm )  ) 

        mcpi0s = self.mcselect( tag    = 'mcpi0' ,
                               source = mcpi0   ,
                               cuts   = mccut   )

        # prepare a triplets (pi0,gamma1,gamam2) of good MC pi0
        triplets = []
        for mcpi0 in mcpi0s :
            triplet = goodMCpi0( mcpi0 ) 
            if not triplet  : continue
            triplets.append( triplet ) 

        # get relation table CaloClusters -> MCParticle
        table = self.get("Rec/Relations/EcalClusters2MCParticles")
        
        # get reconstructed pi0 photons
        photons = self.select( tag = 'gamma' , cuts = 'gamma' == ID )

        # NTupel for analyis
        tup   = self.nTuple ( title = "Pi0 Tuple" )

        # loop over gamma-gamma combinations 
        for pi0 in self.loop( formula = 'gamma gamma' , pid='pi0' ) :

            # invarinat mass of 2 photons 
            m12 = pi0.m(1,2)
            if m12 > 250 * MeV or m12 < 50 * MeV : continue

            # the first daughetr reocnstructed gamma
            g1 = pi0.particle(1)
            if PT( g1 ) < 200 * MeV : continue 
            
            # the first daughetr reocnstructed gamma
            g2 = pi0.particle(2)
            if PT( g2 ) < 200 * MeV : continue 

            # extract Ecal cluster from photon 
            cl1 = clusterFromGamma( g1 )
            if not cl1              : continue
            
            # extract Ecal cluster from photon 
            cl2 = clusterFromGamma( g2 )
            if not cl2              : continue

            self.plot( value = m12 / GeV           ,
                       id    = 1                   ,
                       title = "gamma gamma mass " ,
                       low   = 0.050                   , 
                       high  = 0.250 )
            
            # find a "right" MC pion
            good = None 
            for triplet in triplets :
                result = goodPair( g1 , g2 , triplet , mc )
                if not result : continue 
                good = (triplet[0],result[0],result[1])
                break 
                
            if good :
                # "right" MC pion is found 
                self.plot( value = m12 / GeV               ,
                           id    = 2                       ,
                           title = "gamma gamma mass (MC)" ,
                           low   = 0.050                   , 
                           high  = 0.250                   )

            # get the true MC energy conrigutions to the clusters 
            e1 = -100 * GeV 
            e2 = -100 * GeV 
            if good :
                e1  = energyFromMC( cl1 , good[1] , table )
                e2  = energyFromMC( cl2 , good[2] , table )
            if not e1 : e1 = -100 * GeV  
            if not e2 : e2 = -100 * GeV 

            # fill N-Tuple

            # invarinat mass of photon pair 
            tup.column( name = 'mass' , value =  m12       / GeV )

            # transverse momenta of reconstructed photons 
            tup.column( name = 'pt1'  , value =  PT( g1 )  / GeV ) 
            tup.column( name = 'pt2'  , value =  PT( g2 )  / GeV )
            
            # energies of reconstructed photons 
            tup.column( name = 'eg1'  , value =   E( g1 )  / GeV ) 
            tup.column( name = 'eg2'  , value =   E( g2 )  / GeV )
            
            # energies of reconstructed clusters 
            tup.column( name = 'ce1'  , value =  cl1.e()  / GeV ) 
            tup.column( name = 'ce2'  , value =  cl2.e()  / GeV )
            
            # energy depositions from MC photons to clusters  
            tup.column( name = 'de1'  , value =  e1       / GeV ) 
            tup.column( name = 'de2'  , value =  e2       / GeV )

            mce1 = -100 *  GeV
            if good : mce1 = MCE( good[1] )
            mce2 = -100 *  GeV
            if good : mce2 = MCE( good[2] )
            
            # energies of MC photons
            tup.column( name = 'me1'  , value =  mce1 / GeV ) 
            tup.column( name = 'me2'  , value =  mce2 / GeV )
            
            g = True
            if not good : g = False
            # flag to mark a pais with good matching to MC pi0 
            tup.column( name = 'good' , value = g )
            
            tup.write() 
                            
            if not good                           : continue 
            if not abs( E(g1)/mce1 - 1.0 ) < 0.10 : continue 
            if not abs( E(g2)/mce2 - 1.0 ) < 0.10 : continue 
            # "right" MC pion is found and both photons are perfect 
            self.plot( value = m12 / GeV                    ,
                       id    = 3                            ,
                       title = "gamma gamma mass (perfect)" ,
                       low   = 0.050                        , 
                       high  = 0.250                        )
            

        return SUCCESS 
        

# =============================================================================
# Generic job configuration 
# =============================================================================
def configure() :
    gaudi.config( files   =
                  [ '$BENDEREXAMPLEOPTS/BenderExample.opts'     ,   # general options 
                    '$DAVINCIROOT/options/DaVinciTestData.opts' ] ) # input data 
    
    # specific job configuration 
    gaudi.addAlgorithm ( 'LoKiPreLoad/Photons'    )
    desktop = gaudi.tool('Photons.PhysDesktop')
    desktop.ParticleMakerType = 'PhotonParticleMaker'
    maker   = gaudi.tool( 'Photons.PhysDesktop.PhotonParticleMaker' )
    maker.UseAllPhotons = False
    
    gaudi.addAlgorithm ( 'LoKiPreLoad/CnvPhotons' )
    desktop = gaudi.tool('Photons.PhysDesktop')
    desktop.ParticleMakerType = 'CnvPhotonParticleMaker'
    
    # create analysis algorithm and add it to the list of
    pi0 = Pi0('Pi0')    
    gaudi.addAlgorithm ( pi0 ) 
    
    desktop = gaudi.tool('pi0.PhysDesktop')
    desktop.InputLocations  = [ '/Event/Phys/Photons'    ,
                                '/Eveny/Phys/CnvPhotons' ]
    
    # output histogram file 
    hsvc = gaudi.service( 'HistogramPersistencySvc' )
    hsvc.HistogramPersistency = "HBOOK" 
    hsvc.OutputFile = 'pi0.hbook'
    
    # add the printout of the histograms
    hsvc = gaudi.service( 'HbookHistSvc' )
    hsvc.PrintHistos = True
    
    # switch off DaVinci memory histograms
    dv = gaudi.algorithm('DaVinci')
    dv.doHistos = False    
    
    # configure the N-Tuples:
    ntsvc = gaudi.nTupleSvc()
    ntsvc.defineOutput( { 'PI0' : 'pi0tuples.hbook' } , 'HBOOK' )

    # configure my own algorithm
    myAlg = gaudi.algorithm('Pi0')
    myAlg.NTupleLUN = 'PI0'

    evtSel = gaudi.evtSel()
    evtSel.PrintFreq = 50
    
    return SUCCESS

# =============================================================================
# job execution 
# =============================================================================

if __name__ == '__main__' :
    import sys 
    # configure the job
    configure() 
    # run job 
    gaudi.run  ( 5000 )
    # terminate the Application Manager 
    gaudi.exit ()
    
# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
