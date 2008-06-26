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
Particle2MCLinker* DaVinciAssociatorsWrapper::linker(std::string myAlgType, std::vector< std::string > myLocations) {
  //If the linker already exists, just return a pointer to it
  m_algType = myAlgType;
  m_locations = myLocations;
  if ( m_linker != NULL) return m_linker ;
  if (m_algType == "") {
    err() << "No linker type specified!" << endmsg ;
    return NULL ;
  }
  //Otherwise create it with the required method
  int method = -1 ;
  for ( int i = Particle2MCMethod::No ; i<Particle2MCMethod::Max ; ++i){
    verbose() << "Comparing " << i << " " << Particle2MCMethod::algType[i]
              << " and " << m_algType << endmsg ;
    if ( Particle2MCMethod::algType[i] == m_algType ) {
      method = i ;
      break ;
    }
  }
  //If no valid method is specified, break
  if (method < 0) {
    err() << "Invalid method specified!" << endmsg ;
    return NULL ;
  }
  //Otherwise make the required linker 
  info() << "Creating linker for " << m_algType ;
  info() << " using locations " << m_locations ;
  m_linker =  new Particle2MCLinker(this, method, m_locations);
  info() << endmsg ;
  //If something went wrong...
  if ( NULL == m_linker ){
    err() << "Could not create linker " << m_algType << endmsg ;
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

