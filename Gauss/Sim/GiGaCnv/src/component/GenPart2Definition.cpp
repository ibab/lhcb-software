///
/// initial coding by W.Pokorski Feb 2002, based on Particle2Definition by I.Belyaev
/// 
// ============================================================================
/// STD & STL 
#include <string>
#include <vector>
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"
/// HepMC
#include "HepMC/GenParticle.h"
/// GiGa 
#include "GiGa/GiGaException.h"
/// G4 
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
/// local
#include "GenPart2Definition.h"


// ============================================================================
/** Standard constructor
 *  @param Svc pointer to Particle Property Service 
 */ 
// ============================================================================
GenPart2Definition::GenPart2Definition( IParticlePropertySvc* Svc ) 
  : m_ppSvc( Svc )
{
  if( 0 != ppSvc() ) { ppSvc()->addRef() ; }
};

// ============================================================================
/// destructor 
// ============================================================================
GenPart2Definition::~GenPart2Definition()
{
  if( 0 != ppSvc() ){ ppSvc()->release() ; m_ppSvc = 0 ; }
};

// ============================================================================
/** copy constructor
 *  @param right copy 
 */
// ============================================================================
GenPart2Definition::GenPart2Definition( const GenPart2Definition& right )
  : m_ppSvc( right.ppSvc() )
{
  if( 0 != ppSvc() ) { ppSvc()->addRef() ; }
};

// ============================================================================
/** get the Geant4 particle definition from GenParticle object
 *  @param particle pointer to GenParticle
 *  @return pointer to Geant4 particel definition 
 */
// ============================================================================
static const std::string 
ErrMsg1("GiGaCnv::GenPart2Definition: GenParticle* points to NULL!");
static const std::string 
ErrMsg2("GiGaCnv::GenPart2Definition: IParticlePropertySvc* points to NULL!");
static const std::string 
ErrMsg3("GiGaCnv::GenPart2Definition: ParticleProperty* points to NULL!");
static const std::string 
ErrMsg4("GiGaCnv::GenPart2Definition: G4ParticleTable* points to NULL!");
static const std::string 
ErrMsg5("GiGaCnv::GenPart2Definition: G4ParticleDefinition* points to 0 for ");
// ============================================================================
G4ParticleDefinition* 
GenPart2Definition::operator() ( const HepMC::GenParticle* particle ) const
{
  if( 0 == particle ) { throw GiGaException( ErrMsg1 ) ; }
  if( 0 == ppSvc()  ) { throw GiGaException( ErrMsg2 ) ; }
  ///

  //  const long ID = particle->pdg_id();

  const int StdHepID =  particle->pdg_id();

  //    ParticleProperty* pProp = ppSvc()->find( ID ) ; 
  //    if( 0 == pProp   )  { throw GiGaException( ErrMsg3 ) ; }
  //    const int StdHepID = pProp->jetsetID() ;
  G4ParticleTable* table  = G4ParticleTable::GetParticleTable();
  if( 0 == table   )  { throw GiGaException( ErrMsg4 ) ; }
  /// find by jetset index
  G4ParticleDefinition* pDef = table->FindParticle( StdHepID ); 
  /// find by name

  //  if( 0 == pDef ) { pDef = table->FindParticle( pProp->particle() ) ;}
  if( 0 == pDef ) { throw GiGaException( ErrMsg5 );
                                         //+ pProp->particle() ); 
  }  
  
  ///
  return pDef;
};

// ============================================================================
