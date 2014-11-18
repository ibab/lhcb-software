// $Id: GiGaSensDetSequence.cpp,v 1.3 2008-07-26 15:44:17 robbep Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// from GiGa 
//#include "GiGa/GiGaMACROs.h"
#include "GiGaCnv/IGiGaGeomCnvSvc.h"

// local
#include "GiGaSensDetSequence.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GiGaSensDetSequence
//
// 2006-07-12 : Gloria CORTI
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GiGaSensDetSequence )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GiGaSensDetSequence::GiGaSensDetSequence( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : G4VSensitiveDetector( name )
  ,GiGaSensDetBase ( type, name , parent )
{

  m_sensDets.clear();
  m_sensDetsNames.clear();
  declareProperty( "Members", m_sensDetsNames );

}

//=============================================================================
// Destructor
//=============================================================================
GiGaSensDetSequence::~GiGaSensDetSequence() {} 


//=============================================================================
// initialize (Gaudi)  
//=============================================================================
StatusCode GiGaSensDetSequence::initialize() {

  StatusCode sc = GiGaSensDetBase::initialize(); ///< Initialize base class
  if ( sc.isFailure() ) return sc;  // error printed already
  
  debug() << "==> Initialize" << endmsg;
  
  IGiGaGeomCnvSvc* geoSvc = svc< IGiGaGeomCnvSvc > ( "GiGaGeo" );

  for ( std::vector< std::string >::const_iterator 
          iName = m_sensDetsNames.begin(); m_sensDetsNames.end() != iName; 
        ++iName ) {    

    // Instanciate sensitive detectors via GiGaGeo service (they belong
    // to GiGaGeo: do not clean them up!)
    IGiGaSensDet* aDet;
    StatusCode sc = geoSvc->sensitive( *iName, aDet );
    if ( sc.isFailure() ) { 
      return Error("Could no create SensDet :: " + (*iName) , sc ); 
    }
    if ( 0 == aDet ) { 
      return Error("Could no create SensDet :: " + (*iName) );
    }
    m_sensDets.push_back( aDet );
    
  }

  return StatusCode::SUCCESS;
  
}


//=============================================================================
// finalize (Gaudi)
//=============================================================================
StatusCode GiGaSensDetSequence::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GiGaSensDetBase::finalize(); // must be called after all other actions  
}


//=============================================================================
// Initialize (G4 method called at the begin of each G4event)  
// see G4VSensitiveDetector 
//=============================================================================
void GiGaSensDetSequence::Initialize( G4HCofThisEvent* HCE ) {

  // Call the Initialize method of each sensitive detector in sequence
  for ( std::vector< IGiGaSensDet* >::const_iterator 
          iSDet = m_sensDets.begin(); m_sensDets.end() != iSDet; ++iSDet ) {

    (*iSDet)->Initialize( HCE );

  }

}


//=========================================================================
// ProcessHits (G4 method called for each step in a sensitive detector)
//=========================================================================
bool GiGaSensDetSequence::ProcessHits( G4Step* step, 
                                       G4TouchableHistory* history ) {

  // Call the ProcessHits method of each sensitive detector in sequence
  bool processed = false;
  for ( std::vector< IGiGaSensDet* >::const_iterator 
          iSDet = m_sensDets.begin(); m_sensDets.end() != iSDet; ++iSDet ) {

    
    bool procSDet = (*iSDet)->processStep( step, history );
    processed = ( processed || procSDet );

  }
  return processed;

}


//=============================================================================
// EndOfEvent (G4 method called at the end of each G4event)  
// see G4VSensitiveDetector 
//=============================================================================
void GiGaSensDetSequence::EndOfEvent( G4HCofThisEvent* HCE ) {
  
  // Call the EndOfEvent method of each sensitive detector in sequence
  for ( std::vector< IGiGaSensDet*>::const_iterator 
          iSDet = m_sensDets.begin(); m_sensDets.end() != iSDet; ++iSDet ) {

    (*iSDet)->EndOfEvent( HCE );

  }

}


//=============================================================================
