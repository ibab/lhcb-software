// $Id: LumiTool.cpp,v 1.1 2003-10-06 16:16:40 cattanem Exp $ 

// Include files
#include "LumiTool.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "Event/GenHeader.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <fstream>

//--------------------------------------------------------------------
//
//  ClassName  : LumiTool
// 
//  Determines probability of interactions for a given luminosity and
//  returns a number of interactions according to this probability.
//
//  The instantaneous luminosity is taken from /Event/Gen/Header.
//
//  Author :  Marco Cattaneo  29/09/2003
//  Based on SicbLumiTool
//--------------------------------------------------------------------

// Instantiation of a static factory class used by clients to create
// instances of this service

static ToolFactory<LumiTool> s_factory;
const IToolFactory& LumiToolFactory = s_factory;

//
// Standard Constructor
LumiTool::LumiTool( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent ) 
  : AlgTool( type, name, parent ) {

  if(getenv("PARAMFILESROOT") != NULL) {
    m_filename = std::string(getenv( "PARAMFILESROOT" )) +
      std::string( "/data/beam.cdf");
  }
  else {
    m_filename = std::string( "beam.cdf" );
  }
  declareProperty( "BeamFile", m_filename );

  m_bunchCrossRate = 29.49;
  m_totalXSection  = 102.4;
  m_EDS = 0;
  m_randSvc = 0;

  // Declare the interface that is implemented
  declareInterface<ILumiTool>(this);

}

// Destructor
LumiTool::~LumiTool() {
}

StatusCode LumiTool::finalize() 
{
  if( 0 != m_EDS )     { m_EDS->release();     m_EDS = 0; }
  if( 0 != m_randSvc ) { m_randSvc->release(); m_randSvc = 0; }

  return StatusCode::SUCCESS;
}


StatusCode LumiTool::initialize() 
{
  MsgStream log( msgSvc(), name() );

  // Tool needs to use internally the event data and random numbers services
  StatusCode sc = service("EventDataSvc", m_EDS, true);
  if( sc.isFailure ()) {
    log << MSG::ERROR << "EventDataSvc not found" << endmsg;
    return sc;
  }

  sc = service("RndmGenSvc", m_randSvc, true);
  if( sc.isFailure() ) {
    log << MSG::ERROR << "RndmGenSvc not found" << endmsg;
    return sc;
  }

  // Get the beam parameter data
  sc = getBeamParams();
  if( sc.isFailure() ) return sc;
  
  log << MSG::INFO << "BunchCrossRate  " << m_bunchCrossRate << endmsg;
  log << MSG::INFO << "Total X-section " << m_totalXSection << endmsg;
  
  return StatusCode::SUCCESS;
}


StatusCode LumiTool::numInteractions( int& nEvents ) {
  MsgStream log( msgSvc(), name() );
  nEvents = -1;

  // Get luminosity from generated event
  float currentLumi = -1;  // in units of 1.0**32 /(cm2*s)
  SmartDataPtr<GenHeader> pEvent( m_EDS, GenHeaderLocation::Default );
  if( 0 != pEvent ) {
    currentLumi = pEvent->luminosity() * cm2 * s / 1.e32;
  }
  else {
    log << MSG::ERROR << "Unable to retrieve GenHeader" << endreq;
    return StatusCode::FAILURE;
  }

  if( currentLumi > 0. ) {
    log << MSG::DEBUG << " Luminosity of main event: " << currentLumi << endmsg;
  } 
  else { 
    log << MSG::ERROR << " Bad lumi in main event: " << currentLumi << endmsg;
    return StatusCode::FAILURE;
  }

  //  Average number of interactions per bunch crossing
  float averageInter = m_totalXSection * currentLumi / m_bunchCrossRate / 10.;
  
  SmartIF<IRndmGen> genRndmPois;
  StatusCode sc;

  sc = m_randSvc->generator(Rndm::Poisson(averageInter), genRndmPois.pRef());
  if(sc.isSuccess()){
    float fltNEvents = genRndmPois->shoot();
    float tmp = fltNEvents - (int)fltNEvents;
    if ( tmp >= 0.5){
      nEvents = (int)(fltNEvents+0.5);
    }
    else {
      nEvents = (int)(fltNEvents);
    }
    log << MSG::DEBUG << " averageInter, fltNEvents, nEvents "
        << averageInter  << " " << fltNEvents   << " " << nEvents << endmsg;
  }
  else {
    log << MSG::ERROR << " failed to get Poisson random number" << endmsg;
  }
  
  return sc;
}

StatusCode LumiTool::getBeamParams() {

  // Current implementation reads directly from beam.cdf file.
  // Should be moved to a tool 

  MsgStream log( msgSvc(), name() );
  char line[ 255 ];
  std::ifstream infile( m_filename.c_str() );
  std::vector<float> params;

  if ( infile ) {
    log << MSG::INFO << "Opened beam params file : " << m_filename << endmsg;

    // Skip the header till GEOMETRY
    do {
      infile.getline( line, 255 );
    } while( line[0] != 'G' );

    // Loop over parameters
    while( infile ) {
      infile.getline( line, 255 );
      if( 'E' == line[0] ) break;
      if( '#' == line[0] ) continue;
      char* token = strtok( line, " " );
      if( token ) {
        if( '!' == token[0] ) continue;
        std::string sValue = token;
        float fValue = atof( sValue.c_str() );
        log << MSG::DEBUG << sValue << " " << fValue << endmsg;
        params.push_back( fValue );
      }
    }
    infile.close();
  }
  else {
    log << MSG::ERROR << "Failed to open beam params file " 
        << m_filename << endmsg;
    return StatusCode::FAILURE;
  }

  m_bunchCrossRate = params[14];
  m_totalXSection  = params[15];

  return StatusCode::SUCCESS;
}

        

