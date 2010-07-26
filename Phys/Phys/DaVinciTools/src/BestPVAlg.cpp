// $Id: BestPVAlg.cpp,v 1.1 2010-07-26 16:12:40 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCb
#include "Kernel/IOnOffline.h"
#include "Kernel/IRelatedPVFinder.h"
#include "Event/RecVertex.h"
#include "Event/Particle.h"
#include "Kernel/Particle2Vertex.h"
#include "Relations/Get.h"
// local
#include "BestPVAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BestPVAlg
//
// 2010-07-26 : Juan Palacios
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( BestPVAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BestPVAlg::BestPVAlg( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : 
  GaudiAlgorithm ( name , pSvcLocator ),
  m_particleInputLocation(""),
  m_PVInputLocation(LHCb::RecVertexLocation::Primary),
  m_P2PVInputLocation(""),
  m_P2PVOutputLocation(""),
  m_useTable(false),
  m_OnOffline(0),
  m_pvRelator(0)
{
  declareProperty("ParticleInputLocation",  m_particleInputLocation);
  declareProperty("PrimaryVertexInputLocation",  m_PVInputLocation);
  declareProperty("P2PVRelationsInputLocation",  m_P2PVInputLocation);
  declareProperty("P2PVRelationsOutputLocation",  m_P2PVOutputLocation);
  
}
//=============================================================================
// Destructor
//=============================================================================
BestPVAlg::~BestPVAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode BestPVAlg::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  if (m_P2PVInputLocation!="") m_useTable=true;
  
  if ( m_useTable ) { 
    if ( m_particleInputLocation!="" ||
         m_PVInputLocation!=LHCb::RecVertexLocation::Primary)  {
      return Error("You have set P2PVRelationsLocation and one of ParticleInputLocation and PrimaryVertexInputLocation.",
                   StatusCode::FAILURE);
    }
  } else {
    if (""==m_particleInputLocation) 
      return Error("ParticleInputLocation not set");
  }

  if (""==m_P2PVOutputLocation) return Error("P2PVRelationsOutputLocation not set");
  
  m_OnOffline = tool<IOnOffline>("OnOfflineTool",this);

  if (0==m_OnOffline) return Error("Failed to get IOnOffline tool");
  
  m_pvRelator = tool<IRelatedPVFinder>(m_OnOffline->relatedPVFinderType(), 
                                       this);
 
  if (0==m_pvRelator) return Error("Failed to get IRelatedPVFinder tool");

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode BestPVAlg::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  Particle2Vertex::Table* table = (m_useTable) ? this->tableFromTable() : this->table();
  
  if (0!=table) {
    if ( msgLevel(MSG::VERBOSE) ) {
      verbose() << "Storing relations table in " 
                << m_P2PVOutputLocation << endmsg;
      verbose() << "Table size " << table->relations().size() << endmsg;
    }
    put(table, m_P2PVOutputLocation);

  } else {
    Error("No table created!",10).ignore();
  }

  return StatusCode::SUCCESS;
}
//=============================================================================
Particle2Vertex::Table* BestPVAlg::table() const
{

  typedef LHCb::Particle::Range Particles;
  typedef LHCb::RecVertex::Range Vertices;

  Particle2Vertex::Table* table = new Particle2Vertex::Table();

  if ( !exist< LHCb::Particle::Range>(m_particleInputLocation) || 
       !exist< LHCb::RecVertex::Range>(m_PVInputLocation) ) return table;

  Particles particles = get<Particles>(m_particleInputLocation);
  Vertices vertices = get<Vertices>(m_PVInputLocation);

  if (particles.empty() || vertices.empty()) return table;

  for (Particles::const_iterator iPart = particles.begin();
       iPart != particles.end(); ++ iPart) {
    const LHCb::VertexBase* vtx = 
      m_pvRelator->relatedPV(*iPart, 
                             LHCb::VertexBase::ConstVector(vertices.begin(), 
                                                           vertices.end()));


    table->relate(*iPart, vtx);
  }
  return table;
 
}
//=============================================================================
Particle2Vertex::Table* BestPVAlg::tableFromTable() const
{
  typedef LHCb::Particle::ConstVector Particles;
  typedef LHCb::VertexBase::ConstVector Vertices;
  typedef LHCb::RecVertex::ConstVector PVs;
  typedef Particle2Vertex::Table Table;
  typedef Particle2Vertex::Table::InvType InvTable;
  
  const Table* inputTable = i_get<Table>(m_P2PVInputLocation);

  if (0==inputTable) return 0;

  Table* table = new Table();

  const InvTable invTable(*inputTable, 1);

  InvTable::Range invRange = invTable.relations();

  Particles particles;

  Relations::getUniqueTo(invRange.begin(), invRange.end(), particles);

  if ( msgLevel(MSG::VERBOSE) ) {
    verbose() << "tableFromTable found " 
              << particles.size() << " Particles" 
              << endmsg;
  }
  
  for (Particles::const_iterator iPart = particles.begin();
       iPart != particles.end(); ++iPart) {

    const Table::Range range = inputTable->relations(*iPart);

    Vertices vertices;

    Relations::getUniqueTo(range.begin(), range.end(), vertices);

    if ( msgLevel(MSG::VERBOSE) ) verbose() << "tableFromTable found " 
                                            << vertices.size() 
                                            << " related vertices" << endmsg;

    const LHCb::VertexBase* vtx = m_pvRelator->relatedPV(*iPart, 
                                                         LHCb::VertexBase::ConstVector(vertices.begin(), vertices.end()));

    table->relate(*iPart, vtx);
  }

  return table;

}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode BestPVAlg::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
