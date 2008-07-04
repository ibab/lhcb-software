// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/IIncidentSvc.h"

// local
#include "DaVinciAssociatorsWrapper.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DaVinciAssociatorsWrapper
//
// 2008-06-26 : Vladimir Gligorov (based on code by P. Koppenburg)
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( DaVinciAssociatorsWrapper );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DaVinciAssociatorsWrapper::DaVinciAssociatorsWrapper( const std::string& type,
                                                              const std::string& name,
                                                              const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_linker(NULL)
{
  declareInterface<IDaVinciAssociatorsWrapper>(this);
  declareInterface<IIncidentListener>(this);
}
//=============================================================================
// Destructor
//=============================================================================
DaVinciAssociatorsWrapper::~DaVinciAssociatorsWrapper() {} 
//=============================================================================
// Make & return the linker
//============================================================================= 
Particle2MCLinker* DaVinciAssociatorsWrapper::linker( /*std::string*/ int myMethod, 
                                                      std::vector<std::string> myLocations 
                                                    ) {
  //If the linker already exists, just return a pointer to it
  m_locations = myLocations;
  if ( m_linker != NULL) return m_linker ;
  if ((myMethod <= Particle2MCMethod::No) || (myMethod >= Particle2MCMethod::Max)) {
    err() << "No linker type specified!" << endmsg ;
    return NULL ;
  }

  //Otherwise make the required linker 
  info() << "Creating linker for " << Particle2MCMethod::algType[myMethod] ;
  info() << " using locations " << m_locations ;
  m_linker =  new Particle2MCLinker(this, myMethod, m_locations);
  info() << endmsg ;

  //If something went wrong...
  if ( NULL == m_linker ){
    err() << "Could not create linker " << Particle2MCMethod::algType[myMethod] << endmsg ;
    return NULL ;
  }
  return m_linker ; 
}
//=============================================================================
// initialize
//=============================================================================
StatusCode DaVinciAssociatorsWrapper::initialize() {
  StatusCode sc = GaudiTool::initialize();
  incSvc()->addListener( this, IncidentType::EndEvent, 100 );
  return sc;
} 
//=============================================================================
// finalize
//=============================================================================
StatusCode DaVinciAssociatorsWrapper::finalize() {
  return GaudiTool::finalize() ; 
}
//=============================================================================
// Implementation of Listener interface
//=============================================================================
void DaVinciAssociatorsWrapper::handle(const Incident& thisIncident){
  verbose() << "An incident has occured" << endmsg ;
  if (thisIncident.type() == "EndEvent") {
    verbose() << "Deleting linker at end of event incident" << endmsg ;
    if ( NULL != m_linker ) delete m_linker ;
    m_linker = NULL;
  }
  return;
}

