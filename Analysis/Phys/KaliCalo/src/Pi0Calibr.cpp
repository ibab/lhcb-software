// $Id: Pi0Calibr.cpp,v 1.1.1.1 2005-05-31 13:03:31 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revison:$
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files 
// ============================================================================
// Event 
// ============================================================================
#include "Event/CaloHypo.h"
#include "Event/CaloCluster.h"
#include "Event/CaloDigit.h"
// ============================================================================
#include "Event/CaloDataFunctor.h"
// ============================================================================
#include "Event/ProtoParticle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKi.h"
// ============================================================================

  
  template<>
  inline Tuples::Tuple& operator<<
    ( Tuples::Tuple&                              tuple , 
      const Tuples::TupleColumn<CaloCellID>&      item ) 
  {
    if ( !tuple.valid() ) { return tuple ; } // no action 
    //
    const std::string& name   = item.name  () ;
    const CaloCellID&  cellID = item.value () ;
    //
    tuple -> column ( name + "a"  , cellID.area () , 0 ,   7 ) ;
    tuple -> column ( name + "r"  , cellID.row  () , 0 , 127 ) ;  
    tuple -> column ( name + "c"  , cellID.col  () , 0 , 127 ) ;  
    //
    return tuple ;
  } ;

/// anonymous namespace to hide local functions 
namespace 
{
  inline const ProtoParticle* protoP ( const Particle* gamma ) 
  {
    if ( 0 == gamma ) { return 0 ; }
    const ContainedObject* origin = gamma->origin() ;
    return dynamic_cast<const ProtoParticle*>( origin ) ;
  };
  inline const CaloHypo*  hypo( const Particle* gamma ) 
  {
    if ( 0 == gamma         ) { return 0 ; }
    const ProtoParticle* pp = protoP( gamma ) ;
    if ( 0 == pp            ) { return 0 ; }
    if ( pp->calo().empty() ) { return 0 ; }
    return (*(pp->calo().begin())) ;
  };
  inline const CaloCluster* cluster ( const Particle* gamma ) 
  {
    if ( 0 == gamma            ) { return 0 ; }
    const CaloHypo* h = hypo( gamma ) ;
    if ( 0 == h                ) { return 0 ; }
    if ( h->clusters().empty() ) { return 0 ; }
    return (*(h->clusters().begin()));
  };
  inline CaloCellID cellID( const Particle* gamma ) 
  {
    if ( 0 == gamma  ) { return CaloCellID() ; }
    const CaloCluster* cl = cluster( gamma ) ;
    if ( 0 == cl     ) { return CaloCellID() ; }
    return cl->seed() ;  
  };
  
  template <class FROM> 
  inline double energyFrom 
  ( const Particle* gamma , 
    const FROM&     from  ) 
  {
    if ( 0 == gamma ) { return -1 * TeV ; }
    const CaloHypo* h = hypo( gamma ) ;
    if ( 0 == h     ) { return -1 * TeV ; }
    double energy = 0 ;
    typedef CaloHypo::Digits Digits ;
    const Digits& digits = h->digits() ;
    for ( Digits::const_iterator idigit = digits.begin() ; 
          digits.end() != idigit ; ++idigit ) 
    {
      const CaloDigit* digit = *idigit ;
      if ( 0 == digit    ) { continue ; }
      if ( from( digit ) ) { energy += digit->e() ; }
    }
    return energy ;
  }
  
};


LOKI_ALGORITHM ( LoKi_Pi0Calibr ) 
{
  using namespace LoKi       ;
  using namespace LoKi::Cuts ;
  
  CaloDataFunctor::DigitFromCalo spd ( "Spd" ) ;
  CaloDataFunctor::DigitFromCalo prs ( "Prs" ) ;
  
  // get all photons with 
  Range gamma = select ( "g" , ( "gamma" == ID ) && ( PT > 300 * MeV ) ) ;
  
  Tuple tuple = nTuple ( " PI0 tuple " ) ;
  
  for ( Loop pi0 = loop( "g g" , "pi0" ) ; pi0 ; ++pi0 ) 
  {
    const double m12 = pi0->mass ( 1 , 2 ) ;
    if ( m12 > 200 * MeV ) { continue ; }  // CONTINUE
    
    const Particle* g1 = pi0(1) ;
    if ( 0 == g1         ) { continue ; }  // CONTINUE 
    
    const Particle* g2 = pi0(2) ;
    if ( 0 == g2         ) { continue ; }  // CONTINUE
    
    LoKi::LorentzVector p12 = pi0->p(1,2) ;
    
    tuple << Tuples::Column ( "m12"  , m12             / GeV ) ;
    
    tuple << Tuples::Column ( "p0"   , p12             / GeV ) ;
    tuple << Tuples::Column ( "g1"   , g1->momentum()  / GeV ) ;
    tuple << Tuples::Column ( "g2"   , g2->momentum()  / GeV ) ;
    
    tuple << Tuples::Column ( "pt"   , p12.perp()      / GeV ) ;
    tuple << Tuples::Column ( "pt1"  , PT ( g1 )       / GeV ) ;
    tuple << Tuples::Column ( "pt2"  , PT ( g2 )       / GeV ) ;
    
    tuple << Tuples::Column ( "spd1" , energyFrom ( g1 , spd ) / GeV ) ;
    tuple << Tuples::Column ( "prs1" , energyFrom ( g1 , prs ) / GeV ) ;
    tuple << Tuples::Column ( "spd2" , energyFrom ( g2 , spd ) / GeV ) ;
    tuple << Tuples::Column ( "prs2" , energyFrom ( g2 , prs ) / GeV ) ;

    const CaloCellID cel1 = cellID( g1 ) ;
    const CaloCellID cel2 = cellID( g2 ) ;
    
    tuple << Tuples::Column ( "cel1" , cel1 ) ;
    tuple << Tuples::Column ( "cel2" , cel2 ) ;
    
    tuple << Tuples::Column ( "ind1" , cel1.index()    ) ;
    tuple << Tuples::Column ( "ind2" , cel2.index()    ) ;
    
    tuple << Tuples::Column ( "cat1" , cel1.index()%15 ) ;
    tuple << Tuples::Column ( "cat2" , cel2.index()%15 ) ;
    
    tuple -> write () ;

  }
  
  return StatusCode::SUCCESS ;
};
// ============================================================================
// The END 
// ============================================================================
