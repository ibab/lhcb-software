// $Id: FlavourTaggingSummary.cpp,v 1.3 2003-06-16 16:30:51 odie Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "Event/EventHeader.h"
#include "Event/Collision.h"
#include "Event/FlavourTag.h"
#include "Event/L0DUReport.h"
#include "Event/L1Report.h"

#include "MCTools/IVisPrimVertTool.h"

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

  StatusCode sc = toolSvc()->retrieveTool( "VisPrimVertTool", m_visTool, this );
  if( sc.isFailure() ) {
    msg << MSG::FATAL << "Unable to retrieve the VisPrimVertTool" << endreq;
    return sc;
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

  SmartDataPtr<L0DUReport> l0(eventSvc(), L0DUReportLocation::Default);
  SmartDataPtr<L1Report> l1(eventSvc(), L1ReportLocation::Default);

  SmartDataPtr<Collisions> collisions(eventSvc(), CollisionLocation::Default);

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
    unsigned int t;
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
    msg << " T ";
    if( l0 )
      msg << (l0->decision() ? 1 : 0);
    else
      msg << "N/A";
    msg << ' ';
    if( l1 )
      msg << (l1->decision() ? 1 : 0);
    else
      msg << "N/A";    
    msg << " C ";
    if( collisions ) {
      msg << collisions->size() << " V ";
      long v;
      if( m_visTool->countVertices(v).isSuccess() )
        msg << v;
      else
        msg << "N/A";
      //msg << " [";
      //if( collisions->size() > 0 ) {
      //  Collisions::const_iterator ci;
      //  for( ci = collisions->begin(); ci != collisions->end(); ci++ ) {
      //    if( !m_visTool->isVisible(*ci) )
      //      continue;
      //    msg << (*ci)->processType() << ", ";
      //  }
      //  msg << "]";
      //} else
      //  msg << " [N/A]";
    } else
      msg << "N/A V N/A [N/A]";
    msg << " :";
    for( int l=0; l<nloc; l++ )
      msg << ' ' << std::setw(2) << (*s_i)[l];
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
