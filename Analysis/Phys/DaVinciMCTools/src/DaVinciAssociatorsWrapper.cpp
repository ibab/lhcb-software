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
DECLARE_TOOL_FACTORY( DaVinciAssociatorsWrapper )

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
Particle2MCLinker* DaVinciAssociatorsWrapper::linker(const Particle2MCMethod::AssociationMethod& method,
                                                     const std::vector<std::string>& locations
                                                     )
 {
  //If the linker already exists, just return a pointer to it
  if ( m_linker != NULL) {
    Warning("Linker already exists, returning it!",StatusCode::SUCCESS,0);
    return m_linker ;
  }
  if ((method <= Particle2MCMethod::No) || (method >= Particle2MCMethod::Max)) {
    err() << "No linker type specified!" << endmsg ;
    return NULL ;
  }

  //Otherwise make the required linker
  debug() << "Creating linker for " << Particle2MCMethod::algType[method] ;
  debug() << " using locations " << locations ;
  m_linker =  new Particle2MCLinker(this, method, locations);
  debug() << endmsg ;

  //If something went wrong...
  if ( NULL == m_linker ){
    err() << "Could not create linker " << Particle2MCMethod::algType[method] << endmsg ;
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

