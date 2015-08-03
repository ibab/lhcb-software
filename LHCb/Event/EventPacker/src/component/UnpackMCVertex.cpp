// $Id: UnpackMCVertex.cpp,v 1.5 2009-11-07 12:20:39 jonrob Exp $
// Include files 

#include "Event/MCVertex.h"
#include "Event/StandardPacker.h"
#include "Event/PackedMCVertex.h"
// local
#include "UnpackMCVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : UnpackMCVertex
//
// 2005-03-18 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( UnpackMCVertex )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
UnpackMCVertex::UnpackMCVertex( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::PackedMCVertexLocation::Default );
  declareProperty( "OutputName", m_outputName = LHCb::MCVertexLocation::Default );
  declareProperty( "AlwaysCreateOutput",         m_alwaysOutput = false     );
  //setProperty( "OutputLevel", 1 );
}
//=============================================================================
// Destructor
//=============================================================================
UnpackMCVertex::~UnpackMCVertex() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode UnpackMCVertex::execute()
{

  // CRJ : If packed data does not exist just return (by default). Needed for packing of 
  //     : spillover which is not neccessarily available for each event
  if ( !m_alwaysOutput && !exist<LHCb::PackedMCVertices>(m_inputName) ) return StatusCode::SUCCESS;

  const LHCb::PackedMCVertices* dst = 
    getOrCreate<LHCb::PackedMCVertices,LHCb::PackedMCVertices>( m_inputName );

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Size of PackedMCVertices = " << dst->mcVerts().size() << endmsg;

  LHCb::MCVertices* newMCVertices = new LHCb::MCVertices();
  put( newMCVertices, m_outputName );

  StandardPacker pack(this);

  // Packing version
  const char pVer = dst->packingVersion();

  newMCVertices->reserve( dst->mcVerts().size() );
  for ( const LHCb::PackedMCVertex& src : dst->mcVerts() )
  {

    LHCb::MCVertex* vert = new LHCb::MCVertex( );
    newMCVertices->insert( vert, src.key );
    vert->setPosition( Gaudi::XYZPoint( pack.position( src.x ), 
                                        pack.position( src.y ), 
                                        pack.position( src.z ) ) );
    vert->setTime( src.tof );
    vert->setType( (LHCb::MCVertex::MCVertexType) src.type );

    int hintID;
    int key;
    if ( -1 != src.mother )
    {
      if ( ( 0==pVer && pack.hintAndKey32( src.mother, dst, newMCVertices, hintID, key ) ) ||
           ( 0!=pVer && pack.hintAndKey64( src.mother, dst, newMCVertices, hintID, key ) ) )
      {
        SmartRef<LHCb::MCParticle> ref( newMCVertices, hintID, key );
        vert->setMother( ref );
      }
      else { Error( "Corrupt MCVertex Mother MCParticle SmartRef detected" ).ignore(); }
    }
    
    for ( const auto& I : src.products )
    {
      if ( ( 0==pVer && pack.hintAndKey32( I, dst, newMCVertices, hintID, key ) ) ||
           ( 0!=pVer && pack.hintAndKey64( I, dst, newMCVertices, hintID, key ) ) )
      {
        SmartRef<LHCb::MCParticle> ref( newMCVertices, hintID, key );
        vert->addToProducts( ref );
      }
      else { Error( "Corrupt MCVertex Daughter MCParticle SmartRef detected" ).ignore(); }
    }
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
