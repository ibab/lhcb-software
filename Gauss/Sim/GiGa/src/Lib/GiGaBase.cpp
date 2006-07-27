// $Id: GiGaBase.cpp,v 1.24 2006-07-27 09:33:00 gcorti Exp $
#define GIGA_GIGABASE_CPP 1 

// includes 
// from STL
#include <string>
#include <map>

// from GiGa 
#include "GiGa/IGiGaSvc.h"
#include "GiGa/IGiGaSetUpSvc.h"
#include "GiGa/GiGaException.h"
#include "GiGa/GiGaUtil.h" 
#include "GiGa/GiGaBase.h" 

//=============================================================================
// Standard constructor 
//=============================================================================
GiGaBase::GiGaBase( const std::string& type,
                    const std::string& Name,
                    const IInterface* parent )
  : GaudiTool( type , Name , parent )
  , m_gigaSvc( 0 ) 
  , m_setupSvc( 0 ) 
{

  if( 0 == svcLoc() ) 
  { throw GiGaException("GiGaBase():: ISvcLocator* points to NULL!") ; }

  /// Interfaces
  declareInterface<IGiGaInterface>    ( this );
  declareInterface<IIncidentListener> ( this );

  /// Properties
  declareProperty( "GiGaService"     , m_gigaName  = "GiGa" );
  declareProperty( "GiGaSetUpService", m_setupName = "GiGa" );

}


//=============================================================================
// destructor 
//=============================================================================
GiGaBase::~GiGaBase() {}


//=============================================================================
// initialize the object (Gaudi)
//=============================================================================
StatusCode GiGaBase::initialize() 
{
  // initialize the base class 
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) { 
    return Error("Could not initialize the base class AlgTool",sc);
  }

  // Check that can locate Services
  if( 0 == svcLoc() ) {
    throw GiGaException("GiGaBase::ini ISvcLocator* points to NULL!");
  }

  // Load GiGa
  if( !m_gigaName.empty() ) { 
    m_gigaSvc = svc<IGiGaSvc>( m_gigaName , true );
  }
  else { 
    Warning("GiGa Service is not requested to be located"); 
  }

  // If the setup service is the same, just get the interface
  if( !m_setupName.empty() ) { 
    if( m_setupName == m_gigaName ) {
      // cast to correct type
      m_setupSvc = dynamic_cast<IGiGaSetUpSvc*>( m_gigaSvc ); 
    }
    else {
      m_setupSvc = svc<IGiGaSetUpSvc> ( m_setupName , true );
    }
  }
  else { 
    Warning("GiGaSetUp Service is not requested to be located");
  }

  return StatusCode::SUCCESS;
}


//=============================================================================
// finalize the object (Gaudi)
//=============================================================================
StatusCode GiGaBase::finalize() { 
  return GaudiTool::finalize();
}


//=============================================================================
// handle the incident
//=============================================================================
void  GiGaBase::handle( const Incident& incident ) 
{ 
  Print ( " Incident = '"  + incident .type   () + "' " + 
          " \tsource = '"  + incident .source () + "' " ) ; 
}


//=============================================================================











