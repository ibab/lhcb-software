// $Id: ProcessPhase.cpp,v 1.1 2003-05-30 12:01:59 cattanem Exp $

// Include files
#include "Kernel/ProcessPhase.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

typedef std::vector<Algorithm*>  AlgList;

//------------------------------------------------------------------------------
//
// Implementation of class :  ProcessPhase
//
// Author:      Marco Cattaneo
// Created:     17th December 1999
//------------------------------------------------------------------------------

static const AlgFactory<ProcessPhase>  s_factory;
const IAlgFactory& ProcessPhaseFactory = s_factory;


ProcessPhase::ProcessPhase( const std::string& name, ISvcLocator* pSvcLocator )
 : Algorithm( name, pSvcLocator ) {
	m_detList.clear();
	declareProperty( "DetectorList", m_detList );
}

ProcessPhase::~ProcessPhase() { 
}

StatusCode ProcessPhase::initialize() {

  MsgStream  log(msgSvc(),name());
  std::string algMsg = "";

	// Create subalgorithms
	for ( VectorName::iterator it=m_detList.begin(); it != m_detList.end(); it++){
    std::string algName = name() + (*it) + "Seq";
		Algorithm* m_Alg;
		StatusCode sc = createSubAlgorithm( "Sequencer", algName,  m_Alg);
    if( sc.isFailure() ) {
		  log << MSG::FATAL << "Unable to create " << (*it)
          << " sequencer" << endreq;
		  return sc;
		}
		else {
      algMsg += " ";
      algMsg += (*it);
      log << MSG::DEBUG << "Created " << (*it) << " sequencer" << endreq;
    }
	}
  log << MSG::INFO << "Sequencers created successfully:" << algMsg << endreq;
  return StatusCode::SUCCESS;
}

StatusCode ProcessPhase::execute()     {

  MsgStream log(msgSvc(),name());

	// Execution of sub-algorithms (sequencers)
	AlgList* m_algList = subAlgorithms();
	for ( AlgList::iterator it = m_algList->begin(); it != m_algList->end(); it++)
  {
		(*it)->resetExecuted();
		StatusCode sc = (*it)->sysExecute();
    if( sc.isFailure() ) {
		  log << MSG::FATAL << "Unable to execute " << (*it)->name() 
          << " sequencer" << endreq;
		  return sc;
		}
	}

	log << MSG::DEBUG << "Executed " << name() << endreq;
	return StatusCode::SUCCESS;
}

StatusCode ProcessPhase::finalize() {
  
  MsgStream log(msgSvc(),name());
	log << MSG::DEBUG << "Finalization completed successfully" << endreq;
	return StatusCode::SUCCESS;
}
