// $Id: Particle2Particle.cpp,v 1.6 2006-01-31 10:34:15 gcorti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2003/01/23 09:20:38  ibelyaev
//  few fixes for Win2K platform
//
// Revision 1.4  2002/04/24 14:50:30  ibelyaev
//  update for newer versions of Gaudi, LHCb and G4
//
// Revision 1.3  2001/08/12 17:24:55  ibelyaev
// improvements with Doxygen comments
//
// Revision 1.2  2001/07/25 17:19:32  ibelyaev
// all conversions now are moved from GiGa to GiGaCnv
//
// Revision 1.1  2001/07/24 11:13:56  ibelyaev
// package restructurization(III) and update for newer GiGa
// 
// ============================================================================
/// STD & STL 
#include <string>
#include <vector>
/// GaudiKernel
/// LHCbEvent 
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
/// GiGa 
#include "GiGa/GiGaException.h"
/// GiGaCnv 
/// G4 
#include "G4ParticleDefinition.hh"
#include "G4PrimaryParticle.hh"
// local
#include "Particle2Particle.h"

// ============================================================================
/** @file Particle2Particle.cpp
 *  
 *  Implementation file for class : Particle2Particle
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 22/07/2001 
 */
// ============================================================================

// ============================================================================
/** standard constructor
 *  @param Svc pointer to particle property service 
 */
// ============================================================================
Particle2Particle::Particle2Particle( LHCb::IParticlePropertySvc* Svc )
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
Particle2Particle::operator() 
  ( const LHCb::MCParticle* particle ) const 
{ 
  if( 0 == particle ) { throw GiGaException( ErrMsg1 ) ; }
  G4ParticleDefinition* pDef = definition( particle );
  if( 0 == pDef     ) { throw GiGaException( ErrMsg2 ) ; }   
  ///
  G4PrimaryParticle* Particle = 
    new G4PrimaryParticle( pDef                      , 
                           particle->momentum().px() ,
                           particle->momentum().py() ,
                           particle->momentum().pz() );
  
  /** for Decay vertices one CURRENTLY should follow a 
   *  little bit incorrect way
   *  NB - one loose the information about proper decay time!
   *  But it is a current property of Geant4! 
   *  This piece of code should be modified later
   */  
  typedef SmartRefVector<LHCb::MCVertex>::const_iterator   ITV;
  typedef SmartRefVector<LHCb::MCParticle>::const_iterator ITP;
  for( ITV pVertex = particle->endVertices().begin(); 
       particle->endVertices().end() != pVertex ; ++pVertex ) 
    { 
      const LHCb::MCVertex* vertex = *pVertex ; 
      if( 0 == vertex )             { continue ; } ///< constinue !
      for( ITP pParticle = vertex->products().begin(); 
           vertex->products().end() != pParticle ; ++pParticle ) 
        {
          const LHCb::MCParticle* mcp = *pParticle ;
          if( 0 == mcp )     { continue ; } ///< continue !
          /// recursion 
          G4PrimaryParticle* p = (*this)( mcp ) ; 
          if( 0 != p  ) { Particle->SetDaughter( p ); } 
        } 
    }  
  ///
  return Particle;

};

// ============================================================================
// The END 
// ============================================================================
