///
/// initial coding by W.Pokorski Feb 2002, based on Particle2Particle by I. Belyaev
/// 
// ============================================================================
/// STD & STL 
#include <string>
#include <vector>
/// GaudiKernel
/// HepMC 
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
/// GiGa 
#include "GiGa/GiGaException.h"
/// GiGaCnv 
/// G4 
#include "G4ParticleDefinition.hh"
#include "G4PrimaryParticle.hh"
// local
#include "GenPart2GenPart.h"

// ============================================================================
/** standard constructor
 *  @param Svc pointer to particle property service 
 */
// ============================================================================
GenPart2GenPart::GenPart2GenPart( IParticlePropertySvc* Svc )
  : m_p2d( Svc )
{};

// ============================================================================
/// destructor 
// ============================================================================
GenPart2GenPart::~GenPart2GenPart(){};


// ============================================================================
static const std::string 
ErrMsg1("GiGaCnv::GenPart2GenPart: GenParticle* points to NULL!");
static const std::string 
ErrMsg2("GiGaCnv::GenPart2GenPart: G4ParticleDefinition* points to NULL!");


// ============================================================================
G4PrimaryParticle* 
GenPart2GenPart::operator() ( const HepMC::GenParticle* particle ) const 
{ 
  if( 0 == particle ) { throw GiGaException( ErrMsg1 ) ; }
  G4ParticleDefinition* pDef = definition( particle );
  if( 0 == pDef     ) { throw GiGaException( ErrMsg2 ) ; }   
  ///
  G4PrimaryParticle* Particle = 
    new G4PrimaryParticle( pDef , 
                           particle->momentum().px()*GeV ,
                           particle->momentum().py()*GeV ,
                           particle->momentum().pz()*GeV );
  
  ///
  return Particle;

};

// ============================================================================
