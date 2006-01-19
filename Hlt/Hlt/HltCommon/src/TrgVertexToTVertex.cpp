// $Id: TrgVertexToTVertex.cpp,v 1.1.1.1 2006-01-19 10:01:59 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "TrgVertexToTVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrgVertexToTVertex
//
// 2005-12-16 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrgVertexToTVertex );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrgVertexToTVertex::TrgVertexToTVertex( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

  declareProperty("InputVerticesName", 
                  m_inputVerticesName = TrgVertexLocation::Velo3D);
  declareProperty("OutputVerticesName",
                  m_outputVerticesName = "Hlt/Vertices/PrimaryVertices");

}
//=============================================================================
// Destructor
//=============================================================================
TrgVertexToTVertex::~TrgVertexToTVertex() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrgVertexToTVertex::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  debug() << "==> Initialize" << endreq;
  PatDataStore* store     = tool<PatDataStore>( "PatDataStore" );
  if (store == NULL) 
    warning() << " error not pat Data store " << endreq;
  
  // get the dataProvider
  TrgDataProvider* trgData = tool<TrgDataProvider>( "TrgDataProvider" );
  if (trgData == NULL) 
    warning() << " error not Trg Data Provider " << endreq;

  m_ivertices = trgData->vertices(m_inputVerticesName);
  m_overtices = store->createVertexContainer(m_outputVerticesName);

  release(store);
  release(trgData);

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrgVertexToTVertex::execute() {

  debug() << "==> Execute" << endmsg;
  //! get the pointers to the vertices containers
  
  for (TrgVertices::const_iterator it = m_ivertices->begin();
       it != m_ivertices->end(); it++) {
    const TrgVertex& vertex = *(*it);
    TVertex* tvertex = m_overtices->newEntry();
    copy(vertex,*tvertex);
  }
  return StatusCode::SUCCESS;
};

void TrgVertexToTVertex::copy(const TrgVertex& vertex, TVertex& tvertex) 
{
  tvertex.setPosition(vertex.position());
  tvertex.setPositionErr(vertex.positionErr());
  tvertex.setChi2(vertex.chi2());
  tvertex.setNDoF(vertex.nDoF());

  debug() << " TrgVertex " <<  vertex << endreq;
  debug() << " TVertex "   << tvertex << endreq;
  
  debug() << " TrgVertex pos " <<  vertex.position() << endreq;
  debug() << " TVertex   pos " << tvertex.position() << endreq;
  debug() << " TrgVertex posErr " <<  vertex.positionErr() << endreq;
  debug() << " TVertex   posErr " << tvertex.positionErr() << endreq;
  debug() << " TrgVertex chi2 " <<  vertex.chi2() << endreq;
  debug() << " TVertex   chi2 " << tvertex.chi2() << endreq;
  debug() << " TrgVertex ndof " <<  vertex.nDoF() << endreq;
  debug() << " TVertex   ndof " << tvertex.nDoF() << endreq;
  
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode TrgVertexToTVertex::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
