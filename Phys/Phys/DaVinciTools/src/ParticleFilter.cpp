// $Id: ParticleFilter.cpp,v 1.9 2008-08-27 14:15:26 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// from Event
#include "Event/Particle.h"

// local
#include "ParticleFilter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ParticleFilter
//
// 15/03/2002 : Paul Colrain
// 20/06/2002 : Sandra Amato: modified to include counters
// 08/09/2004 : P. Koppenburg: remove counters and histograms
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( ParticleFilter );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ParticleFilter::ParticleFilter( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent ) {

  // declare additional Interface
  declareInterface<IParticleFilter>(this);

  // declare properties
  declareProperty( "CriteriaNames", m_criteriaNames );

}

//=============================================================================
// initialize() method, build the vector of FilterCriterions
//=============================================================================
StatusCode ParticleFilter::initialize()
{

  const StatusCode sc = GaudiTool::initialize() ;
  if (!sc) return sc ;

  debug() << ">>> ParticleFilter::initialize() " << endreq;
  debug() << ">>> Setting FilterCriterion... " << endreq;

  IFilterCriterion* icrit;
  std::vector<std::string>::iterator it;
  for ( it = m_criteriaNames.begin(); it != m_criteriaNames.end(); it++ ) {
    StatusCode sc = toolSvc()->retrieveTool( *it, icrit, this );
    if( sc.isFailure() ) return StatusCode::FAILURE;
    m_criteria.push_back( icrit );
    debug() << ">>> Criteria " << *it << " set successfully" << endreq;
  }

  debug() << ">>> Criteria size " << m_criteria.size() << endreq;
  debug() << ">>> Particle Property Service " << endreq;
  m_ppSvc = svc<IParticlePropertySvc>("ParticlePropertySvc",true);
  if (!m_ppSvc){
    err() << "No ParticlePropertySvc" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Filter
//=============================================================================
StatusCode ParticleFilter::filter( const LHCb::Particle::ConstVector& input,
                                   LHCb::Particle::ConstVector& output )
{

  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Filter sees " << input.size() << " particles" << endreq;

  LHCb::Particle::ConstVector::const_iterator ip;
  for ( ip = input.begin(); ip != input.end(); ++ip ) {
    bool passed = true ;
    std::vector<IFilterCriterion*>::iterator ic;
    for( ic = m_criteria.begin(); ic != m_criteria.end(); ++ic ) {
      if ( msgLevel(MSG::VERBOSE) ) verbose() << "Starting filter" << endreq;
      if ((*ic)->isSatisfied(*ip)) {
        if ( msgLevel(MSG::VERBOSE) ) verbose() << "Passed!" << endreq;
      } else {
        if ( msgLevel(MSG::VERBOSE) ) verbose() << "Failed" << endreq;
        passed = false ;
        break ;  // no need to look further
      }
    }
    if (passed) output.push_back( *ip );
  }

  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Filter outputs " << output.size() << " particles" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Filter positive particles
//=============================================================================
StatusCode ParticleFilter::filterPositive( const LHCb::Particle::ConstVector& input,
                                           LHCb::Particle::ConstVector& output ){
  LHCb::Particle::ConstVector temp ;
  LHCb::Particle::ConstVector::const_iterator ip;
  for ( ip = input.begin(); ip != input.end(); ip++ ) {
    if ( (*ip)->charge() > 0) temp.push_back( *ip ) ;
  }
  return filter(temp, output);
}
//=============================================================================
// Filter negative particles
//=============================================================================
StatusCode ParticleFilter::filterNegative( const LHCb::Particle::ConstVector& input,
                                           LHCb::Particle::ConstVector& output ){
  LHCb::Particle::ConstVector temp ;
  LHCb::Particle::ConstVector::const_iterator ip;
  for ( ip = input.begin(); ip != input.end(); ip++ ) {
    if ( (*ip)->charge() < 0) temp.push_back( *ip ) ;
  }
  return filter(temp, output);
}
//=============================================================================
// Filter by PID - the slower method
//=============================================================================
StatusCode ParticleFilter::filterByPID( const LHCb::Particle::ConstVector& input,
                                        LHCb::Particle::ConstVector& output,
                                        const std::string& name,
                                        const bool alsoCC ){
  if (!m_ppSvc){
    err() << "No ParticlePropertySvc" << endreq;
    return StatusCode::FAILURE;
  }
  ParticleProperty* PP = m_ppSvc->find(name);
  if (!PP){
    err() << "Could not find particle property for " << name << endreq;
    return StatusCode::FAILURE;
  }
  const int pid = PP->pdgID();
  //
  // now get it:
  return filterByPID(input,output,pid,alsoCC);

}
//=============================================================================
// Filter by PID - the real method
//=============================================================================
StatusCode ParticleFilter::filterByPID( const LHCb::Particle::ConstVector& input,
                                        LHCb::Particle::ConstVector& output,
                                        const int& pid,
                                        const bool alsoCC ){

  LHCb::Particle::ConstVector temp ;
  LHCb::Particle::ConstVector::const_iterator ip;
  for ( ip = input.begin(); ip != input.end(); ip++ ) {
    if ( (*ip)->particleID().pid() == pid) temp.push_back( *ip ) ;
    else if ( alsoCC && (abs((*ip)->particleID().pid()) == abs(pid)))
      temp.push_back( *ip ) ;
  }
  return filter(temp, output);
}
//=============================================================================
// Finalize
//=============================================================================
StatusCode ParticleFilter::finalize()
{

  debug() << ">>> Release criteria tools" << endreq;

  while ( m_criteria.size() > 0 ) {
    IFilterCriterion* icrit =  m_criteria.back();
    m_criteria.pop_back();
    toolSvc()->releaseTool( icrit );
  }

  return GaudiTool::finalize();
}



