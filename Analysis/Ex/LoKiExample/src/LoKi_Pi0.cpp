// $Id: LoKi_Pi0.cpp,v 1.1 2005-02-08 11:23:11 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKi.h"
// ============================================================================

/** @file
 *  Implementation file for class LoKi_Pi0
 *  simple demo algorithm for pi0 -> gamma gamma analysis
 *  @date 2005-02-07 
 *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
 */

typedef std::vector<const MCParticle*>                  Triplet  ;
typedef std::vector<Triplet>                            Triplets ;
typedef Triplet                                         Pair  ;

typedef IRelationWeighted<CaloCluster,MCParticle,float> Table ;

namespace 
{

  bool goodMCgamma ( const MCParticle* gamma ) 
  {
    if ( 0 == gamma                            ) { return false ; }
    if ( LoKi::Cuts::MCPT( gamma ) < 100 * MeV ) { return false ; }
    double z = 12 * meter ;
    if ( !gamma->endVertices().empty() ) 
    { 
      const MCVertex* v = gamma->endVertices()[0] ;
      if( 0 != v  ) { z = v ->position().z() ; }
    } 
    if ( z < 10 * meter                        ) { return false ; }
    //
    return true ;
  };
  
  
  Triplet goodMCPi0
  ( const MCParticle* pi0  ) 
  {
    if ( 0 == pi0                       ) { return Triplet() ; }
    if ( 1 != pi0->endVertices().size() ) { return Triplet() ; }
    const MCVertex* ev = pi0->endVertices()[0] ;
    if ( 0 == ev                        ) { return Triplet() ; }
    if ( 2 != ev->products().size()     ) { return Triplet() ; }
    const MCParticle* gamma1 = ev->products()[0] ;
    if ( 0 == gamma1                    ) { return Triplet() ; }
    if ( !goodMCgamma ( gamma1 )        ) { return Triplet() ; }    
    const MCParticle* gamma2 = ev->products()[1] ;
    if ( 0 == gamma2                    ) { return Triplet() ; }
    if ( !goodMCgamma ( gamma2 )        ) { return Triplet() ; }    
    Triplet triplet ;
    triplet.push_back( pi0    ) ;
    triplet.push_back( gamma1 ) ;
    triplet.push_back( gamma2 ) ;
    //
    return triplet ;
  };

  const CaloCluster* clusterFromGamma
  ( const Particle* gamma ) 
  {
    if ( 0 == gamma               ) { return 0  ; }
    // get the protoparticle 
    const ProtoParticle* pp = LoKi::Extract::ppFromParticle( gamma ) ;
    if ( 0 == pp                  ) { return 0  ; }
    if ( pp->calo().empty()       ) { return 0  ; }
    // get the first hypo
    const CaloHypo* hypo  = pp->calo() [0] ;
    if ( 0 == hypo                ) { return 0  ; }
    if ( hypo->clusters().empty() ) { return 0  ; }
    // get the firts cluster 
    return hypo->clusters()[0] ;
  };

  Pair goodPair( const Particle*      gamma1  , 
                 const Particle*      gamma2  , 
                 const Triplet&       triplet ,
                 const LoKi::MCMatch& mc      )
  {
    if ( 0 == gamma1         ) { return Pair() ; }
    if ( 0 == gamma2         ) { return Pair() ; }
    if ( 3 != triplet.size() ) { return Pair() ; }
    //
    const MCParticle* mcp  = triplet[0] ;
    const MCParticle* mcg1 = triplet[1] ;
    const MCParticle* mcg2 = triplet[2] ;
    // 
    if ( ! mc->match( gamma1 , mcp ) ) { return Pair() ;}
    if ( ! mc->match( gamma2 , mcp ) ) { return Pair() ;}
    //
    if ( mc->match( gamma1 , mcg1 )  && mc->match( gamma2 , mcg2 ) ) 
    {
      Pair p ; p.push_back( mcg1 ) ; p.push_back( mcg2 ) ; return p ;
    }
    if ( mc->match( gamma1 , mcg2 )  && mc->match( gamma2 , mcg1 ) ) 
    {
      Pair p ; p.push_back( mcg2 ) ; p.push_back( mcg1 ) ; return p ;
    }
    return Pair() ;
  };
  
  double energyFromMC
  ( const CaloCluster* cluster  , 
    const MCParticle*  particle ,
    const Table*       table    ) 
  {
    if ( 0 == cluster  ) { return -1 * TeV ; }
    if ( 0 == particle ) { return -1 * TeV ; }
    if ( 0 == table    ) { return -1 * TeV ; }
    Table::Range range = table->relations( cluster ) ;
    for( Table::Range::iterator entry = range.begin() ; 
         range.end() != entry ; ++entry ) 
    { 
      const MCParticle* mcp = entry->to() ;
      if ( mcp == particle ) { return entry->weight() ; } 
    } 
    return 0 * GeV ;
  };

};


LOKI_ALGORITHM( LoKi_Pi0 ) 
{
  // avoid long names 
  using namespace LoKi       ;
  using namespace LoKi::Cuts ;

  // select true MC pi0 which originates 
  // near the primary vertex 
  MCMatch mc     = mctruth() ;
  MCRange mcpi0_ = mc->findDecays( "pi0 -> gamma gamma" );
  MCRange mcpi0s = mcselect( "mcpi0" , mcpi0_ , MCVXFUN( MCVZ ) < 50 * cm ) ;
  
  // prepare triplets (pi0,gamma1,gamma2) of good MC pi0
  Triplets triplets;
  for( MCRange::iterator imc = mcpi0s.begin() ; mcpi0s.end() != imc ; ++imc ) 
  {
    const Triplet& triplet = goodMCPi0( *imc ) ;
    if ( !triplet.empty() ) { triplets.push_back( triplet ) ; }    
  };

  // get the relatin table CaloClusters->MCParticle
  const Table* table = get<Table>("Rec/Relations/EcalClusters2MCParticles");
  
  // get all reconstucted photons 
  Range photons = select( "gamma" , "gamma" == ID ) ;

  // Book N-tuple
  Tuple tuple = nTuple("Pi0 Tuple") ;

  for ( Loop pi0 = loop( "gamma gamma" , "pi0" ) ; pi0 ; ++pi0 )
  {
    
    // invariant mass of gamma gamma pair 
    const double m12 = pi0->mass(1,2);
    
    if ( m12 > 250 * MeV    ) { continue ; }
    if ( m12 <  50 * MeV    ) { continue ; }
    
    const Particle* gamma1 = pi0(1) ;
    const Particle* gamma2 = pi0(2) ;
    
    if ( 0 == gamma1 || 0 == gamma2 ) { continue ; }
    
    if ( PT( gamma1 ) < 200 * MeV ) { continue ; }
    if ( PT( gamma2 ) < 200 * MeV ) { continue ; }
    
    const CaloCluster* cl1 = clusterFromGamma( gamma1 ) ;
    if ( 0 == cl1 ) { continue ; }
    
    const CaloCluster* cl2 = clusterFromGamma( gamma2 ) ;
    if ( 0 == cl2 ) { continue ; }

    plot( m12 / GeV , 1 , "gamma gamma mass" , 0.050 , 0.250 ) ;
    
    // find "right" MC pi0 
    Triplet good ;
    for ( Triplets::const_iterator itriplet = triplets.begin() ;
          triplets.end() != itriplet ; ++itriplet ) 
    {
      Pair p = goodPair( gamma1 , gamma2 , *itriplet , mc ) ;
      if ( p.empty() ) { continue ; }
      good.push_back( (*itriplet)[0] ) ;
      good.push_back( p[0] ) ; 
      good.push_back( p[1] ) ;
      break  ;                                               // BREAK 
    }
    
    if ( !good.empty() ) 
    { plot( m12 / GeV , 2 , "gamma gamma mass (MC)" , 0.050 , 0.250 ) ; }
    
    double e1 = -1 * TeV ;
    double e2 = -1 * TeV ;
    if ( !good.empty() ) 
    {
      e1 = energyFromMC( cl1 , good[1] , table ) ;
      e2 = energyFromMC( cl2 , good[2] , table ) ;
    };
    
    // invarinat mass of photon pair 
    tuple->column( "mass" , m12 / GeV ) ;
    
    // transverse momenta of reconstructed photons 
    tuple -> column( "pt1"  , PT ( gamma1 ) /  GeV ) ;
    tuple -> column( "pt2"  , PT ( gamma2 ) /  GeV ) ;
    
    // energies of reconstructed photons 
    tuple -> column( "eg1"  ,  E ( gamma1 ) /  GeV ) ;
    tuple -> column( "eg2"  ,  E ( gamma2 ) /  GeV ) ;

    // energies of reconstructed clusters 
    tuple -> column( "ce1"  ,  cl1->e()     /  GeV ) ;
    tuple -> column( "ce2"  ,  cl2->e()     /  GeV ) ;

    // energy depositions from MC photons to clusters  
    tuple -> column( "de1"  ,  e1           /  GeV ) ;
    tuple -> column( "de2"  ,  e2           /  GeV ) ;

    double mce1 = -1 * TeV ;
    if ( !good.empty() ) { mce1 = MCE( good[1] ) ; }
    double mce2 = -1 * TeV ;
    if ( !good.empty() ) { mce2 = MCE( good[2] ) ; }
    
    // energies of MC Photons  
    tuple -> column( "me1"  ,  mce1         /  GeV ) ;
    tuple -> column( "me2"  ,  mce2         /  GeV ) ;
    
    // flag for good matching
    tuple -> column( "good" , !good.empty() ) ;
    
    tuple -> write() ;
    
    if ( good.empty()                            ) { continue ; }
    if ( fabs( E( gamma1 ) / mce1 - 1.0 ) > 0.10 ) { continue ; }
    if ( fabs( E( gamma2 ) / mce2 - 1.0 ) > 0.10 ) { continue ; }
    
    plot( m12 / GeV , 3 , "gamma gamma mass (perfect)" , 0.050 , 0.250 ) ; 
    
  }; // end of the loop over pi0 
  
  return StatusCode::SUCCESS ;

}; 



// ============================================================================
// The END 
// ============================================================================
