// $Id: ParticleFilter.cpp,v 1.1 2002-03-27 20:35:00 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IToolSvc.h"

// from DaVinciTools
#include "DaVinciTools/IFilterCriterion.h"

// from Event
#include "Event/Particle.h"

// local
#include "ParticleFilter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ParticleFilter
//
// 15/03/2002 : Paul Colrain
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<ParticleFilter>          s_factory ;
const        IToolFactory& ParticleFilterFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ParticleFilter::ParticleFilter( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : AlgTool ( type, name , parent ) {

  // declare additional Interface
  declareInterface<IParticleFilter>(this);

  // declare properties
  declareProperty( "CriteriaNames", m_criteriaNames );
  
}

//=============================================================================
// initialize() method, build the vector of FilterCriterions
//=============================================================================
StatusCode ParticleFilter::initialize() {
  
  IToolSvc* tlSvc = 0;
  if ( serviceLocator() ) {
    StatusCode sc = StatusCode::FAILURE;
    sc = serviceLocator()->service( "ToolSvc", tlSvc );
  }

  MsgStream          log( msgSvc(), name() );
  log << MSG::DEBUG << ">>> ParticleFilter::initialize() " << endreq;
  log << MSG::DEBUG << ">>> Setting FilterCriterion... " << endreq;
  IFilterCriterion* icrit;
  std::vector<std::string>::iterator it;
  for ( it = m_criteriaNames.begin(); it != m_criteriaNames.end(); it++ ) {
    StatusCode sc = tlSvc->retrieveTool( *it, icrit, this );
    if( sc.isFailure() ) return StatusCode::FAILURE;
    m_criteria.push_back( icrit );
    log << MSG::DEBUG << ">>> Criteria " << *it << " set successfully" 
        << endreq;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Filter
//=============================================================================
StatusCode ParticleFilter::filter( const ParticleVector& input,
                                   ParticleVector& output ){

  ParticleVector::const_iterator ip;  
  for ( ip = input.begin(); ip != input.end(); ip++ ) {
    std::vector<IFilterCriterion*>::iterator ic;
    bool passed = true;
    for( ic = m_criteria.begin(); ic != m_criteria.end(); ic++ ) { 
      if( !( (*ic)->isSatisfied(*ip) ) ) passed = false;
    }
    if ( passed ) output.push_back( *ip );
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Filter positive particles
//=============================================================================
StatusCode ParticleFilter::filterPositive( const ParticleVector& input,
                                   ParticleVector& output ){

  ParticleVector::const_iterator ip;  
  for ( ip = input.begin(); ip != input.end(); ip++ ) {
    std::vector<IFilterCriterion*>::iterator ic;
    bool passed = true;
    for( ic = m_criteria.begin(); ic != m_criteria.end(); ic++ ) { 
      if( !( (*ic)->isSatisfied( *ip ) ) ) passed = false;
    }
    if ( (*ip)->charge() < 0. ) passed = false;    
    if ( passed ) output.push_back( *ip );
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Filter negative particles
//=============================================================================
StatusCode ParticleFilter::filterNegative( const ParticleVector& input,
                                   ParticleVector& output ){

  ParticleVector::const_iterator ip;  
  for ( ip = input.begin(); ip != input.end(); ip++ ) {
    std::vector<IFilterCriterion*>::iterator ic;
    bool passed = true;
    for( ic = m_criteria.begin(); ic != m_criteria.end(); ic++ ) { 
      if( !( (*ic)->isSatisfied( *ip ) ) ) passed = false;
    }
    if ( (*ip)->charge() > 0. ) passed = false;    
    if ( passed ) output.push_back( *ip );
  }

  return StatusCode::SUCCESS;
}
//=============================================================================





