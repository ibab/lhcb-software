// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.1  2001/07/24 11:13:57  ibelyaev
/// package restructurization(III) and update for newer GiGa
/// 
// ============================================================================
/// Include files
/// STD & STL
#include <string>
/// LHCbEvent 
#include "LHCbEvent/MCParticle.h"
#include "LHCbEvent/MCVertex.h"
/// GiGa
#include "GiGa/GiGaException.h"
/// G4 
#include "G4PrimaryVertex.hh"
// local
#include "Vertex2Vertex.h"

/** Implementation file for class : Vertex2Vertex
 *  
 *  @author Vanya  Belyaev
 *  @date 22/07/2001 
 */

// ============================================================================
/** standard constructor
 *  @param Svc pointer to Particle Property Service 
 */
// ============================================================================
Vertex2Vertex::Vertex2Vertex( IParticlePropertySvc* Svc ) 
  : m_p2p ( Svc )
{};

// ============================================================================
/// destructor 
// ============================================================================
Vertex2Vertex::~Vertex2Vertex(){};

// ============================================================================
/** perform the conversion of  MCVertex object into 
 *  G4PrimaryVertex object
 *  @param  vertex pointer to MCVertex object 
 *  @return pointer to converted G4PrimaryVertex object
 */
// ============================================================================
static const std::string 
ErrMsg1("GiGaCnv::Vertex2Vertex: MCVertex* points to NULL!");

// ============================================================================
G4PrimaryVertex* Vertex2Vertex::operator() ( const MCVertex* vertex ) const
{
  if( 0 == vertex ) { throw GiGaException( ErrMsg1 ); }
  /// skip empty vertices 
  if( vertex->daughterMCParticles().empty() ) { return 0; } 
  G4PrimaryVertex* Vertex = 
    new G4PrimaryVertex( vertex->position().x() ,
                         vertex->position().y() ,
                         vertex->position().z() ,
                         vertex->timeOfFlight() ) ;
  ///
  typedef SmartRefVector<MCParticle>::const_iterator IT;
  for( IT pParticle = vertex->daughterMCParticles().begin() ;
       vertex->daughterMCParticles().end() != pParticle ; ++pParticle )
    {
      if( 0 == *pParticle ) { continue ; }              ///< CONTINUE!
      G4PrimaryParticle* Particle = particle( *pParticle );
      if( 0 != Particle   ) { Vertex->SetPrimary( Particle ); }
    }
  /// remove empty vertices 
  if( 0 == Vertex->GetNumberOfParticle() ) { delete Vertex ; Vertex = 0 ; }
  ///
  return Vertex ;
};

// ============================================================================
