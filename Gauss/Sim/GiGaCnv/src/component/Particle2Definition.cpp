// $Id: Particle2Definition.cpp,v 1.5 2006-01-31 10:34:15 gcorti Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2002/12/04 16:25:19  ibelyaev
//  remove extra calls for 'addRef'
//
// Revision 1.3  2002/04/24 14:50:30  ibelyaev
//  update for newer versions of Gaudi, LHCb and G4
//
// Revision 1.2  2001/08/12 17:24:54  ibelyaev
// improvements with Doxygen comments
//
// Revision 1.1  2001/07/24 11:13:56  ibelyaev
// package restructurization(III) and update for newer GiGa
// 
// ============================================================================
/// STD & STL 
#include <string>
#include <vector>
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
/// LHCb 
#include "Kernel/ParticleProperty.h"
#include "Kernel/IParticlePropertySvc.h"
/// LHCbEvent 
#include "Event/MCParticle.h"
/// GiGa 
#include "GiGa/GiGaException.h"
/// G4 
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
/// local
#include "Particle2Definition.h"

/** @file Particle2Definition
 *  
 *  Implementation file for class : Particle2Definition
 *   
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 * @date 22/07/2001 
 */

// ============================================================================
/** Standard constructor
 *  @param Svc pointer to Particle Property Service 
 */ 
// ============================================================================
Particle2Definition::Particle2Definition( LHCb::IParticlePropertySvc* Svc ) 
  : m_ppSvc( Svc )
{ if( 0 != ppSvc() ) { ppSvc()->addRef() ; } };
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
Particle2Definition::~Particle2Definition()
{ if( 0 != ppSvc() ){ ppSvc()->release() ; m_ppSvc = 0 ; } };
// ============================================================================

// ============================================================================
/** copy constructor
 *  @param right copy 
 */
// ============================================================================
Particle2Definition::Particle2Definition( const Particle2Definition& right )
  : m_ppSvc( right.ppSvc() )
{ if( 0 != ppSvc() ) { ppSvc()->addRef() ; } };
// ============================================================================

// ============================================================================
/** get the Geant4 particle definition from MCParticle object
 *  @param particle pointer to MCParticle
 *  @return pointer to Geant4 particel definition 
 */
// ============================================================================
static const std::string 
ErrMsg1("GiGaCnv::Particle2Definition: MCParticle* points to NULL!");
static const std::string 
ErrMsg2("GiGaCnv::Particle2Definition: IParticlePropertySvc* points to NULL!");
static const std::string 
ErrMsg3("GiGaCnv::Particle2Definition: ParticleProperty* points to NULL!");
static const std::string 
ErrMsg4("GiGaCnv::Particle2Definition: G4ParticleTable* points to NULL!");
static const std::string 
ErrMsg5("GiGaCnv::Particle2Definition: G4ParticleDefinition* points to 0 for ");
// ============================================================================
G4ParticleDefinition* 
Particle2Definition::operator() 
  ( const LHCb::MCParticle* particle ) const
{
  if( 0 == particle ) { throw GiGaException( ErrMsg1 ) ; }
  if( 0 == ppSvc()  ) { throw GiGaException( ErrMsg2 ) ; }
  //
  const long StdHepID = particle->particleID().pid();
  // Particle table 
  G4ParticleTable* table  = G4ParticleTable::GetParticleTable();
  if( 0 == table   )  { throw GiGaException( ErrMsg4 ) ; }
  // find by jetset index
  G4ParticleDefinition* pDef = table->FindParticle( StdHepID ); 
  // find by name 
  if( 0 == pDef ) 
    { 
      const LHCb::ParticleProperty* pProp = ppSvc()->find( LHCb::ParticleID(StdHepID) ) ;
      if( 0 == pProp   )  { throw GiGaException( ErrMsg3 ) ; }
      pDef = table->FindParticle( pProp->particle() ) ;
      if( 0 == pDef ) { throw GiGaException( ErrMsg5 + pProp->particle() ); }
    }
  ///
  return pDef;
};

// ============================================================================
// The End 
// ============================================================================
