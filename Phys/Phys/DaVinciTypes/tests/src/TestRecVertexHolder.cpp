// $Id: $
// Include files 

// LHCb
#include "Event/RecVertex.h"

// local
#include "TestRecVertexHolder.h"

#include "Kernel/RecVertexHolder.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TestRecVertexHolder
//
// 2010-12-06 : Juan Palacios
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TestRecVertexHolder )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TestRecVertexHolder::TestRecVertexHolder( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_dummyPVLocation("Rec/Vertex/DummyPVs"),
    m_clonePVLocation("Rec/Vertex/ClonePVs")
{

}
//=============================================================================
// Destructor
//=============================================================================
TestRecVertexHolder::~TestRecVertexHolder() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TestRecVertexHolder::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TestRecVertexHolder::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  createDummyPVs();
  

  const LHCb::RecVertex::Container* pvs = 
    get<LHCb::RecVertex::Container>(m_dummyPVLocation);

  if (0==pvs) {
    return Error("Found no PV container in "+ m_dummyPVLocation);
  } else {
    info() << "Found " << pvs->size() << " PVs in " << m_dummyPVLocation
           << endmsg;
  }
  
  
  LHCb::RecVertex::Container* clonePVs = new LHCb::RecVertex::Container;

  put(clonePVs, m_clonePVLocation);

  LHCb::RecVertex::Container::const_iterator iPV = pvs->begin();
  LHCb::RecVertex::Container::const_iterator iPVEnd = pvs->end();


  // the following should, but does not, compile.
  //  const LHCb::RecVertex* tmp = new LHCb::RecVertex();  
  //  clonePVs->insert(tmp);

  info() << "Clone and test PVs" << endmsg;

  for ( ; iPV != iPVEnd ; ++iPV) {
    
    const LHCb::RecVertexHolder pvHolder = clonePV(*iPV);

    //    testRecVertexHolder(pvHolder);
    
    if (!inTES(pvHolder)) {
      //      clonePVs->insert(pvHolder);
      clonePVs->insert(const_cast<LHCb::RecVertex*>(pvHolder.vertex()));
    }
  
  }
  

  return StatusCode::SUCCESS;
}
//=============================================================================
void TestRecVertexHolder::createDummyPVs() 
{

  info() << "Creating dummy RecVertices in " + m_dummyPVLocation << endmsg;

  LHCb::RecVertex::Container* dummyPVs = new LHCb::RecVertex::Container;

  put(dummyPVs, m_dummyPVLocation);

  for (int i=0; i<10; i++) {
    LHCb::RecVertex* pv = new LHCb::RecVertex;
    dummyPVs->insert(pv);
  }
  

}
//=============================================================================
const LHCb::RecVertexHolder TestRecVertexHolder::clonePV(const LHCb::RecVertex* pv) const 
{

  return LHCb::RecVertexHolder(new LHCb::RecVertex(*pv));
 
}
//=============================================================================
const LHCb::RecVertexHolder TestRecVertexHolder::returnPV(const LHCb::RecVertex* pv) const 
{
  
  return LHCb::RecVertexHolder(pv);
  
}

//============================================================================= 
const LHCb::RecVertexHolder TestRecVertexHolder::createPV() const 
{

  return LHCb::RecVertexHolder(new LHCb::RecVertex());
  
}
//=============================================================================
void TestRecVertexHolder::testRecVertexHolder(const LHCb::RecVertexHolder pvHolder) const
{

  // test cast to LHCb::RecVertex.
  info() << "Test cast to RecVertex*" << endmsg;
  const LHCb::RecVertex* pv = pvHolder;
  if( pv )  // Hack to avoid unused variable warning when NDEBUG is set
    assert(pv == pvHolder.vertex());
  else
    assert(pv == pvHolder.vertex());
  

  // test -> operator
  info() << "Test operator ->" << endmsg;
  assert(pvHolder->isPrimary() == pvHolder.vertex()->isPrimary());
  
  info() << "Test null ptr and operator!()" << endmsg;
  assert(pvHolder->technique() == pvHolder.vertex()->technique());

  assert (!LHCb::RecVertexHolder());
  

  return;
}
//=============================================================================
bool TestRecVertexHolder::inTES(const LHCb::RecVertex* PV) const 
{
  return ( 0!=PV && 0!=PV->parent());
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode TestRecVertexHolder::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
