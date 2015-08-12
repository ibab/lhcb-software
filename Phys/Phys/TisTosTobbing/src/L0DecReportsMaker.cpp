// $Id: L0DecReportsMaker.cpp,v 1.2 2010-07-07 20:48:40 tskwarni Exp $
// #define DEBUGCODE
// Include files
//#include "boost/lexical_cast.hpp"
#include <boost/functional/hash.hpp>

// from Gaudi
#include "GaudiKernel/StatusCode.h"

#include "Event/HltDecReports.h"

#include "Event/L0DUReport.h"


// local
#include "L0DecReportsMaker.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : L0DecReportsMaker
//
// 2010-06-23 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0DecReportsMaker )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  L0DecReportsMaker::L0DecReportsMaker( const std::string& name,
                                        ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty("InputL0DUReportLocation",
                  m_inputL0DUReportLocation= LHCb::L0DUReportLocation::Default );

  declareProperty("OutputHltDecReportsLocation",
                  m_outputHltDecReportsLocation= "HltLikeL0/DecReports" );
}


//=============================================================================
// Destructor
//=============================================================================
L0DecReportsMaker::~L0DecReportsMaker() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0DecReportsMaker::execute()
{

  if ( msgLevel(MSG::VERBOSE) ) verbose() << "==> Execute" << endmsg;

  // create output container and put it on TES
  HltDecReports* outputSummary = new HltDecReports();
  put( outputSummary, m_outputHltDecReportsLocation );

  const L0DUReport* pL0DUReport = 0;
  //  L0 Decision Unit Report
  //       try without RootInTES first
  if( exist<L0DUReport>( m_inputL0DUReportLocation.value(), false ) ){
    pL0DUReport = get<L0DUReport>( m_inputL0DUReportLocation.value(), false );
    //     now try at RootInTES location
  } else if( exist<L0DUReport>( m_inputL0DUReportLocation ) ){
    pL0DUReport = get<L0DUReport>( m_inputL0DUReportLocation.value());
  }
  if( !pL0DUReport ){return Error( " No L0DUReport at " + m_inputL0DUReportLocation.value(), StatusCode::SUCCESS, 10 );}

  if ( msgLevel(MSG::VERBOSE) ) verbose() << " L0 global decision= " << pL0DUReport->decision() << endmsg;


  // copy TCK
  outputSummary->setConfiguredTCK(  pL0DUReport->tck() );


  // insert global decision
  const std::string selname = "L0Global";
  bool dec = pL0DUReport->decision();
  HltDecReport decReport(dec,0,0,3);
  if( outputSummary->insert(selname,decReport)== StatusCode::FAILURE ){
    Warning(" Failed to add HltDecReport selName "+selname
            +" to its container ",StatusCode::SUCCESS, 10 );
  }


  //       individual L0 trigger lines
  if (0==pL0DUReport->configuration()) {
    return Warning("Null configuration",StatusCode::SUCCESS,10) ;
  }
  L0DUChannel::Map channels = pL0DUReport->configuration()->channels();
  for( L0DUChannel::Map::const_iterator it=channels.begin();it!=channels.end();++it){
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " it->second " << it->second << endmsg;
    if (0==it->second){
      Warning("Channel config is empty",StatusCode::SUCCESS,10).ignore();
      continue;
    }
    if( ( it->second->decisionType() & L0DUDecision::Physics ) == 0 )continue;
    if ( msgLevel(MSG::VERBOSE) ){
      verbose() << " L0 trigger name= " << it->first << " decision= " << pL0DUReport->channelDecision(it->second->id()) << endmsg;
      for( LHCb::L0DUElementaryCondition::Map::const_iterator ic = it->second->elementaryConditions().begin();
           ic != it->second->elementaryConditions().end(); ++ic){
        if ( msgLevel(MSG::VERBOSE) ) verbose() << "        " << ic->first << endmsg;
      }
    }
    std::string selName = "L0" + it->first + "Decision";
    bool dec = pL0DUReport->channelDecision(it->second->id());
    // only physics decisions
    HltDecReport decReport(dec,0,0,0);


    // insert selection into the container
    if( outputSummary->insert(selName,decReport) == StatusCode::FAILURE ){
      Warning(" Failed to add HltDecReport selName "+selName
              +" to its container ",StatusCode::SUCCESS, 10 );
    }

  }


  // ----------------- printout ------------------------
  if ( msgLevel(MSG::VERBOSE) ){

    verbose() << " ======= HltDecReports size= " << outputSummary->size() << endmsg;
    verbose() << *outputSummary << endmsg;

  }

  return StatusCode::SUCCESS;
}

