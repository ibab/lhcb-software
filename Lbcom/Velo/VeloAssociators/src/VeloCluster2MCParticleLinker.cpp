// Include files 

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// event model
#include "Event/VeloCluster.h"
#include "Event/MCParticle.h"
#include "Event/MCHit.h"

// Linkers
#include "Linker/LinkerWithKey.h"
#include "Linker/LinkerTool.h"

// local
#include "VeloCluster2MCParticleLinker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloCluster2MCParticleLinker
//
// 2006-03-15 : Tomasz Szumlak
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VeloCluster2MCParticleLinker )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloCluster2MCParticleLinker::VeloCluster2MCParticleLinker( const std::string& name,
                                                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_asctName ( LHCb::VeloClusterLocation::Default ),
    m_inputClusters ( LHCb::VeloClusterLocation::Default ),
    m_inputParts ( LHCb::MCParticleLocation::Default )
{ }
//=============================================================================
// Destructor
//=============================================================================
VeloCluster2MCParticleLinker::~VeloCluster2MCParticleLinker() {}
//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloCluster2MCParticleLinker::execute() {

  bool isDebug = msgLevel(MSG::DEBUG);

  if(isDebug) debug() << "==> Execute" << endmsg;
  // input objects
  LHCb::VeloClusters* clusters;
  if(!exist<LHCb::VeloClusters>(m_inputClusters)){
    error()<< " ==> There si no VeloClusters at: " << m_inputClusters <<endmsg;
    return ( StatusCode::FAILURE );
  }else{
    clusters=get<LHCb::VeloClusters>(m_inputClusters);
    if(isDebug) debug()<< "assoc size of clusters: " << clusters->size() <<endmsg;    
  }
  //
  LHCb::MCParticles* parts;
  if(!exist<LHCb::MCParticles>(m_inputParts)){
    error()<< " ==> There is no MCParticles at: " << m_inputParts <<endmsg;
    return ( StatusCode::FAILURE );
  }else{
    parts=get<LHCb::MCParticles>(m_inputParts);
    if(isDebug) debug()<< "size of parts: " << parts->size() <<endmsg;
  }
  // linker table VeloClusters -> MCParticles
  LinkerWithKey<LHCb::MCParticle, LHCb::VeloCluster>
               myLink(evtSvc(), msgSvc(), asctName());
  LHCb::VeloClusters::const_iterator cluIt;
  std::map<const LHCb::MCParticle*, double> relations;
  // get the associations form VeloClusters -> MCHit
  typedef LinkerTool<LHCb::VeloCluster, LHCb::MCHit> asctTool;
  typedef asctTool::DirectType Table;
  typedef Table::Range Range;
  typedef Table::iterator iterator;
  //
  asctTool associator(evtSvc(), LHCb::VeloClusterLocation::Default+"2MCHits");
  const Table* table=associator.direct();
  if(!table){
    error()<< " ==> Empty table with associations " <<endmsg;
    return ( StatusCode::FAILURE );
  }
  //
  for(cluIt=clusters->begin(); cluIt!=clusters->end(); cluIt++){    
    Range range1=table->relations(*cluIt);
    iterator asctIt;
    // make associations
    if(isDebug) debug()<< "table: " << range1.size() <<endmsg;
    
    for(asctIt=range1.begin(); asctIt!=range1.end(); asctIt++){
      const LHCb::MCHit* aHit=asctIt->to();
      const LHCb::MCParticle* aPart=aHit->mcParticle();
      if(isDebug)   debug()<< "energy: " << aPart->momentum().e()/Gaudi::Units::GeV
			   << " [GeV]" <<endmsg;
      //
      relations[aPart]+=asctIt->weight();
    }
    // linkere table
    if(!relations.empty()){
      if(isDebug) debug()<< " ==> Make links " <<endmsg;
      //
      std::map<const LHCb::MCParticle*, double>::const_iterator relIt;
      for(relIt=relations.begin(); relIt!=relations.end(); relIt++){
        SmartRef<LHCb::MCParticle> myPart=(*relIt).first;
        double weight=(*relIt).second;
        myLink.link((*cluIt), myPart, weight);
      }
      //
    }else{
      if(isDebug) debug()<< " ==> No associations made! " <<endmsg;
    }
    relations.clear();
  }
  //
  return StatusCode::SUCCESS;
}
//=============================================================================
