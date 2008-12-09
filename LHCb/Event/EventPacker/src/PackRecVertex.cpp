// $Id: PackRecVertex.cpp,v 1.3 2008-12-09 08:13:23 ocallot Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/RecVertex.h"
#include "Event/PackedRecVertex.h"
#include "Kernel/StandardPacker.h"
// local
#include "PackRecVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PackRecVertex
//
// 2008-11-14 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PackRecVertex );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PackRecVertex::PackRecVertex( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::RecVertexLocation::Primary );
  declareProperty( "OutputName", m_outputName = LHCb::PackedRecVertexLocation::Primary );
}
//=============================================================================
// Destructor
//=============================================================================
PackRecVertex::~PackRecVertex() {} 

//=============================================================================
// Main execution
//=============================================================================
StatusCode PackRecVertex::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::RecVertices* verts = get<LHCb::RecVertices>( m_inputName );
  
  LHCb::PackedRecVertices* out = new LHCb::PackedRecVertices();
  put( out, m_outputName );
  out->setVersion( 1 );

  StandardPacker pack;

  for ( LHCb::RecVertices::const_iterator itV = verts->begin(); verts->end() != itV; ++itV ) {
    LHCb::RecVertex*      vert = *itV;
    LHCb::PackedRecVertex pVert;

    pVert.key        = vert->key();
    pVert.technique  = vert->technique();
    pVert.chi2       = pack.fltPacked( vert->chi2() );
    pVert.nDoF       = vert->nDoF();
    pVert.x          = pack.position( vert->position().x() );
    pVert.y          = pack.position( vert->position().y() );
    pVert.z          = pack.position( vert->position().z() );

    // convariance Matrix
    double err0 = sqrt( vert->covMatrix()(0,0) );
    double err1 = sqrt( vert->covMatrix()(1,1) );
    double err2 = sqrt( vert->covMatrix()(2,2) );
    
    pVert.cov00 = pack.position( err0 );
    pVert.cov11 = pack.position( err1 );
    pVert.cov22 = pack.position( err2 );
    pVert.cov10 = pack.fraction( vert->covMatrix()(1,0)/err1/err0 );
    pVert.cov20 = pack.fraction( vert->covMatrix()(2,0)/err2/err0 );
    pVert.cov21 = pack.fraction( vert->covMatrix()(2,1)/err2/err1 );

    //== Store the Tracks
    pVert.firstTrack = out->sizeRefs();
    for ( SmartRefVector<LHCb::Track>::const_iterator itT = vert->tracks().begin();
          vert->tracks().end() != itT; ++itT ) {
      int myRef = pack.reference( out, (*itT)->parent(), (*itT)->key() );
      out->addRef( myRef );
    }
    pVert.lastTrack = out->sizeRefs();

    //== Handles the ExtraInfo
    pVert.firstInfo = out->sizeExtra();
    for ( GaudiUtils::VectorMap<int,double>::iterator itE = vert->extraInfo().begin(); 
          vert->extraInfo().end() != itE; ++itE ) {
      out->addExtra( (*itE).first, pack.fltPacked( (*itE).second ) );
    }
    pVert.lastInfo = out->sizeExtra();
    out->addEntry( pVert );
  }

  // Clear the registry address of the unpacked container, to prevent reloading
  verts->registry()->setAddress( 0 );

  return StatusCode::SUCCESS;
}
//=============================================================================
