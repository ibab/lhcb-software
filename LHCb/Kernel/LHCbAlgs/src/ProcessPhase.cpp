// $Id: ProcessPhase.cpp,v 1.7 2007-05-31 12:54:59 cattanem Exp $

// Include files
#include "ProcessPhase.h"
#include "GaudiKernel/IJobOptionsSvc.h"

//------------------------------------------------------------------------------
//
// Implementation of class :  ProcessPhase
//
// Author:      Marco Cattaneo
// Created:     17th December 1999
//------------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ProcessPhase )


ProcessPhase::ProcessPhase( const std::string& name, ISvcLocator* pSvcLocator )
 : GaudiSequencer( name, pSvcLocator ) {
	m_detList.clear();
	declareProperty( "DetectorList", m_detList );
}

ProcessPhase::~ProcessPhase() { 
}

StatusCode ProcessPhase::initialize() {

  std::string myMeasureProp;
  getProperty( "MeasureTime", myMeasureProp ).ignore();
  IJobOptionsSvc* jobSvc = svc<IJobOptionsSvc>("JobOptionsSvc");

	// Declare sequences to the phase
  std::string myMembers = "{";
	for ( VectorName::iterator it=m_detList.begin(); it != m_detList.end(); it++){
    if( m_detList.begin() != it ) myMembers += ",";
    std::string algName = name() + (*it) + "Seq";
    myMembers +=  "\"GaudiSequencer/" + algName + "\"";
    // Sequences are not yet instantiated, so set MeasureTime property directly 
    // in the catalogue. Uses same value as the parent ProcessPhase
    StringProperty p( "MeasureTime", myMeasureProp );
    jobSvc->addPropertyToCatalogue( algName, p ).ignore();
  }
  myMembers += "}";
  setProperty( "Members", myMembers ).ignore();
  release( jobSvc ).ignore();

  return GaudiSequencer::initialize();
}
