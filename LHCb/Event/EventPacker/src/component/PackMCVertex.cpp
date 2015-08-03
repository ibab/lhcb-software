// $Id: PackMCVertex.cpp,v 1.7 2009-11-26 10:50:49 cattanem Exp $
// Include files

// from Boost
#include "boost/numeric/conversion/bounds.hpp"

#include "Event/MCVertex.h"
#include "Event/StandardPacker.h"
#include "Event/PackedMCVertex.h"
// local
#include "PackMCVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PackMCVertex
//
// 2005-03-18 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PackMCVertex )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  PackMCVertex::PackMCVertex( const std::string& name,
                              ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::MCVertexLocation::Default );
  declareProperty( "OutputName", m_outputName = LHCb::PackedMCVertexLocation::Default );
  declareProperty( "AlwaysCreateOutput",         m_alwaysOutput = false     );
  declareProperty( "DeleteInput",                m_deleteInput  = false     );
}

//=============================================================================
// Destructor
//=============================================================================
PackMCVertex::~PackMCVertex() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PackMCVertex::execute()
{

  // Check to see if the output data already exists.
  if( exist<LHCb::PackedMCVertices>(m_outputName) ) {
    if (msgLevel(MSG::DEBUG) )
      debug() << "Output already exists at '" << m_outputName << "'" << endmsg;
    return StatusCode::SUCCESS;
  }

  // If input does not exist, and we aren't making the output regardless, just return
  if ( !m_alwaysOutput && !exist<LHCb::MCVertices>(m_inputName) ) {
    if (msgLevel(MSG::DEBUG) )
      debug() << "Input does not exist at '" << m_inputName << "'" << endmsg;
    return StatusCode::SUCCESS;
  }

  LHCb::MCVertices* verts = getOrCreate<LHCb::MCVertices,LHCb::MCVertices>( m_inputName );
  if( msgLevel(MSG::DEBUG) )
    debug() << m_inputName << " contains " << verts->size()
            << " MCVertices to convert." << endmsg;

  StandardPacker pack(this);
  static const double smallest = boost::numeric::bounds<float>::smallest();
  static const double largest  = boost::numeric::bounds<float>::highest();
  static const double tiny     = boost::numeric::bounds<double>::smallest();

  LHCb::PackedMCVertices* out = new LHCb::PackedMCVertices();
  put( out, m_outputName );

  // Packing version
  const char pVer = LHCb::PackedMCVertices::defaultPackingVersion();
  out->setPackingVersion(pVer);

  out->mcVerts().reserve( verts->size() );
  for ( const LHCb::MCVertex* vert : *verts )
  {
    out->mcVerts().push_back( LHCb::PackedMCVertex() );
    LHCb::PackedMCVertex& newVert = out->mcVerts().back();

    newVert.key  = vert->key();
    newVert.x    = pack.position( vert->position().x() );
    newVert.y    = pack.position( vert->position().y() );
    newVert.z    = pack.position( vert->position().z() );

    // Protect crazy vertex times (no need for fabs, is always positive!)
    if( vert->time() > 0. && vert->time() < smallest ) 
    {
      Warning( "PackedVertex.tof underflow, set to 0.", StatusCode::SUCCESS, 0 ).ignore();

      if( msgLevel(MSG::DEBUG) ) {
        if( vert->time() < tiny )
          debug() << "time smaller than " << tiny;
        else
          debug() << "time " << vert->time();

        debug() << " set to zero for vertex "
                << vert->key() << " of type " << vert->type() << endmsg;
      }

      newVert.tof = 0.;
    }
    else if ( vert->time() > largest ) {
      Warning( "PackedVertex.tof overflow, set to max float", StatusCode::SUCCESS, 0 ).ignore();
      if( msgLevel(MSG::DEBUG) )
        debug() << "time " << vert->time() << " set to "
                << largest << " for vertex "
                << vert->key() << " of type " << vert->type() << endmsg;
      newVert.tof = (float)largest ;
    }
    else
    {
      newVert.tof  = (float)vert->time();   // What scale ?
    }

    newVert.type = vert->type();

    newVert.mother = -1;
    if ( 0 != vert->mother() ) 
    {
      newVert.mother = ( 0==pVer ? 
                         pack.reference32( out, vert->mother()->parent(),
                                           vert->mother()->key() ) :
                         pack.reference64( out, vert->mother()->parent(),
                                           vert->mother()->key() ) );
    }
    for ( SmartRefVector<LHCb::MCParticle>::const_iterator itP = vert->products().begin();
          vert->products().end() != itP; ++itP ) 
    {
      newVert.products.push_back( 0==pVer ? 
                                  pack.reference32( out, (*itP)->parent(), (*itP)->key() ) :
                                  pack.reference64( out, (*itP)->parent(), (*itP)->key() ) );
    }

    if( msgLevel(MSG::VERBOSE) ) verbose() << "Vertex packed OK" << endmsg;
  }

  // If requested, remove the input data from the TES and delete
  if ( UNLIKELY(m_deleteInput) )
  {
    const StatusCode sc = evtSvc()->unregisterObject( verts );
    if ( sc.isSuccess() )
    {
      delete verts;
      verts = NULL;
    }
    else
    {
      return Error("Failed to delete input data as requested", sc );
    }
  }
  else
  {
    // Clear the registry address of the unpacked container, to prevent reloading
    verts->registry()->setAddress( 0 );
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
