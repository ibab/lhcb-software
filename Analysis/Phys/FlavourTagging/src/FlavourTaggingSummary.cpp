// $Id: FlavourTaggingSummary.cpp,v 1.1 2003-03-27 09:54:14 odie Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "Event/EventHeader.h"
#include "Event/FlavourTag.h"

// local
#include "FlavourTaggingSummary.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FlavourTaggingSummary
//
// 2003-03-27 : Olivier Dormond
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<FlavourTaggingSummary>          s_factory ;
const        IAlgFactory& FlavourTaggingSummaryFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FlavourTaggingSummary::FlavourTaggingSummary( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
{
  m_tagsLocations.clear();
  m_tagsLocations.push_back( FlavourTagLocation::User );
  declareProperty("TagsLocations", m_tagsLocations);
}

//=============================================================================
// Destructor
//=============================================================================
FlavourTaggingSummary::~FlavourTaggingSummary() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode FlavourTaggingSummary::initialize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Initialise" << endreq;

  if( m_tagsLocations.size() == 0 ) {
    msg << MSG::FATAL << "Nothing to monitor!" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode FlavourTaggingSummary::execute() {

  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Execute" << endreq;

  SmartDataPtr<EventHeader> evtHead(eventSvc(), EventHeaderLocation::Default );
  if( !evtHead ) {
    msg << MSG::ERROR << "Couldn't retrieve the event head!" << endreq;
    return StatusCode::SUCCESS;
  }

  int nloc = m_tagsLocations.size();
  std::vector<std::vector<int> > summaries;
  std::vector<std::string>::const_iterator loc_i;
  for( int l = 0; l < nloc; l++ ) {
    msg << MSG::DEBUG << "Monitoring location: " << m_tagsLocations[l] <<endreq;
    SmartDataPtr<FlavourTags> tags(eventSvc(), m_tagsLocations[l]);
    if( tags == 0 )
      continue;
    msg << MSG::DEBUG << "Will monitor " << tags->size() << " tags." << endreq;
    FlavourTags::const_iterator tag_i;
    int t;
    for( t=0, tag_i=tags->begin(); tag_i!=tags->end(); tag_i++, t++ ) {
      int tag = 0;
      switch( (*tag_i)->decision() ) {
      case FlavourTag::b:       tag = -1;     break;
      case FlavourTag::bbar:    tag = 1;      break;
      case FlavourTag::none:    tag = 0;      break;
      }
      if( t >= summaries.size() )
        summaries.push_back(std::vector<int>(nloc,0));
      summaries[t][l] = tag;
    }
  }

  std::vector<std::vector<int> >::const_iterator s_i;
  for( s_i = summaries.begin(); s_i != summaries.end(); s_i++ ) {
    msg << MSG::INFO << "TAGGING SUMMARY "
        << evtHead->runNum() << ' ' << evtHead->evtNum();
    for( int l=0; l<nloc; l++ )
      msg << ' ' << (*s_i)[l];
    msg << endreq;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FlavourTaggingSummary::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
