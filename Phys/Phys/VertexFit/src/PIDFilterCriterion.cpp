// $Id: PIDFilterCriterion.cpp,v 1.1.1.1 2004-08-24 06:35:15 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/GaudiException.h"

// local
#include "PIDFilterCriterion.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PIDFilterCriterion
//
// 19/03/2002 : Paul Colrain
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<PIDFilterCriterion>          s_factory ;
const        IToolFactory& PIDFilterCriterionFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PIDFilterCriterion::PIDFilterCriterion( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : GaudiTool ( type, name , parent ) {

  // declare additional interface
  declareInterface<IFilterCriterion>(this);

  // declare properties
  declareProperty( "ParticleNames", m_partNames );  
  declareProperty( "ParticleCodes", m_partCodes );  
  declareProperty( "ConfidenceLevels", m_confLevels );  

  // get pointer to ParticlePropertySvc
  m_ppSvc = 0;
  if ( serviceLocator() ) {
    StatusCode sc = StatusCode::FAILURE;
    //    sc = serviceLocator()->service( "ParticlePropertySvc", m_ppSvc );
    sc = service( "ParticlePropertySvc", m_ppSvc, true );
    if( sc.isFailure ()) {
      throw GaudiException("ParticlePropertySvc Not Found", 
                           "FilterCriterionException", 
                           StatusCode::FAILURE );
    }
  }

}

//=============================================================================
// initialize() method
//=============================================================================
StatusCode PIDFilterCriterion::initialize() {

  StatusCode sc = GaudiTool::initialize() ;
  if ( !sc) return sc ;

  if ( m_partNames.size() == 0 && m_partCodes.size() == 0 ) {
    err() << ">>>   PIDFilterCriterion::initialize() " << endreq;
    err() << ">>>   Properties have not been set " << endreq;
    return StatusCode::FAILURE;
  }

  if ( m_partNames.size() != 0 && m_partCodes.size() != 0 ) {
    err() << ">>>   PIDFilterCriterion::initialize() " << endreq;
    err() << ">>>   Attempt made to set Particle Identity both by name and code! " << endreq;
    return StatusCode::FAILURE;
  }

  // get codes corresponding to names
  if ( m_partNames.size() > 0 ) {
    std::vector< std::string >::iterator in;  
    for ( in = m_partNames.begin(); in != m_partNames.end(); in++ ) {    
      ParticleProperty* partProp = m_ppSvc->find( *in );
      if ( partProp == 0 ) {
        err() << "Cannot retrieve properties for particle \""
            << *in << "\" " << endreq;
         return StatusCode::FAILURE;
      }
      m_partCodes.push_back( partProp->jetsetID() );  
    }
  }
  
  // get names corresponding to codes
  if ( m_partNames.size() == 0 && m_partCodes.size() > 0 ) {
    std::vector< int >::iterator ic;  
    for ( ic = m_partCodes.begin(); ic != m_partCodes.end(); ic++ ) {    
      ParticleProperty* partProp = m_ppSvc->findByStdHepID( *ic );
      if ( partProp == 0 ) {
        err() << "Cannot retrieve properties for particle code "
            << *ic << endreq;
         return StatusCode::FAILURE;
      }
      m_partNames.push_back( partProp->particle() );  
    }
  }

  // Check CL vector is empty or not the same size as particles type
  if ( m_confLevels.size() == 0 ) {
    //    err() << " CL list is empty: you must set it for all particles" << endreq;
    //   return StatusCode::FAILURE;
    warning() << " CL list is empty: Will be set to 0 for all particles!" << endreq;
    for ( unsigned int i = 0; i < m_partCodes.size(); i++ ) {
      m_confLevels.push_back(0.0);
    }
  } else if ( m_partNames.size() != m_confLevels.size() ) {
    err() << ">>>   PIDFilterCriterion::initialize() " << endreq;
    err() << ">>>   Wrong number of Confidence Levels provided " << endreq;
    return StatusCode::FAILURE;   
  }

  debug() << ">>>   PIDFilterCriterion::initialize() " 
      << endreq;
  debug() << ">>>   Cuts are " << endreq;
  debug() << ">>>   Particle names    ";
  for (std::vector< std::string >::iterator in = m_partNames.begin(); 
                                            in != m_partNames.end(); in++ ) {
    debug() << *in << "  ";    
  }
  debug() << endreq;
  debug() << ">>>   Particle codes    ";
  for (std::vector< int >::iterator ic = m_partCodes.begin();
                                    ic != m_partCodes.end(); ic++ ) {
    debug() << *ic << "  ";    
  }
  debug() << endreq;
  debug() << ">>>   Confidence levels    ";
  for (std::vector< double >::iterator icl = m_confLevels.begin();
                                       icl != m_confLevels.end(); icl++ ) {
    debug() << *icl << "  ";    
  }
  debug() << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Test if filter is satisfied
//=============================================================================
bool PIDFilterCriterion::isSatisfied( const Particle* const & part ) {

  bool passed = false;  

  for ( unsigned int i = 0; i < m_partCodes.size(); i++ ) {
    if ( part->particleID().pid() == m_partCodes[i] &&
         part->confLevel() >= m_confLevels[i] ) passed = true;
  }
  verbose() << "Particle has PID " << part->particleID().pid() 
            << " with CL " << part->confLevel() << ". Passed = " << passed << endreq;    
  return passed;
  
}

//=============================================================================
// Test if filter is satisfied
//=============================================================================
bool PIDFilterCriterion::operator()( const Particle* const & part ) {

  return this->isSatisfied( part );

}

//=============================================================================
