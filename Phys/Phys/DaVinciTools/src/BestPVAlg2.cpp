// $Id$
// Include files 

// local
#include "BestPVAlg2.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BestPVAlg2
//
// 2010-07-26 : Juan Palacios
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( BestPVAlg2 )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BestPVAlg2::BestPVAlg2( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : 
  GaudiAlgorithm ( name , pSvcLocator ),
  m_PVInputLocation(LHCb::RecVertexLocation::Primary),
  m_particleInputLocations(),
  m_P2PVInputLocations(),
  m_useTables(false),
  m_pvRelator(0)
{
  declareProperty("PrimaryVertexInputLocation",  m_PVInputLocation);
  declareProperty("ParticleInputLocations",  m_particleInputLocations);
  declareProperty("P2PVRelationsInputLocations",  m_P2PVInputLocations);
  declareProperty("PVRelatorName", 
                  m_pvRelatorName = DaVinci::DefaultTools::PVRelator );
}

//=============================================================================
// Destructor
//=============================================================================
BestPVAlg2::~BestPVAlg2() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode BestPVAlg2::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  if (!m_P2PVInputLocations.empty()) m_useTables=true;
  
  if ( m_useTables ) { 
    if ( (!m_particleInputLocations.empty()) ||
         (m_PVInputLocation!=LHCb::RecVertexLocation::Primary) )  {
      return Error("You have set P2PVRelationsLocations and one of ParticleInputLocations and PrimaryVertexInputLocation.",
                   StatusCode::FAILURE);
    }
  }
  
  m_pvRelator = tool<IRelatedPVFinder>( m_pvRelatorName, this );
 
  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode BestPVAlg2::execute()
{

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  if (m_useTables) 
  {
    this->tablesFromTables();
  }
  else
  {
    if ( !getIfExists<LHCb::RecVertex::Range>(m_PVInputLocation).empty() ) 
    {
      this->tables();
    }    
  }

  return StatusCode::SUCCESS;
}

//=============================================================================

void BestPVAlg2::tables() const
{

  typedef LHCb::Particle::Range Particles;
  typedef LHCb::RecVertex::Range Vertices;

  typedef DaVinci::Map::Particle2VertexBase P2PVMap;

  for ( std::vector<std::string>::const_iterator iLoc = m_particleInputLocations.begin(); 
        iLoc != m_particleInputLocations.end(); ++iLoc ) 
  {
    
    Particles particles = getIfExists<Particles>(*iLoc);

    if ( !particles.empty() ) 
    {
      P2PVMap* table = new P2PVMap();
      const std::string tableLoc = tableLocation(*iLoc);
      put( table, tableLoc );

      Vertices vertices = get<Vertices>(m_PVInputLocation);

      for ( Particles::const_iterator iPart = particles.begin();
            iPart != particles.end(); ++iPart )
      {
        const LHCb::VertexBase* vtx = 
          m_pvRelator->relatedPV(*iPart, 
                                 LHCb::VertexBase::ConstVector(vertices.begin(), 
                                                               vertices.end()));
        table->insert(*iPart, vtx);

      }
      checkTable(table, tableLoc);
    }
    
  }

}

//=============================================================================
void BestPVAlg2::tablesFromTables() const
{
  typedef LHCb::Particle::ConstVector Particles;
  typedef LHCb::VertexBase::ConstVector Vertices;
  typedef LHCb::RecVertex::ConstVector PVs;
  typedef DaVinci::Map::Particle2VertexBase P2PVMap;
  typedef Particle2Vertex::Table Table;
  typedef Particle2Vertex::Table::InvType InvTable;
 
  std::vector<std::string>::const_iterator iLoc = m_P2PVInputLocations.begin();
  std::vector<std::string>::const_iterator locEnd = m_P2PVInputLocations.end();
 
  for ( ; iLoc != locEnd ; ++iLoc) {

    const Table* inputTable = i_get<Table>(*iLoc);

    if (inputTable) {

      P2PVMap* table = new P2PVMap();
      const std::string tableLoc = tableLocation(*iLoc);
      put( table,  tableLoc );

      const InvTable invTable(*inputTable, 1);

      InvTable::Range invRange = invTable.relations();

      Particles particles;

      Relations::getUniqueTo(invRange.begin(), invRange.end(), particles);

      if ( msgLevel(MSG::VERBOSE) ) {
        verbose() << "tableFromTable found " 
                  << particles.size() << " Particles" 
                  << endmsg;
      }
  
      for ( Particles::const_iterator iPart = particles.begin();
            iPart != particles.end(); ++iPart ) 
      {

        const Table::Range range = inputTable->relations(*iPart);

        Vertices vertices;

        Relations::getUniqueTo(range.begin(), range.end(), vertices);

        if ( msgLevel(MSG::VERBOSE) ) verbose() << "tableFromTable found " 
                                                << vertices.size() 
                                                << " related vertices" << endmsg;

        const LHCb::VertexBase* vtx = 
          m_pvRelator->relatedPV(*iPart, 
                                 LHCb::VertexBase::ConstVector(vertices.begin(), 
                                                               vertices.end()));

        table->insert(*iPart, vtx);
      }
      checkTable(table, tableLoc);
    } // if inputTable 

  } // location loop
  

}
//=============================================================================
std::string BestPVAlg2::tableLocation(const std::string& location) const
{
  std::string tableLocation(location);
  DaVinci::StringUtils::removeEnding(tableLocation, "/");
  return tableLocation + "/" + this->name() + "_P2PV";
}
//=============================================================================
void BestPVAlg2::checkTable(const DaVinci::Map::Particle2VertexBase* table,
                            const std::string& tableLoc) const
{
  
  if (0!=table) {
    if ( msgLevel(MSG::VERBOSE) ) {
      verbose() << "Checking relations table in " 
                << tableLoc << endmsg;
      verbose() << "Table size " << table->size() << endmsg;
    }
  } else {
    Error("No table created!",10).ignore();
  }
}

//=============================================================================
