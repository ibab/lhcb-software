// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.2  2001/07/25 17:19:32  ibelyaev
/// all conversions now are moved from GiGa to GiGaCnv
///
/// Revision 1.1  2001/07/24 11:13:56  ibelyaev
/// package restructurization(III) and update for newer GiGa
/// 
// ============================================================================
/// STD & STL 
#include <string>
#include <vector>
/// GaudiKernel
/// LHCbEvent 
#include "LHCbEvent/MCParticle.h"
#include "LHCbEvent/MCVertex.h"
/// GiGa 
#include "GiGa/GiGaException.h"
/// GiGaCnv 
/// G4 
#include "G4ParticleDefinition.hh"
#include "G4PrimaryParticle.hh"
// local
#include "Particle2Particle.h"

// ============================================================================
/** Implementation file for class : Particle2Particle
 * 
 *  @author Vanya Belyaev
 *  @date 22/07/2001 
 */
// ============================================================================

// ============================================================================
/** standard constructor
 *  @param Svc pointer to particle property service 
 */
// ============================================================================
Particle2Particle::Particle2Particle( IParticlePropertySvc* Svc )
  : m_p2d( Svc )
{};

// ============================================================================
/// destructor 
// ============================================================================
Particle2Particle::~Particle2Particle(){};

// ============================================================================
/** convert MCParticle object into G4Primary particle object
 *  The method is recursive!
 *
 *  For Decay vertices one CURRENTLY should follow a 
 *  little bit incorrect way
 *  NB - one loose the information about proper decay time!
 *  But it is a current property of Geant4! 
 *  This piece of code should be modified later
 *
 *  @param particle pointer to MCParticle object
 *  @return pointer to new G4Primary particle
 */
// ============================================================================
static const std::string 
ErrMsg1("GiGaCnv::Particle2Particle: MCParticle* points to NULL!");
static const std::string 
ErrMsg2("GiGaCnv::Particle2Particle: G4ParticleDefinition* points to NULL!");


// ============================================================================
G4PrimaryParticle* 
Particle2Particle::operator() ( const MCParticle* particle ) const 
{ 
  if( 0 == particle ) { throw GiGaException( ErrMsg1 ) ; }
  G4ParticleDefinition* pDef = definition( particle );
  if( 0 == pDef     ) { throw GiGaException( ErrMsg2 ) ; }   
  ///
  G4PrimaryParticle* Particle = 
    new G4PrimaryParticle( pDef , 
                           particle->fourMomentum().px() ,
                           particle->fourMomentum().py() ,
                           particle->fourMomentum().pz() );
  
  // for Decay vertices one CURRENTLY should follow a 
  // little bit incorrect way
  // NB - one loose the information about proper decay time!
  // But it is a current property of Geant4! 
  // This piece of code should be modified later
  //
  
  typedef SmartRefVector<MCVertex>::const_iterator   ITV;
  typedef SmartRefVector<MCParticle>::const_iterator ITP;
  for( ITV pVertex = particle->decayMCVertices().begin(); 
       particle->decayMCVertices().end() != pVertex ; ++pVertex ) 
    { 
      const MCVertex* vertex = *pVertex ; 
      if( 0 == vertex )             { continue ; } ///< constinue !
      for( ITP pParticle = vertex->daughterMCParticles().begin(); 
           vertex->daughterMCParticles().end() != pParticle ; ++pParticle ) 
        {
          if( 0 == *pParticle )     { continue ; } ///< continue !
          /// recursion 
          G4PrimaryParticle* p = (*this)( *pParticle ) ; 
          if( 0 != p ) { Particle->SetDaughter( p ); } 
        } 
    }  
  ///
  return Particle;

};

// ============================================================================
