// $Id: BTaggingMonitor.cpp,v 1.1 2004-05-03 13:54:09 pkoppenb Exp $
// local
#include "BTaggingMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BTaggingMonitor
//
// 2004-02-15 : Marco Musy
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<BTaggingMonitor>          s_factory ;
const        IAlgFactory& BTaggingMonitorFactory = s_factory ; 

//=============================================================================
BTaggingMonitor::BTaggingMonitor( const std::string& name,
				  ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator ), m_tags_locations(0)
{
  m_tags_locations.clear();
  m_tags_locations.push_back( FlavourTagLocation::User );
  declareProperty("TagsLocations", m_tags_locations);
  declareProperty("MCAssociator",  m_nameMCAsct = "Particle2MCLinksAsct");
}

//=============================================================================
BTaggingMonitor::~BTaggingMonitor() {}; 

//=============================================================================
StatusCode BTaggingMonitor::initialize()
{
  MsgStream req(msgSvc(), name());
  req << MSG::DEBUG << "==> Initialize" << endreq;

  if( m_tags_locations.size() == 0 ) {
    req << MSG::WARNING << "Nothing to monitor!" << endreq;
    return StatusCode::SUCCESS;
  }

  StatusCode sc = toolSvc()->retrieveTool( m_nameMCAsct, m_pAsctLinks, this);
  if( sc.isFailure() || 0 == m_pAsctLinks) {
    req << MSG::FATAL << "Unable to retrieve Link Associator tool"<<endreq;
    return sc;
  }
  sc = toolSvc()->retrieveTool( "DebugTool", m_debug );
  if( sc.isFailure() ) {
    req << MSG::FATAL << "Unable to retrieve Debug tool "<< endreq;
    return sc;
  }
  sc = service("ParticlePropertySvc", ppSvc);
  if( sc.isFailure() ) {
    req << MSG::FATAL << "Unable to locate Particle Property Service"<<endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode BTaggingMonitor::execute() {

  MsgStream  req( msgSvc(), name() );

  if( !filterPassed() ) {
    req << MSG::DEBUG << "Event was not selected. "<< endreq;
    setFilterPassed( false );
    return StatusCode::SUCCESS;
  }
  setFilterPassed( true );
 
  std::vector<std::string>::const_iterator loc_i;
  for(loc_i=m_tags_locations.begin(); loc_i!=m_tags_locations.end(); loc_i++){

    req <<MSG::DEBUG<< "Monitoring location: "<< (*loc_i)+"/Tags" << endreq;
    SmartDataPtr<FlavourTags> tags(eventSvc(), (*loc_i)+"/Tags");
    if( !tags ) {
      req<< MSG::DEBUG << "No tags found. " << endreq;
      continue;
    }
    req << MSG::DEBUG<< "Will monitor " << tags->size() << " tags." << endreq;

    FlavourTags::const_iterator ti;
    for( ti=tags->begin(); ti!=tags->end(); ti++ ) {
      req << MSG::DEBUG << "Tag Result= " << (*ti)->decision() 
	  <<"  category= "<< (*ti)->category() <<endreq;
      if( (*ti)->taggedB() ) {
	req << MSG::DEBUG << "taggedB P="<< (*ti)->taggedB()->p()/GeV <<endreq;
	//m_debug->printTree( (*ti)->taggedB() );
      }
      else req << MSG::DEBUG <<"B not found." <<endreq;
    }

    req << MSG::DEBUG << "Monitoring location: "<<(*loc_i)+"/Taggers" <<endreq;
    SmartDataPtr<Particles> tagCands( eventSvc(), (*loc_i)+"/Taggers" );
    if( !tagCands ) {
      req<< MSG::DEBUG << "No taggers found. " << endreq;
      continue;
    }
    req << MSG::DEBUG << "NR Taggers found= " << tagCands->size() << endreq;

    Particles::const_iterator ip;
    for( ip=tagCands->begin(); ip!=tagCands->end(); ip++ ){
      req << MSG::DEBUG << "tagger ID: " << (*ip)->particleID().pid()
	  << "  P=" << (*ip)->p()/GeV << endreq;

      //look into MC truth
      if( m_pAsctLinks->tableExists() ) {
	MCParticle* mcp = m_pAsctLinks->associatedFrom( *ip );
	if( mcp ) {
	  req << MSG::DEBUG << "  MCtagger ID: " << mcp->particleID().pid()
	      << "  P=" << mcp->momentum().vect().mag()/GeV << endreq;
	  m_debug->printAncestor(mcp);
	} else req << MSG::DEBUG <<"Asct MCParticle does not exist!"<< endreq;
      } else req << MSG::DEBUG <<"AsctLinks table does not exist!"<< endreq;
    }

    // Fill the PhysDesktop particle and vertex vectors.
    StatusCode scDesktop = desktop()->getInput();
    if( scDesktop.isFailure() ) {
      req << MSG::ERROR << "Unable to fill PhysDesktop " << endreq;
      return StatusCode::FAILURE;
    }
    // Retrieve the particles from PhysDesktop
    const ParticleVector& parts = desktop()->particles();

    for( ip=parts.begin(); ip!=parts.end(); ip++) {
      req << MSG::VERBOSE << " Dump all parts in Desktop:" << endreq;
	std::string sa= "none";
	MCParticle* mcp = m_pAsctLinks->associatedFrom( *ip );
	if(mcp) sa=ppSvc->findByStdHepID(mcp->particleID().pid())->particle();
	std::string sp=
	  ppSvc->findByStdHepID((*ip)->particleID().pid())->particle();
	req << MSG::VERBOSE << sp << " assoc. to mcp " << sa 
	    << "  (P="<< (*ip)->p()/GeV <<" GeV)"<< endreq;
    }
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
StatusCode BTaggingMonitor::finalize(){ return StatusCode::SUCCESS; }
//=============================================================================

