// $Id: PIDFilter.cpp,v 1.1 2005-02-15 07:31:50 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "PIDFilter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PIDFilter
//
// 2004-10-07 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<PIDFilter>          s_factory ;
const        IAlgFactory& PIDFilterFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PIDFilter::PIDFilter( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
    , m_particleNames()
    , m_particleIDs()
    , m_reject(false)
{
  declareProperty( "ParticleNames", m_particleNames   );
  declareProperty( "Reject", m_reject   );
}
//=============================================================================
// Destructor
//=============================================================================
PIDFilter::~PIDFilter() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode PIDFilter::initialize() {

  debug() << "==> Initialize" << endmsg;
  if ( m_particleNames.empty() ){
    if ( m_reject ) warning() << "No particles given to reject, will be doing nothing" << endmsg ;
    else warning() << "No particles given to accept, will be doing nothing" << endmsg ;
  }
  else {
    if (m_reject) info() << "Will reject all of " << m_particleNames << endmsg ;
    else info() << "Will keep all of " << m_particleNames << endmsg ;
  }
  
  std::vector<std::string>::const_iterator i;
  for ( i = m_particleNames.begin(); i != m_particleNames.end(); ++i) {
    ParticleProperty *pp = ppSvc()->find(*i);
    if (pp==0) {
      fatal() << " Unable to retrieve particle property for " << *i << endmsg;
      return StatusCode::FAILURE;
    }
    int pid = pp->jetsetID();
    m_particleIDs.push_back(pid);
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode PIDFilter::execute() {

  debug() << "==> Execute" << endmsg;

  // code goes here  
  typedef ParticleVector PV;
  PV pv = desktop()->particles();
  PV pc ;
  verbose() << "Retrieved " << pv.size() << " particles from desktop" << endmsg;  PV::iterator i ;
  for ( i=pv.begin(); i!=pv.end(); ++i ) {
    if (*i==0) {
      err() << "Retrieved null particle ptr from desktop" << endmsg;   
    } else {
      bool passes = m_reject ; // if reject, passes if not found
      int pid = (*i)->particleID().pid() ;
      std::vector<int>::const_iterator pp ;
      for ( pp=m_particleIDs.begin(); pp!=m_particleIDs.end(); ++pp ) {
        if ( pid == (*pp) ){
          passes = (!m_reject) ; // if !reject passes if found
          break ;
        }
      }
      if ( passes ){
        Particle *clone = (*i)->clone();
    //    clone->setOrigin(*i);   // PK: causes troubles with origin
    // somewhat tricky... would prefer to insert a 'link' 
    // into TES instead of a clone...
        pc.push_back(clone);
        debug() << "Clone " << pid << " with momentum " << 
          clone->momentum() << " m=" << clone->mass() << endreq ;
      }
    } 
  }
  // write output
  bool m_found = (!pc.empty()) ;  
  if ( m_found ) info() << "Saved " << pc.size() << " from " << pv.size() 
           << " candidates" << endmsg;
  else debug() << "Writing nothing to output" << endmsg;

  StatusCode scDesktop = desktop()->saveTrees(pc);
  if ( scDesktop) setFilterPassed( m_found );

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PIDFilter::finalize() {

  debug() << "==> Finalize" << endmsg;

  return  StatusCode::SUCCESS;
}

//=============================================================================
