// $Id$

// local
#include "PVRelatorAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PVRelatorAlg
//
// 2009-05-23 : Juan Palacios
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PVRelatorAlg )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  PVRelatorAlg::PVRelatorAlg( const std::string& name,
                              ISvcLocator* pSvcLocator)
    :
    GaudiAlgorithm ( name , pSvcLocator ),
    m_particleInputLocation(""),
    m_PVInputLocation(LHCb::RecVertexLocation::Primary),
    m_P2PVInputLocation(""),
    m_P2PVOutputLocation(""),
    m_useTable(false),
    m_pvRelator(0)
{
  declareProperty("ParticleInputLocation",  m_particleInputLocation);
  declareProperty("PrimaryVertexInputLocation",  m_PVInputLocation);
  declareProperty("P2PVRelationsInputLocation",  m_P2PVInputLocation);
  declareProperty("P2PVRelationsOutputLocation",  m_P2PVOutputLocation);
  declareProperty("PVRelatorName", 
                  m_pvRelatorName = DaVinci::DefaultTools::PVRelator );

}
//=============================================================================
// Destructor
//=============================================================================
PVRelatorAlg::~PVRelatorAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PVRelatorAlg::initialize()
{
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

  m_pvRelator = tool<IRelatedPVFinder>( m_pvRelatorName, this );

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PVRelatorAlg::execute() 
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  Particle2Vertex::WTable* table = (m_useTable) ? this->tableFromTable() : this->table();

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
Particle2Vertex::WTable* PVRelatorAlg::table() const
{

  typedef LHCb::Particle::Range Particles;
  typedef LHCb::RecVertex::Range Vertices;
  typedef Particle2Vertex::LightWTable RelTable;

  Particle2Vertex::WTable* table = new Particle2Vertex::WTable();

  Particles particles = getIfExists<Particles>(m_particleInputLocation);
  Vertices vertices   = getIfExists<Vertices>(m_PVInputLocation);

  if ( particles.empty() || vertices.empty() ) return table;

  for ( Particles::const_iterator iPart = particles.begin();
        iPart != particles.end(); ++iPart ) 
  {
    const RelTable bestPVTable =
      m_pvRelator->relatedPVs(*iPart,
                              LHCb::VertexBase::ConstVector(vertices.begin(),
                                                            vertices.end()));

    const RelTable::Range range = bestPVTable.relations();

    table->merge(range);
  }
  return table;

}
//=============================================================================
Particle2Vertex::WTable* PVRelatorAlg::tableFromTable() const
{
  typedef LHCb::Particle::ConstVector Particles;
  typedef LHCb::VertexBase::ConstVector Vertices;
  typedef LHCb::RecVertex::ConstVector PVs;
  typedef Particle2Vertex::LightWTable RelTable;
  typedef Particle2Vertex::WTable Table;
  typedef Particle2Vertex::WTable::InvType InvTable;

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

    const RelTable bestPVTable = m_pvRelator->relatedPVs(*iPart, LHCb::VertexBase::ConstVector(vertices.begin(), vertices.end()));

    const RelTable::Range rel = bestPVTable.relations();

    if ( msgLevel(MSG::VERBOSE) ) {
      verbose() << "Going to add " << rel.size()
                << " relations to table" << endmsg;
    }

    table->merge(rel);
  }

  return table;

}

//=============================================================================
