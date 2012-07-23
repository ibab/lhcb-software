// $Id: Vertex2Vertex.cpp,v 1.5 2006-01-31 10:34:15 gcorti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2003/01/23 09:20:38  ibelyaev
//  few fixes for Win2K platform
//
// Revision 1.3  2002/04/24 14:50:31  ibelyaev
//  update for newer versions of Gaudi, LHCb and G4
//
// Revision 1.2  2001/08/12 17:24:55  ibelyaev
// improvements with Doxygen comments
//
// Revision 1.1  2001/07/24 11:13:57  ibelyaev
// package restructurization(III) and update for newer GiGa
// 
// ============================================================================
/// Include files
/// STD & STL
#include <string>
/// LHCbEvent 
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
/// GiGa
#include "GiGa/GiGaException.h"
/// G4 
#include "G4PrimaryVertex.hh"
// local
#include "Vertex2Vertex.h"

/** @file Vertex2Vertex.cpp
 *  
 *  Implementation file for class : Vertex2Vertex
 *  
 *  @author Vanya  Belyaev Ivan.Belyaev@itep.ru
 *  @date 22/07/2001 
 */

// ============================================================================
/** standard constructor
 *  @param Svc pointer to Particle Property Service 
 */
// ============================================================================
Vertex2Vertex::Vertex2Vertex( LHCb::IParticlePropertySvc* Svc ) 
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
ErrMsg1("GiGaCnv::Vertex2Vertex: LHCb::MCVertex* points to NULL!");

// ============================================================================
G4PrimaryVertex* Vertex2Vertex::operator() ( const LHCb::MCVertex* vertex ) const
{
  if( 0 == vertex ) { throw GiGaException( ErrMsg1 ); }
  /// skip empty vertices 
  if( vertex->products().empty() ) { return 0; } 
  G4PrimaryVertex* Vertex = 
    new G4PrimaryVertex( vertex->position().x() ,
                         vertex->position().y() ,
                         vertex->position().z() ,
                         vertex->time() ) ;
  ///
  typedef SmartRefVector<LHCb::MCParticle>::const_iterator IT;
  for( IT pParticle = vertex->products().begin() ;
       vertex->products().end() != pParticle ; ++pParticle )
    {
      const LHCb::MCParticle* mcp = *pParticle ;
      if( 0 == mcp  ) { continue ; }              ///< CONTINUE!
      G4PrimaryParticle* Particle = particle( mcp );
      if( 0 != Particle   ) { Vertex->SetPrimary( Particle ); }
    }
  /// remove empty vertices 
  if( 0 == Vertex->GetNumberOfParticle() ) { delete Vertex ; Vertex = 0 ; }
  ///
  return Vertex ;
};

// ============================================================================
// The END 
// ============================================================================
