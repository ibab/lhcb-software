/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
// Include files
/// STD and STL 
#include <string>
#include <vector>
/// GaudiKernel
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ParticleProperty.h"
/// LHCbEvent 
#include "LHCbEvent/MCParticle.h"
/// GiGa 
#include "GiGa/GiGaTrajectory.h"
#include "GiGa/GiGaException.h"
/// G4 
#include "G4ParticleDefinition.hh"
// local
#include "Trajectory2Particle.h"

/** Implementation file for class : Trajectory2Particle
 * 
 *  @author Vanya Belyaev
 *  @date   22/07/2001 
 */

/// ===========================================================================
/** standard constructor
 *  @param  Svc   pointer to Particle Property Service 
 */
/// ===========================================================================
Trajectory2Particle::Trajectory2Particle( IParticlePropertySvc* Svc )
  : m_ppSvc ( Svc   )
{
  if( 0 != ppSvc() ) { ppSvc()->addRef() ; }
};

/// ===========================================================================
/// destructor 
/// ===========================================================================
Trajectory2Particle::~Trajectory2Particle()
{
  if( 0 != ppSvc() ) { ppSvc()->release(); m_ppSvc = 0 ;}
};

/// ===========================================================================
/** copy constructor 
 *  @param right const reference to object 
 */
/// ===========================================================================
Trajectory2Particle::
Trajectory2Particle( const Trajectory2Particle& right )
  : m_ppSvc( right.ppSvc() )
{
  if( 0 != ppSvc() ) { ppSvc()->addRef() ; }
};

/// ===========================================================================
/** perform the conversion of G4VTrajectory object to 
 *  MCParticle object. 
 *  @param trajectory pointer to G4VTrajectory object 
 *  @return pointer to converted MCParticle object
 */
/// ===========================================================================
static const std::string 
ErrMsg1("GiGaCnv::Trajectory2Particle::IParticlePropertySvc* points to NULL!");
static const std::string 
ErrMsg2("GiGaCnv::Trajectory2Particle::GiGaTrajectory* points to NULL!");
static const std::string 
ErrMsg3("GiGaCnv::Trajectory2Particle::G4ParticleDefinition* points to NULL!");
static const std::string 
ErrMsg4("GiGaCnv::Trajectory2Particle::ParticleProperty* points to NULL for ");
/// ===========================================================================
MCParticle* 
Trajectory2Particle::operator() ( const GiGaTrajectory* trajectory ) const
{
  if( 0 == ppSvc ()   ) { throw GiGaException( ErrMsg1 ) ; }
  if( 0 == trajectory ) { throw GiGaException( ErrMsg2 ) ; }
  
  const G4ParticleDefinition* pDef = trajectory->partDef() ;
  if( 0 == pDef       ) { throw GiGaException( ErrMsg3 );}
  
  ParticleProperty* pP = ppSvc()->findByStdHepID( pDef->GetPDGEncoding() );
  if( 0 == pP ) { pP = ppSvc()->find( pDef->GetParticleName() ) ;}
  if( 0 == pP ) { throw GiGaException( ErrMsg4 + pDef->GetParticleName() ); }
  
  /// create and fill new MCparticle object 
  MCParticle* particle = new MCParticle();
  particle->setFourMomentum( trajectory->fourMomentum() );
  particle->setParticleID( ParticleID( pP->geantID() ) );
  ///
  return particle;
  ///
};

/// ===========================================================================
